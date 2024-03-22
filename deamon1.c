#include <stdio.h>
#include <stdlib.h>

int main(int argCount, char *arg[]) 
{
   printf("ilość arg: %d \n",argCount);

   int x = atoi(arg[1]);
   printf("argument liczba 1: %d \n",x);
   
   char *y = arg[2];
   printf("argument string 2: %s \n",y);

      char *y = arg[2];
   printf("argument string 3: %s \n",y);
   return 0;
}