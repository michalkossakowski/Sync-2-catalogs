#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

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
// make - kompilowanie pliku zamiast gcc