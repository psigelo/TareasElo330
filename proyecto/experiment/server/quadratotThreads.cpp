/*

	Proyecto de programacion de sistemas - elo 330.

	Pascal Sigel
	Oscar Silva

	fecha: noviembre/2014
	---------------------------------------------------

	Descripcion
	-----------





*/

#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <fstream> 
#include <sys/ioctl.h>
#include <vector>
#include <iostream>
#include <NEAT>
#include <string>
#include <poll.h>

using namespace std;
using namespace ANN_USM;

pthread_mutex_t mutex_lecturaFileDescriptor = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_ObtenerOrganismo = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cantidadHebrasListas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Inutil = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sincronizacionInicial = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t HayOrganismosNuevos = PTHREAD_COND_INITIALIZER ;
pthread_cond_t TodasHebrasEstanListas = PTHREAD_COND_INITIALIZER ;
pthread_cond_t NEAT_OK = PTHREAD_COND_INITIALIZER ;

int neatIsReady=0;

vector < char * > hostClientes; 	// Notar que todos los elementos tambien estan ene le heap
vector < char * > puertoClientes; 	// Notar que todos los elementos tambien estan ene le heap


int HyperNEATFile_fd;
int NEATFile_fd;
unsigned int cantidadHebrasListas = 0;


int organismosDisponibles;
int posicionOrganismo; // Dado que muchos hilos trabajaran en conjunto se debe saber cual es el organismo a cargar en el siguiente hilo.
Population * poblacion;

/*
	procesamientoDatosEntrada:
	
	Se procesan los datos que son entregados por el usuario.

	Se setea valores de:
		> hilosDisponibles

*/
void procesamientoDatosEntrada(int argc, char ** argv);

/*

	creacionHilos():
		> Se crean los hilos clientes.
		> Se inicializan con su HyperNEAT correspondiente.
		> Se espera por su respuesta positiva.

*/
void creacionHilos();

/*

	Se crea el NEAT principal el cual distribuira sus organismos a cada hilo, los organismos los enviara a traves. 

*/
void creacionNEAT(int argc, char ** argv);




/*
	procesarDatosHyperNEAT: 
		Se procesan los datos de HyperNEAT y se guardan en un filedescriptor HyperNEATFile_fd para su posterior uso en el setup de los hilos.
*/
void procesarDatosHyperNEAT (char * rutaArchivoHyperNEAT);


/*
	
	procesarDatosClientes:
		Se desifra la cantidad de clientes totales y ademas se guadan en el vector de rutas de clientes clientes

*/
void procesarDatosClientes 	(char * rutaArchivoClientes );


/*

	HiloCliente(void * tid):
		Se crea un hilo cliente que ha
	
*/
void * HiloCliente(void * tid);


int main(int argc, char ** argv){

	srand(time(0));	// Se crea semilla random nueva.
	procesamientoDatosEntrada( argc, argv );
	creacionHilos();
	creacionNEAT( argc, argv );

}



/*
	argv[1]: Ruta a archivo de configuraciones HyperNEAT.
	argv[2]: Ruta a archivo de configuraciones NEAT.
	argv[3]: Ruta a archivo de genoma NEAT.
	argv[4]: Archivo con todas las ips y puertos correspondientes de los clientes.
*/

void procesamientoDatosEntrada(int argc, char ** argv){

	if(argc==5)
	{
		procesarDatosHyperNEAT 	(argv[1]);
		procesarDatosClientes 	(argv[4]);
	}
	else
	{
		printf("Los datos han sido mal ingresados, la forma correscta es:\n %s [ruta configuraciones HyperNEAT] [ruta configuraciones NEAT] [ruta configuraciones Clientes] \n", argv[0]);
	}

}




void procesarDatosHyperNEAT (char * rutaArchivoHyperNEAT){
	HyperNEATFile_fd = open (rutaArchivoHyperNEAT, O_RDONLY);
}


/*
	HAY QUE PROBAR SI FUNCIONA
*/
void procesarDatosClientes 	(char * rutaArchivoClientes ){

	/*
		Se pasa el archivo a un string.
	*/
	ifstream file (rutaArchivoClientes);
	file.seekg (0, file.end);
	int length = file.tellg();
	file.seekg (0, file.beg);
	char buffer[length]; 
	file.read (buffer,length);
	file.close();



	char * pchHost;
	char * pchPuerto;
	char delimiters[] = " \n\t";


	pchHost 	= strtok (buffer,	delimiters); // Es el primer host
	pchPuerto 	= strtok (NULL  ,	delimiters); // Es el primer puerto

	// ahora se puede comenzar con el loop.

	while (pchHost != NULL)
	{
		char * host = (char *) malloc(strlen(pchHost)*sizeof(char));
		char * puerto = (char *) malloc(strlen(pchPuerto)*sizeof(char));

		memcpy(host,pchHost,strlen(pchHost));
		memcpy(puerto,pchPuerto,strlen(pchPuerto));

		hostClientes.push_back  (host  );
		puertoClientes.push_back(pchPuerto);

		pchHost   = strtok (NULL, delimiters);
		pchPuerto = strtok (NULL, delimiters);
	}

}


/*

	Se deben crear todos los hilos de comunicaciones 

*/

