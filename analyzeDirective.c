#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "analyzeDirective.h"

#define NUM_BYTES 14
#define OPCODE 4
#define MAX_LINE 81
#define ORDERS 16
#define ELSE 2
#define BASE 10
#define REGISTERY 8
#define DIRECTIVES 4
#define MAX_DIRECTIVE_SIZE 7

/* 
   This file is a mimic of the heart of the project(analyzeOrder.c), 
   here we turn all the text to numbers of directives (.data, .string, .entry and .extern) lines in the file
*/

/* Static table for directive names */
static char *directiveNames[] = {".data", ".string", ".entry", ".extern"};

/* turn lable to word */
int lableToWordsData(char *lableName, int lineNumber, char *lableContent, char *fName)
{
	int i;
	while(isspace(*lableContent))
		lableContent++;
	i = dataToWords(lableContent, lineNumber, fName);
	return i;
}

/* Analyze and make words from the string, return how many words were created */
int dataToWords(char str[], int lineNumber, char *fName)
{
	int i;
	int dir;
	int wordsNum;
	int counter;
	int contentLen;
	char *tempSTR;
	char *tmp;
	char *content;
	char *newDirective;
	int error = 0;
	int validIntFlag = 1;
	content = calloc(strlen(str), sizeof(char));
	tempSTR = calloc(strlen(str), sizeof(char));
	tmp = calloc(strlen(str), sizeof(char));

	dir = wordsNum = counter = contentLen = 0;

	strcpy(content, str);
	
	strcpy(tempSTR, str);

	newDirective = strtok(tempSTR, " ");

	for(i = 0; i < DIRECTIVES; i++)
	{
		if(strcmp(newDirective, directiveNames[i]) == 0)
		{
			dir = i;
		}
	}

	switch(dir)
	{
		/* .data */
		case 0:
			for(i = 0; i <= strlen(newDirective); i++)
			{
				content++;
			}
			while(isspace(*content))
				content++;

			error = checkData(content, lineNumber);
			if(error)
				return -1;

			contentLen = strlen(content);
			counter = 0;

			while (counter < contentLen && validIntFlag)
			{
				strcpy(tmp, content);
				tempSTR = strtok(tmp, ",");
			    	if (!checkIfInt1(tempSTR))
					/* Not valid int - give error message */
					validIntFlag = 0;
			    	counter += strlen(tempSTR);
				content += strlen(tempSTR);
			    	charToWord(tempSTR, lineNumber, 0, fName);
			    	wordsNum++;
			    	lineNumber++;
				content++;
				counter++;
			}

		break;
		
		/* .string */
		case 1:
			for(i = 0; i <= strlen(newDirective); i++)
				content++;
			while(isspace(*content))
				content++;

			error = checkString(content, lineNumber);
			if(error)
				return -1;

			/* Skip '"' */
			content++;
			counter = 0;
			contentLen = strlen(content);

			while(counter < contentLen - 3 && *content != '\0')
			{
				charToWord(content, lineNumber, 1, fName);
				wordsNum++;
				content++;
				lineNumber++;
				counter++;
			}

			strcpy(content, "end");
			charToWord(content, lineNumber, 1, fName);
			lineNumber++;
			wordsNum++;
		break;

		/* .entry */
		case 2:
			for(i = 0; i <= strlen(newDirective); i++)
				content++;
			while(isspace(*content))
				content++;

			writeEntry(content, lineNumber, fName);
		break;

		/* .extern */
		case 3:
			for(i = 0; i <= strlen(newDirective); i++)
				content++;
			while(isspace(*content))
				content++;
			
			writeExtern(content, lineNumber, fName);
		break;
	}

	return wordsNum;
}

/* Turn text to numbers */
void charToWord(char *temp, int lineNumber, int dir, char *fName)
{
	int word[NUM_BYTES] = {0};
	int num;
	/* .data */
	if(dir == 0)
	{
		num = atoi(temp);
		numToBinaryData(num, word);
		writeFirstPass(lineNumber, word, 1, 1, fName);
	}
	/* .string */
	else if(dir == 1)
	{
		if(strcmp(temp, "end") == 0)
		{
			writeFirstPass(lineNumber, word, 1, 1, fName);
		}
		else
		{
			numToBinaryData((int)temp[0], word);
			writeFirstPass(lineNumber, word, 1, 1, fName);
		}
	}
}

/* 
The function checks wether the recived input is of integer type or not
Returns 1 if there is no issue, otherwise returns 0.
*/
int checkIfInt1(char *input)
{
	char *ptr;
	long num;
	num = strtol(input, &ptr, BASE);

	if (num == 0)
	{
		if (*input == '0')
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (num == -1)
		return 1;
	
	return 1;
}

/* Convert decimal to binary 12 bits */
void numToBinaryData(int firstNum, int extraWord[])
{
	int i;

	unsigned int first = 1 << (NUM_BYTES - 1); /* Initialize most significant bit */
	if(firstNum < 0)
	{
		firstNum = (1 << (NUM_BYTES)) + firstNum; /* Convert to 2 complement */
		first = 1 << (NUM_BYTES - 1);
	}

	for(i = 0; i < NUM_BYTES; i++)
	{
		if(firstNum & first)
			extraWord[i] = 1;
		else
			extraWord[i] = 0;

		first = first >> 1;
	}
}

