#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "writeANDsecondPass.h"
#include "validateString.c"

/* 
	This code writes all the files and translate the file to machine code (1=/, 0=.) in the end
*/

#define MAX_LINE_SIZE 30
#define MAX_LINE_SIZE_LABLE 31
#define WORD_SIZE 14
#define MAX_F 81

/* Write first pass content which is lable name to a temp file*/ 
void writeFirstPassLable(char *lableName, int lineNumber, char *fName)
{
	char *str;
	FILE *fp;
	
	fName = strtok(fName, ".");

	strcat(fName, ".txt");

	fp = fopen(fName, "a+");
    	if (fp == NULL) 
	{
        	printf("Error: file could not be opened or created.\n");
        	return;
    	}
	
	str = calloc(MAX_LINE_SIZE_LABLE, sizeof(char));

	sprintf(str, "%d ", lineNumber);

	strcat(str, lableName);
	strcat(str, "\n");

	fputs(str, fp);

	fclose(fp);
}

/* Write the first pass content to a temp file */
void writeFirstPass(int lineNumber, int word[], int ex, int directive, char *fName)
{
	int i;
	char *str;
	FILE *fp;

	fName = strtok(fName, ".");	

  	strcat(fName, ".txt");

	fp = fopen(fName, "a+");

    	if (fp == NULL) 
	{
        	printf("Error: file could not be opened or created.\n");
        	return;
    	}

	if(directive)
	{
		str = calloc(MAX_F, sizeof(char));
		sprintf(str, "%d D", lineNumber);
	}
	else
	{
		str = calloc(MAX_LINE_SIZE, sizeof(char));
		sprintf(str, "%d ", lineNumber);
	}

	if(!ex)
	{
		for(i = WORD_SIZE-1; i>=0; i--)
		{
			if(i != 0)
				sprintf(str + strlen(str), "%d", word[i]);
			else
				sprintf(str + strlen(str), "%d\n", word[i]);
		}
	}
	else
	{
		for(i = 0; i < WORD_SIZE; i++)
		{
			if(i != WORD_SIZE - 1)
				sprintf(str + strlen(str), "%d", word[i]);
			else
				sprintf(str + strlen(str), "%d\n", word[i]);
		}
	}

	fputs(str, fp);

	fclose(fp);

	free(str);
}

