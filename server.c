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
#define PORT 8080

extern int errno;

typedef struct thData {
  int id;
  int player[2];
} thData;

static void *Treat(void *); // functia executata de fiecare thread ce realizeaza comunicarea cu clientii
static void initializareTabla(int board[6][7]);
bool validareInput(char *input);
bool mutareValida(int board[6][7], int coloana);
void PlayGame(void *);
void adaugaPiesa(int board[6][7], int coloana, int jucator);
int JocTerminat(int board[6][7], int player);


bool playing[200];
int clients[200];
int waitingQ = 0;

int main(int argc, char *argv[]) 
{
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
      td->player[0] = clients[i];
      playing[i] = 1;
      bzero(&ans, 100);
      strcat(ans, "You are Player1\n");
      if (write (clients[i], ans, 100) <= 0)
      {
        perror ("[server]Eroare la write() catre client.\n");
        continue;		
      }
    }
    else if (waitingQ == 2) {
      for (i = 0; i < 200 && playing[i] != 0; i++);
      td->player[1] = clients[i];
      playing[i] = 1;
      bzero(&ans, 100);
      strcat(ans, "You are Player2\n");
      if (write (clients[i], ans, 100) <= 0)
      {
        perror ("[server]Eroare la write() catre client.\n");
        continue;		
      }
      waitingQ = 0;
      sleep(1);
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
		PlayGame((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
};

static void initializareTabla(int board[6][7]) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            board[i][j] = 0;
        }
    }
}

bool validareInput(char *input) {
    if (strcmp(input, "0") > 0 && strcmp(input, "8") < 0) {
        return true;
    } else {
        return false;
    }
}

bool mutareValida(int board[6][7], int coloana) {
    if (board[0][coloana - 1] == 0) {
        return true;
    }
    return false;
}

void adaugaPiesa(int board[6][7], int coloana, int jucator) {
    int linie = 5;
    while (board[linie][coloana - 1] != 0) {
        linie--;
    }
    board[linie][coloana - 1] = jucator;
}

int JocTerminat(int board[6][7], int player) {
    // verific daca unul din jucatori a castigat
    //verific pe linie
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 4; j++) 
        if (board[i][j] == player) {
          int row = 1;
          for (int k = 1; k <= 3; k++) 
            if (board[i][j + k] == player)
              row++;
          if (row == 4) return 1;
        }
    // verific pe coloana
    for (int j = 0; j < 7; j++)
      for (int i = 0; i < 3; i++)
        if (board[i][j] == player) {
          int col = 1;
          for (int k = 1; k <= 3; k++) 
            if (board[i + k][j] == player)
              col++;
          if (col == 4) return 1;
        }
    // verific diagonala principala
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++)
            if (board[i][j] == player) {
              int diag = 1;
              for (int k = 1; k <= 3; k++)
                if (board[i + k][j + k] == player)
                  diag++;
              if (diag == 4) return 1;
            }
    // verific diagonala secundara
    for (int i = 3; i < 6; i++)
        for (int j = 0; j < 4; j++)
            if (board[i][j] == player) {
              int diag = 1;
              for (int k = 1; k <= 3; k++)
                if (board[i - k][j + k] == player)
                  diag++;
              if (diag == 4) return 1;
            }
    //  verific daca se mai pot adauga piese
    for (int i = 0; i < 7; i++) {
        if (board[0][i] == 0) return 0;
    }
    // egalitate
    return 2;
}

