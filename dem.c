#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

void program();

// // Funkcja obsługi sygnału SIGINT (Ctrl+C)
// void handle_signal(int sig) {
//     if (sig == SIGINT) {
//         printf("\nOtrzymano sygnał SIGINT. Zatrzymywanie demona.\n");
//         exit(EXIT_SUCCESS);
//     }
// }

int main(int count, char* arg[])
{
    // pid_t pid;
    // // Przechwytuj sygnał SIGINT (Ctrl+C)
    // signal(SIGINT, handle_signal);
    // // Stwórz nowy proces potomny
    // pid = fork();

    long int prog = 2000000; // domyślny próg rozmiaru do kopiowania
    long int slep = 10; // długośc spania
    int R = 0; // czy użyta opcja -R

    // sprawdzenie ilości argumentów  
    if(count==6){
        if(strcmp(arg[5],"-R")==0){ // ./pro a b SLEEP PROG -R
            R=1;
            prog= atol(arg[4]);
            slep = atol(arg[3]);
        }
    }
    else if(count==5){
        if(strcmp(arg[4],"-R")==0){ //./pro a b SLEEP -R
            R=1;
            slep = atol(arg[3]);
        }
        else{ // ./pro a b SLEEP PROG
            prog = atol(arg[4]);
            slep = atol(arg[3]);
        }
    }
    else if(count==4){
        if(strcmp(arg[3],"-R")==0) // ./pro a b -R
            R=1;
        else
            slep = atol(arg[3]); // ./pro a b SLEEP
    }
    else if(count!=3){ // sprawdzenie ilości argumentów, przerwanie jeżeli jest ich za mało lub za dużo     //./pro a b
        printf("!!! Podano za malo lub za duzo argumentow !!!\n");
        return 0;
    }

    // wywołanie demona
    daemon(1, 0);
    while (1) {
        program(arg[1],arg[2],prog,R);
        sleep(slep);
    }
    return 0;
}

//ps aux - pokaż procesy
//kill pid - zamknij proces
//rm -r - usuwanie

// ./dem a b slep prog -R