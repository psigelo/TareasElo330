#include "erp_udp.h"

/*
	erp_upd : 
		Emulador de Retardo y Pérdida de paquetes en transferencias UDP.
	Sintaxis: 
		erp_udp retardo_promedio variación_retardo porcentaje_pérdida puerto_local [host_remoto] puerto_remoto 	
	fecha:
		30/10/2014
	Autores: 
		Pascal Sigel
		Oscar Silva
*/



//  Las variables globales que son usadas algun par de hilos:
//====================================================================================================


	// Sockets:
		int fileDescriptorSocketCliente; // Es usado para la comunicacion con el cliente.
		int fileDescriptorsocketServidor; // Es usado para la comunicacion con el servidor

	// Tubos: (Hacen de FIFO) 
		int pipeBuferCliente[2]; // Entra la informacion que llega desde el cliente que posteriormente es llevada al servidor.
		int pipeBuferClienteTiempo[2]; // Envia la informacion en paralelo del tiempo correspondiente al paquete que esta en la cola del tubo pipeBuferCliente 
		int pipeBuferServidor[2]; // Entra la informacion que viene desde el servidor y que posteriormente sera llevada al cliente.
		int pipeBuferServidorTiempo[2]; // Envia la informacion en paralelo del tiempo correspondiente al paquete que esta en la cola del tubo pipeBuferServidor

	// Variables para el calculo del tiempo
		int retardoPromedio; 
		int variacionRetardo;  
		int porcentajePerdida; 

	// Les estructuras de direccion de sockets.
		struct sockaddr_in datosDondeRecibirAlCliente; // Es usado para recibir la informacion del cliente.
		struct sockaddr_in datosComunicacionRemoto; // Es usado para enviar la informacion al servidor
		struct sockaddr_in datosLlegadaDatosServidor; // Es usado para recibir la informacion del servidor
		struct sockaddr_in direccionCliente; // Es usado para enviar la informacion al servidor


	// Sincronizacion de hilos
		pthread_mutex_t  FIFOCliente 			= PTHREAD_MUTEX_INITIALIZER; // El tubo que almacena los mensajes del cliente es protegido para que no se lea o se escriba incorrectamente
		pthread_mutex_t  FIFOServidor 			= PTHREAD_MUTEX_INITIALIZER; // El tubo que almacena los mensajes del servidor es protegido para que no se lea o se escriba incorrectamente
		pthread_mutex_t  mutex_cliaddr 			= PTHREAD_MUTEX_INITIALIZER; // Dado que el cliente puede variar de forma aleatoria (por comodidad) entonces para que no se pueda corromper la direccion del cliente es protegida.
		pthread_cond_t   FIFOClienteCambiado 	= PTHREAD_COND_INITIALIZER ; // En caso que haya nueva informacion del cliente entonces es necesario hacer el calculo de su retardo en otro caso el hilo ."salidaDatosHaciaServidor" duerme.
		pthread_cond_t   FIFOServidorCambiado 	= PTHREAD_COND_INITIALIZER ; // En caso que haya nueva informacion del servidor entonces es necesario hacer el calculo de su retardo en otro caso el hilo ."salidaDatosHaciaServidor" duerme.


	int length; 
//====================================================================================================


