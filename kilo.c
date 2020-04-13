#include<unistd.h>
#include<termios.h>
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>

struct termios og_termios;

void DisableRawMode(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios);
}

void EnableRawMode(){
	tcgetattr(STDIN_FILENO, &og_termios);
	atexit(DisableRawMode);
	
	struct termios raw = og_termios;
	raw.c_iflag &= ~(ICRNL | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main()
{
	EnableRawMode();
	
	char c;
	
	while (read(STDIN_FILENO, &c ,1) == 1 && c != 'q'){
		if(iscntrl(c)){
			printf("%d\r\n",c);
		}
		else{
			printf("%d(%c)\r\n",c,c);
		}
	}
	return 0;
}
