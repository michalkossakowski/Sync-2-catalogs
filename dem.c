#include <stdio.h> // głowna biblioteka c
#include <stdlib.h> // biblioteka pomocnicza do c do malloc i free
#include <string.h> // bilbioteka do napisów w c
#include <sys/stat.h> // uzyskiwanie informacji o pliku
#include <unistd.h> // unix standard close/read/write/fork/sleep/exec
#include <sys/types.h> 
#include <signal.h>
#include <ctype.h> // do obsługi isdigit() - sprawdzenie czy zmiennajest liczbą

// deklaracja funkcji z drugiego pliku pro.c
void program();
int czy_katalog();
void wpisz_do_log();

// co zrobic po odebraniu sigusr1
void sigusr1_handler(int signum) {
    wpisz_do_log("Obudzono demona w wyniku sygnału");
}

int main(int count, char* arg[])
{
    signal(SIGUSR1, sigusr1_handler);

    char *a;
    char *b;
    long int prog;
    long int slep;
    int R;

    prog = 2000000; // domyślny próg rozmiaru do kopiowania
    slep = 10; // długośc spania potem mozna ustawić na 5 min
    R = 0; // czy użyta opcja -R

    // sprawdzenie ilości argumentów  
    // ./pro a b SLEEP PROG -R
    if(count==6)
    {
        if(strcmp(arg[5],"-R")==0)
        { 
            R=1;
            if(isdigit(*arg[3]) && isdigit(*arg[4]))
            {
                prog = atol(arg[4]);
                slep = atol(arg[3]);
            }
            else
            {
                printf("!!! Podano zły format argumentów !!!\n");
                return 0;
            }
        }
        else
        {
            printf("!!! Podano zły format argumentów !!!\n");
            return 0;
        }
    }
    else if(count==5)
    {
        //./pro a b SLEEP -R
        if(strcmp(arg[4],"-R")==0)
        { 
            R=1;
            if(isdigit(*arg[3]))
            {
                slep = atol(arg[3]); 
            }
            else
            {
                printf("!!! Podano zły format argumentu !!!\n");
                return 0;
            }
        }
        // ./pro a b SLEEP PROG
        else
        {
            if(isdigit(*arg[3]) && isdigit(*arg[4]))
            {
                prog = atol(arg[4]);
                slep = atol(arg[3]);
            }
            else
            {
                printf("!!! Podano zły format argumentów !!!\n");
                return 0;
            }
            
        }
    }
    else if(count==4)
    {
        // ./pro a b -R
        if(strcmp(arg[3],"-R")==0) 
            R=1;
        else
        {
            // ./pro a b SLEEP
            if(isdigit(*arg[3]))
            {
                slep = atol(arg[3]); 
            }
            else
            {
                printf("!!! Podano zły format argumentów !!!\n");
                return 0;
            }
        }
    }
    // sprawdzenie ilości argumentów, przerwanie jeżeli jest ich za mało lub za dużo     //./pro a b
    else if(count!=3)
    { 
        printf("!!! Podano za malo lub za duzo argumentow !!!\n");
        return 0;
    }

    a = arg[1];
    b = arg[2];

    // sprawdzenie czy pierwszy argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(a)==0)
    {
        printf("!!! Pierwszy argument: %s  nie jest katalogiem !!!\n",a);
        return 0;
    }
    printf("> Katalog 1: %s \n",a);

    // sprawdzenie czy drugi argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(b)==0)
    {
        printf("!!! Drugi argument: %s nie jest katalogiem !!!\n",b);
        return 0;
    }
    printf("> Katalog 2: %s \n",b);

    printf("> Slep=%ld Prog=%ld Rekrurencja=%d \n",slep,prog,R);

    // wywołanie demona
    daemon(1, 0);
    wpisz_do_log("Uruchomienie demona");

    while (1) {
        wpisz_do_log("Obudzono demona okresowo");
        program(a,b,prog,R);
        wpisz_do_log("Zaśnięcie demona okresowo");
        sleep(slep);
    }
    return 0;
}

//ps aux - pokaż procesy
//kill pid - zamknij proces
//rm -r - usuwanie

// ./dem a b slep prog -R
// make - kompilowanie pliku zamiast gcc
// cat /car/log/syslog -odczytywanie logów
// kill -SIGUSR1 pid  - wysyła sygnał siguser