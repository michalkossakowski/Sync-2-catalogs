#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h> // do obsługi isdigit() czy jest liczbą

void program();
int czy_katalog();

int main(int count, char* arg[])
{
    long int prog = 2000000; // domyślny próg rozmiaru do kopiowania
    long int slep = 10; // długośc spania potem mozna ustawić na 5 min
    int R = 0; // czy użyta opcja -R

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

    char *a = arg[1];
    char *b = arg[2];

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