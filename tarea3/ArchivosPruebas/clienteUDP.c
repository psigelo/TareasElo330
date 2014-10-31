#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>

#define PORTNUMBER  56789

int main(int argc, char * argv[])  {
    int n, s, len;
    socklen_t len2;
    char buf[1024];
    char hostname[64];
    struct hostent *hp;
    struct sockaddr_in name;
    struct sockaddr_in datosSocket;
    struct sockaddr_in cliaddr;
    char mesg [1024*64];

    switch(argc) {
        case 1:
          gethostname(hostname, sizeof(hostname));
          break;
        case 2:
           
          strcpy(hostname, argv[1]);
          break;
        default:
          printf("Usage: %s [server_name]\n", argv[0]);
          exit(-1);
    }
    
        
    /* Look up our host's network address. */
    hp = gethostbyname(hostname);

    /* Create a socket in the INET domain.  */
    s = socket(AF_INET, SOCK_DGRAM, 0);

    /* Create the address of the server. */
    name.sin_family = AF_INET;
    name.sin_port = htons(PORTNUMBER);
    memcpy(&name.sin_addr, hp->h_addr_list[0], hp->h_length);
    len = sizeof(struct sockaddr_in);
    len2 = sizeof(struct sockaddr_in);


    datosSocket.sin_family = AF_INET;
    datosSocket.sin_port = htons( 34567 );
    datosSocket.sin_addr.s_addr=htonl(INADDR_ANY); // Recibe de cualquier entrada de red.

    int error = bind(s, (struct sockaddr *) &datosSocket, len2);
    if(  error != 0){
        printf("No se pudo asignar puerto\n");
        printf("Errno: %d\n", errno);
        if (errno == 98){
            printf("Error::Port is already in use\n");
        }
        exit(1);
    }


    /* Connect to the server. Optional to tell transport layer that
     * we will use this socket to send UDP data to that
     * destination, so we don't need to provide the destination each time. */
 /*    connect(s, (struct sockaddr *) &name, len); */

    /* Read from standard input, and copy the
     * data to the socket.  */

    struct pollfd ClientePoll;
    ClientePoll.fd = 0;
    ClientePoll.events = POLLIN;


    struct pollfd recepcionPoll;
    recepcionPoll.fd = s;
    recepcionPoll.events = POLLIN;

    while (1) {
        n = poll( &ClientePoll,1,100);
        if(n>0){
            printf("1\n");
            if((n = read(0, buf, sizeof(buf))) > 0){
>                sendto(s, buf, n, 0, (struct sockaddr*) &name, len);
            }
        }
        else if(n==0){
            //printf("2\n");
            n = poll(&recepcionPoll,1,100);
            if(n>0){
                printf("3\n");
                n = recvfrom(s,mesg,sizeof(mesg),0,(struct sockaddr *)&cliaddr, &len2);
                printf("\n-----------------------------------------------------\nSe ha recibido: %s\n",mesg);
            }
        }
    }
      /* We could use just send or write if we have connected
       * the socket before  */
/*       send(s, buf, n, 0);    */
    
      /* send an empty message to close the server*/
    sendto(s, buf, 0, 0, (struct sockaddr*) &name, len);
    close(s);
    exit(0);
}
