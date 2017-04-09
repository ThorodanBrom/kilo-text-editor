/*** includes ***/

#include<errno.h>
#include<stdio.h>
#include<ctype.h>
#include<termios.h>
#include<unistd.h>
#include<stdlib.h>

/*** data ***/

struct termios org_termios;

/*** terminal ***/

void die(const char *s)
{
    //perror checks the errno returned
    perror(s);
    exit(1);
}

void disableRAWMode()
{
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&org_termios)== -1)
        die("tcsetattr");
}

void enableRAWMode()
{
    /* writing the terminal attribs to the structure -> tcgetattr
     turn off echo mode
     save attribs -> tcsetattr */

    if(tcgetattr(STDIN_FILENO,&org_termios)== -1)
        die("tcgetattr");

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
    //VMIN -> min no of bytes before read() returns
    //VTIME -> max time before ''''
    raw.c_cc[VMIN]=0;
    raw.c_cc[VTIME]=1;

    //tscaflush -> when to apply the changes
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw)== -1)
        die("tcsetattr");

}

/*** init ***/

int main()
{
    enableRAWMode();
    //read 1 byte from standard i/p to c
    while(1)
    {
        //checking if it is a control character
        char c='\0';
        if(read(STDIN_FILENO,&c,1)== -1 && errno !=EAGAIN)
            die("read");
        if(iscntrl(c))
        {
            printf("%d\r\n",c);
        }
        else
        {
            printf("%d ('%c')\r\n",c,c);
        }
        if(c=='q')
            break;
    }
    return 0;
}
