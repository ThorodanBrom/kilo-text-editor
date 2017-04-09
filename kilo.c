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
       echo mode off!!
       turned off ctrl+z and ctrl+c with ISIG
       turned off ctrl+s and ctrl+q with IXON
       turned off ctrl+o, ctrl+v with IEXTEN
       turned off ctrl+m with ICRNL -> makes it a carriage return
       fixed \n to \r\n with OPOST
       */

    raw.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
    raw.c_iflag &= ~(IXON|ICRNL|BRKINT|INPCK|ISTRIP);
    raw.c_cflag |= (CS8);
    raw.c_oflag &= ~(OPOST);

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
            printf("%d\r\n",c);
        }
        else
        {
            printf("%d ('%c')\r\n",c,c);
        }
    }
    return 0;
}
