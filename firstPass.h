
/* Linked list for our labels, their name, content and position(value) */
typedef struct labelList node;

/* First pass over the preAssembler file to find labels and put them in our lniked list while checking for errors */
int firstPassForLabels(char *fName);

/* The function checks the node for errors in syntax, returns 0 if there are errors, otherwise return 1 */
int checkLabel(node * temp, node * head, int lineCounter);

/* The function creates new node for each label */
node *createNode(FILE *fptr, char *name, char str[], int line);

/* The function receives a string and checks if it is equal to an order  */
int isOrder(char *name);

/* The function receives a string and checks if it is equal to a registery  */
int isRegistery(char *name);

/* The function receives a string and checks if it is equal to a directive  */
int isDirective(char *name);

/* Free all memory allocated for the linked list */
void clear(node *head);

/* The function checks wether the recived input is of integer type or not, and in the correct range
Returns 1 if there is no issue, otherwise returns 0. */
int checkIfInt(char *start);

/* Return the lable value if its lable, 0 if not */
int isLable(node *head, char *lableName);
