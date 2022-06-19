#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "interface.h"
#include "standard.h"

struct winsize size;

int interfaceHelpMB()
{
	const int width = 20;

	int row = 3;

	printf("\e[%d;%dH┌Help", row, size.ws_col/2-width/2);
	for(int i = 0; i < width-3-4; i++) printf("─");
	printf("┐\n");
	row++;

	for(int i = 0; i < sizeof(st_Help)/sizeof(char*); i++)
	{
		printf("\e[%d;%dH", row, size.ws_col/2-width/2);
		printf("│%-*s│▒", width-3, st_Help[i]);
		row++;
	}

	printf("\e[%d;%dH└", row, size.ws_col/2-width/2);
	for(int i = 0; i < width-3; i++) printf("─");
	printf("┘▒\n");
	row++;

	printf("\e[%d;%dH", row, size.ws_col/2-width/2+2);
	for(int i = 0; i < width-2; i++) printf("▒");
	printf("\n");

	fflush(stdout);

	getchar();
}

int interfaceQuitMB()
{
	const int width = 36;
	int selected = 0;

	while(1)
	{
		printf("\e[%d;%dH┌", size.ws_row/2-2, size.ws_col/2-width/2);
		for(int i = 0; i < width-3; i++) printf("─");
		printf("┐\n");

		printf("\e[%d;%dH", size.ws_row/2-2+1, size.ws_col/2-width/2);
		printf("│%-*s│▒", width-3, " Are You sure, You want to quit?");

		printf("\e[%d;%dH", size.ws_row/2-2+2, size.ws_col/2-width/2);
		printf("│");
		if(selected == 1) printf("\e[35m");
		printf("%-*s\e[0m", width/2-1, "    yes");
		if(selected == 0) printf("\e[35m");
		printf("%*s\e[0m│▒", width/2-2, "no    ");

		printf("\e[%d;%dH└", size.ws_row/2-2+3, size.ws_col/2-width/2);
		for(int i = 0; i < width-3; i++) printf("─");
		printf("┘▒\n");

		printf("\e[%d;%dH", size.ws_row/2-2+4, size.ws_col/2-width/2+2);
		for(int i = 0; i < width-2; i++) printf("▒");
		printf("\n");

		fflush(stdout);

		char input = getchar();
	
		switch(input)
		{
			case 'y':
				selected = 1;
				break;
			case'n':
				selected = 0;
				break;
			case 'l':
			case 'h':
				selected++;
				selected%=2;
				break;
			case '\n':
				return selected;
		}
	}
}

void interfacePrintDisplay(char *title, char *str, int margin, int alignR)
{
	printf("\n");
	printf("%*s┌%s", margin-1, "", title);
	for(int i = 0; i < size.ws_col-margin*2-3-strlen(title); i++) printf("─");
	printf("┐\n");

	printf("%*s│", margin-1, "");

	if(alignR)
		printf("%*s│▒\n", size.ws_col-margin*2-3, str);
	else
		printf("%-*s│▒\n", size.ws_col-margin*2-3, str);

	printf("%*s└", margin-1, "");
	for(int i = 0; i < size.ws_col-margin*2-3; i++) printf("─");
	printf("┘▒\n");

	printf("%*s", margin+1, "");
	for(int i = 0; i < size.ws_col-margin*2-2; i++) printf("▒");
	printf("\n");

}

void interfaceRun()
{
	int res;

	do
	{
		ioctl(0, TIOCGWINSZ, &size);

		switch(st_Update())
		{
			case RES_HELP:
				interfaceHelpMB();
				break;
			case RES_QUIT:
				if(interfaceQuitMB()) return;
				break;
		}
	}
	while(1);
}
