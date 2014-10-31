#include "erp_udp.h"


// Se trabajara todo global por simplicidad.
int fileDescriptorSocketCliente;
int fileDescriptorsocketServidor;
int pipeBuferCliente[2];
int pipeBuferClienteTiempo[2];
int pipeBuferServidor[2];
int pipeBuferServidorTiempo[2];
int retardoPromedio;
int variacionRetardo;
int porcentajePerdida;
int localPort;
int remotePort;
char remoteHostname[64];
struct sockaddr_in datosComunicacionCliente;
struct sockaddr_in datosComunicacionRemoto;
struct sockaddr_in datosLlegadaDatosServidor;
int length;
struct sockaddr_in direccionCliente;


pthread_mutex_t  FIFOCliente = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  FIFOServidor = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t FIFOClienteCambiado = PTHREAD_COND_INITIALIZER;
pthread_cond_t FIFOServidorCambiado = PTHREAD_COND_INITIALIZER;


pthread_mutex_t  mutex_cliaddr = PTHREAD_MUTEX_INITIALIZER;

long max(long t_1, long t_2)
{
    return (t_1 > t_2) ? t_1 : t_2;
}



void * entradaDatosDesdeCliente(void *null) {
	struct timespec t_llegada;
	int n;
	char buffer[64*2014]; // Se le asigna el largo que puede llegar a tener un paquete UDP
	socklen_t len;
	len = sizeof( struct sockaddr_in );
	struct sockaddr_in direccionCliente_tmp;
	//while ((n = recv(fileDescriptorSocketCliente, buffer, sizeof(buffer), 0)) > 0){
	while(1){
		n = recvfrom(fileDescriptorSocketCliente,buffer,sizeof(buffer),0,(struct sockaddr *)&direccionCliente_tmp,&len);

		pthread_mutex_lock(&mutex_cliaddr);
		direccionCliente.sin_family = direccionCliente_tmp.sin_family; // No estoy seguro que esto funcione bien.
		direccionCliente.sin_port = direccionCliente_tmp.sin_port;
		direccionCliente.sin_addr.s_addr = direccionCliente_tmp.sin_addr.s_addr;
		memcpy(direccionCliente.sin_zero,direccionCliente_tmp.sin_zero,8);
		pthread_mutex_unlock(&mutex_cliaddr);
		
		clock_gettime(CLOCK_REALTIME, &t_llegada);
		pthread_mutex_lock(&FIFOCliente);
		write(pipeBuferCliente[1], buffer, n);
		write(pipeBuferClienteTiempo[1] ,&t_llegada, sizeof(struct timespec) );
		pthread_cond_signal(&FIFOClienteCambiado);
		pthread_mutex_unlock(&FIFOCliente);		
		bzero(buffer,sizeof(buffer));
		usleep(1);
	}
	pthread_exit(NULL);
}



