#ifndef ERP_UDP_H
#define ERP_UDP_H

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
#include <poll.h>
#include <sys/types.h>  
#include <sys/stat.h> 
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#define TIME_OUT_POLL_MSEC 100


void procesamientoDatosDeEntrada();
long max(long t_1, long t_2);
void * entradaDatosDesdeCliente(void *null);
void * salidaDatosHaciaServidor(void *null);
void * entradaDatosDesdeServidor(void *null);
void * SalidaDatosHaciaCliente(void *null);
void exitOnCtrC(int signal);
void creacionSockets();
void creacionTubos();





#endif