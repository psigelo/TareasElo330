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

void procesamientoDatosDeEntrada();



#define FIFO_1 "fifo"
#define FIFO_2 "fifo_2"
#define TIME_OUT_POLL_MSEC 100

#endif