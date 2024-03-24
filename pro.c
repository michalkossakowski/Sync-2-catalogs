// dodajemy niezbędne bilbioteki
#include <stdio.h> // głowna biblioteka c
#include <stdlib.h> // biblioteka pomocnicza do c
#include <sys/stat.h> // uzyskiwanie informacji o pliku

// funkcja sprawdzajacy czy sciezka wskazuje na katalog
int czy_katalog(char *sciezka) {
    struct stat s; // stat przechowuje informcje o pliku/katalogu
    stat(sciezka, &s); // pobranie informacji o pliku ze sciezki do zmiennej s
    return S_ISDIR(s.st_mode); // sprawdzenia czy podana sciezka to katalog
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




}

