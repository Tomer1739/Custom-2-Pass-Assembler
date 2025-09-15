#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "firstPass.h"   
#include "analyzeOrder.c"
#include "analyzeDirective.c"  

#define MAX_LINE 81
#define ORDERS 16
#define REGISTERY 8
#define DIRECTIVES 4
#define MAX_LABEL_NAME 30
#define MAX_SIZE 128
#define BASE 10  

/* This is the first pass after preAssembler, here we make our linked list to hold all the labels in the file
   and their content, while also differanting orders and data, and counting the lines.
   in the end we start the second pass and turn everything to machine code */
 
/* Linked list for our labels, their name, content and position(value) */
struct labelList{
	char *labelName;   
	char *labelContent; 
	int labelValue; /* Adrress of lable in memory */
	int order; /* int to determine if the content of the label is order(1) or data(0) */
	struct labelList * next; 
};

/* 
	Final project by Tomer Gresler 208633578
	code part(first run over file)
*/

/* First pass over the preAssembler file to find labels and put them in our lniked list while checking for errors
return 1 if there is an error, otherwise return 0 */
int firstPassForLabels(char *fName)
{

	/* Counter for lines */ 
	int lineCounter = 100;   

	/* Counter for orders */
	int IC = 0;  
	   
	/* Counter for directives */ 
	int DC = 0;

	char str[MAX_LINE];
	char tempSTR[MAX_LINE];
	char content[MAX_LINE]; 
	char *name;
	int order; /* Boolean to say if the line we are on is an order or label */
	node * head = NULL;/* Pointers for linked list nodes */
	node * temp = NULL;
	node * p = NULL;
	int labelError = 0; /* Boolean for error in label */
	int error = 0; /* Boolean for error in file */
	int errorSeceondPass = 0; /* Boolean for error in file during second pass */
	int i = 0; 
	FILE *fptr;

	fName = strtok(fName, ".");
	strcat(fName, ".am");

	fptr = fopen(fName, "r");

	if (fptr == NULL) 
	{
        	printf("Error: file could not be opened or created.\n");
        	return 0;
    	}
 
	/* Go through the file */  
	while((fgets(str, MAX_LINE, fptr)) != NULL) 
	{
		/* If there was an error on the last label creation, mark that there is an error in the file */
		if(labelError)
			error = 1;

		order = 0;
		strcpy(tempSTR,str);
		if(strcmp(str, orderNames[STOP]) == 0 || strcmp(str, orderNames[STOP-1]) == 0)
		{
			order = 1;
		}
		else
		{
			if(!strstr(str, " "))
			{
				order = -1;
			}
			else
			{
				name = strtok(str, " ");
				if(name == NULL)
				{
					order = -1;
					return -1;
				}
				/* Check if label or order  */
				else if(strlen(name) == 3 || strlen(name) == 4)
				{ 
					order = isOrder(name);
				}
			}
		}  

		/* Make a new label if it is not an order or Directive(.data for example) line while checking its syntax */
		if(!order)
		{
			/* Directive line(.data / .string / .entry / .extern) without label */
			if(isDirective(name))
			{ 
				i = dataToWords(tempSTR, lineCounter, fName);
				/* error */
				if(i == -1)
					error = 1;
				lineCounter = lineCounter + i;
				DC += i;
			} 
			/* Label */ 
			else  
			{    
				i = 0;
				while(isspace(tempSTR[i]))
					i++; 
 
				strcpy(content, tempSTR+strlen(name) + i);
 
				if(head == NULL)
				{
					/* First label make him the head of our list */
					head = createNode(fptr, name, content, lineCounter);
					labelError = checkLabel(head, NULL, lineCounter);

					if(!labelError)
					{
						name = strtok(content, " ");
						if(isOrder(name))
						{
							i = lableToWords(head->labelName, lineCounter, head->labelContent, head->order, fName);
							if(i == -1)
								error = 1;
							lineCounter = lineCounter + i;
							IC += i;
						}
						else
						{
							i = lableToWordsData(head->labelName, lineCounter, head->labelContent, fName);
							if(i == -1)
								error = 1;
							lineCounter = lineCounter + i;
							DC += i;
						}  
					}
				}
				else
				{
					/* New macro, but not first. add to list at last place */
					temp = createNode(fptr, name, content, lineCounter);
			  		labelError = checkLabel(temp, head, lineCounter);
				 	
					if(!labelError)
					{
				 		name = strtok(content, " ");
		
				 		if(isOrder(name))
						{
							i = lableToWords(temp->labelName, lineCounter, temp->labelContent, temp->order, fName);
							if(i == -1)
								error = 1;
							temp->labelValue = lineCounter;
							lineCounter = lineCounter + i;
					 		IC += i; 
						}
						else
						{ 
							i = lableToWordsData(temp->labelName, lineCounter, temp->labelContent, fName);
							if(i == -1)
								error = 1;
							lineCounter = lineCounter + i;
							DC += i;
						}
					


						p = head;    
		
			 	 	 	while(p->next != NULL)
							p = p->next;

						p->next = temp;
					}
				}
			}    
		} 
		else if(order == -1)
		{
			printf("ERROR in line %d: line must contain spaces(except for stop and rts orders)\n", lineCounter);
			error = 1;
		}
		/* Order */  
		else  
		{ 
			i = orderToWords(tempSTR, 1, lineCounter, fName);
			if(i == -1)
				error = 1;
			lineCounter = lineCounter + i;
			IC += i;
		}  
   
	}  

	errorSeceondPass = writeSecondPass(head, IC, DC, fName);

	clear(head);

	fclose(fptr); 

	if(error || errorSeceondPass || labelError)
		return 1;  

	return 0; 
}

