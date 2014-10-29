#include "erp_udp.h"

// Se trabajara todo global por simplicidad.
int fileDescriptorSocketCliente;
int fileDescriptorsocketServidor;

int pipeBuferCliente[2];

int retardoPromedio;
int variacionRetardo;
int porcentajePerdida;
int localPort;
int remotePort;
char remoteHostname[64];
struct sockaddr_in datosComunicacionCliente;
struct sockaddr_in datosComunicacionRemoto;
int length;



pthread_mutex_t  FIFOCliente = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t FIFOClienteCambiado = PTHREAD_COND_INITIALIZER;


struct timespec t_llegada, t_reenvio;

long max(long t_1, long t_2)
{
    long m = (t_1 > t_2) ? t_1 : t_2;
    return m;
}

void * entradaDatosDesdeCliente(void *null) {
	int n;
	char buffer[50];
	while ((n = recv(fileDescriptorSocketCliente, buffer, 1, 0)) > 0){

		clock_gettime(CLOCK_REALTIME, &t_llegada);

		pthread_mutex_lock(&FIFOCliente);
        write(pipeBuferCliente[1], buffer, n);
        pthread_cond_signal(&FIFOClienteCambiado);
        pthread_mutex_unlock(&FIFOCliente);
        bzero(buffer,50);
	}
	pthread_exit(NULL);
} 

void * salidaDatosHaciaServidor(void *null) {

	char buffer[50];
	struct pollfd fd_FIFOCliente_in;
	fd_FIFOCliente_in.fd = pipeBuferCliente[0];
	fd_FIFOCliente_in.events = POLLIN;

	int n;

	clock_gettime(CLOCK_REALTIME, &t_reenvio);

	while(1){
		pthread_mutex_lock(&FIFOCliente);
		n = poll( &fd_FIFOCliente_in ,1,TIME_OUT_POLL_MSEC);
		if(n>0){
			n = read(fd_FIFOCliente_in.fd, buffer, 1);




			
			// AQUI VA EL TEMA DEL RETARDO.
			// A modo de prueba se muestra por pantalla

			long t_ll, t_re, t1, t2, u_sec;
			t_ll = 1000000*t_llegada.tv_sec + t_llegada.tv_nsec/1000;
	        t_re = 1000000*t_reenvio.tv_sec + t_reenvio.tv_nsec/1000;
	        t1 = max(t_ll + retardoPromedio - variacionRetardo, t_re);
	        t2 = t_ll + retardoPromedio + variacionRetardo;
	        u_sec = rand()%(long)(t2-t1);
	        printf("t1: %f\t t2: %f\t tiempo[s]: %f\n",(double)(t1/1000000.0),(double)(t2/1000000.0),(double)((t2-t1)/1000000.0));
	        usleep(u_sec);
	        //<<<<<<<<<if((rand()%101) < porcentajePerdida) continue;
				
			
			// HACER SOCKET PARA COMUNICACION CON SERVIDOR
			sendto(fileDescriptorsocketServidor, buffer, 1, 0, (struct sockaddr*) &datosComunicacionRemoto, length);


			clock_gettime(CLOCK_REALTIME, &t_reenvio);



		}

		else if(n == 0){
			pthread_cond_wait(&FIFOClienteCambiado, &FIFOCliente);
		}

		else{
			printf("Error en comando Poll.\n");
			exit(1);
		}

		pthread_mutex_unlock(&FIFOCliente);
	}

	pthread_exit(NULL);
} 

void * entradaDatosDesdeServidor(void *null) {

	pthread_exit(NULL);
} 

void * SalidaDatosHaciaCliente(void *null) {

	pthread_exit(NULL);
} 


void exitOnCtrC(int signal){
	close(fileDescriptorSocketCliente);
	close(fileDescriptorsocketServidor);
	close(pipeBuferCliente[0]);
	close(pipeBuferCliente[1]);
	printf("Saliendo del programa\n");
	exit(1);
}


int main(int argc, char ** argv){
	signal(SIGINT , exitOnCtrC );
	
	procesamientoDatosDeEntrada(argc,argv);

	fileDescriptorSocketCliente  = socket(AF_INET, SOCK_DGRAM, 0);
	fileDescriptorsocketServidor = socket(AF_INET, SOCK_DGRAM, 0);

	length = sizeof(struct sockaddr_in);
	int error = bind(fileDescriptorSocketCliente, (struct sockaddr *) &datosComunicacionCliente, length);
	if(  error != 0){
		printf("No se pudo asignar puerto\n");
		printf("Errno: %d\n", errno);
		if (errno == 98){
			printf("Error::Port is already in use\n");
		}
		exit(1);
	}
	
	
	
	pipe(pipeBuferCliente);

	pthread_t tid;
	pthread_create(&tid, NULL, entradaDatosDesdeCliente, NULL);
	pthread_create(&tid, NULL, salidaDatosHaciaServidor, NULL);


	pause();

	close(fileDescriptorSocketCliente);
	close(fileDescriptorsocketServidor);
	return 0;
}




void procesamientoDatosDeEntrada(int argc, char ** argv){
	if(argc == 6){
		// Se cargan los datos del usuario
		retardoPromedio   = atoi(argv[1]);
		variacionRetardo  = atoi(argv[2]);
		porcentajePerdida = atoi(argv[3]);

		//Se configura el socket del cliente
		datosComunicacionCliente.sin_family = AF_INET;
		datosComunicacionCliente.sin_port = htons( strtol(argv[4],NULL,10) );
		datosComunicacionCliente.sin_addr.s_addr = htonl(INADDR_ANY); // Recibe de cualquier entrada de red.
		//Se configura el socket del servidor
		datosComunicacionRemoto.sin_family = AF_INET;
		datosComunicacionRemoto.sin_port = htons( strtol(argv[5],NULL,10) );
		datosComunicacionRemoto.sin_addr.s_addr=inet_addr("127.0.0.1"); // Envía datos a la misma maquina.
	}
	else if(argc==7){
		// Se cargan los datos del usuario
		retardoPromedio   = atof(argv[1]);
		variacionRetardo  = atof(argv[2]);
		porcentajePerdida = atof(argv[3]);

		//Se configura el socket del cliente
		datosComunicacionCliente.sin_family = AF_INET;
		datosComunicacionCliente.sin_port = htons( atoi(argv[4]) );
		datosComunicacionCliente.sin_addr.s_addr=htonl(INADDR_ANY); // Recibe de cualquier entrada de red.

		//Se configura el socket del servidor
		datosComunicacionCliente.sin_family = AF_INET;
		datosComunicacionCliente.sin_port = htons( atoi(argv[6]) );
		datosComunicacionCliente.sin_addr.s_addr = inet_addr( argv[5] ); 
	}
	else{
		printf("No se ingresaron correctamente los argumontos.\nEl metodo correcto de ejecucion es %s variacion_retardo porcentaje_perdida puerto_local [host_remoto] puerto_remoto\n", argv[0]);
		exit(1);
	}
}