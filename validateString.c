#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_ERROR 81

/* This file checks for errors in syntax of all the lines according to their different types */

/* Check that after hashtag (#) comes a number */
int checkAfterHashtag(char *temp, int i, int lineNumber);

/* Check syntax for .data line, return 1 if there is error, otherwise return 0 */
int checkData(char *str, int lineNumber)
{
	char *temp;
	char *num;
	int counter = 0;
	int len = 0;
	char end[MAX_LINE_ERROR];
	int countEnd = 1;
	
	temp = calloc(MAX_LINE_ERROR, sizeof(char));
	num = calloc(MAX_LINE_ERROR, sizeof(char));
	/* Skip spaces and check that data doesn't end with comma */
	strcpy(end, str);
	while(isspace(end[strlen(end) - countEnd]))
	{
		countEnd++;
	}
	if(end[strlen(end) - countEnd] == ',')
	{
		printf("ERROR in line %d: .data can not end in a comma\n", lineNumber);
		return 1;
	}

	len = strlen(str);
	
	strcpy(temp, str);
	/* Run over the .data line contents and check for errors */
	while(counter < len)
	{
		num = strtok(temp, ",");
		
		if(str[strlen(num)+1] == ',')
		{
			printf("ERROR in line %d: .data can not have two commas in a row\n", lineNumber);
			return 1;
		}	
		
		while(isspace(*num))
		{
			num++;
			str++;
			counter++;
		}

		if(!checkIfInt(num))
		{
			printf("ERROR in line %d: .data can only receive numbers\n", lineNumber);
			return 1;
		}
		str += strlen(num);
		counter += strlen(num);

		while(isspace(*str))
		{
			temp++;
			counter++;
		}

		/* skip comma */
		str++;
		counter++;

		strcpy(temp, str);
	}
	return 0;
}

/* Check syntax for .string line, return 1 if there is error, otherwise return 0 */
int checkString(char *str,  int lineNumber)
{
	char *temp;
	
	temp = calloc(MAX_LINE_ERROR, sizeof(char));

	strcpy(temp, str);

	if(*temp != '"')
	{
		printf("ERROR in line %d: .string must start with quotation marks (\")\n", lineNumber);
		return 1;
	}

	while(*(temp+3) != '\0')
		temp++;

	if(*temp != '"')
	{
		printf("ERROR in line %d: .string must end with quotation marks (\")\n", lineNumber);
		return 1;
	}
	
	return 0;
}

/* Check syntax for order line style 0, return 1 if there is error, otherwise return 0 */
int checkOrder(char *str, int orderNum, int lineNumber)
{
	char *temp;
	int len = 0;
	int i, j;
	int commaCounter = 0;
	int commaBetweenOperands = 0;
	
	temp = calloc(MAX_LINE_ERROR, sizeof(char));

	strcpy(temp, str);

	/* mov cmp add sub lea */
	if(orderNum == 0 || orderNum == 1 || orderNum == 2 || orderNum == 3 || orderNum == 6)
	{
		len = strlen(temp);
		for(i = 0; i < len; i++)
		{
			if(*(temp+i) == ',')
				commaCounter++;
			else if(*(temp+i) == '#')
			{
				if(checkAfterHashtag(str, i, lineNumber))
					return 1;
			}
		}
		if(commaCounter != 1)
		{
			printf("ERROR in line %d: mov,cmp,add,sub,lea orders must have only one comma(2 operands)\n", lineNumber);
			return 1;
		}
	}
	/* not clr inc dec red prn */
	else if(orderNum == 4 || orderNum == 5 || orderNum == 7 || orderNum == 8 || orderNum == 11 || orderNum == 12)
	{
		len = strlen(temp);
		for(i = 0; i < len; i++)
		{
			if(*(temp+i) == ',')
				commaCounter++;
			else if(*(temp+i) == '#')
			{
				if(checkAfterHashtag(str, i, lineNumber))
					return 1;
			}
		}
		if(commaCounter != 0)
		{
			printf("ERROR in line %d: not,clr,inc,dec,red,prn orders must have no commas(0 operands)\n", lineNumber);
			return 1;
		}
	}
	/* jmp bne jsr */
	else if(orderNum == 9 || orderNum == 10 || orderNum == 13)
	{
		len = strlen(temp);
		for(i = 0; i < len; i++)
		{
			if(*(temp+i) == ',')
				commaCounter++;
			else if(*(temp+i) == '#')
			{
				if(checkAfterHashtag(str, i, lineNumber))
					return 1;
			}
			else if(*(temp+i) == '(')
			{
				j = 0;
				while(*(temp+i+j) != ')')
				{
					if(isspace(*(temp+i+j)))
					{
						printf("ERROR in line %d: jmp,bne,jsr orders cant have spaces between operands\n", lineNumber);
						return 1;	
					}
					else if(*(temp+i+j) == ',')
						commaBetweenOperands++;
					j++;
				}
				if(commaBetweenOperands != 1)
				{
					printf("ERROR in line %d: jmp,bne,jsr orders must receive 0 or 2 operands\n", lineNumber);
					return 1;	
				}
			}
		}
		if(commaCounter != 0 && commaCounter != 1)
		{
			printf("ERROR in line %d: jmp,bne,jsr orders must have 0 or 1 commas (0 or 2 operands)\n", lineNumber);
			return 1;
		}
	}

	return 0;
}

