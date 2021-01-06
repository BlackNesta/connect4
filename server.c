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
#include <stdbool.h>

#define MAX_CLIENTS 200
#define BUFFER_SIZE 4096
#define PORT 8089

extern int errno;

typedef struct thData
{
  int id;
  int player1;
  int player2;
}thData;

static void *Treat(void *); // functia executata de fiecare thread ce realizeaza comunicarea cu clientii
void Raspunde(void *);

bool playing[200];
int clients[200];
int waitingQ = 0;

int main(int argc, char *argv[]) {
  struct sockaddr_in server; // server structure
  struct sockaddr_in from;
  char ans[100] = "";
  int sd; // socket descriptor
  int pid;
  pthread_t th[100]; // Identificatorii thread-urilor care se vor crea
  int client;
  thData * td;
  int length = sizeof(from);
  int threads = 0;
  int i;
  
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
    
    printf("[server]Waiting on %d PORT...\n", PORT);
    fflush(stdout);

    //  accept client
    client = accept(sd, (struct sockaddr *) &from, &length);
    if (client < 0) {
      perror("[server]Client accept error\n");
      return errno;
    }
    else {
      for (i = 0; i < 200 && clients[i] != 0; i++);
      clients[i] = client;
      waitingQ++;
    }
    if (waitingQ == 1) {
      td = (struct thData*)malloc(sizeof(struct thData));	
      td->id = threads++;
      for (i = 0; i < 200 && playing[i] != 0; i++);
      td->player1 = clients[i];
      playing[i] = 1;
      bzero(ans, 100);
      strcat(ans, "You are Player1");
      if (write (clients[i], ans, 100) <= 0)
      {
        perror ("[server]Eroare la write() catre client.\n");
        continue;		/* continuam sa ascultam */
      }
    }
    else if (waitingQ == 2) {
      for (i = 0; i < 200 && playing[i] != 0; i++);
      td->player2 = clients[i];
      playing[i] = 1;
      strcat(ans, "You are Player2");
      if (write (clients[i], ans, 100) <= 0)
      {
        perror ("[server]Eroare la write() catre client.\n");
        continue;		/* continuam sa ascultam */
      }
      waitingQ = 0;
      pthread_create(&th[threads], NULL, &Treat, td);
    }
  }
}


static void *Treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);	
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		Raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
};


void Raspunde(void *arg)
{
  int nr, i = 0;
	struct thData tdL; 
	tdL= *((struct thData*)arg);
	nr++;      
		      
  if (write (tdL.player1, "You are first to move", 100) <= 0)
  {
    printf("[Thread %d] ",tdL.id);
    perror ("[Thread]Eroare la write() catre client.\n");
  }
	else
		printf ("[Thread %d]Mesajul a fost trasmis cu succes catre player1.\n",tdL.id);

  if (write (tdL.player2, "You are second to move", 100) <= 0)
  {
    printf("[Thread %d] ",tdL.id);
    perror ("[Thread]Eroare la write() catre client.\n");
  }
	else
		printf ("[Thread %d]Mesajul a fost trasmis cu succes catre player2.\n",tdL.id);	

}