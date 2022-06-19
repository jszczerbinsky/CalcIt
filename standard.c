#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include "interface.h"

#define ENTRY_SIZE 1024
#define HELP_ENTRY_WIDTH 20

extern struct winsize size;

const char *st_Help[] = {
	"c - Clear", 
	"e - Clear entry",
	"",
};

double st_ProcessEntry(char *str)
{
	if(str[0] == '\0') return 0;

	double l = atof(str);
	str[0] = '\0';
	return l;
}

void st_ExpressionPut(char **expressionStr, char * str)
{
	if(*expressionStr)
		(*expressionStr) = realloc(*expressionStr, (strlen(*expressionStr) + strlen(str) + 1) * sizeof(char));
	else
		(*expressionStr) = malloc((strlen(str) + 1)*sizeof(char));

	strcat(*expressionStr, str);
}

void st_ProcessLastOperation(char *entryStr, double *ans, int *commaInserted, char lastOperator)
{
	*commaInserted = 0;
	switch(lastOperator)
	{
		case '+':
			*ans += st_ProcessEntry(entryStr);
			break;
		case '-':
			*ans -= st_ProcessEntry(entryStr);
			break;
		case '/':
			*ans /= st_ProcessEntry(entryStr);
			break;
		case '*':
			*ans *= st_ProcessEntry(entryStr);
			break;
	}

}

int st_Update()
{
	static char *expressionStr = NULL;
	static char entryStr[ENTRY_SIZE] = "";
	static char ansStr[ENTRY_SIZE] = "";
	static unsigned int commaInserted = 0;
	static double ans = 0;
	static char lastOperator = '+';
	static unsigned int printAns = 0;
	static unsigned int clearAns = 0;
	static double mem = 0;

	if(!expressionStr) expressionStr = strdup("");

	int displayMarginSize = size.ws_col/10;
	printf("\e[2J\e[H");

	interfacePrintDisplay("Expression", expressionStr, displayMarginSize, 0);

	if(printAns)
	{
		snprintf(ansStr, ENTRY_SIZE, "%0.10lf", ans);
		for(int i = strlen(ansStr)-1; i >= 1; i--) 
		{
			if(ansStr[i] == '0') ansStr[i] = '\0';
			else if(ansStr[i] == '.')
			{
				ansStr[i] = '\0';
				break;
			}
			else break;
		}
		interfacePrintDisplay("Entry", ansStr, displayMarginSize, 1);
		printAns = 0;
	}
	else interfacePrintDisplay("Entry", entryStr, displayMarginSize, 1);

	double ansBackup;
	unsigned int ansBackuped = 0;

	if(clearAns) 
	{
		ansBackup = ans;
		ansBackuped = 1;
		ans = 0;
		clearAns = 0;
	}

	printf("\n\n");
	/*
	printf("%*s+ - Add\n", displayMarginSize, "");
	printf("%*s- - Subtract\n", displayMarginSize, "");
	printf("%*s* - Multiply\n", displayMarginSize, "");
	printf("%*s/ - Divide\n", displayMarginSize, "");
	printf("%*sc - Clear\n", displayMarginSize, "");
	printf("%*se - Clear entry\n", displayMarginSize, "");
	*/

	int helpEntryPerLine = (size.ws_col-displayMarginSize*2)/HELP_ENTRY_WIDTH;
	int i = 0;

	while(i < sizeof(st_Help)/sizeof(char*))
	{
		if(i%helpEntryPerLine == 0)
			printf("%-*s", displayMarginSize, "");
		printf("%-*s", HELP_ENTRY_WIDTH, st_Help[i]);
		if(i%helpEntryPerLine == helpEntryPerLine-1) printf("\n");
		i++;
	}


	fflush(stdout);
			
	char input = getchar();	

	if(input >= '0' && input <= '9')
		strncat(entryStr, &input, 1);

	switch(input)
	{
		case 127:
			int l = strlen(entryStr);
			if(l > 0)
			{
				if(entryStr[l-1] == '.') commaInserted = 0;
				entryStr[l-1] = '\0';
			}
			break;	
		case ',':
		case '.':
			if(!commaInserted)
			{
				const char *commaStr = ".";
				commaInserted = 1;
				strncat(entryStr, commaStr, 1);
			}
			break;
		case '+':
		case '-':
		case '/':
		case '*':
			if(entryStr[0] != '\0')
			{
				st_ExpressionPut(&expressionStr, entryStr);
				st_ExpressionPut(&expressionStr, " ");
				char operatorStr[2] = { input, '\0' };
				st_ExpressionPut(&expressionStr, operatorStr);
				st_ExpressionPut(&expressionStr, " ");
				st_ProcessLastOperation(entryStr, &ans, &commaInserted, lastOperator);
			}
			else
			{
				if(ansBackuped)
				{
					ans = ansBackup;
					st_ExpressionPut(&expressionStr, "Ans ");
					char operatorStr[2] = { input, '\0' };
					st_ExpressionPut(&expressionStr, operatorStr);
					st_ExpressionPut(&expressionStr, " ");

				}
				else
				{
					if(!expressionStr || expressionStr[0] == '\0')
					{
						st_ExpressionPut(&expressionStr, "0 ");
						char operatorStr[2] = { input, '\0' };
						st_ExpressionPut(&expressionStr, operatorStr);
						st_ExpressionPut(&expressionStr, " ");
					}
					else
						expressionStr[strlen(expressionStr)-2] = input;
				}
				}
			lastOperator = input;
			printAns = 1;
			break;
		case '=':
		case '\n':
			st_ProcessLastOperation(entryStr, &ans, &commaInserted, lastOperator);
			free(expressionStr);
			expressionStr = strdup("");
			printAns = 1;
			clearAns = 1;
			lastOperator = '+';
			commaInserted = 0;
			for(int i = 0; i < strlen(entryStr); i++)
				if(entryStr[i] == '.') 
				{
					commaInserted = 1;
					break;
				}
			break;
		case 'e':
			commaInserted = 0;
			entryStr[0] = '\0';
			break;
		case 'c':
			commaInserted = 0;
			entryStr[0] = '\0';
			lastOperator = '+';
			free(expressionStr);
			expressionStr = strdup("");
			ans = 0;
			break;
		case 'p':
			if(ansBackuped) 
			{
				mem += ansBackup;
				ans = ansBackup;
				clearAns = 1;
				printAns = 1;
			}
			else mem += ans;
			break;
		case 'o':
			if(ansBackuped) 
			{
				mem -= ansBackup;
				ans = ansBackup;
				clearAns = 1;
				printAns = 1;
			}
			else mem -= ans;
			break;
		case 'n':
			mem = 0;	
			if(ansBackuped) 
			{
				mem += ansBackup;
				ans = ansBackup;
				clearAns = 1;
				printAns = 1;
			}
			break;
		case 'm':
			ans = mem;
			printAns = 1;
			clearAns = 1;
			free(expressionStr);
			expressionStr = strdup("");
			entryStr[0] = '\0';
			break;
		case 'q':
			return RES_QUIT;
			break;
	}

	return RES_NONE;
}
