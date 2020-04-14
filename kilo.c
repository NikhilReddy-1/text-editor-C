#include<unistd.h>
#include<termios.h>
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<errno.h>

/*defines*/

#define CTRL_KEY(k) ((k) & 0x1f)

/*Data called*/
struct termios og_termios;

struct editConfig {
	struct termios og_termios;
};

struct editConfig E;

/*Terminal*/

void die(const char *s){
	perror(s);
	exit(1);
}

void DisableRawMode(){
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.og_termios) == -1)
		die("tcsetattr fail");
}

void EnableRawMode(){
	if(tcgetattr(STDIN_FILENO, &E.og_termios) == -1) die("tcgetattr fail");
	atexit(DisableRawMode);
	
	struct termios raw = E.og_termios;
	raw.c_iflag &= ~(ICRNL | IXON | INPCK | ISTRIP | BRKINT);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag &= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetatte fail");
}

char editReadKey(){
	int nread;
	char c;
	while((nread = read(STDIN_FILENO ,&c, 1)) != 1){
		if (nread == -1 && errno != EAGAIN) die("read fail");
	}
	return c;
}

/*output*/

void editDrawRows(){
	int y;
	for(y=0;y<24;y++){
		write(STDOUT_FILENO, "-\r\n", 3);
	}
}


void editRefreshScreen(){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO , "\x1b[H", 3);
	
	editDrawRows();
	
	write(STDOUT_FILENO, "\x1b[H", 3);
}

/*input*/

void editProcessKeypress(){
	char c = editReadKey();
	
	switch (c) {
		case CTRL_KEY('q'):
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
		}
	}

/*init*/

int main()
{
	EnableRawMode();
	
	while (1){
		editRefreshScreen();
		editProcessKeypress();
	}
	return 0;
}
