#include<unistd.h>
int main()
{
   char c;
   //read 1 byte from standard i/p to c
   while(read(STDIN_FILENO,&c,1)==1);
   return 0;
}
