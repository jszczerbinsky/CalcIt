#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include "interface.h"

int main(int argc, char *argv[])
{
	struct termios oldTermAttr, newTermAttr;
	tcgetattr(STDIN_FILENO, &oldTermAttr);
	newTermAttr = oldTermAttr;
	newTermAttr.c_lflag &= ~(ICANON | ECHO );
	tcsetattr(STDIN_FILENO, TCSANOW, &newTermAttr);

	char buff[4*1024];

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, buff, _IOFBF, 4*1024);

	interfaceRun();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldTermAttr);
}
