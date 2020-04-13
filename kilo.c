#include<unistd.h>
#include<termios.h>
#include<stdlib.h>

struct termios og_termios;

void DisableRawMode(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios);
}

void EnableRawMode(){
	tcgetattr(STDIN_FILENO, &og_termios);
	atexit(DisableRawMode);
	
	struct termios raw = og_termios;
	raw.c_lflag &= ~(ECHO);
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main()
{
	EnableRawMode();
	
	char c;
	
	while (read(STDIN_FILENO, &c ,1) == 1 && c != 'q');
	return 0;
}