#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preAssembler.h"

#define MACRO "mcr"
#define ENDMACRO "endmcr"
#define MAX_LINE 81
#define MAX_SYMBOL 31

/* 
	Final project by Tomer Gresler 208633578
	preAssembler part(pre run over file)
*/

/* 
	This program receives a file
	it then runs through the file searching for macro's
	macro's are represented by 'mcr' in the file
	if the program finds any macro, it copies its name and content to a linked list
	the program then creates and writes a new file, copying the received file to it with changes
	the changes are that our new file will contain no macro's,
	and in any place where there was a call for macro(represented by the name of the macro)
	it will be changed to the content
	the new file will be called 'afterStep1.am'
 */

/* Linked list for our macro names and their content, to later be put into our new file */
struct macroList{
	char *macroName;
	char *macroContent;
	struct macroList * next;
};

/* Find all macro names in the file and put them in the linked list */
void changeMacros(FILE *fptr, char *fName)
{
	char str[MAX_LINE];
	char *pos;
	int countMacros = 0;
	char *name;
	node_mcr * temp = NULL; /* Pointers for linked list nodes */
	node_mcr * head = NULL;
	node_mcr * p = NULL;
	FILE *afterStep1; /* Our new file pointer */

	strcat(fName, ".am");

	/* Create and open our new file */
	afterStep1 = fopen(fName, "w");

	/* Find all macros and their content, and put into linked list */
	while((fgets(str, MAX_LINE, fptr)) != NULL)
	{
		pos = strstr(str, MACRO);
		/* Found new macro */
		if(pos != NULL)
		{
			/* Get the namae of the macro */
			pos += strlen(MACRO);
			name = strtok(pos, " ");

			if(countMacros == 0)
			{
				/* First macro make him the head of our list */
				head = createNodeMcr(fptr, name, pos);
			}
			else
			{
				/* New macro, but not first. add to list at last place */
				temp = createNodeMcr(fptr, name, pos);
				p = head;
				while(p->next != NULL)
					p = head->next;

				p->next = temp;
			}
			/* Count how many macros we have */
			countMacros++;
		}/* End if where we found new macro */
	}/* End while */

	fseek(fptr, 0, SEEK_SET);

	write(fptr, head, afterStep1);

	clearMcr(head);

	fclose(afterStep1);
}

/* Write our new file replacing all macros */
void write(FILE *fptr, node_mcr *head, FILE *afterStep1)
{
	char str[MAX_LINE];
	char *token;
	char *pos;
	node_mcr *p;
	int i, allSpace; /* Variables to check if a line if all spaces */
	int flag = 1;

	while((fgets(str, MAX_LINE, fptr)) != NULL)
	{
		token = strstr(str, MACRO);
		/* Found new macro */
		if(token != NULL)
		{
			/* Continue to the end of the macro WITHOUT writing it to the new file */
			while(fgets(str, MAX_LINE, fptr) != NULL && flag)
			{
				token = strtok(str, "\n");
				/* Reached endmcr */
				if (token != NULL && strcmp(token, ENDMACRO) == 0)
					flag = 0;
			}
		}
		/* We are not on macro so lets write to the file */
		else
		{
			p = head;
			/* Check if the line we are going to write is a macro name, if so then
			write the macro content instead, if not then just write the line */
			while(p != NULL && flag)
			{
				pos=strstr(str,p->macroName);
				if(pos!=NULL)
					flag = 0;
				p = p->next;
			}
			if(p == NULL)
			{
				allSpace = 1;
				for(i = 0; i < strlen(str); i++)
				{
					if(!isspace(str[i]))
						allSpace = 0;
				}
				/* Ignore lines who are only white spaces and notes (lines that start with ';')  */
				if(!allSpace)
				{
					if(str[0] != ';')
					{
						fputs(str, afterStep1);
					}
				}
			}
			else
			{
				fputs(p->macroContent, afterStep1);
			}
		}
	}

}

/* The function creates new node for each macro */
node_mcr *createNodeMcr(FILE *fptr, char *name, char *pos)
{
	int contentLen = 0;
	char str[MAX_LINE];
	char *token;
	node_mcr * temp;
	int flag = 1;

	/* Allocate memory for our new node */
	temp = (node_mcr *)malloc(sizeof(node_mcr));
	temp->macroName = calloc(strlen(name), sizeof(char));
	temp->macroContent = calloc(MAX_LINE, sizeof(char));
	temp->next = NULL;

	strcpy(temp->macroName, name);
		
	/* Write the content of the macro into our node */			
	while(fgets(str, MAX_LINE, fptr) != NULL && flag)
	{
		token = strtok(str, "\n");
		/* Reached endmcr */
		if (token != NULL && strcmp(token, ENDMACRO) == 0)
			flag = 0;

		temp->macroContent = realloc(temp->macroContent, contentLen + strlen(token) + 1);
		if(temp->macroContent == NULL)
		{
			printf("Error: could not reallocate memory.\n");
			flag = 0;
		}
		strcat(temp->macroContent, token);
		strcat(temp->macroContent, "\n");
		contentLen += strlen(token);
	}

	/* Redirect our pointer from changeMacros to the endmcr line
	where our macro ends, so that strstr will work for any more macros */
	pos = token;

	return temp;
}

/* Free all memory allocated for the linked list */
void clearMcr(node_mcr *head)
{
	if(head == NULL)
		return;
	clearMcr(head->next);
	free(head->macroName);
	free(head->macroContent);
	free(head);
}

