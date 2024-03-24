// dodajemy niezbędne bilbioteki
#include <stdio.h> // głowna biblioteka c
#include <stdlib.h> // biblioteka pomocnicza do c do malloc i free
#include <string.h> // bilbioteka do napisów w c
#include <sys/stat.h> // uzyskiwanie informacji o pliku
#include <unistd.h> // unix standar close/read/write/fork/sleep/exec
#include <fcntl.h> // open O_RDONLY  O_CREAT

#include <sys/types.h>
#include <dirent.h>

// funkcja sprawdzajacy czy sciezka wskazuje na katalog
int czy_katalog(char *sciezka) {
    struct stat sciezka_stat; // stat przechowuje informcje o pliku/katalogu
    stat(sciezka, &sciezka_stat); // pobranie informacji o pliku ze sciezki do zmiennej s
    return S_ISDIR(sciezka_stat.st_mode); // sprawdzenia czy podana sciezka to katalog
}

void kopiuj(char *a,char *b) {

    int in = open(a,O_RDONLY); //
    int out = open(b,O_WRONLY|O_CREAT);
    int size = 1024;
    char *buffor = (char *)malloc(size);
    while(1){
        int r = read(in,buffor,size);
        write(out,buffor,r);
        if(r < size){break;}
    }
    free(buffor);
    close(in);
    close(out);
}

void synchronizuj(char *a,char *b) {
    DIR *zrodlo, *cel;
    struct dirent *entry;
    struct stat statbuf;

    zrodlo = opendir(a);
    cel = opendir(b);

    while ((entry = readdir(zrodlo)) != NULL) {
        char zrodloPath[1024];
        char celPath[1024];

        snprintf(zrodloPath, sizeof(zrodloPath), "%s/%s", a, entry->d_name);
        snprintf(celPath, sizeof(celPath), "%s/%s", b, entry->d_name);

        if (lstat(zrodloPath, &statbuf) < 0) {
            perror("lstat");
            closedir(zrodlo);
            closedir(cel);
            exit(EXIT_FAILURE);
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                synchronizuj(zrodloPath, celPath);
            }
        } else {
            struct stat destStat;
            if (lstat(celPath, &destStat) == 0) {
                // If file exists in cel but not in zrodlo, remove it
                if (access(zrodloPath, F_OK) == -1) {
                    printf("Removing %s\n", celPath);
                    unlink(celPath);
                }
            } else {
                printf("Copying %s to %s\n", zrodloPath, celPath);
                kopiuj(zrodloPath, celPath);
            }
        }
    }

    closedir(zrodlo);
    closedir(cel);
}


int main(int count, char * arg[]) // count - liczba argumentów , arg - tablica zawierająca argumenty
{
    // sprawdzenie ilości argumentów, przerwanie jeżeli jest ich za mało lub za dużo
    if(count!=3){ 
        printf("Podano za malo lub za duzo argumentow !!!\n");
        return 0;
    }
    // przypisanie pobraych argumentów do zmienncyh
    char *a = arg[1];
    char *b = arg[2];

    // sprawdzenie czy pierwszy argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(a)==0){
        printf("Pierwszy argument: %s  nie jest katalogiem !!!\n",a);
        return 0;
    }
    printf("Katalog 1: %s \n",a);

    // sprawdzenie czy drugi argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(b)==0){
        printf("Drugi argument: %s nie jest katalogiem !!!\n",b);
        return 0;
    }
    printf("Katalog 2: %s \n",b);

    //wywołanie funkcji do synchronizacji
    synchronizuj(a,b);
    
    // jak wszystko ok to komunikat
    printf("Zsynchronizowano pomyślnie");
}

