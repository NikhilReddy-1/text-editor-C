#include<unistd.h>
#include<termios.h>
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<errno.h>
#include<sys/ioctl.h>

/*defines*/

#define CTRL_KEY(k) ((k) & 0x1f)

/*Data called*/
struct termios og_termios;

struct editConfig {
	int screenrows;
	int screencols;
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

int getWindowsSize(int *rows, int *cols){
	struct winsize ws;
	
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
		return -1;
	}
	else {
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}
/*output*/

void editDrawRows(){
	int y;
	for(y=0;y< E.screenrows;y++){
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

void initEdtior(){
	if (getWindowsSize(&E.screenrows, &E.screencols) == -1) die("getWindowsSize");
}

int main()
{
	EnableRawMode();
	initEdtior();
	while (1){
		editRefreshScreen();
		editProcessKeypress();
	}
	return 0;
}