int main(int argc, char ** argv){
	pthread_t tid[4];
	signal(SIGINT , exitOnCtrC ); // Para manejar la interrupcion control + c de tal manera que se cierre correctamente el programa.
	// Se procesa la informacion que el usuario ingresa por consola.
	procesamientoDatosDeEntrada(argc,argv);
	// Se crean los sockets que seran usados para la comunicacion con cliente y servidor.
	creacionSockets();
	// Se crean los tubos que hacen como buffers FIFO y los que proveen de la informacion temporal.
	creacionTubos();	
	// Se crean 4 hebras. 
	//==================================================================
	// hebra 1: Almacena datos que llegan desde el cliente
	pthread_create(&tid[0], NULL, entradaDatosDesdeCliente, NULL); 
	// hebra 2: Distribuye datos al servidor simulando retardo y perdida
	pthread_create(&tid[1], NULL, salidaDatosHaciaServidor, NULL);
	// hebra 3: Almacena datos que llegan desde el servidor
	pthread_create(&tid[2], NULL, entradaDatosDesdeServidor, NULL);
	// hebra 4: Distribuye datos al cliente simulando retardo y perdida
	pthread_create(&tid[3], NULL, SalidaDatosHaciaCliente, NULL);
	//==================================================================
	pause();// la hebra inicial se envia a dormir.
	// En cualquier caso que despierte es por algun error entonces se debe cerrar correspondientemente los recursos usados.
	exitOnCtrC(0); // si se envia 0 entonces no ejecuta el exit dentro de la funcion.
	return 0;
}




void procesamientoDatosDeEntrada(int argc, char ** argv){
	if(argc == 6){
		// Se cargan los datos del usuario
		retardoPromedio   = atoi(argv[1]);
		variacionRetardo  = atoi(argv[2]);
		porcentajePerdida = atoi(argv[3]);

		//Se configura el socket del cliente
		datosDondeRecibirAlCliente.sin_family = AF_INET;
		datosDondeRecibirAlCliente.sin_port = htons( strtol(argv[4],NULL,10) );
		datosDondeRecibirAlCliente.sin_addr.s_addr = htonl(INADDR_ANY); // Recibe de cualquier entrada de red.

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
		datosDondeRecibirAlCliente.sin_family = AF_INET;
		datosDondeRecibirAlCliente.sin_port = htons( atoi(argv[4]) );
		datosDondeRecibirAlCliente.sin_addr.s_addr=htonl(INADDR_ANY); // Recibe de cualquier entrada de red.

		//Se configura el socket del servidor
		datosComunicacionRemoto.sin_family = AF_INET;
		datosComunicacionRemoto.sin_port = htons( atoi(argv[6]) );
		datosComunicacionRemoto.sin_addr.s_addr = inet_addr( argv[5] ); 
	}
	else{
		printf("No se ingresaron correctamente los argumontos.\nEl metodo correcto de ejecucion es %s variacion_retardo porcentaje_perdida puerto_local [host_remoto] puerto_remoto\n", argv[0]);
		exit(1);
	}

	// Se configura el socket por el cual se recibira la informacion del servidor.
	datosLlegadaDatosServidor.sin_family = AF_INET;
	datosLlegadaDatosServidor.sin_port = htons( strtol(argv[4],NULL,10) + 1 );
	datosLlegadaDatosServidor.sin_addr.s_addr=htonl(INADDR_ANY); // Recibe de cualquier entrada de red.
}

// max: calcula el maximo entre dos long
long max(long t_1, long t_2)
{
    return (t_1 > t_2) ? t_1 : t_2;
}

 

