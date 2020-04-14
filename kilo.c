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

/*Terminal*/

void die(const char *s){
	perror(s);
	exit(1);
}

void DisableRawMode(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios);
}

void EnableRawMode(){
	if(tcgetattr(STDIN_FILENO, &og_termios) == -1) die("tcgetattr fail");
	atexit(DisableRawMode);
	
	struct termios raw = og_termios;
	raw.c_iflag &= ~(ICRNL | IXON | INPCK | ISTRIP | BRKINT);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag &= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetatte fail");
}

/*init*/

int main()
{
	EnableRawMode();
	
	char c;
	
	while (1){
		char c = '\0';
		if(read(STDIN_FILENO, &c ,1) == -1 && errno != EAGAIN) die("read fail");
		
		if(iscntrl(c)){
			printf("%d\r\n",c);
		}
		else{
			printf("%d(%c)\r\n",c,c);
		}
	if(c == 'q')break;
	}
	return 0;
}
