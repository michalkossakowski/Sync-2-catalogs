#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

// Funkcja obsługi sygnału SIGINT (Ctrl+C)
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nOtrzymano sygnał SIGINT. Zatrzymywanie demona.\n");
        exit(EXIT_SUCCESS);
    }
}

int main() {
   // pid_t pid;
    int folder_num = 1; // Licznik folderów

    // // Przechwytuj sygnał SIGINT (Ctrl+C)
    // signal(SIGINT, handle_signal);

    // // Stwórz nowy proces potomny
    // pid = fork();

    // // Jeśli nie udało się stworzyć procesu potomnego, zakończ
    // if (pid < 0) {
    //     perror("Błąd forkowania");
    //     exit(EXIT_FAILURE);
    // }

    // // Jeśli udało się stworzyć proces potomny, zakończ proces nadrzędny
    // if (pid > 0) {
    //     exit(EXIT_SUCCESS);
    // }

    // Zmień katalog roboczy na / (jeśli "nochdir" ustawione na 0)
    // Nie zamykaj standardowych deskryptorów plików (jeśli "noclose" ustawione na 0)
    if (daemon(1, 0) == -1) {
        perror("Błąd tworzenia demona");
        exit(EXIT_FAILURE);
    }

// Tworzenie nazwy foldera z inkrementowanym numerem
        char folder_name[256];
        snprintf(folder_name, sizeof(folder_name), "demonowy_folder_%d", folder_num);


    // Utwórz katalog z uprawnieniami 0777 (rwxrwxrwx)
    if (mkdir(folder_name, 0777) == -1) {
        perror("Błąd tworzenia katalogu");
        exit(EXIT_FAILURE);
    }

    // Główna pętla demoniczna
    while (1) {
        // Wymuszenie wypisania bufora
        fflush(stdout);
         // Zwiększ numer foldera
        folder_num++;
        // Poczekaj 20 sekund
        sleep(20);
    }

    exit(EXIT_SUCCESS);
}
