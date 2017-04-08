#include<termios.h>
#include<unistd.h>
void enableRAWMode()
{
    /* writing the terminal attribs to the structure -> tcgetattr
     turn off echo mode
     save attribs -> tcsetattr */
    struct termios raw;
    tcgetattr(STDIN_FILENO,&raw);
    /* ECHO=0010 -> then NOT is, and bitwise AND to the c_lflag
       echo mode off!!*/
    raw.c_lflag &= ~(ECHO);
    //tscaflush -> when to apply the changes
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw);

}
int main()
{
    enableRAWMode();
    char c;
    //read 1 byte from standard i/p to c
    while(read(STDIN_FILENO,&c,1)==1 && c!='q');
    return 0;
}
