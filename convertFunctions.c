#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE 10
#define LENGTH 14

/* This file contains utility functions used to convert to binary */

/* Convert decimal to binary 12 bits */
void numToBinary(int firstNum, int extraWord[])
{
	int i;

	unsigned int first = 1 << (LENGTH - 3); /* Initialize most significant bit */
	if(firstNum < 0)
	{
		firstNum = (1 << (LENGTH - 2)) + firstNum; /* Convert to 2 complement */
		first = 1 << (LENGTH - 3);
	}

	for(i = 0; i < LENGTH - 2; i++)
	{
		if(firstNum & first)
			extraWord[i] = 1;
		else
			extraWord[i] = 0;

		first = first >> 1;
	}

}

/* Convert decimal to binary 4 bits */
void decimalToBinaryOPCODE(int num, int opCode[])
{	
	int i;
	for(i = 0; num > 0; i++)
	{
		opCode[i] = num%2;
		num=num/2;
	}
}

/* Set address of lable in word */
void setAddress1(char *lableName, int lineNumber, char *fName)
{
	char *temp = lableName;
	while(*temp != '\n' && *temp != '\0')
	{
		temp++;
	}
	
	*temp = '\0';

	writeFirstPassLable(lableName, lineNumber, fName);
}


/* 
The function checks wether the recived input is of integer type or not
Returns 1 if there is no issue, otherwise returns 0.
*/
int checkIfInt(char *input)
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
