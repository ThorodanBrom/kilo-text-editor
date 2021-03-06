/*** includes ***/

#include<errno.h>
#include<stdio.h>
#include<ctype.h>
#include<termios.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<stdlib.h>

/*** defines ***/
//bitwise ANDs k and 00011111 ie. removes 6th and 7th bit from char k
#define CTRL_KEY(k) ((k)&0x1f)

/*** data ***/
struct editorConfig
{
    struct termios org_termios;
    int screenrows;
    int screencols;
};

struct editorConfig E;
/*** terminal ***/

void die(const char *s)
{
    // in case an error is there, clear screen, reset cursor and print
    write(STDOUT_FILENO,"\x1b[2J",4);
    write(STDOUT_FILENO,"\x1b[H",3);
    //perror checks the errno returned
    perror(s);
    exit(1);
}


void disableRAWMode()
{
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&E.org_termios)== -1)
        die("tcsetattr");
}

void enableRAWMode()
{
    /* writing the terminal attribs to the structure -> tcgetattr
     turn off echo mode
     save attribs -> tcsetattr */

    if(tcgetattr(STDIN_FILENO,&E.org_termios)== -1)
        die("tcgetattr");

    //when program over, run disableraw() to go back to cooked

    atexit(disableRAWMode);
    struct termios raw=E.org_termios;

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

char editorReadKey()
{
    int nread;
    char c;
    while((nread=read(STDIN_FILENO,&c,1))!=1)
    {
        if(nread==-1 && errno!=EAGAIN)
            die("read");
    }
    return c;
}

/* winsize and TIOCGWINSZ from sys/ioctl.h
   ioctl() places row and col num in winsize */
int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws)==-1||ws.ws_col==0)
    {
        return -1;
    }
    else
    {
        *cols=ws.ws_col;
        *rows=ws.ws_row;
        return 0;
    }
}


/*** output ***/

//draws each row of buffer of text edited
void editorDrawRows()
{
    int y;
    for(y=0;y<24;y++)//24 rows
    {
        write(STDOUT_FILENO,"~\r\n",3);
    }
}        

/* writing 4 bytes to terminal
   1st byte -> \x1b - escape char (27)
   <esc>[ tells to perform some tex formatting
   J -> erase in display -> argument is 2, means clear entire screen 
   H -> set cursor at top of screen
*/
   
void editorRefreshScreen()
{
    write(STDOUT_FILENO,"\x1b[2J",4);
    write(STDOUT_FILENO,"\x1b[H",3);
    editorDrawRows();
    write(STDOUT_FILENO,"\x1b[H",3);
}    

/*** input ***/

void editorProcessKeypress()
{
    char c=editorReadKey();
    switch(c)
    {
        case CTRL_KEY('q'): write(STDOUT_FILENO,"\x1b[2J",4);
                            write(STDOUT_FILENO,"\x1b[H",3);
                            exit(0);
        break;
    }
}                    

/*** init ***/

void initEditor()
{
    if(getWindowSize(&E.screenrows,&E.screencols)==-1)
        die("getWindowSize");
}    

int main()
{
    enableRAWMode();
    initEditor();
    while(1)
    {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
}