/* The function checks the node for errors in syntax, returns 1 if there are errors, otherwise return 0 */
int checkLabel(node * temp, node * head, int lineCounter)
{
	char *name; 
	char *tempSTR;
	node * p = head; 
	tempSTR = calloc(MAX_LINE, sizeof(char));
	strcpy(tempSTR, temp->labelName);
	
	/* Check if label name was already used */
	while(p != NULL)
	{
		if(strcmp(temp->labelName, p->labelName) == 0)
		{
			printf("Error in line %d: label name already used.\n", lineCounter);
			return 1;
		}
		p = p->next;
	} 

	/* Check if label name contains comma */ 
	while(*tempSTR != '\0')
	{
		if(*tempSTR == ',')
		{
			printf("Error in line %d: label name cannot contain commas.\n", lineCounter);
			return 1;
		}
		tempSTR++;
	}
  
	/* Check if label name is too long */ 
	if(strlen(temp->labelName) > MAX_LABEL_NAME)
	{ 
		printf("Error in line %d: label name too long.\n", lineCounter);
		return 1;
	}     
  
	/* Check that label ends with ':' */
	if(temp->labelName[strlen(temp->labelName) - 1] != ':')
	{
		printf("Error in line %d: label must end in ':' and have no spaces in it.\n", lineCounter);
		return 1; 
	}

	/* Check that label starts with an alphabetic letter */
	if(isalpha(temp->labelName[0]) == 0)
	{
		printf("Error in line %d: first letter must be of the alphabet.\n", lineCounter);
		return 1;
	}

	name = temp->labelName;
	name--;/* Point to label name and remove last char (':') */
	/* Check that label name isnt equal to order name */
	if(isOrder(name))
	{
		printf("Error in line %d: label name can't be an order.\n", lineCounter);
		return 1;
	}
 
	/* Check that label name isnt equal to registery name */
	if(isRegistery(name)) 
	{
		printf("Error in line %d: label name can't be a register name.\n", lineCounter);
		return 1;
	}
	 
	
	return 0;
}

/* The function creates new node for each label */
node *createNode(FILE *fptr, char *name, char content[], int line)
{
	char tempSTR[MAX_LINE];

	node * temp = NULL;    
 
	temp = (node *)malloc(sizeof(node));
	temp->labelName = calloc(strlen(name), sizeof(char));
	strcpy(temp->labelName, name);
	temp->labelContent = calloc(MAX_LINE, sizeof(char));
	strcat(temp->labelContent, content);
	strcat(temp->labelContent, "\n"); 

	temp->labelValue = line; 

	strcpy(tempSTR, content);
	temp->order = isOrder(strtok(tempSTR, " "));

	temp->next = NULL;

	return temp; 
}

/* The function receives a string and checks if it is equal to an order  */
int isOrder(char *name)
{
	int i;
	for(i = 0; i < ORDERS; i++)
	{
		if(strcmp(name, orderNames[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/* The function receives a string and checks if it is equal to a directive  */
int isDirective(char *name)
{
	int i;
	for(i = 0; i < DIRECTIVES; i++)
	{
		if(strcmp(name, directiveNames[i]) == 0)
		{
			return 1;
		}
	}

	return 0;
}

/* The function receives a string and checks if it is equal to a registery  */
int isRegistery(char *name)
{ 
	int i;  
	for(i = 0; i < REGISTERY; i++) 
	{ 
		if(strcmp(name, registeryNames[i]) == 0)
			return 1;
	}
 
	return 0; 
} 

/* Free all memory allocated for the linked list */ 
void clear(node *head)
{
	if(head == NULL)
		return; 
	clear(head->next);    
	free(head->labelName);     
	free(head->labelContent);               
	free(head);    
}

/* Return the lable value if its lable, 0 if not */
int isLable(node *head, char *lableName)
{
	char str[MAX_LINE]; 
	node * p = head;

	/* Check if label name was already used */
	while(p != NULL)
	{
		strcpy(str, p->labelName);
		str[strlen(p->labelName) - 1] = '\n'; 
		if(strcmp(str, lableName) == 0)
			return p->labelValue;

		p = p->next;
	} 

	return 0;
}
