#include <stdio.h> // głowna biblioteka c
#include <stdlib.h> // biblioteka pomocnicza do c do malloc i free
#include <string.h> // bilbioteka do napisów w c
#include <sys/stat.h> // uzyskiwanie informacji o pliku
#include <unistd.h> // unix standard close/read/write/fork/sleep/exec
#include <signal.h> // do obsługi sygnału SIGUSR1
#include <ctype.h> // do obsługi isdigit() - sprawdzenie czy zmiennajest liczbą

// deklaracja funkcji z drugiego pliku pro.c przed main aby można było ich użyć w main
void program();
int czy_katalog();
void wpisz_do_log();

// po odebraniu sigusr zostaje przerwana petla while i wykonuje sie od nowa
void odbierz_sigusr1() {
    wpisz_do_log("Obudzono demona w wyniku sygnału"); // do logów wpisujemy odpowiedni komunikat
}

int main(int count, char* arg[]) // count - ilość argumentów, arg[] - tablica z podanymi argumentami
{
    signal(SIGUSR1, odbierz_sigusr1); //wywołanie fukncji odbierz_sigusr1 po odebraniu sygnału sigusr1 przy pomocy singal

    //deklaracja zmiennych przechowujących podane parametry
    char *a;
    char *b;
    long int prog = 2000000; // domyślny próg rozmiaru do kopiowania
    long int slep = 15; // domyślna długośc spania (15 sekund)
    int R = 0; // czy użyta opcja rekurencji 1 - tak , 0 - nie


    // sprawdzenie ilości argumentów  
    // ./d a b [slep] [prog] [-R] - schemat kolejnosci podawania argumentow 
    if(count==6) //dla 5 argumentów, count==6 bo 1 argument to liczba argumentów a my podajemy jescze 5
    {
        if(strcmp(arg[5],"-R")==0) // sprawdzenie czy użytko rekurencji
        { 
            R=1; // włączenie rekurencji
            if(isdigit(*arg[3]) && isdigit(*arg[4])) // sprawdzenie czy 3 i 4 argument są liczbami
            {
                prog = atol(arg[4]); // przypisanie progu rozmiaru pliku i czasu spania do zmiennych
                slep = atol(arg[3]);
            }
            else // jeżeli nie są liczbami to błąd
            {
                printf("!!! Podano zły format argumentów !!!\n");
                return 0;
            }
        }
        else // jeżeli ostatni z 6 argumentów to nie "-R"
        {
            printf("!!! Podano zły format argumentów !!!\n");
            return 0;
        }
    }
    else if(count==5) // dla 4 argumentów
    {
        //./pro a b [SLEEP] [-R] - schemat kolejnosci podawania argumentow 
        if(strcmp(arg[4],"-R")==0) // sprawdzenie czy 4 argument to "-R"
        { 
            R=1; // włączenie rekurencji
            if(isdigit(*arg[3])) // sprawdzenie czy  3 argument jest liczbą
            {
                slep = atol(arg[3]); // przypisanie czasu spania do zmiennej
            }
            else// jeżeli nie jest liczbą to błąd
            {
                printf("!!! Podano zły format argumentu !!!\n");
                return 0;
            }
        }
        // ./pro a b [SLEEP] [PROG] - schemat kolejnosci podawania argumentow 
        else // dla 4 argumentów ale zamiast rekurencji został podany prób
        {
            if(isdigit(*arg[3]) && isdigit(*arg[4])) // sprawdzenie czy argument 3 i 4 są liczbami
            {
                prog = atol(arg[4]); // przypisanie progu rozmiaru pliku i czasu spania do zmiennych
                slep = atol(arg[3]);
            }
            else // jeżeli nie są liczbami tobłąd
            {
                printf("!!! Podano zły format argumentów !!!\n");
                return 0;
            }
            
        }
    }
    else if(count==4) // dla 3 argumentów
    {
        // ./pro a b [-R] - schemat kolejnosci podawania argumentow 
        if(strcmp(arg[3],"-R")==0) // czy dodatkowy 3 argument to rekurencja
            R=1; // włączenie rekurencji
        else //jeżeli to nie rekurencja to sprawdz czy liczba
        {
            // ./pro a b [SLEEP]  - schemat kolejnosci podawania argumentow 
            if(isdigit(*arg[3])) // czy dodatkowy 3 argument jest liczbą
            {
                slep = atol(arg[3]); // przypisanie czasu spania do zmiennej
            }
            else // jeżeli nie jest liczbą to błąd
            {
                printf("!!! Podano zły format argumentów !!!\n");
                return 0;
            }
        }
    }
    // ./pro a b - schemat kolejnosci podawania argumentow, minimalnie dwa katalog a i b
    else if(count!=3)  // sprawdzenie ilości argumentów, przerwanie jeżeli jest ich za mało lub za dużo 
    { 
        printf("!!! Podano za malo lub za duzo argumentow !!!\n");
        return 0;
    }

    //przypisanie katalogów do zmiennych
    a = arg[1];
    b = arg[2];

    // sprawdzenie czy pierwszy argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(a)==0)
    {
        printf("!!! Pierwszy argument: %s  nie jest katalogiem !!!\n",a);
        return 0;
    }
    printf(">>Katalog 1: %s \n",a);

    // sprawdzenie czy drugi argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(b)==0)
    {
        printf("!!! Drugi argument: %s nie jest katalogiem !!!\n",b);
        return 0;
    }
    printf(">>Katalog 2: %s \n",b);

    printf(">>Slep=%ld Prog=%ld Rekrurencja=%d \n",slep,prog,R); // wypisanie na ekran pobranych argumentów

    daemon(1, 0); // wywołanie demona
    wpisz_do_log("Uruchomienie demona"); // wpisywanie komunikatu o uruchomieniu do logów

    //główna pętla działania demona
    while (1) {
        wpisz_do_log("Obudzono demona"); // wpisywanie komunikatu o obudzeniu demona do logów
        program(a,b,prog,R); // wykonanie synchronizacji katalogów
        wpisz_do_log("Zaśnięcie demona");// wpisywanie komunikatu o zaśnięciu demona do logów
        sleep(slep); // czekanie na następne obudzenie przez wybrany czas
    }
    return 0;
}

///// przydatne polecenia do testowania demona w linuxie /////
// ps aux - pokaż procesy
// kill pid - zamknij proces (pid - identyfikator procesu)
// rm -r - usuwanie katalogu i plików w środku
// make - kompilowanie pliku zamiast gcc
// ./d a b [slep] [prog] [-R]  - wywołanie naszego demona  [] - opcjonalne argumenty
// cat /car/log/syslog - odczytywanie logów
// tail /car/log/syslog -odczytywanie najnowszych logów 
// kill -SIGUSR1 pid  - wysyła sygnał siguser do procesu o indentyfikatorze pid