#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "preAssembler.c"
#include "firstPass.c"

/* 
	Final project by Tomer Gresler 208633578
*/

/*
	This project is a two part pass assembler.
	First, we run over the origin file and trnaslte the macros
	Then, we run over the new file and translate the commands and data(.txt file will be created for you to view)
	Lastly, we run over the commands and data and translate them to machine language where 1 is / and 0 is .(.ob file)

	This file is the main file who starts the proccess.
*/

#define MAX_FILE 81

void clearOldFiles(char *fName);              
void clearTempFiles(char *fName);            

int main(int argc, char* argv[])
{
	int i;   
	char *str;  
	char *fName;      
	FILE *fp; 
	int error = 0;   

	str = calloc(MAX_FILE, sizeof(char));           
	fName = calloc(MAX_FILE, sizeof(char));        

	for(i = 1; i < argc; i++)
	{
		memset(fName, 0, MAX_FILE);         
		strcpy(fName, argv[i]);

		memset(str, 0, MAX_FILE);       

		strcpy(str, argv[i]);         

	 	strcat(str, ".as");  

		fp = fopen(str, "r");           

		if (fp == NULL) 
		{ 
			printf("Error: %s file could not be opened.\n", str);
	    	}
		else
		{
			/* Remove old files */
			clearOldFiles(fName);             
  			/* First part, we run over the origin file and trnaslte the macros */
			changeMacros(fp, fName);                       
  
			fclose(fp);     
			/* Second and third part */
			error = firstPassForLabels(fName);     

			memset(fName, 0, MAX_FILE);          
			strcpy(fName, argv[i]);      

			/* Critical error during creation, delete files */
			if(error)
			{
				printf("FILE %s WAS NOT PROCCESSED, HAS ERRORS.\n", fName);
				clearOldFiles(fName);
				memset(fName, 0, MAX_FILE);       
				strcpy(fName, argv[i]);  
			}     
			else
			{   
				printf("FILE %s HAS BEEN PROCCESSED SUCCSEFULLY.\n", fName);
			}                      

			/* Remove temp files used to store data */
			clearTempFiles(fName);    
		}       
	}

	exit(0); 
}

/* If the assembler was already called on a file with this name, remove all the old files that were created */
void clearOldFiles(char *fName)
{
	char *str;  
	str = calloc(MAX_FILE, sizeof(char));   
	
	strcpy(str, fName);
	strcat(str,".am");
	remove(str);
	str = strtok(str, ".");
	strcat(str,".txt");
	remove(str);
	str = strtok(str, ".");
	strcat(str,".ob");
	remove(str);
	str = strtok(str, ".");
	strcat(str,".ent");
	remove(str);
	str = strtok(str, ".");
	strcat(str,".ext");
	remove(str);
	str = strtok(str, ".");
	strcat(str,"ent.txt");
	remove(str);
	memset(str, 0, MAX_FILE);
	strcpy(str, fName);
	strcat(str,"ext.txt");
	remove(str);

	free(str);    
}

/* Remove temp files used to store entry and extern data (didnt remove 'fName'.txt so the evaluator can
 see the binary code before it is being translated to machine code) */
void clearTempFiles(char *fName)
{
	char *str;  
	str = calloc(MAX_FILE, sizeof(char));      
 
	strcpy(str, fName); 
	strcat(str,"ent.txt"); 
	remove(str);
	memset(str, 0, MAX_FILE);
	strcpy(str, fName);
	strcat(str,"ext.txt");
	remove(str);

	free(str);
}

