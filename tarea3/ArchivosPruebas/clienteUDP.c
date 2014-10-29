#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

#define PORTNUMBER  56789

int main(int argc, char * argv[])  {
    int n, s, len;
    char buf[1024];
    char hostname[64];
    struct hostent *hp;
    struct sockaddr_in name;

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

    /* Connect to the server. Optional to tell transport layer that
     * we will use this socket to send UDP data to that
     * destination, so we don't need to provide the destination each time. */
 /*    connect(s, (struct sockaddr *) &name, len); */

    /* Read from standard input, and copy the
     * data to the socket.  */
    while ((n = read(0, buf, sizeof(buf))) > 0) 
      sendto(s, buf, n, 0, (struct sockaddr*) &name, len);
      /* We could use just send or write if we have connected
       * the socket before  */
/*       send(s, buf, n, 0);    */
    
      /* send an empty message to close the server*/
    sendto(s, buf, 0, 0, (struct sockaddr*) &name, len);
    close(s);
    exit(0);
}
