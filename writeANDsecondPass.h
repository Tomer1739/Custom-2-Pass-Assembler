/* Write first pass content which is lable name */
void writeFirstPassLable(char *lableName, int lineNumber, char *fName);

/* Write the first pass content to a temp file */
void writeFirstPass(int lineNumber, int word[], int ex, int directive, char *fName);

/* Create and write the final files */
int writeSecondPass(node *head, int IC, int DC, char *fName);

/* Create and write the entry file ent */
void writeEntry(char *lableName, int lineNumber, char *fName);

/* Create and write the extern file ext */
void writeExtern(char *lableName, int lineNumber, char *fName);

/* Turn lable name to word */
void lableNameToWrd(int lableValue, int word[]);

/* Turn word to our base two represantation, 1 = '/', 0 = '.' */
void wordToUniqueBaseTwo(char str[], int word[]);

/* Create and write the entry file ent */
void writeEntrySecond(node *head, char *fName);

/* Create and write the extern file ext */
void writeExternSecond1(char *word, int lineNumber, char *fName);
