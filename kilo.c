#include<stdio.h>
#include<ctype.h>
#include<termios.h>
#include<unistd.h>
#include<stdlib.h>
struct termios org_termios;

void disableRAWMode()
{
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&org_termios);
}
void enableRAWMode()
{
    /* writing the terminal attribs to the structure -> tcgetattr
     turn off echo mode
     save attribs -> tcsetattr */

    tcgetattr(STDIN_FILENO,&org_termios);
    //when program over, run disableraw() to go back to cooked
    atexit(disableRAWMode);
    struct termios raw=org_termios;

    /* ECHO=0010 -> then NOT is, and bitwise AND to the c_lflag
       echo mode off!!*/

    raw.c_lflag &= ~(ECHO|ICANON);

    //tscaflush -> when to apply the changes
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw);

}
int main()
{
    enableRAWMode();
    char c;
    //read 1 byte from standard i/p to c
    while(read(STDIN_FILENO,&c,1)==1 && c!='q')
    {
        //checking if it is a control character
        if(iscntrl(c))
        {
            printf("%d\n",c);
        }
        else
        {
            printf("%d ('%c')\n",c,c);
        }
    }
    return 0;
}
