#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define MAX_CLIENTS 20
#define BUFFER_SIZE 4096
#define PORT 8081

extern int errno;

int main(int argc, char *argv[]) {
  struct sockaddr_in server; // server structure
  struct sockaddr_in from;
  char msg[100];
  char ans[100] = "";
  int sd; // socket descriptor
  
  // create socket
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("[server]Socket error\n");
    return errno;
  }

  bzero(&server, sizeof(server));
  bzero(&from, sizeof(from));
  
  // socket family
  server.sin_family = AF_INET;

  // accept any address
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  // comunicasion port
  server.sin_port = htons(PORT);

  //bind socket
  if (bind (sd, (struct sockaddr *) &server, sizeof(struct  sockaddr)) == -1) {
    perror("[server]Bind error\n");
    return errno;
  }

  if (listen(sd, 100) == -1) {
    perror("[server]Listen error\n");
    return errno;
  }

  while (1) {
    int client;
    int length = sizeof(from);
    
    printf("[server]Waiting on %d PORT...\n", PORT);
    fflush(stdout);

    //  accept client
    client = accept(sd, (struct sockaddr *) &from, &length);
    if (client < 0) {
      perror("[server]Client accept error\n");
      return errno;
    }

    /* 
      * connection succesfull
    */
    bzero(msg, 100);
    printf("[server]Waiting message...\n");
    fflush(stdout);

    //  reading message
    if (read(client, msg, 100) <= 0) {
      perror("[server]Read error from client.\n");
      close(client);
      continue;
    }
    printf("[server]Message recived...\n");

    bzero(ans, 100);
    strcat(ans, "Hello ");
    strcat(ans, msg);
    printf("[server]Sending answer...\n");

    //  sending answer
    if (write(client, ans, 100) <= 0) {
      perror("[server]Write error.\n");
      continue;
    }
    else {
      printf("[server]Answer send succesfully...\n");
      close(client);
    }

  }

}