#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "analyzeOrder.h"
#include "writeANDsecondPass.c"
#include "convertFunctions.c"

#define NUM_BYTES 14
#define OPCODE 4
#define MAX_LINE 81
#define ORDERS 16
#define ELSE 2
#define REGISTERY 8
#define STOP 15
#define MAX_LABLE 30

/* 
   This file is the heart of the project, where the magic happens
   here we turn all the text to numbers(that needed to be converted)
   including labels and orders, analyzing the string and sending it to be checked for errors
   before translating it and doing the hard work
*/

/* Static table for order names */
static char *orderNames[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts\n", "stop\n"};

/* Static table for order op code value in decimal */
static int orderValue[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

/* Static table for registery names */
static char *registeryNames[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

/* turn lable to word */
int lableToWords(char *lableName, int lineNumber, char *lableContent, int order, char *fName)
{
	int i;
	while(isspace(*lableContent))
		lableContent++;
	if(strcmp(lableContent, "stop") == 0 || strcmp(lableContent, "stop\n\n") == 0)
		i = orderToWords(orderNames[STOP], 0, lineNumber, fName);
	else if(strcmp(lableContent, "rts") == 0 || strcmp(lableContent, "rts\n\n") == 0)
		i = orderToWords(orderNames[STOP-1], 0, lineNumber, fName);
	else
		i = orderToWords(lableContent, order, lineNumber, fName);
	return i;
}

/* Analyze and make words from the string, return how many words were created */
int orderToWords(char str[], int order, int lineNumber, char *fName)
{
	char *orderName;
	char *lableNameWord;
	char *lableNameWordEx;
	char *lableNameWordEx1;
	char temp[MAX_LINE];
	char *content;
	char *firstNum;
	char *firstParameter;
	char *secondParameter;
	int opCode[OPCODE] = {0};
	int arr[NUM_BYTES] = {0};
	int extraWord[NUM_BYTES] = {0};
	int extraWordTwo[NUM_BYTES] = {0};
	int extraWordThree[NUM_BYTES] = {0};
	int tempWord[NUM_BYTES] = {0};
	int i, num, counter, registery, origin, externLable;
	int ex, ex1, ex2;
	int wordsNum;
	int flag = 1;
	int callLable, callLableEx, callLableEx1;
	/* if both are 4 it means the extra word is a lable */
	ex = ex1 = ex2 = wordsNum = callLable = callLableEx = callLableEx1 = 0;
	lableNameWord = calloc(MAX_LABLE, sizeof(char));
	lableNameWordEx = calloc(MAX_LABLE, sizeof(char));
	lableNameWordEx1 = calloc(MAX_LABLE, sizeof(char));
	externLable = 0;
	registery = 0;


	i = 0;
	while(i < REGISTERY && flag)
	{
		if(strstr(str,registeryNames[i]))
		{
			registery = 1;
			flag = 0;
		}
		else i++;
	}

	strcpy(temp, str);

	content = calloc(strlen(str), sizeof(char));
	
	/* Order line */
	if(order)
	{
		orderName = strtok(temp, " ");
		strcpy(content, str+strlen(orderName)); 

		/* skip spaces */
		while(isspace(*content))
			content++;

		/* Find order value for this order */
		i = 0;
		flag = 1;
		while(i < ORDERS && flag)
		{
			if(strcmp(orderNames[i], orderName) == 0) flag = 0;
			else i++;
		}

		/* The order is stop or rts */
		if(i == STOP || i == STOP-1)
		{
			if(*content == '\0')
			{
				if(i == STOP-1)
				{
					for(i = 9; i >= 7; i--)
						arr[i] = 1;
				}
				else
				{
					for(i = 9; i >= 6; i--)
						arr[i] = 1;
				}
				writeFirstPass(lineNumber, arr, 0, 0, fName);
				wordsNum++;
				return wordsNum;
			}
			else
			{
				printf("ERROR: stop and rts orders can't have chars after it");
				return -1;
			}
		}

		if(checkOrder(content, i, lineNumber))
			return -1;

		decimalToBinaryOPCODE(orderValue[i], opCode);
		
		for(i = 9; i >= 6; i--)
			arr[i] = opCode[i-OPCODE-ELSE];

		/* Immidiate (0) */
		if(*content == '#')
		{
			content++;
			/* add syntax checks */
		
			if(checkIfInt(content))
			{
				counter = 0;
				/* Advance until comma */
				while(*content != ',' && !(isspace(*content)) && *content != '\0')
				{
					content++;
					counter++;
				}
				/* save the number */
				firstNum = calloc(counter, sizeof(char));		
				memcpy(firstNum, content - counter, counter);
				num = atoi(firstNum);

				numToBinary(num, extraWord);
				ex=1;

				while(isspace(*content))
					content++;
				
				if(*content != '\0')
				{
					/* Skip the comma */
					content++;

					for(i = 3; i >= 2; i--)
					{
						if(registery)
							arr[i] = 1;
						else if(i == 2)
							arr[i] = 1;
					}
				}

				/*free(firstNum);*/
	
			}
		}
		/* Jump (2) */
		else if(strstr(str, "("))
		{
			/* add check that it is jump function */
			/* add check no spaces anywhere */
			arr[3] = 1;			

			counter = 0;
			origin = 0;

			/* Advance until parenthesis = '(' */
			while(*content != '(')
			{
				content++;
				counter++;
			}

			/* save the lable name and make new word for its address */
			firstNum = calloc(counter, sizeof(char));		
			memcpy(firstNum, content - counter, counter);
			strcpy(lableNameWord, firstNum);
			callLable = 4;
			ex = 4;

			/* Skip the parenthesis = '(' */
			content++;
			counter = 0;

			/* Advance until comma */
			while(*content != ',')
			{
				content++;
				counter++;
			}
			firstParameter = calloc(counter, sizeof(char));			
			memcpy(firstParameter, content - counter, counter);

			/* Skip comma */
			content++;
			counter = 0;

			/* add check that the string ends with ')' and doesnt continue after */
			while(*content != ')')
			{
				content++;
				counter++;
			}
			secondParameter = calloc(counter, sizeof(char));
			memcpy(secondParameter, content - counter, counter);

			registery = 0;
			for(i = 0; i < REGISTERY; i++)
			{
				if(strcmp(registeryNames[i], firstParameter) == 0)
				{
					origin = 1;
					registery++;
				}
				if(strcmp(registeryNames[i], secondParameter) == 0)
				{
					origin = 0;
					registery++;
				}
			}

			/* Only one parameter is registery */
			if(registery == 1)
			{
				if(origin)
				{
					for(i = 13; i >= 12; i--)
						arr[i] = 1;
					if(*secondParameter != '#')
					{
						arr[10] = 1;
						strcpy(lableNameWordEx, secondParameter);
						callLableEx = 4;
						ex1 = 4;
					}
					else
					{
						secondParameter++;
						num = atoi(secondParameter);
						numToBinary(num, extraWordThree);
					}

					firstParameter++;
					num = atoi(firstParameter);
					numToBinary(num, tempWord);

					for(i = 0; i < NUM_BYTES; i++)
					{
						if(tempWord[i] == 1)
							extraWordTwo[i-6] = 1;
					}

					ex1=ex2=1;
				}
				else
				{
					
					for(i = 11; i >= 10; i--)
						arr[i] = 1;

					if(*firstParameter != '#')
					{
						arr[12] = 1;
						strcpy(lableNameWordEx, firstParameter);
						callLableEx = 4;
						ex1 = 4;
					}
					else
					{
						firstParameter++;
						num = atoi(firstParameter);
						numToBinary(num, extraWordTwo);
					}

					secondParameter++;
					num = atoi(secondParameter);

					numToBinary(num, extraWordThree);
					ex1=ex2=1;
				}
	
			}
			/* Both parameters are registery*/
			else if(registery == 2)
			{
				for(i = 13; i >= 10; i--)
					arr[i] = 1;

				
				secondParameter++;
				num = atoi(secondParameter);
				numToBinary(num, extraWordTwo);

				firstParameter++;
				num = atoi(firstParameter);
				numToBinary(num, tempWord);
				for(i = 0; i < NUM_BYTES; i++)
				{
					if(tempWord[i] == 1)
						extraWordTwo[i-6] = 1;
				}
				ex1=1;
			}
			/* No registery */
			else
			{
				if(*firstParameter == '#' && *secondParameter == '#')
				{
					/* error? */
				}
				else
				{
					if(*firstParameter == '#')
					{
						firstParameter++;
						num = atoi(firstParameter);
						numToBinary(num, extraWordTwo);
				
						strcpy(lableNameWordEx, secondParameter);
						callLableEx = 4;
						ex1 = 4;

						arr[10] = 1;
					}
					else if(*secondParameter == '#')
					{
						secondParameter++;
						num = atoi(secondParameter);
						numToBinary(num, extraWordTwo);

						strcpy(lableNameWordEx, firstParameter);
						callLableEx = 4;
						ex1 = 4;

						arr[12] = 1;
					}
					else
					{
						arr[10] = 1;
						arr[12] = 1;

						strcpy(lableNameWordEx, firstParameter);
						callLableEx = 4;
						ex1 = 4;
						strcpy(lableNameWordEx1, secondParameter);
						callLableEx1 = 4;
						ex2 = 4;
					}
					/* ex1=ex2=1; */
				}
			}
			
			/*free(firstNum);
			free(firstParameter);
			free(secondParameter);*/

		}
		/* Register Direct (3) */
		else if(registery)
		{
			counter = 0;
			origin = 0;
			ex = 1;
			/* Advance until comma */
			while(*content != ',' && !(isspace(*content)))
			{
				content++;
				counter++;
			}
			/* save the origin */
			firstNum = calloc(counter, sizeof(char));
			memcpy(firstNum, content - counter, counter);

			/* skip spaces */
			while(isspace(*content))
				content++;
			/* Skip the comma */
			content++;
			/* skip spaces */
			while(isspace(*content))
				content++;

			i = strlen(content);
			if(content[i - 1] == '\n')
				content[i - 1] = '\0';

			registery = 0;
			for(i = 0; i < REGISTERY; i++)
			{
				if(strcmp(registeryNames[i], firstNum) == 0)
				{
					origin = 1;
					registery++;
				}
				if(strcmp(registeryNames[i], content) == 0)
				{
					origin = 0;
					registery++;
				}
			}

			/* Only one registery */
			if(registery == 1)
			{
				if(origin)
				{
					for(i = 5; i >= 4; i--)
						arr[i] = 1;
					if(*content != '#')
						arr[2] = 1;

					firstNum++;
					num = atoi(firstNum);

					numToBinary(num, tempWord);

					for(i = 0; i < NUM_BYTES; i++)
					{
						if(tempWord[i] == 1)
							extraWord[i-6] = 1;
					}

					strcpy(lableNameWordEx, content);
					callLableEx = 4;
					ex1 = 4;
				}
				else
				{
					for(i = 3; i >= 2; i--)
						arr[i] = 1;

					if(*firstNum != '#')
						arr[4] = 1;	

					content++;
					num = atoi(content);

					numToBinary(num, extraWord);

					strcpy(lableNameWordEx, content);
					callLableEx = 4;
					ex1 = 4;
				}
				
			}
			/* Two registery */
			else
			{
				for(i = 5; i >= 2; i--)
					arr[i] = 1;

				content++;
				num = atoi(content);
				numToBinary(num, extraWord);

				firstNum++;
				num = atoi(firstNum);
				numToBinary(num, tempWord);
				for(i = 0; i < NUM_BYTES; i++)
				{
					if(tempWord[i] == 1)
						extraWord[i-6] = 1;
				}
			}

			/*free(firstNum);*/
			
		}
		/* Direct(lable) (1) */
		else
		{
			if(*content != '\0')
			{
				arr[2] = 1;
				/* get address and put it in extra word */
				strcpy(lableNameWord, content);
				callLable = 4;
				ex = 4;
							
	
				if(externLable)
					extraWord[13] = 1;
					/* era = 01 */
				else
					extraWord[12] = 1;
					/* era = 10 */
			}
		}

		writeFirstPass(lineNumber, arr, 0, 0, fName);
		wordsNum++;

		if(ex)
		{
			if(ex == callLable)
			{
				lineNumber++;
				setAddress1(lableNameWord, lineNumber, fName);
				wordsNum++;
			}
			else
			{
				lineNumber++;
				writeFirstPass(lineNumber, extraWord, 1, 0, fName);
				wordsNum++;
			}
			
		}

		if(ex1)
		{
			if(ex1 == callLableEx)
			{
				lineNumber++;
				setAddress1(lableNameWordEx, lineNumber, fName);
				wordsNum++;
			}
			else
			{
				lineNumber++;
				writeFirstPass(lineNumber, extraWordTwo, 1, 0, fName);
				wordsNum++;
			}
		}

		if(ex2)
		{
			if(ex2 == callLableEx1)
			{
				lineNumber++;
				setAddress1(lableNameWordEx1, lineNumber, fName);
				wordsNum++;
			}
			else
			{
				lineNumber++;
				writeFirstPass(lineNumber, extraWordThree, 1, 0, fName);
				wordsNum++;
			}
		}
	}
	/* stop or rts order in lable */
	else
	{
		/* Find stop or rts */
		i = 14;
		flag = 1;
		while(i < ORDERS && flag)
		{
			if(strcmp(orderNames[i], str) == 0) flag = 0;
			else i++;
		}

		/* rts */
		if(i == STOP-1)
		{
			for(i = 9; i >= 7; i--)
				arr[i] = 1;
		}
		/* stop */
		else
		{
			for(i = 9; i >= 6; i--)
				arr[i] = 1;
		}
		writeFirstPass(lineNumber, arr, 0, 0, fName);
		wordsNum++;
		return wordsNum;
		
	}

	return wordsNum;
}