/* Create and write the final files - turn numbers to machine language */
int writeSecondPass(node *head, int IC, int DC, char *fName)
{
	/* Mostly temp elements to hold the values of the lines we are reading which are in number (0001000...) to machine (.../...) */
	int i = 0;
	FILE *fp;
	FILE *fpOb;
	char *fNameOriginal;
	char *fNameOB;
	char str[MAX_LINE_SIZE];
	char write[MAX_LINE_SIZE];
	char wordBaseTwo[WORD_SIZE + 2];
	char ICDC[MAX_LINE_SIZE];
	char *lineNumber;
	int lableValue = 0;
	int line = 0;
	int dataLine = 0;
	char *word;
	char *tempSTR;
	int wordLableName[WORD_SIZE] = {0};
	int wordNew[WORD_SIZE] = {0};
	int directCounter = 0;
	word = calloc(MAX_LINE_SIZE, sizeof(char));
	tempSTR = calloc(MAX_LINE_SIZE, sizeof(char));
	fNameOB = calloc(MAX_LINE_SIZE, sizeof(char));
	fNameOriginal = calloc(MAX_LINE_SIZE, sizeof(char));
	
	strcpy(fNameOriginal, fName);
	fName = strtok(fName, ".");
	strcpy(fNameOB, fName);
	strcat(fName, ".txt");
	fp = fopen(fName, "r");
	strcat(fNameOB, ".ob");
	fpOb = fopen(fNameOB, "a+");

    	if (fp == NULL) 
	{
        	printf("Error: %s file could not be opened or created.\n", fName);
        	return 1;
    	}

	sprintf(ICDC, "     %d   %d\n", IC, DC);
	fputs(ICDC, fpOb);

	while((fgets(str, MAX_LINE, fp)) != NULL) 
	{
		strcpy(tempSTR, str);
		lineNumber = strtok(str, " ");
		line = atoi(lineNumber);
    		strcpy(word, tempSTR+strlen(lineNumber)+1);	
	
		lableValue = isLable(head, word);
	
		/* lable */
		if(lableValue)
		{
			sprintf(write, "0%d ", line - directCounter);
			
			lableNameToWrd(lableValue, wordLableName);
			/* ERA */
			wordLableName[12] = 1;
			
			wordToUniqueBaseTwo(wordBaseTwo, wordLableName);

			strcat(write, wordBaseTwo);

			fputs(write, fpOb);
	
		}
		/* data line */
		else if(word[0] == 'D' && (word[1] == '0' || word[1] == '1'))
		{
			directCounter++;
		}
		/* word */
		else if(checkIfInt(word))
		{
			sprintf(write, "0%d ", line - directCounter);
			for(i = 0; i < WORD_SIZE; i++)
			{
				if(word[i] == '1')
					wordNew[i] = 1;
				else
					wordNew[i] = 0;
			}

			wordToUniqueBaseTwo(wordBaseTwo, wordNew);

			strcat(write, wordBaseTwo);

			fputs(write, fpOb);
		}
		/* Extern */
		else
		{
			sprintf(write, "0%d ", line - directCounter);

			for(i = 0; i < WORD_SIZE - 1; i++)
			{
				wordNew[i] = 0;
			}

			wordNew[13] = 1;

			wordToUniqueBaseTwo(wordBaseTwo, wordNew);

			strcat(write, wordBaseTwo);

			fputs(write, fpOb);

			strcpy(tempSTR, str);
			lineNumber = strtok(str, " ");
			line = atoi(lineNumber);
    			strcpy(word, tempSTR+strlen(lineNumber)+1);

			writeExternSecond1(word, line - directCounter, fName);
		}
	}

	/* Move the file position indicator to the beginning of the file */
	fseek(fp, 0, SEEK_SET); 

	/* Write all data lines AFTER all order lines */
	dataLine = line - directCounter;

	while((fgets(str, MAX_LINE, fp)) != NULL) 
	{
		strcpy(tempSTR, str);
		lineNumber = strtok(str, " ");
    		strcpy(word, tempSTR+strlen(lineNumber)+1);	

		if(word[0] == 'D' && (word[1] == '0' || word[1] == '1'))
		{
			dataLine++;
		
			sprintf(write, "0%d ", dataLine);

			for(i = 1; i < WORD_SIZE + 1; i++)
			{
				if(word[i] == '1')
					wordNew[i-1] = 1;
				else
					wordNew[i-1] = 0;
			}

			wordToUniqueBaseTwo(wordBaseTwo, wordNew);

			strcat(write, wordBaseTwo);

			fputs(write, fpOb);
		}
	}	

	fclose(fp);

	fclose(fpOb);

	strcpy(fName, fNameOriginal);

	writeEntrySecond(head, fName);

	strcpy(fName, fNameOriginal);

	if(checkExternInFile(head, fName))
		return 1;
	else if(checkExternEntry(fName))
		return 1;

	free(word);
	free(tempSTR);
	free(fNameOB);
	free(fNameOriginal);

	return 0;
}

/* Turn word to our base two represantation, 1 = '/', 0 = '.' */
void wordToUniqueBaseTwo(char *str, int word[])
{
	int i = 0;
	
	if(word[0] == 'D')
	{
		for(i = 1; i < WORD_SIZE + 1; i++)
		{
			if(word[i - 1] == 1)
                		str[i] ='/';
            		else
                		str[i] ='.';
		}
	}
	else
	{
		for(i = 0; i < WORD_SIZE; i++)
		{
			if(word[i] == 1)
                		str[i] ='/';
            		else
                		str[i] ='.';
		}
	}
	
	str[WORD_SIZE] = '\n';
	str[WORD_SIZE+1] = '\0';

}

/* Create and write the first entry file (ent.txt) to hold inital entry values */
void writeEntry(char *lableName, int lineNumber, char *fName)
{
	char *str;
	FILE *fp;
	char *end;
	char *fName1;
	fName1 = calloc(MAX_F, sizeof(char)); 
	str = calloc(MAX_F, sizeof(char));

	strcpy(fName1, fName);

	fName1 = strtok(fName1, ".");

	end = fName1 + strlen(fName1) - 3;

	if (strcmp(end, "ent") == 0)
	{
		strcat(fName1, ".txt");
	}
	else
	{
		strcat(fName1, "ent.txt");
	}

	fp = fopen(fName1, "a+");


    	if (fp == NULL)
	{
        	printf("Error: file could not be opened or created.\n");
        	return;
    	}

	if(lableName[strlen(lableName)-1] == '\n')
		lableName[strlen(lableName)-1] = '\0';

	strcat(str, lableName);
	str[strlen(lableName)] = ' ';
	strcat(str, lableName);
	strcat(str, "\n");

	fputs(str, fp);

	fclose(fp);

	free(str);
	free(fName1);
}

