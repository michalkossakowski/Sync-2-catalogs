#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>



int main(int count, char* arg[])
{
 

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