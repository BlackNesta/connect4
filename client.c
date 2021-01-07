#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#define CERC 0x25EF
#define GOL 0x2800
#define ROSU "\033[1;31m"
#define VERDE "\033[0;32m"
#define GALBEN "\033[0;33m"
#define ALBASTU "\033[0;34m"
#define RESET "\033[0m"

extern int errno;
int port;

void afisareTabla(int board[6][7]);
wchar_t transformaInPiesa(int piesa);

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

  bzero(&msg, 100);
  if (read (sd, msg, 100) < 0) {
      perror("[client]Read error1\n");
      return errno;
  }
  printf("%s", msg);

  bzero(&msg, 100);
  if (read (sd, &msg, 100) < 0) {
      perror("[client]Read error1\n");
      return errno;
  }
  printf("%s", msg);
  
  int board[6][7];
  char input[100];
  while(1) {
    
    bzero(&board, 100);
    fflush(stdout);
    //read board
    if (read (sd, board, sizeof(board)) < 0) {
      perror("[client]Read error3\n");
      return errno;
    }
    afisareTabla(board);
    //read msg
    bzero(&msg, 100);
    if (read(sd, &msg, 100) < 0) {
      perror("[client]Read error3\n");
      return errno;
    }
    else {
      if ((strcmp(msg, "Enter your move: ") == 0)) {
        while (strcmp(msg, "Enter your move: ") == 0) {
          printf(msg);
          bzero(&input, 100);
          scanf("%s", &input);
          if (write (sd, input, 100) <= 0) {
            perror("[client]Write error1");
            return errno;
          }
          bzero(&msg, 100);
          if (read(sd, &msg, 100) < 0) {
            perror("[client]Read error3\n");
            return errno;
          }
        }
      }
      else if (strcmp(msg, "Waiting opponent to move.\n") == 0) {
        printf(msg);
      }
      else {
        printf(msg);
        exit(0);
      }
    }
  }
}

wchar_t transformaInPiesa(int piesa) {
    if (piesa == 0) {
        return GOL;
    } else {
        return CERC;
    }
}

void afisareTabla(int board[6][7]) {
    setlocale( LC_ALL, "en_US.UTF-8" );
    printf("\n           ┌───────────────────┐    \n");
      printf("           │     CONNECT 4     │    \n");
      printf("           └───────────────────┘    \n\n");
      printf("         ↓   ↓   ↓   ↓   ↓   ↓   ↓  \n");
      printf("       ╷   ╷   ╷   ╷   ╷   ╷   ╷   ╷\n");
    for (int i = 0; i < 5; i++) {
        printf("       |");
        for (int j = 0; j < 7; j++) {
            int piesa = board[i][j];
            if (piesa == 1) {
                printf(ROSU);
            } else if (piesa == 2) {
                printf(GALBEN);
            }
            printf(" %lc ", transformaInPiesa(piesa));
            printf(RESET);
            printf("|");
            
        }
        printf("\n       ├───┼───┼───┼───┼───┼───┼───┤\n");
    }
    printf("       |");
    for (int j = 0; j < 7; j++) {
        int piesa = board[5][j];
            if (piesa == 1) {
                printf(ROSU);
            } else if (piesa == 2) {
                printf(GALBEN);
            }
            printf(" %lc ", transformaInPiesa(piesa));
            printf(RESET);
            printf("|");
    }
    printf("\n       └───┴───┴───┴───┴───┴───┴───┘\n");
      printf("         1   2   3   4   5   6   7  \n\n");
}