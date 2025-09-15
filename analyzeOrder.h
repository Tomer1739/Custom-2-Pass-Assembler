
/* turn lable to word */
int lableToWords(char *lableName, int lineNumber, char *lableContent, int order, char *fName);

/* Analyze and make words from the string, return how many words were created */
int orderToWords(char str[], int order, int lineNumber, char *fName);

/* Convert decimal to binary 12 bits */
void numToBinary(int firstNum, int extraWord[]);

/* Convert decimal to binary 4 bits */
void decimalToBinaryOPCODE(int num, int opCode[]);

/* Set address of lable in word */
void setAddress1(char *lableName, int lineNumber, char *fName);

/* 
The function checks wether the recived input is of integer type or not
Returns 1 if there is no issue, otherwise returns 0.
*/
int checkIfInt(char *input);
