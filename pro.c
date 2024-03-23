#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int czy_katalog(const char *sciezka) {
    struct stat s;
    stat(sciezka, &s);
    return S_ISDIR(s.st_mode);
}

int main(int count, char * arg[])
{

    char *k1 = arg[1];

    printf("Katalog 1: %s \n",k1);

    char *k2 = arg[2];
    printf("Katalog 2: %s \n",k2);

}

//Schemat demona z ksiazki


// #include <sys/types.h>
// #include <sys/stat.h>
// #include <stdlib.h> 
// #include <stdio.h> 
// #include <fcntl.h>
// #include <unistd.h>
// #include <linux/fs.h>

// // funkcja main count-ilość argumentów, arg[] - tablica z argumentami
// int main (void)
// {
//  pid_t pid;
//  int i;
//  /* stwórz nowy proces */
//  pid = fork ( );
//  if (pid == -1)
//  return -1;
//  else if (pid != 0)
//  exit (EXIT_SUCCESS);
//  /* stwórz nową sesję i grupę procesów */
//  if (setsid ( ) == -1)
//  return -1;
//  /* ustaw katalog roboczy na katalog główny */
//  if (chdir ("/") == -1)
//  return -1;
//  /* zamknij wszystkie pliki otwarte - użycie opcji NR_OPEN to przesada, lecz działa */
//  for (i = 0; i < NR_OPEN; i++)
//  close (i);
//  /* przeadresuj deskryptory plików 0, 1, 2 na /dev/null */
//  open ("/dev/null", O_RDWR); /* stdin */
//  dup (0); /* stdout */
//  dup (0); /* stderror */
//  /* tu należy wykonać czynności demona… */
//  return 0;
// }