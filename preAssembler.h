
/* Linked list for our macro names and their content, to later be put into our new file */
typedef struct macroList node_mcr;

/* Find all macro names and put them in the linked list */
void changeMacros(FILE *fptr,  char *fName);

/* Create new node for each macro */
node_mcr *createNodeMcr(FILE *fptr, char *name, char *pos);

/* Write our new file */
void write(FILE *fptr, node_mcr *head, FILE *afterStep1);

/* Free all memory allocated for the linked list */
void clearMcr(node_mcr *head);