void * salidaDatosHaciaServidor(void *null) {
	struct timespec t_reenvio;
	char buffer[64*1024]; // Se le asigna el largo que puede llegar a tener un paquete UDP
	struct pollfd fd_FIFOCliente_in;
	fd_FIFOCliente_in.fd = pipeBuferCliente[0];
	fd_FIFOCliente_in.events = POLLIN;
	int n;
	clock_gettime(CLOCK_REALTIME, &t_reenvio);
	while(1){
		pthread_mutex_lock(&FIFOCliente);
		n = poll( &fd_FIFOCliente_in ,1,TIME_OUT_POLL_MSEC);
		if(n>0){
			struct timespec tiempoLlegadaPaquete;
			n=read(fd_FIFOCliente_in.fd, buffer, sizeof(buffer));
			read(pipeBuferClienteTiempo[0], &tiempoLlegadaPaquete, sizeof(struct timespec));
			//printf("buffer: %s \ttiempo, sec:%ld  nsec%ld\n", buffer, tiempo.tv_sec , tiempo.tv_nsec);
			
			long t_ll, t_re, t1, t2, u_sec;
			t_ll = 1000000*tiempoLlegadaPaquete.tv_sec + tiempoLlegadaPaquete.tv_nsec/1000;
			t_re = 1000000*t_reenvio.tv_sec + t_reenvio.tv_nsec/1000;
			t1 = max(t_ll + retardoPromedio - variacionRetardo, t_re);
			t2 = t_ll + retardoPromedio + variacionRetardo;
			u_sec = rand()%(long)(t2-t1);
			usleep(u_sec);
			if((rand()%101) > porcentajePerdida) {
				sendto(fileDescriptorsocketServidor, buffer, n, 0, (struct sockaddr*) &datosComunicacionRemoto, length);
				bzero(buffer,sizeof(buffer));
			}
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
	socklen_t len;
	char mesg[64*1024];
	int n;
	struct timespec t_llegada;
	while(1){
		n = recvfrom(fileDescriptorsocketServidor,mesg,sizeof(mesg),0,(struct sockaddr *)&datosComunicacionRemoto,&len);
		clock_gettime(CLOCK_REALTIME, &t_llegada);
		pthread_mutex_lock(&FIFOServidor);
		write(pipeBuferServidor[1], mesg, n);
		write(pipeBuferServidorTiempo[1] ,&t_llegada, sizeof(struct timespec) );
		pthread_cond_signal(&FIFOServidorCambiado);
		pthread_mutex_unlock(&FIFOServidor);		
		bzero(mesg,sizeof(mesg));
	}
	pthread_exit(NULL);
} 

void * SalidaDatosHaciaCliente(void *null) {
	struct timespec t_reenvio;
	char buffer[64*1024]; // Se le asigna el largo que puede llegar a tener un paquete UDP
	struct pollfd fd_FIFOServidor_in;
	fd_FIFOServidor_in.fd = pipeBuferServidor[0];
	fd_FIFOServidor_in.events = POLLIN;
	int n;
	long t_ll, t_re, t1, t2, u_sec;
	clock_gettime(CLOCK_REALTIME, &t_reenvio);
	while(1){
		pthread_mutex_lock(&FIFOServidor);
		n = poll( &fd_FIFOServidor_in ,1,TIME_OUT_POLL_MSEC);
		if(n>0){
			struct timespec tiempoLlegadaPaquete;
			n=read(fd_FIFOServidor_in.fd, buffer, sizeof(buffer));
			read(pipeBuferServidorTiempo[0], &tiempoLlegadaPaquete, sizeof(struct timespec));
			t_ll = 1000000*tiempoLlegadaPaquete.tv_sec + tiempoLlegadaPaquete.tv_nsec/1000;
			t_re = 1000000*t_reenvio.tv_sec + t_reenvio.tv_nsec/1000;
			t1 = max(t_ll + retardoPromedio - variacionRetardo, t_re);
			t2 = t_ll + retardoPromedio + variacionRetardo;
			u_sec = rand()%(long)(t2-t1);
			usleep(u_sec);
			if((rand()%101) > porcentajePerdida) {
				pthread_mutex_lock(&mutex_cliaddr);
				sendto(fileDescriptorSocketCliente, buffer, n, 0, (struct sockaddr*) &direccionCliente, length);
				pthread_mutex_unlock(&mutex_cliaddr);
				bzero(buffer,sizeof(buffer));
			}
			clock_gettime(CLOCK_REALTIME, &t_reenvio);	
		}
		else if(n == 0){
			pthread_cond_wait(&FIFOServidorCambiado, &FIFOServidor);
		}
		else{
			printf("Error en comando Poll.\n");
			exit(1);
		}
		pthread_mutex_unlock(&FIFOServidor);
	}
	pthread_exit(NULL);
} 


void exitOnCtrC(int signal){
	close(fileDescriptorSocketCliente);
	close(fileDescriptorsocketServidor);
	close(pipeBuferCliente[0]);
	close(pipeBuferCliente[1]);
	close(pipeBuferClienteTiempo[0]);
	close(pipeBuferClienteTiempo[1]);
	close(pipeBuferServidor[0]);
	close(pipeBuferServidor[1]);
	close(pipeBuferServidorTiempo[0]);
	close(pipeBuferServidorTiempo[1]);
	printf("\nSaliendo del programa...\n");
	if(signal != 0) exit(1);
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
	error = bind(fileDescriptorsocketServidor, (struct sockaddr *) &datosLlegadaDatosServidor, length);
	if(  error != 0){
		printf("No se pudo asignar puerto\n");
		printf("Errno: %d\n", errno);
		if (errno == 98){
			printf("Error::Port is already in use\n");
		}
		exit(1);
	}
	pipe(pipeBuferCliente);
	pipe(pipeBuferClienteTiempo);
	pipe(pipeBuferServidor);
	pipe(pipeBuferServidorTiempo);
	pthread_t tid[4];
	pthread_create(&tid[0], NULL, entradaDatosDesdeCliente, NULL);
	pthread_create(&tid[1], NULL, salidaDatosHaciaServidor, NULL);
	pthread_create(&tid[2], NULL, entradaDatosDesdeServidor, NULL);
	pthread_create(&tid[3], NULL, SalidaDatosHaciaCliente, NULL);
	pause();
	exitOnCtrC(0);
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
	datosLlegadaDatosServidor.sin_family = AF_INET;
	datosLlegadaDatosServidor.sin_port = htons( strtol(argv[4],NULL,10) + 1 );
	datosLlegadaDatosServidor.sin_addr.s_addr=htonl(INADDR_ANY); // Recibe de cualquier entrada de red.
}