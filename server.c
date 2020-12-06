#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#define PORT 8080
#define CERC 0x25EF
#define GOL 0x2800
#define ROSU "\033[1;31m"
#define VERDE "\033[0;32m"
#define GALBEN "\033[0;33m"
#define ALBASTU "\033[0;34m"
#define RESET "\033[0m"

int **board, *jucator;


// initial tabla este goala
static void initializareTabla(int board[6][7]) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            board[i][j] = 0;
        }
    }
}


// transform din numere in piese pe diferite culori 
static wchar_t transformaInPiesa(int piesa) {
    if (piesa == 0) {
        return GOL;
    } else {
        return CERC;
    }
}

// afisez tabla de joc
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

// verific daca se poate face mutarea
bool mutareValida(int board[6][7], int coloana) {
    if (board[0][coloana - 1] == 0) {
        return true;
    }
    return false;
}

// adaug mutarea pe tabla
void adaugaPiesa(int board[6][7], int coloana, int jucator) {
    int linie = 5;
    while (board[linie][coloana - 1] != 0) {
        linie--;
    }
    board[linie][coloana - 1] = jucator;
}

// verific daca inputul este introdus corect
bool validareInput(char *input) {
    if (strcmp(input, "help") == 0 || strcmp(input, "exit") == 0 || 
            (strcmp(input, "0") > 0 && strcmp(input, "8") < 0)) {
        return true;
    } else {
        return false;
    }
}

// execut comenzile introduse de client
void executaComanda(char *comanda, int board[6][7], int jucator) {
    if (!validareInput(comanda)) {
        printf("Nu ati introdus o mutare valida.\nPentru ajutor introduceti coanda `help`.\n");
        exit(1);
    }

    if (strcmp(comanda, "help") == 0) {
        
        FILE *helpFile;
        
        // #todo send raspunsuri pentru fiecare instanta
        if ((helpFile = fopen("help.txt", "r")) == NULL) {
            printf("Error! Nu se poate deschide fisierul `help.txt` \n");
            exit(1);
        }

        char buff[255];
        while (fgets(buff, sizeof(buff), helpFile)) {
            printf(buff);
        }
    } else {
        int col = comanda - '0';
        if (!mutareValida(board, col)) {

            printf("Coloana %d este plina. Introduceti alta mutare!");
        } else {
            adaugaPiesa(board, col, jucator);
            jucator = 1 - jucator;
        }
    }
    // todo send table 
}

bool JocTerminat(int table[6][7]) {
    // verific daca unul din jucatori a castigat
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j) {
            int x = table[i][j];
            int linie  = 1;
            int coloana = 1;
            int diagonala = 1;
            for (int k = 1; k <= 3; k++) {
                if (table[i][j + k] == x) {
                    linie++;
                }
                if (table[i + k][j] == x) {
                    coloana++;
                }
                if (table[i + k][j + k] == x) {
                    coloana++;
                }
            }
            if (linie == 4 || coloana == 4 || diagonala == 4) {
                // todo send winer;
                return true;
            }
        }
    }

    // verfic daca se mai pot adauga piese
    for (int i = 0; i < 7; i++) {
        if (table[0][i] == 0) return false;
    }

    // daca nimeni nu castigat si se mai pot adauga piese inseamna ca jocul s-a terminat cu egalitate
    // todo send draw
    
    return true;

}

int main() {
    
    return 0;
}