/*
	entradaDatosDesdeCliente:
		Recibe los datos provenientes del cliente y los almacena el el tubo pipeBuferCliente, además en el tubo pipeBuferClienteTiempo
		almacena el tiempo en el cual fue recibido.

		Si el cliente llegase a cambiar entonces el programa es capas de darse cuenta lo cual significa que la direccion 
		del cliente se modifica en este hilo y se lee en el hilo SalidaDatosHaciaCliente por lo  que hay que usar mutex.

*/
void * entradaDatosDesdeCliente(void *null) {
	struct timespec t_llegada;
	int n;
	char buffer[64*2014]; // Se le asigna el largo que puede llegar a tener un paquete UDP
	socklen_t len;
	len = sizeof( struct sockaddr_in );
	struct sockaddr_in direccionCliente_tmp;


	while(1){
		// Se recibe la informacion del cliente
		n = recvfrom(fileDescriptorSocketCliente,buffer,sizeof(buffer),0,(struct sockaddr *)&direccionCliente_tmp,&len);

		// Se almacena la direccion del cliente para su posterior uso en el envio de informacion hacia el cliente.
		pthread_mutex_lock(&mutex_cliaddr);
		// Se copia la informacion temporal del cliente en la informacion global del cliente:
		direccionCliente.sin_family = direccionCliente_tmp.sin_family; // No estoy seguro que esto funcione bien.
		direccionCliente.sin_port = direccionCliente_tmp.sin_port;
		direccionCliente.sin_addr.s_addr = direccionCliente_tmp.sin_addr.s_addr;
		memcpy(direccionCliente.sin_zero,direccionCliente_tmp.sin_zero,8);
		pthread_mutex_unlock(&mutex_cliaddr);
		
		// Se obtiene el tiempo de llegada del paquete UDP.
		clock_gettime(CLOCK_REALTIME, &t_llegada);

		pthread_mutex_lock(&FIFOCliente); // Se bloquea el tubo para no corromper informacion
		// Se envia el paquete por el tubo (que simula un buffer FIFO) 
		write(pipeBuferCliente[1], buffer, n);
		// Se envia el tiempo del paquete por otro tubo.
		write(pipeBuferClienteTiempo[1] ,&t_llegada, sizeof(struct timespec) );

		pthread_cond_signal(&FIFOClienteCambiado); // Se avisa que el tubo ha sido modificado para que el hilo salidaDatosHaciaServidor despierte (si estuvo durmiendo por no haber datos).
		pthread_mutex_unlock(&FIFOCliente);	// Se desbloquea el tubo.

		bzero(buffer,sizeof(buffer));
		usleep(1);
	}
	pthread_exit(NULL);
}

/*
	entradaDatosDesdeServidor:
		Recibe los datos provenientes del servidor y los almacena el el tubo pipeBuferServidor, además en el tubo pipeBuferServidorTiempo
		almacena el tiempo en el cual fue recibido.

		Si el servidor llegase a cambiar entonces el programa es capas de darse cuenta lo cual significa que la direccion 
		del servidor se modifica en este hilo y se lee en el hilo SalidaDatosHaciaCliente por lo  que hay que usar mutex.

*/
void * entradaDatosDesdeServidor(void *null) {
	socklen_t len;
	char mesg[64*1024];
	int n;
	struct timespec t_llegada;
	while(1){
		// Se recibe la informacion del servidor
		n = recvfrom(fileDescriptorsocketServidor,mesg,sizeof(mesg),0,(struct sockaddr *)&datosComunicacionRemoto,&len);
		// Se obtiene el tiempo de llegada del paquete UDP.
		clock_gettime(CLOCK_REALTIME, &t_llegada);
		pthread_mutex_lock(&FIFOServidor);// Se bloquea el tubo para no corromper informacion
		// Se envia el paquete por el tubo (que simula un buffer FIFO) 
		write(pipeBuferServidor[1], mesg, n);
		// Se envia el tiempo del paquete por otro tubo.
		write(pipeBuferServidorTiempo[1] ,&t_llegada, sizeof(struct timespec) );
		pthread_cond_signal(&FIFOServidorCambiado); // Se avisa que el tubo ha sido modificado para que el hilo salidaDatosHaciaCliente despierte (si estuvo durmiendo por no haber datos)
		pthread_mutex_unlock(&FIFOServidor);		
		bzero(mesg,sizeof(mesg));
	}
	pthread_exit(NULL);
} 


/*
	salidaDatosHaciaServidor:
		Se leen los datos del tubo pipeBuferCliente y su respectivo tiempo del tubo pipeBuferClienteTiempo con los que 
		se calcula el retardo y su posible perdida de datos, luego son enviados al servidor. 

		En caso de que no hayan datos que envie el cliente entonces este hilo se va a dormir.
*/

