
/* turn lable to word */
int lableToWordsData(char *lableName, int lineNumber, char *lableContent, char *fName);

/* Analyze and make words from the string, return how many words were created */
int dataToWords(char str[], int lineNumber, char *fName);

/* Convert decimal to binary 12 bits */
void numToBinaryData(int firstNum, int extraWord[]);

/* */
void charToWord(char *temp, int lineNumber, int dir, char *fName);

/* 
The function checks wether the recived input is of integer type or not
Returns 1 if there is no issue, otherwise returns 0.
*/
int checkIfInt1(char *input);


