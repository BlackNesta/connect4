#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

extern int errno;
int port;

int main(int argc, char *argv[]) {

  int sd;
  struct sockaddr_in server;
  char msg[100];

  if (argc != 3) {
    printf("Sintax: %s <server_address> <port>\n", argv[0]);
    return -1;
  }

  port = atoi(argv[2]);

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket error.\n");
    return errno;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons(port);

  //  connect to server
  if (connect(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1) {
    perror("[client]Connect error\n");
    return errno;
  }

  if (read (sd, msg, 100) < 0) {
      perror("[client]Read error\n");
      return errno;
  }
  printf("[client]%s", msg);

  while(1) {
    bzero(msg, 100);
    fflush(stdout);

    if (read (sd, msg, 100) < 0) {
      perror("[client]Read error\n");
      return errno;
    }

    printf("[client]%s\n", msg);
    close (sd);
  }
}