void * salidaDatosHaciaServidor(void *null) {
	struct timespec t_reenvio;
	char buffer[64*1024]; // Se le asigna el largo que puede llegar a tener un paquete UDP
	long t_ll, t_re, t1, t2, u_sec;

	// La siguiente estructura busca que se lean los datos sólo si existen.
	// En caso de que no existan datos entonces el hilo se va a dormir, buscando lograr eficiencia de los recursos.
	// ==========================================
	struct pollfd fd_FIFOCliente_in;
	fd_FIFOCliente_in.fd = pipeBuferCliente[0];
	fd_FIFOCliente_in.events = POLLIN; 
	//===========================================
	int n;
	clock_gettime(CLOCK_REALTIME, &t_reenvio);

	while(1){
		pthread_mutex_lock(&FIFOCliente);
		n = poll( &fd_FIFOCliente_in ,1,TIME_OUT_POLL_MSEC); // Si el descriptor de archivos "pipeBuferCliente[0]" tiene informacion nueva entonces n > 0, en otro caso n = 0 y en caso de error n < 0.
		if(n>0){ // CASO EN QUE HAY NUEVA INFORMACION EN EL TUBO, O SEA LLEGO UN PAQUETE NUEVO.
			struct timespec tiempoLlegadaPaquete;
			// Se lee el paquete
			n=read(fd_FIFOCliente_in.fd, buffer, sizeof(buffer));
			// Se lee el tiempo en que llego el paquete.
			read(pipeBuferClienteTiempo[0], &tiempoLlegadaPaquete, sizeof(struct timespec));

			// SE CALCULA EL RETRASO Y SE DUERME EL TIEMPO CORRESPONDIENTE.
			//=========================================================================================================================
			t_ll = 1000000*tiempoLlegadaPaquete.tv_sec + tiempoLlegadaPaquete.tv_nsec/1000; // tiempo de llegada en una variable long
			t_re = 1000000*t_reenvio.tv_sec + t_reenvio.tv_nsec/1000; // tiempo de re-envio en una variable long
			t1 = max(t_ll + retardoPromedio - variacionRetardo, t_re);
			t2 = t_ll + retardoPromedio + variacionRetardo;
			u_sec = rand()%(long)(t2-t1);
			usleep(u_sec);
			//=========================================================================================================================

			// PUEDE SER QUE EL PAQUETE SE PIERDA
			if((rand()%101) > porcentajePerdida) { // EN CASO QUE NO SE PIERDA SE ENVIA
				sendto(fileDescriptorsocketServidor, buffer, n, 0, (struct sockaddr*) &datosComunicacionRemoto, length);
				bzero(buffer,sizeof(buffer));
			}
			clock_gettime(CLOCK_REALTIME, &t_reenvio);	
		}
		else if(n == 0){ // EN CASO QUE NO HAYA NUEVA INFORMACION DEL CLIENTE ENTONCES EL HILO SE VA A DORMIR HASTA QUE HAYA NUEVA INFORMACION
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


/*
	salidaDatosHaciaCliente:
		Se leen los datos del tubo pipeBuferServidor y su respectivo tiempo del tubo pipeBuferServidorTiempo con los que 
		se calcula el retardo y su posible perdida de datos, luego son enviados al servidor. 

		En caso de que no hayan datos que envie el servidor entonces este hilo se va a dormir.
*/
void * SalidaDatosHaciaCliente(void *null) {
	struct timespec t_reenvio;
	char buffer[64*1024]; // Se le asigna el largo que puede llegar a tener un paquete UDP

	// La siguiente estructura busca que se lean los datos sólo si existen.
	// En caso de que no existan datos entonces el hilo se va a dormir, buscando lograr eficiencia de los recursos.
	// ==========================================
	struct pollfd fd_FIFOServidor_in;
	fd_FIFOServidor_in.fd = pipeBuferServidor[0];
	fd_FIFOServidor_in.events = POLLIN;
	//===========================================

	int n;
	long t_ll, t_re, t1, t2, u_sec;
	clock_gettime(CLOCK_REALTIME, &t_reenvio);
	while(1){
		pthread_mutex_lock(&FIFOServidor);
		n = poll( &fd_FIFOServidor_in ,1,TIME_OUT_POLL_MSEC);// Si el descriptor de archivos "pipeBuferServidor[0]" tiene informacion nueva entonces n > 0, en otro caso n = 0 y en caso de error n < 0.
		if(n>0){// CASO EN QUE HAY NUEVA INFORMACION EN EL TUBO, O SEA LLEGO UN PAQUETE NUEVO.
			struct timespec tiempoLlegadaPaquete;
			// Se lee el paquete
			n=read(fd_FIFOServidor_in.fd, buffer, sizeof(buffer));
			// Se lee el tiempo en que llego el paquete.
			read(pipeBuferServidorTiempo[0], &tiempoLlegadaPaquete, sizeof(struct timespec));

			// SE CALCULA EL RETRASO Y SE DUERME EL TIEMPO CORRESPONDIENTE.
			//=========================================================================================================================
			t_ll = 1000000*tiempoLlegadaPaquete.tv_sec + tiempoLlegadaPaquete.tv_nsec/1000;
			t_re = 1000000*t_reenvio.tv_sec + t_reenvio.tv_nsec/1000;
			t1 = max(t_ll + retardoPromedio - variacionRetardo, t_re);
			t2 = t_ll + retardoPromedio + variacionRetardo;
			u_sec = rand()%(long)(t2-t1);
			usleep(u_sec);
			//=========================================================================================================================
			// PUEDE SER QUE EL PAQUETE SE PIERDA
			if((rand()%101) > porcentajePerdida) {// EN CASO QUE NO SE PIERDA SE ENVIA
				pthread_mutex_lock(&mutex_cliaddr);
				sendto(fileDescriptorSocketCliente, buffer, n, 0, (struct sockaddr*) &direccionCliente, length);
				pthread_mutex_unlock(&mutex_cliaddr);
				bzero(buffer,sizeof(buffer));
			}
			clock_gettime(CLOCK_REALTIME, &t_reenvio);	
		}
		else if(n == 0){// EN CASO QUE NO HAYA NUEVA INFORMACION DEL CLIENTE ENTONCES EL HILO SE VA A DORMIR HASTA QUE HAYA NUEVA INFORMACION
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

// en caso de que el usuario aprete cntrol + c se entiende que desea abortar el programa y se liberan los recursos usados.
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


/*
	creacionSockets:
		Se crean y configuran los sockets que se usaran para la comunicacion con el cliente y con el servidor
*/
void creacionSockets(){
	fileDescriptorSocketCliente  = socket(AF_INET, SOCK_DGRAM, 0); // Desde este socket se recibira y enviara la informacion con el cliente
	fileDescriptorsocketServidor = socket(AF_INET, SOCK_DGRAM, 0); // Desde este socket se recibira y enviara la informacion con el servidor
	length = sizeof(struct sockaddr_in);
	int error = bind(fileDescriptorSocketCliente, (struct sockaddr *) &datosDondeRecibirAlCliente, length); // se asegura un puerto para la comunicacion con el cliente.
	if(  error != 0){
		printf("No se pudo asignar puerto\n");
		printf("Errno: %d\n", errno);
		if (errno == 98){
			printf("Error::Port is already in use\n");
		}
		exit(1);
	}
	error = bind(fileDescriptorsocketServidor, (struct sockaddr *) &datosLlegadaDatosServidor, length); // se asegura un puerto para la comunicacion con el servidor.
	if(  error != 0){
		printf("No se pudo asignar puerto\n");
		printf("Errno: %d\n", errno);
		if (errno == 98){
			printf("Error::Port is already in use\n");
		}
		exit(1);
	}
}

/*
	creacionTubos:
		Se crean los tubos que pueden ser usados como buffer FIFO.
		además se crean unos tubos que son usados para informar el tiempo de llegada de los paquetes correspondientes.
*/
void creacionTubos(){
	pipe(pipeBuferCliente);
	pipe(pipeBuferClienteTiempo);
	pipe(pipeBuferServidor);
	pipe(pipeBuferServidorTiempo);
}