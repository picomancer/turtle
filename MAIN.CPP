
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "TURTLE.H"

using namespace std;

int main(int argc, char *argv[])
{
   int ruleb[8] = {7, 'F', '[', '-', 'B', ']', '+', 'B'};
/*
   int rulef[3] = {2, 'F', 'F'};
   int initial[2] = {1, 'B'};
*/
   int initial[2] = {1, 'F'};
   int rulef[9] = {8, 'F', '-', 'F', '+', '+', 'F', '-', 'F'};

   Dcfg r;
   int *t;
   int i, j;

   r.addRule('B', ruleb);
   r.addRule('F', rulef);
   t = initial;
   for(i=1;i<=5;i++)
   {
      t = r.apply(t);
      for(j=1;j<=t[0];j++)
         printf("%c", t[j]);
      printf("\n");

      r.setAxiom(initial);
      r.restart(i);
      while(1)
      {
         j = r.getNext();
         if (j == -1)
            break;
         else
            printf("%c", j);
      }
      printf("\n");
   }

   system("PAUSE");
   return EXIT_SUCCESS;
}