/* Create and write the first extern file (ext.txt) to hold inital extern values */
void writeExtern(char *lableName, int lineNumber, char *fName)
{
	char *str;
	FILE *fp;
	char *end;
	char *fName1;
	fName1 = calloc(MAX_F, sizeof(char)); 
	str = calloc(MAX_F, sizeof(char));

	strcpy(fName1, fName);

	fName1 = strtok(fName1, ".");

	end = fName1 + strlen(fName1) - 3;

	if (strcmp(end, "ext") == 0)
	{
		strcat(fName1, ".txt");
	}
	else
	{
		strcat(fName1, "ext.txt");
	}

	fp = fopen(fName1, "a+");

    	if (fp == NULL)
	{
        	printf("Error: file could not be opened or created.\n");
        	return;
    	}

	if(lableName[strlen(lableName)-1] == '\n')
		lableName[strlen(lableName)-1] = '\0';

	strcat(str, lableName);
	str[strlen(lableName)] = ' ';
	strcat(str, lableName);
	strcat(str, "\n");

	fputs(str, fp);

	fclose(fp);

	free(fName1);
	free(str);
}

/* Turn lable name to word */
void lableNameToWrd(int lableValue, int word[])
{
	numToBinary(lableValue, word);
}

/* Create and write the final entry file ent after checking for errors */
void writeEntrySecond(node *head, char *fName)
{
	FILE *fp;
	FILE *fpEnt;
	char *fNameEnt;
	char str[MAX_LINE_SIZE];
	char write[MAX_LINE_SIZE] = "";
	char tmp[MAX_LINE_SIZE];
	char *lineNumber;
	int lableValue = 0;
	char *word;
	char *tempSTR;
	word = calloc(MAX_LINE_SIZE, sizeof(char));
	tempSTR = calloc(MAX_LINE_SIZE, sizeof(char));
	fNameEnt = calloc(MAX_LINE_SIZE, sizeof(char));

	fName = strtok(fName, ".");

	strcpy(fNameEnt, fName);

	strcat(fName, "ent.txt");
	fp = fopen(fName, "r");

    	if (fp == NULL)
	{
        	/* No entry */
        	return;
    	}

	strcat(fNameEnt, ".ent");
	fpEnt = fopen(fNameEnt, "a+");

	if (fpEnt == NULL)
	{
        	printf("Error: file could not be opened or created.\n");
        	return;
    	}

	while((fgets(str, MAX_LINE, fp)) != NULL) 
	{
		memset(write, 0, MAX_LINE_SIZE);
		lableValue = 0;

		strcpy(tempSTR, str);
		lineNumber = strtok(str, " ");
    		strcpy(word, tempSTR+strlen(lineNumber)+1);

		/* Check that entry lable apears as label in this file */
		lableValue = isLable(head, word);

		if(lableValue)
		{
			strcat(write, lineNumber);
			write[strlen(lineNumber)] = ' ';
			sprintf(tmp, "%d", lableValue);
			strcat(write, tmp);
			strcat(write, "\n");

			fputs(write, fpEnt);
		}
		else
		{
			printf("Excusable error: %s defined as entry but is not a lable in this file \n", word);
		}
	}

	fclose(fp);

	fclose(fpEnt);

	free(word);
	free(tempSTR);
	free(fNameEnt);
}

/* Create and write the extern file the final extern file ext after checking for errors  */
void writeExternSecond1(char *word, int lineNumber, char *fName)
{
	FILE *fpExt;
	char *fNameExt;
	char write[MAX_LINE_SIZE] = "";
	char *tmp;

	fNameExt = calloc(MAX_LINE_SIZE, sizeof(char));
	tmp = calloc(MAX_LINE_SIZE, sizeof(char));

	fName = strtok(fName, ".");

	strcpy(fNameExt, fName);

	strcat(fNameExt, ".ext");
	fpExt = fopen(fNameExt, "a+");

	if (fpExt == NULL)
	{
        	printf("Error: file could not be opened or created.\n");
        	return;
    	}

	strcat(write, word);
	if(write[strlen(write)-1] == '\n')
		write[strlen(write)-1] = ' ';

	write[strlen(word)] = ' ';
	sprintf(tmp, "%d", lineNumber);
	strcat(write, tmp);
	strcat(write, "\n");

	fputs(write, fpExt);

	fclose(fpExt);

	free(fNameExt);
	free(tmp);
}