void creacionHilos()
{

	pthread_t threads[hostClientes.size()];
	int rc;
	unsigned int i[hostClientes.size()];
	
	for( unsigned j=0; j < hostClientes.size(); j++ ){
		cout << "main() : creating thread, " << j << endl;
		i[j]=j;
		rc = pthread_create(&threads[j], NULL, HiloCliente, (void *) &(i[j]));
	  if (rc){
	     cerr << "Error:unable to create thread," << rc << endl;
	     exit(-1);
	  }
	}

}



void * HiloCliente(void * tid)
{
	int n, s, len;
	int read_fd;
	int id;
	int count;
    struct hostent *hp;
    struct sockaddr_in name;
    int organismoElecto;
	id =  * ((int*) tid);
   
	
	hp = gethostbyname(hostClientes.at(id));
    s = socket(AF_INET, SOCK_STREAM, 0);
    name.sin_family = AF_INET;
    name.sin_port = htons( atoi( puertoClientes.at(id) ) );
    memcpy(&name.sin_addr, hp->h_addr_list[0], hp->h_length);
    len = sizeof(struct sockaddr_in);
    connect(s, (struct sockaddr *) &name, len);


	// ---- SE ENVIA EL ARCHIVO DE CONFIGURACIONES DE HYPERNEAT
	pthread_mutex_lock(&mutex_lecturaFileDescriptor);
		struct stat stat_buf;
		fstat (HyperNEATFile_fd, &stat_buf);
		sendfile(s,HyperNEATFile_fd , 0, stat_buf.st_size); 
	pthread_mutex_unlock(&mutex_lecturaFileDescriptor);
	//---------------------------------------------------------

	// AHORA EL CLIENTE TIENE LAS CONFIGURACIONES DE HYPERNEAT.


	struct pollfd resepcionSocket;
	resepcionSocket.fd = s;
	resepcionSocket.events = POLLIN;



	pthread_mutex_lock(&sincronizacionInicial);
		if(neatIsReady != 1){
			pthread_cond_wait(&NEAT_OK,&sincronizacionInicial);
		}
	pthread_mutex_unlock(&sincronizacionInicial);


	while(1){
		// SIEMPRE Y CUANDO SE MANTENGA LA CONEXI´ONSE DEBE SACAR UN ORGANISMO DE LOS QUE QUEDEN Y SE DEBE ENVIAR AL CLIENTE EL ORGANISMO.

		pthread_mutex_lock(&mutex_ObtenerOrganismo);
		if(organismosDisponibles > 0){
			// Se debe revizar esto porque es muy poco eficiente la solucion usada.
			--organismosDisponibles;
			organismoElecto=organismosDisponibles;
			pthread_mutex_unlock(&mutex_ObtenerOrganismo);

			stringstream ostram_organismo;
			ostram_organismo << poblacion->organisms.at(organismoElecto);
			string intermedio = ostram_organismo.str();
			char organismoArray[intermedio.size()];
			strcpy(organismoArray, intermedio.c_str());
			write(s,organismoArray,intermedio.size());




			// AHORA SE ESPERA POR EL FITNESS

			
	        
	        while(1){
	       		n = poll( &resepcionSocket ,1,10); 
	       		if(n>0){
	       			ioctl(resepcionSocket.fd, FIONREAD, &count);
	       			char buf[count];
	        		recv( resepcionSocket.fd, buf, count, 0);
	        		(poblacion->organisms.at(organismoElecto)).fitness=atof(buf);
	        		break;
	       		}
	       	}


		}
		else{
			pthread_mutex_lock(&mutex_cantidadHebrasListas);
			cantidadHebrasListas++;
			if(cantidadHebrasListas==hostClientes.size()){
				pthread_cond_signal(&TodasHebrasEstanListas);
			}
			pthread_mutex_unlock(&mutex_cantidadHebrasListas);

			pthread_cond_wait(&HayOrganismosNuevos, &mutex_ObtenerOrganismo);

			pthread_mutex_lock(&mutex_cantidadHebrasListas);
			cantidadHebrasListas--;
			pthread_mutex_unlock(&mutex_cantidadHebrasListas);
		}
		


	}


	pthread_exit(NULL);
}


void creacionNEAT(int argc, char ** argv){
	char ruta[]=".";
	poblacion = new Population(argv[2],argv[3], (char *) "NEAT_QUADATOT_MULTI" ,ruta);
	cerr << poblacion->GENERATIONS << "\t" << poblacion->POPULATION_MAX << "\t" << poblacion->organisms.size() << endl;

	organismosDisponibles=poblacion->POPULATION_MAX;
	

	pthread_mutex_lock(&sincronizacionInicial);
		if(neatIsReady == 1){
			pthread_cond_broadcast(&NEAT_OK);
		}
	pthread_mutex_unlock(&sincronizacionInicial);

	for (int i = 0; i < poblacion->GENERATIONS; ++i){
		pthread_cond_wait(&TodasHebrasEstanListas, &mutex_Inutil);
		poblacion->epoch();
		organismosDisponibles=poblacion->POPULATION_MAX;
		pthread_cond_broadcast(&HayOrganismosNuevos);
		poblacion->print_to_file_currrent_generation();

	}

	cout << "Fitness champion: " << poblacion->fitness_champion << "\n\n"<< endl;
	cout << poblacion->champion.ANN_function() << endl;
	cout << poblacion->champion << endl;
	
}