/* Check that extern lable does not apear in file */
int checkExternInFile(node *head, char *fName)
{
	char *str;
	FILE *fp;
	char *lable;
	int error = 0;
	str = calloc(MAX_LINE_ERROR, sizeof(char));
	lable = calloc(MAX_LINE_ERROR, sizeof(char));
	
	strcpy(str, fName);
	strtok(str, ".");
	
	strcat(str, "ext.txt");

	fp = fopen(str, "r");

	/* No extern in file */
	if(fp == NULL)
	{
		free(str);
		free(lable);
		return 0;
	}

	while((fgets(str, MAX_LINE_ERROR, fp)) != NULL)
	{
		lable = strtok(str, " ");
		strcat(lable, "\n");
		if(isLable(head, lable))
		{
			lable[strlen(lable) - 1] = ':'; 
			printf("ERROR with extern lable %s already defined as lable in file.\n", lable);
		
			error = 1;
		}
	}
	
	free(lable);
	fclose(fp);
	if(error)
		return 1;
	return 0;
}

/* Check that extern lable does not apear in file */
int checkExternEntry(char *fName)
{
	char *str;
	char *str1;
	FILE *fpEnt;
	FILE *fpExt;
	char *extName;
	char *entName;
	int error = 0;
	str = calloc(MAX_LINE_ERROR, sizeof(char));
	str1 = calloc(MAX_LINE_ERROR, sizeof(char));
	entName = calloc(MAX_LINE_ERROR, sizeof(char));
	extName = calloc(MAX_LINE_ERROR, sizeof(char));

	
	strcpy(str, fName);
	strtok(str, ".");
	strcat(str, "ent.txt");
	fpEnt = fopen(str, "r");

	/* No entry in file */
	if(fpEnt == NULL)
	{
		free(str);
		free(str1);
		free(entName);
		free(extName);
		return 0;
	}

	memset(str, 0, MAX_LINE_ERROR);
	strcpy(str, fName);
	strtok(str, ".");
	strcat(str, "ext.txt");
	fpExt = fopen(str, "r");

	/* No extern or no entry in file */
	if(fpExt == NULL)
	{
		free(str);
		free(str1);
		free(entName);
		free(extName);
		fclose(fpEnt);
		return 0;
	}

	while((fgets(str, MAX_LINE_ERROR, fpEnt)) != NULL)
	{
		entName = strtok(str, " ");

		while((fgets(str1, MAX_LINE_ERROR, fpExt)) != NULL)
		{
			extName = strtok(str1, " ");

			if(strcmp(entName, extName) == 0)
			{
				printf("ERROR with extern lable %s already defined as entry lable in file.\n", extName);

				error = 1;
			}
		}

		fseek(fpExt, 0, SEEK_SET);
	}
	
	free(extName);
	fclose(fpEnt);
	fclose(fpExt);
	if(error)
		return 1;
	return 0;
}

/* Check that after hashtag (#) comes a number */
int checkAfterHashtag(char *temp, int i, int lineNumber)
{
	int j = 0;
	char *c;
	/* Skip hashtag */
	i++;
	while(*(temp+i+j)=='-' || *(temp+i+j)=='+')
		j++;
	c = temp+i+j;
	if(!checkIfInt(c))
	{
		printf("ERROR in line %d: after '#' must come a number\n", lineNumber);
		return 1;	
	}
	return 0;
}


