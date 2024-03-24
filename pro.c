// dodajemy niezbędne bilbioteki
#include <stdio.h> // głowna biblioteka c
#include <stdlib.h> // biblioteka pomocnicza do c do malloc i free
#include <string.h> // bilbioteka do napisów w c
#include <sys/stat.h> // uzyskiwanie informacji o pliku
#include <unistd.h> // unix standar close/read/write/fork/sleep/exec
#include <fcntl.h> // open O_RDONLY  O_CREAT

#include <sys/types.h>
#include <dirent.h>


int czy_katalog(char *path) // funkcja sprawdzajacy czy sciezka wskazuje na katalog
{
    struct stat info; // stat przechowuje informcje o pliku/katalogu
    stat(path, &info); // pobranie informacji o pliku ze sciezki do zmiennej s
    return S_ISDIR(info.st_mode); // sprawdzenia czy podana sciezka to katalog 
}


void kopiuj(char *a,char *b) //funkcja kopiowania
{
    int in = open(a,O_RDONLY); //
    int out = open(b,O_WRONLY|O_CREAT);
    int size = 1024;
    char *buffor = (char *)malloc(size);
    while(1)
    {
        int r = read(in,buffor,size);
        write(out,buffor,r);
        if(r < size){break;}
    }
    printf("Kopia: %s -> %s\n", a, b);
    free(buffor);
    close(in);
    close(out);
}

void synchronizuj(char *a,char *b) 
{
    DIR *in = opendir(a); // otwarcie katalogu a jako dir
    DIR *out  = opendir(b);// otwarcie katalogu b jako dir

    struct dirent *elem  = readdir(in); // wskaźnik elemnet w katalogu

    while (elem != NULL) // pętla przez wszystki elementy katalogu a
    { 
        char inPath[2048]; // ścieżka do elementu w katalogu a
        char outPath[2048];// ścieżka do elementu w katalogu b
        
        snprintf(inPath, sizeof(inPath), "%s/%s", a, elem->d_name); // połącz nazwe pliku ze pełną ścieżką
        snprintf(outPath, sizeof(outPath), "%s/%s", b, elem->d_name); // połącz nazwe pliku ze pełną ścieżką

        struct stat info; // informacje o pliku
        stat(inPath, &info);// pobranie info o pliku

        if (S_ISDIR(info.st_mode)==0) // jak nie jest katalogiem  to kopiuj
        { 
            kopiuj(inPath, outPath); //funkcja kopiuj
        }

        elem  = readdir(in); // sprawdza nastepny element
    }

    closedir(in); // zamkinj katalog a
    closedir(out); // zamknij katalog b
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
    printf("Zsynchronizowano pomyślnie !!!\n");
}