void PlayGame(void *arg)
{
  struct thData tdL; 
  tdL = *((struct thData*)arg);  
  if (write (tdL.player[0], "You are first to move\n", 100) <= 0) {
    printf("[Thread %d] ",tdL.id);
    perror ("[Thread]Eroare la write() catre client.\n");
  }
  if (write (tdL.player[1], "You are second to move\n", 100) <= 0) {
    printf("[Thread %d] ",tdL.id);
    perror ("[Thread]Eroare la write() catre client.\n");
  }
  sleep(1);
  int player = 0;
  int board[6][7];
  char input[100];
  initializareTabla(&board);
  while (1) {
    //  send table to players
    if (write (tdL.player[player], board, sizeof(board)) <= 0)
    {
      printf("[Thread %d] ",tdL.id);
      perror ("[Thread]Eroare la write() catre client.\n");
      if (write (tdL.player[1 - player], board, sizeof(board)) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
      }
      if (write (tdL.player[1 - player], "Winer!\n", 100) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
      }
      return;
    }
    if (write (tdL.player[1 - player], board, sizeof(board)) <= 0)
    {
      printf("[Thread %d] ",tdL.id);
      perror ("[Thread]Eroare la write() catre client.\n");
      if (write (tdL.player[player], board, sizeof(board)) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
      }
      if (write (tdL.player[player], "Winer!\n", 100) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
      }
      return;
    }

    // send message to players
    if (write (tdL.player[1 - player], "Waiting opponent to move...", 100) <= 0)
    {
      printf("[Thread %d] ",tdL.id);
      perror ("[Thread]Eroare la write() catre client.\n");
      if (write (tdL.player[player], board, sizeof(board)) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
      }
      if (write (tdL.player[player], "Winer!\n", 100) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
      }
      return;
    }
    bzero(&input, 100);
    int col = 0;
    bool ok = false;
    while(!ok){
      while(!validareInput(input)){
        if (write (tdL.player[player], "Enter your move: ", 100) <= 0)
        {
          printf("[Thread %d] ",tdL.id);
          perror ("[Thread]Eroare la write() catre client.\n");
          if (write (tdL.player[1 - player], board, sizeof(board)) <= 0)
          {
            printf("[Thread %d] ",tdL.id);
            perror ("[Thread]Eroare la write() catre client.\n");
          }
          if (write (tdL.player[1 - player], "Winer!\n", 100) <= 0)
          {
            printf("[Thread %d] ",tdL.id);
            perror ("[Thread]Eroare la write() catre client.\n");
          }
          return;
        }
        bzero(&input, 100);
        if (read(tdL.player[player], &input, 100) <= 0) {
          printf("[Thread %d]\n",tdL.id);
          perror ("[Thread]Eroare la write() catre client.\n");
          if (write (tdL.player[1 - player], board, sizeof(board)) <= 0)
          {
            printf("[Thread %d] ",tdL.id);
            perror ("[Thread]Eroare la write() catre client.\n");
          }
          if (write (tdL.player[1 - player], "Winer!\n", 100) <= 0)
          {
            printf("[Thread %d] ",tdL.id);
            perror ("[Thread]Eroare la write() catre client.\n");
          }
          return;
        }
      }
      col = atoi(input);
      ok = mutareValida(board, col);
      bzero(&input, 100);
    }
    if (write (tdL.player[player], "ok", 100) <= 0)
    {
      printf("[Thread %d] ",tdL.id);
      perror ("[Thread]Eroare la write() catre client.\n");
      if (write (tdL.player[1 - player], board, sizeof(board)) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
      }
      if (write (tdL.player[1 - player], "Winer!\n", 100) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
      }
      return;
    }
    adaugaPiesa(board, col, player + 1);
    //  verific terminarea jocului
    int gameState = JocTerminat(board, player + 1);
    if (gameState != 0) {
      if (write (tdL.player[player], board, sizeof(board)) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
        if (write (tdL.player[1 - player], board, sizeof(board)) <= 0)
        {
          printf("[Thread %d] ",tdL.id);
          perror ("[Thread]Eroare la write() catre client.\n");
        }
        if (write (tdL.player[1 - player], "Winer!\n", 100) <= 0)
        {
          printf("[Thread %d] ",tdL.id);
          perror ("[Thread]Eroare la write() catre client.\n");
        }
        return;
      }
      if (write (tdL.player[1 - player], board, sizeof(board)) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
        if (write (tdL.player[player], board, sizeof(board)) <= 0)
        {
          printf("[Thread %d] ",tdL.id);
          perror ("[Thread]Eroare la write() catre client.\n");
        }
        if (write (tdL.player[player], "Winer!\n", 100) <= 0)
        {
          printf("[Thread %d] ",tdL.id);
          perror ("[Thread]Eroare la write() catre client.\n");
        }
        return;
      }
      initializareTabla(board);
    }
    if (gameState == 1) {
      if (write (tdL.player[player], "Winer!\n", 100) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
        return;
      }
      if (write (tdL.player[1 - player], "Loser!\n", 100) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
        return;
      }
      return;
      
    }
    else if (gameState == 2) {
      if (write (tdL.player[player], "Draw!\n", 100) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
        return;
      }
      if (write (tdL.player[1 - player], "Draw!\n", 100) <= 0)
      {
        printf("[Thread %d] ",tdL.id);
        perror ("[Thread]Eroare la write() catre client.\n");
        return;
      }
      return;
    }
    player = 1 - player;
  }
}