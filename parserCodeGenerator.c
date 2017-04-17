/*
	COP3402 Homework #4: Parser & Code Generator Assignment
	4/14/2017
	Philip Rodriguez & Steven Chen
	ph644520 & st140537
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
	Note that actual capacities listed below are one
	less due to null terminator requirements.
 */
#define MAX_INPUT_SIZE 32768
#define MAX_TOKEN_SIZE 64
#define MAX_SYMBOL_TABLE_SIZE 256
#define MAX_NUMBER_SIZE 5

/*
	A set, in this program, is a size SET_SIZE integer array
	filled with -1 for empty slots. Use setInit([set array])
	to fill a new set with -1's.
*/
#define SET_SIZE 256

/*
	If this is set to 0, then declared variables have
	a default value of 0. If it is set to 1, then
	declared variables must be assigned before they can
	be used.
*/
#define ENFORCE_VARIABLE_ASSIGNMENT 0


// --------------------------------------------------Begin Random Additional Code--------------------------------------------------

void eprintf(char * message, int token)
{
	FILE * errorFile = fopen("ef", "a");
	fprintf(errorFile, "An error occurred while parsing token number %d: %s\n", token, message);
	fclose(errorFile);
}

/*
	This method appends to the error file the error with code [code].
 */
void error(int code, int token)
{
    if (code == 1)
        eprintf("Use of = instead of :=.\n", token);
    if (code == 2)
        eprintf("= must be followed by a number.\n", token);
    if (code == 3)
        eprintf("Identifier must be followed by =.\n", token);
    if (code == 4)
        eprintf("const, var, procedure must be followed by identifier.\n", token);
    if (code == 5)
        eprintf("Semicolon or comma missing.\n", token);
    if (code == 6)
        eprintf("Incorrect symbol after procedure declaration.\n", token);
    if (code == 7)
        eprintf("Statement expected.\n", token);
    if (code == 8)
        eprintf("Incorrect symbol after statement part in block.\n", token);
    if (code == 9)
        eprintf("Period expected.\n", token);
    if (code == 10)
        eprintf("Semicolon between statements missing.\n", token);
    if (code == 11)
        eprintf("Undeclared identifier.\n", token);
    if (code == 12)
        eprintf("Assignment to constant or procedure is not allowed.\n", token);
    if (code == 13)
        eprintf("Assignment operator expected.\n", token);
    if (code == 14)
        eprintf("Call must be followed by an identifier.\n", token);
    if (code == 15)
        eprintf("Call of a constant or a variable is meaningless.\n", token);
    if (code == 16)
        eprintf("then expected.\n", token);
    if (code == 17)
        eprintf("Semicolon expected.\n", token); // I modified this one to not include "or }" because that makes no sense to me.
    if (code == 18)
        eprintf("do expected.\n", token);
    if (code == 19)
        eprintf("Incorrect symbol following statement.\n", token);
    if (code == 20)
        eprintf("Relational operator expected.\n", token);
    if (code == 21)
        eprintf("Expression must not contain a procedure identifier.\n", token);
    if (code == 22)
        eprintf("Right parenthesis missing.\n", token);
    if (code == 23)
        eprintf("The preceding factor cannot begin with this symbol.\n", token);
    if (code == 24)
        eprintf("An expression cannot begin with this symbol.\n", token);
    if (code == 25)
        eprintf("This number is too large.\n", token);
    if (code == 26)
        eprintf("The input file is too large.\n", token);
    if (code == 27)
        eprintf("Call must be followed by an identifier for a procedure.\n", token);
    if (code == 28)
        eprintf("Expected a factor, did not find one!\n", token);
    if (code == 29)
    	eprintf("Too many symbols, or a conflicting symbol was found!\n", token);
    if (code == 30)
    	eprintf("Read and write require an identifier after them!\n", token);
    if (code == 31)
    	eprintf("You can only read into a variable!\n", token);
    if (code == 32)
    	eprintf("You can only write a variable or constant!\n", token);
    if (code == 33)
    	eprintf("Use of unassigned variable!\n", token);
    if (code == 34)
    	eprintf("Program requires too many registers to run on the vm!\n", token);
    if (code == 35)
    	eprintf("Semicolon required after procedure declaration.\n", token);
    if (code == 36)
    	eprintf("Semicolon required after procedure block.\n", token);

}

/*
	This method does what it sounds like. It appends one character, [newChar], to
	the end of the passed in string, [string]. Assumes the passed in string is
	large enough to accomodate the extra character.
 */
void appendCharToString(char * string, char newChar)
{
    int len = (int) strlen(string);
    string[len] = newChar;
    string[len+1] = '\0';
}
// --------------------------------------------------End Random Additional Code--------------------------------------------------

// --------------------------------------------------Begin File Management Code--------------------------------------------------

//Our files!
FILE * inputFile;
FILE * outputFile;

/*
	This method will simply open the files, and thus populate
	the variables inputFile and outputFile with meaningful
	addresses.
 */
void openFiles(char* inputFilePath, char* outputFilePath)
{
    inputFile = fopen(inputFilePath, "r");
    outputFile = fopen(outputFilePath, "w");

}
// --------------------------------------------------End File Management Code--------------------------------------------------

// --------------------------------------------------Begin Lexeme List Management Code--------------------------------------------------

//The actual input file data handling...
char lexemeList[MAX_INPUT_SIZE];

/*
	Clears the lexemeList array out with all null terminators.
 */
void clearLexemeList()
{
    for(int i = 0; i < MAX_INPUT_SIZE; i++)
    {
        lexemeList[i] = '\0';
    }
}

/*
	This method will read from the file pointed to by inputFile, and
	populate the variable lexemeList with the last line from the inputFile.
 */
void readInputFile()
{
    //Clear the lexeme list because we're about to write to it
    clearLexemeList();

    //Seek to the end and get the length of the input file in characters...
    fseek(inputFile, 0, SEEK_END);
    int inputSize = (int) ftell(inputFile)-1;

    //Seek back to the beginning
    fseek(inputFile, 0, SEEK_SET);

    //Are we within bounds?
    if (inputSize > MAX_INPUT_SIZE)
    {
        error(26, -1);
    }

    //Iterate character by character through the file...
    int i, place = 0;
    for(i = 0; i < inputSize; i++)
    {
        //Read a character into lexemeList at place...
        fscanf(inputFile, "%c", &lexemeList[place]);

        //Was that character a newline? If so, clear the
        //lexemeList (because there must be another line below)
        if (lexemeList[place] == '\n')
        {
            place = -1;
            clearLexemeList();
        }

        //Increment place...
        place++;
    }

    //Make sure our lexemeList string has a null terminator at the end!
    lexemeList[place] = '\0';
}

// --------------------------------------------------End Lexeme List Management Code--------------------------------------------------

// --------------------------------------------------Begin Token List Management Code--------------------------------------------------

/*
	Here is a list of the token types
 */
int nulsym = 1;
int identsym = 2;
int numbersym = 3;
int plussym = 4;
int minussym = 5;
int multsym = 6;
int slashsym = 7;
int oddsym = 8;
int eqlsym = 9;
int neqsym = 10;
int lesssym = 11;
int leqsym = 12;
int gtrsym = 13;
int geqsym = 14;
int lparentsym = 15;
int rparentsym = 16;
int commasym = 17;
int semicolonsym = 18;
int periodsym = 19;
int becomesym = 20;
int beginsym = 21;
int endsym = 22;
int ifsym = 23;
int thensym = 24;
int whilesym = 25;
int dosym = 26;
int callsym = 27;
int constsym = 28;
int varsym = 29;
int procsym = 30;
int writesym = 31;
int readsym = 32;
int elsesym = 33;

//This list will hold our tokens.
char tokenList[MAX_INPUT_SIZE][MAX_TOKEN_SIZE];

/*
	This method wipes out everything in [tokenList] with null terminators.
 */
void clearTokenList()
{
    for(int t = 0; t < MAX_INPUT_SIZE; t++)
    {
        for(int tc = 0; tc < MAX_TOKEN_SIZE; tc++)
        {
            tokenList[t][tc] = '\0';
        }
    }
}

/*
	This method goes through the contents of [lexemeList] and separates it
	nicely into tokens in [tokenList].
 */
void populateTokenList()
{
    clearTokenList();

    int len = (int) strlen(lexemeList);
    int curToken = 0;
    for(int i = 0; i < len; i++)
    {
        if (lexemeList[i] == ' ')
        {
            curToken++;
            continue;
        }
        else
        {
            appendCharToString(tokenList[curToken], lexemeList[i]);
        }
    }
}

/*
	This method is for debugging sanity. It prints the contents of [tokenList] nicely.
 */
void printTokenList()
{
    printf("Token list contents:\n");
    for(int t = 0; t < MAX_INPUT_SIZE; t++)
    {
        if (strlen(tokenList[t]) > 0)
        {
            printf("Token %d: \"%s\"\n", t, tokenList[t]);
        }
    }
    printf("\n");
}

/*
	Returns an integer representing the type of the token
	at the index [tokenIndex]. Returns 0 if an invalid token
	is at [tokenIndex], meaning the token had no integer value.
 */
int getTokenType(int tokenIndex)
{
    return atoi(tokenList[tokenIndex]);
}

// --------------------------------------------------End Token List Management Code--------------------------------------------------

// --------------------------------------------------Begin Symbol Table Code--------------------------------------------------

typedef struct symbol
{
    int kind;
    char name[12]; // Up to 11 characters, plus one for null terminator
    int val;
    int level;
    int addr;
}
symbol;

symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];

void clearSymbolTable()
{
    for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
    {
        symbolTable[i].kind = -1;
        for(int c = 0; c < 12; c++)
        {
            symbolTable[i].name[c] = '\0';
        }
        symbolTable[i].val = -1;
        symbolTable[i].level = -1;
        symbolTable[i].addr = -1;
    }
}

/*
	This method returns the index in the symbol table of the symbol
	that has the name of name with the highest lexicographical level.
	Returns -1 if the symbol does not exist in the symbol table.
 */
int findSymbolByName(char * name)
{
	int best = -1;
    for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
    {
        if (strcmp(name, symbolTable[i].name) == 0)
        {
        	if (best == -1 || symbolTable[i].level > symbolTable[best].level)
            	best = i;
        }
    }
    return best;
}


/*
	Returns 1 if the symbol table had an entry with the name [name],
	and the level [level].
*/
int containsConflict(char * name, int level)
{
	for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
    {
        if (strcmp(name, symbolTable[i].name) == 0 && symbolTable[i].level == level)
        {
            return 1;
        }
    }
    return 0;
}

/*
	This method puts the symbol [s] in the first available spot in the symbol
	table. Returns 1 if there was a spot, and 0 if the symbol table was full
	or the symbol already existed at that level.
 */
int addSymbol(symbol s)
{
	//If the symbol you are trying to add already existed, fail!
	if (containsConflict(s.name, s.level))
		return 0;
    for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
    {
        if (symbolTable[i].kind == -1)
        {
            //This is a valid spot.
            symbolTable[i].kind = s.kind;
            strcpy(symbolTable[i].name, s.name);
            symbolTable[i].val = s.val;
            symbolTable[i].level = s.level;
            symbolTable[i].addr = s.addr;
            return 1;
        }
    }
    return 0;
}

/*
	Finds all symbols with level [level] and removes them from
	the symbol table (purges the data in the spot).
*/
void removeSymbolsByLevel(int level)
{
	for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
	{
		if (symbolTable[i].level == level)
		{
			//We want to remove the symbol at i...
			symbolTable[i].kind = -1;
		    for(int c = 0; c < 12; c++)
		    {
		        symbolTable[i].name[c] = '\0';
		    }
		    symbolTable[i].val = -1;
		    symbolTable[i].level = -1;
		    symbolTable[i].addr = -1;
		}
	}
}

/*
	This method prints out the symbol table.
 */
void printSymbolTable()
{
    printf("Symbol table:\n");
    for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
    {
        if (symbolTable[i].kind != -1)
        {
            printf("Entry at %d:\n\tkind = %d\n\tname = %s\n\tval = %d\n\tlevel = %d\n\taddr = %d\n", i, symbolTable[i].kind, symbolTable[i].name, symbolTable[i].val, symbolTable[i].level, symbolTable[i].addr);
        }
    }
    printf("\n");
}

// --------------------------------------------------End Symbol Table Code--------------------------------------------------

// --------------------------------------------------Begin Set Stuff--------------------------------------------------

/*
	So, a "set" in the world is an int array of size 256 (SET_SIZE) that maps to symbols...
*/

int firstFactor[SET_SIZE];
int firstTerm[SET_SIZE];
int firstExpression[SET_SIZE];
int firstCondition[SET_SIZE];
int firstStatement[SET_SIZE];
int firstProcedureDeclaration[SET_SIZE];
int firstVarDeclaration[SET_SIZE];
int firstConstDeclaration[SET_SIZE];
int firstBlock[SET_SIZE];
int firstProgram[SET_SIZE];

int followBlock[SET_SIZE];
int followConstantDeclaration[SET_SIZE];
int followVariableDeclaration[SET_SIZE];
int followProcedureDeclaration[SET_SIZE];
int followStatement[SET_SIZE];
int followCondition[SET_SIZE];
int followRelOp[SET_SIZE];
int followExpression[SET_SIZE];
int followTerm[SET_SIZE];
int followFactor[SET_SIZE];

void setInit(int * set)
{
	for(int i = 0; i < SET_SIZE; i++)
		set[i] = -1;
}

void populateSets()
{
	setInit(firstFactor);
	firstFactor[0] = identsym;
	firstFactor[1] = numbersym;
	firstFactor[2] = lparentsym;

	setInit(firstTerm);
	firstTerm[0] = identsym;
	firstTerm[1] = numbersym;
	firstTerm[2] = lparentsym;

	setInit(firstExpression);
	firstExpression[0] = plussym;
	firstExpression[1] = minussym;
	firstExpression[2] = identsym;
	firstExpression[3] = numbersym;
	firstExpression[4] = lparentsym;

	setInit(firstCondition);
	firstCondition[0] = oddsym;
	firstCondition[1] = plussym;
	firstCondition[2] = minussym;
	firstCondition[3] = identsym;
	firstCondition[4] = numbersym;
	firstCondition[5] = lparentsym;

	setInit(firstStatement);
	firstStatement[0] = identsym;
	firstStatement[1] = callsym;
	firstStatement[2] = beginsym;
	firstStatement[3] = ifsym;
	firstStatement[4] = whilesym;
	firstStatement[5] = readsym;
	firstStatement[6] = writesym;

	setInit(firstProcedureDeclaration);
	firstProcedureDeclaration[0] = procsym;

	setInit(firstVarDeclaration);
	firstVarDeclaration[0] = varsym;

	setInit(firstConstDeclaration);
	firstConstDeclaration[0] = constsym;

	setInit(firstBlock);
	firstBlock[0] = constsym;
	firstBlock[1] = varsym;
	firstBlock[2] = procsym;
	firstBlock[3] = identsym;
	firstBlock[4] = callsym;
	firstBlock[5] = beginsym;
	firstBlock[6] = ifsym;
	firstBlock[7] = whilesym;
	firstBlock[8] = readsym;
	firstBlock[9] = writesym;

	setInit(firstProgram);
	firstProgram[0] = constsym;
	firstProgram[1] = varsym;
	firstProgram[2] = procsym;
	firstProgram[3] = identsym;
	firstProgram[4] = callsym;
	firstProgram[5] = beginsym;
	firstProgram[6] = ifsym;
	firstProgram[7] = whilesym;
	firstProgram[8] = readsym;
	firstProgram[9] = writesym;
	firstProgram[10] = periodsym;

	//Follow Sets

	setInit(followBlock);
	followBlock[0] = periodsym;
	followBlock[1] = semicolonsym;

	setInit(followConstantDeclaration);
	followConstantDeclaration[0] = varsym;
	followConstantDeclaration[1] = procsym;
	followConstantDeclaration[2] = identsym;
	followConstantDeclaration[3] = callsym;
	followConstantDeclaration[4] = beginsym;
	followConstantDeclaration[5] = ifsym;
	followConstantDeclaration[6] = whilesym;
	followConstantDeclaration[7] = readsym;
	followConstantDeclaration[8] = writesym;
	followConstantDeclaration[9] = periodsym;
	followConstantDeclaration[10] = semicolonsym;

	setInit(followVariableDeclaration);
	followVariableDeclaration[0] = procsym;
	followVariableDeclaration[1] = periodsym;
	followVariableDeclaration[2] = identsym;
	followVariableDeclaration[3] = callsym;
	followVariableDeclaration[4] = beginsym;
	followVariableDeclaration[5] = ifsym;
	followVariableDeclaration[6] = whilesym;
	followVariableDeclaration[7] = readsym;
	followVariableDeclaration[8] = writesym;
	followVariableDeclaration[9] = semicolonsym;

	setInit(followProcedureDeclaration);
	followProcedureDeclaration[0] = identsym;
	followProcedureDeclaration[1] = callsym;
	followProcedureDeclaration[2] = beginsym;
	followProcedureDeclaration[3] = ifsym;
	followProcedureDeclaration[4] = whilesym;
	followProcedureDeclaration[5] = readsym;
	followProcedureDeclaration[6] = writesym;
	followProcedureDeclaration[7] = periodsym;
	followProcedureDeclaration[8] = semicolonsym;

	setInit(followStatement);
	followStatement[0] = semicolonsym;
	followStatement[1] = endsym;
	followStatement[2] = elsesym;
	followStatement[3] = periodsym;

	setInit(followCondition);
	followCondition[0] = dosym;
	followCondition[1] = thensym;

	setInit(followRelOp);
	followRelOp[0] = plussym;
	followRelOp[1] = minussym;
	followRelOp[2] = identsym;
	followRelOp[3] = numbersym;
	followRelOp[4] = lparentsym;

	setInit(followExpression);
	followExpression[0] = eqlsym;
	followExpression[1] = lesssym;
	followExpression[2] = leqsym;
	followExpression[3] = gtrsym;
	followExpression[4] = geqsym;
	followExpression[5] = neqsym;
	followExpression[6] = dosym;
	followExpression[7] = thensym;
	followExpression[8] = semicolonsym;
	followExpression[9] = endsym;
	followExpression[10] = elsesym;
	followExpression[11] = periodsym;
	followExpression[12] = rparentsym;

	setInit(followTerm);
	followTerm[0] = eqlsym;
	followTerm[1] = lesssym;
	followTerm[2] = leqsym;
	followTerm[3] = gtrsym;
	followTerm[4] = geqsym;
	followTerm[5] = neqsym;
	followTerm[6] = dosym;
	followTerm[7] = thensym;
	followTerm[8] = semicolonsym;
	followTerm[9] = endsym;
	followTerm[10] = elsesym;
	followTerm[11] = periodsym;
	followTerm[12] = rparentsym;
	followTerm[13] = plussym;
	followTerm[14] = minussym;

	setInit(followFactor);
	followFactor[0] = multsym;
	followFactor[1] = slashsym;
	followFactor[2] = plussym;
	followFactor[3] = minussym;
	followFactor[4] = periodsym;
	followFactor[5] = semicolonsym;
	followFactor[6] = endsym;
	followFactor[7] = elsesym;
	followFactor[8] = thensym;
	followFactor[9] = dosym;
	followFactor[10] = eqlsym;
	followFactor[11] = neqsym;
	followFactor[12] = lesssym;
	followFactor[13] = leqsym;
	followFactor[14] = gtrsym;
	followFactor[15] = geqsym;
	followFactor[16] = rparentsym;
}

int setContains(int * set, int val)
{
	for(int i = 0; i < SET_SIZE; i++)
	{
		if (set[i] == val)
			return 1;
	}
	return 0;
}

void setUnion(int * result, int * a, int * b)
{
	int tempresult[SET_SIZE];
	setInit(tempresult);
	int rp = 0;

	for(int i = 0; i < SET_SIZE; i++)
	{
		if (a[i] != -1)
		{
			tempresult[rp] = a[i];
			rp++;
		}
	}

	for(int i = 0; i < SET_SIZE; i++)
	{
		if (b[i] != -1 && !setContains(tempresult, b[i]))
		{
			tempresult[rp] = b[i];
			rp++;
		}
	}

	//Copy tempresult into result.
	for(int i = 0; i < SET_SIZE; i++)
	{
		result[i] = tempresult[i];
	}
}

void setPrint(int * set)
{
	printf("{");
	for(int i = 0; i < SET_SIZE; i++)
	{
		if (set[i] != -1)
		{
			printf(" %d", set[i]);
		}
	}
	printf(" }\n");
}

// --------------------------------------------------End Set Stuff--------------------------------------------------

// --------------------------------------------------Begin Parser/Code Generator Code--------------------------------------------------


/*
	This variable represents the next open register at all times.
*/
 int rc = 0;

 void rcCheck()
 {
 	if (rc > 16)
 	{
 		// If rc is 17, that implies we tried to store at location 16, which
 		// does not exist since our registers only go from 0-15!
 		error(34, -1);

 		//This is an error that cannot be recovered from!
 		exit(1);
 	}
 }

typedef struct code
{
    int op;
    int r;
    int l;
    int m;
} code;

code code_array[MAX_INPUT_SIZE]; // change later
int cx = 0;

void clearCodeArray()
{
	for(int i = 0; i < MAX_INPUT_SIZE; i++)
	{
		code_array[i].op = -1;
		code_array[i].r = -1;
		code_array[i].l = -1;
		code_array[i].m = -1;
	}
}

void printCodeArray()
{
	printf("Code Generation:\n");
	for(int i = 0; i < MAX_INPUT_SIZE; i++)
	{
		if (code_array[i].op == -1)
		{
			break;
		}
		else
		{
			printf("Code Entry %d: %d %d %d %d\n", i, code_array[i].op, code_array[i].r, code_array[i].l, code_array[i].m);
		}
	}
}

void writeCodeArray()
{
	for(int i = 0; i < MAX_INPUT_SIZE; i++)
	{
		if (code_array[i].op == -1)
		{
			break;
		}
		else
		{
			fprintf(outputFile, "%d %d %d %d\n", code_array[i].op, code_array[i].r, code_array[i].l, code_array[i].m);
		}
	}
}

/*
	This variable represents, globally, and at all times, the NEXT token
	to be evaluated. Thus, to get the next token to be evaluated, just
	use tokenList[curToken], which will be the string containing the next
	token to be evaluated. Use curToken++ to move up one token, etc...

	Also! There is a method above called getTokenType that takes an index,
	which curToken is, and returns the type (number) of that token. It is
	useful for checking if your token is a periodsym, for example, by doing
	if (getTokenType(curToken) == periodsym).
 */

int curToken = 0;

void test(int * firstset, int * stopset, int errorCode)
{
	if (!setContains(firstset, getTokenType(curToken)))
	{
		//printf("Skipping on token %d\n", curToken);
		error(errorCode, curToken);
		while (!setContains(firstset, getTokenType(curToken)) && !setContains(stopset, getTokenType(curToken)))
		{
			//This is statement serves to prevent runaway! Without this, some calls to test may cause a segfault!
			if (curToken >= MAX_INPUT_SIZE || strlen(tokenList[curToken]) <= 0)
			{
				return;
			}
			curToken++;
		}
		//printf("Skipped to token %d\n", curToken);
	}

}

/*
	This variable should represent the current lexicographical level at all times...
	Starts at -1 because main's block will increment it by 1 to get 0...
*/
int lexLevel = -1;



void emit(int op, int r, int l, int m);
void doTheAwesomeParsingAndCodeGenerating();
void program();
void block(int * stopset);
void statement(int * stopset);
void condition(int * stopset);
void expression(int * stopset);
void term(int * stopset);
void factor(int * stopset);

void emit(int op, int r, int l, int m)
{
    if (cx > MAX_INPUT_SIZE)
    {
    	//Too much code generated!
        error(26, curToken);
    }
    else
    {
        code_array[cx].op = op;
        code_array[cx].r = r;
        code_array[cx].l = l;
        code_array[cx].m = m;
        cx++;
    }
}

void doTheAwesomeParsingAndCodeGenerating()
{
    program();
}

void program()
{
	//So, if block has a block-level failure, then scan up until you hit a
	//follow symbol for block, or a period, since we expect a period next!
	int pass[SET_SIZE];
	setInit(pass);
	pass[0] = periodsym;
	setUnion(pass, pass, followBlock);

    block(pass);

    //If current token is NOT a period
    if (getTokenType(curToken) != periodsym)
    {
        //Program must end with a period
        error(9, curToken);
    }

	//Always make the last instruction a program termination just in case!
    emit(11, 0, 0, 3);
}


void block(int * stopset)
{
    symbol s;

    //Increment lexLevel and offsetCounter; generate code to increment for FV, SL, DL, RA
    //We strictly set to 4 because it should always begin at 4 for a block...
    lexLevel += 1;
    int offsetCounter = 4;

    int blockJump = cx;
    emit(7, 0, 0, 0);

    //A block can be a constant-declaration, variable declaration, or a statement
    //If current token is a constant symbol...
    if (getTokenType(curToken) == constsym)
    {
        do
        {
            curToken++;
            if (getTokenType(curToken) != identsym)
            {
                //Expected an identifier after constsym...
                error(4, curToken);
            }
            else
            {
		        curToken++;
				//Symbol table kind
		        s.kind = 1;

				//Symbol table identifier
		        strcpy(s.name, tokenList[curToken]);
		        curToken++;
		        if (getTokenType(curToken) != eqlsym)
		        {
		            //Expected equal symbol (NOT become symbol because it's constant declaration)
		            error(3, curToken);
		        }
		        else
		        {
			    	curToken++;
			    }

			    if (getTokenType(curToken) != numbersym)
			    {
			        //Expected a number!
			        error(2, curToken);
			    }
			    else
			    {
					curToken++;
					//Symbol table value
					s.val = atoi(tokenList[curToken]);
					s.level = lexLevel;
					s.addr = offsetCounter;
					offsetCounter++;
					curToken++;
					if (!addSymbol(s))
					{
						//Symbol table full or conflicting symbol!
						error(29, curToken);
					}
			    }

            }
        }
        while(getTokenType(curToken) == commasym);

        //Now we expect a semicolon...
        if (getTokenType(curToken) != semicolonsym)
        {
            //Missing a semicolon!
            error(5, curToken);
        }
        else
        {
        	curToken++;
        }
    }

    //If we've got a variable declaration
    if (getTokenType(curToken) == varsym)
    {
        do
        {
            curToken++;
            if (getTokenType(curToken) != identsym)
            {
                //Following var we expected an identifier...
                error(4, curToken);
            }
            else
            {
		        curToken++;
				//Symbol table kind
				s.kind = 2;
				//Symbol table identifier
				strcpy(s.name, tokenList[curToken]);
				//Symbol table level
				s.level = lexLevel;
				s.val = 0; //Because they're variables which are not assigned initially!
				s.addr = offsetCounter;
				offsetCounter++;
		        curToken++;
				if (!addSymbol(s))
		        {
		        	//Symbol table full or conflicting symbol!
		        	error(29, curToken);
		        }
            }
        }
        while(getTokenType(curToken) == commasym);


        //Now we expect a semicolon...
        if (getTokenType(curToken) != semicolonsym)
        {
            //Missing a semicolon!
            error(5, curToken);
        }
        else
        {
        	curToken++;
        }
    }

    while (getTokenType(curToken) == procsym)
    {
    	curToken++;

    	//We now should have an identifier
    	if (getTokenType(curToken) != identsym)
    	{
    		//Identifier expected!
    		error(4, curToken);
    	}
    	else
    	{

			//We did find an identifier!
			curToken++;

			s.kind = 3;
			strcpy(s.name, tokenList[curToken]);
			s.val = 0; //unused?
			s.level = lexLevel; //TODO: What is this for?
			s.addr = cx; //This cx should be the place where block is going to emit the jump...

			if (!addSymbol(s))
			{
				//Symbol table full or conflicting symbol!
		        error(29, curToken);
			}

			//Symbol table wasn't full!
			curToken++;

			//We now expect a semicolonsym
			if (getTokenType(curToken) != semicolonsym)
			{
				//Semicolon required after procedure declaration
				error(35, curToken);
			}
			else
			{
				curToken++;
			}

			//Now we require the block for the procedure!
			int pass[SET_SIZE];
			setInit(pass);
			pass[0] = semicolonsym;
			setUnion(pass, pass, stopset);
			setUnion(pass, pass, followBlock);

			//ERA1: If we fail to parse block, we may find a statement or block after this block...
			setUnion(pass, pass, firstBlock);
			setUnion(pass, pass, firstStatement);


			block(pass);

			//Semicolon after block!
			if (getTokenType(curToken) != semicolonsym)
			{
				//Semicolon required after procedure block
				error(36, curToken);
			}
			else
			{
				curToken++;
			}
    	}
    }

	//Update that jump we made!
	code_array[blockJump].m = cx;

	//Increment to allocate for all variables!
	emit(6, 0, 0, offsetCounter);

	//We must now emit everything for constant assignment...
	for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
	{
		//If the symbol at i was made from this level, and it was a constant...
		if (symbolTable[i].level == lexLevel && symbolTable[i].kind == 1)
		{
			//We need to assign the constant it's value!
			emit(1, rc, 0, symbolTable[i].val);

			//Note the 0 lexicographical level here; this is because the constant
			//was generated at this level, so we go 0 down from this level! (We modify
			//the current activation record)
			emit(4, rc, 0, symbolTable[i].addr);
		}
	}

    //And there MUST be a statement next... That is, a block must have at least one statement in addition to declarations...

    //We'll tell statement to return to us with a symbol being in it's follow set, .
    int pass[SET_SIZE];
    setInit(pass);
    setUnion(pass, pass, stopset);
    setUnion(pass, pass, followStatement);
    statement(pass);

    //Now, clean up symbol table and decrement lexLevel...
    removeSymbolsByLevel(lexLevel);
    lexLevel--;

    //Return from method...
    emit(2, 0, 0, 0);

    //Make sure we validly return to our caller...
    test(stopset, stopset, 8);
}

void statement(int * stopset)
{
    //Statement could just be like x := 6;
    if (getTokenType(curToken) == identsym)
    {
        curToken++;

        int place = findSymbolByName(tokenList[curToken]);
        if (place == -1)
        {
        	//Could not find symbol
        	error(11, curToken);
        }
        else
        {
			if (symbolTable[place].kind != 2)
			{
				//Cannot assign to procedure or constant
				error(12, curToken);
			}
	    }

	    curToken++;

	    if (getTokenType(curToken) != becomesym)
	    {
			if (getTokenType(curToken) == eqlsym)
			{
				//Equal symbol instead of becomesym
				error(1, curToken);
				curToken++;
			}
			else
			{
	        	//Must be assignment statement
	        	error(13, curToken);
	        }
	    }
	    else
	    {
	    	curToken++;
	    }

	    //Parse an expression...
	    int pass[SET_SIZE];
	    setInit(pass);
	    setUnion(pass, pass, stopset);
	    setUnion(pass, pass, followExpression);
	    expression(pass);

	    if (place != -1)
	    {
			if (ENFORCE_VARIABLE_ASSIGNMENT)
			{
				//Inform the world that this variable has been assigned!
				symbolTable[place].val = 1;
			}

			//Put the resulting expression into the variable
			int levelsDown = lexLevel-symbolTable[place].level;
			emit(4, rc-1, levelsDown, symbolTable[place].addr);
	    }

		//The register the expression was stored into is now free again, because we stored it into the variable in the stack..
		rc = rc-1;
    }
    else if (getTokenType(curToken) == beginsym)
    {
        curToken++;

        int pass[SET_SIZE];
        setInit(pass);
        pass[0] = semicolonsym;
        pass[1] = endsym;
        setUnion(pass, pass, followStatement);
        setUnion(pass, pass, stopset);
        statement(pass);

        while(getTokenType(curToken) == semicolonsym)
        {
            curToken++;
            statement(pass);
        }
        if (getTokenType(curToken) != endsym)
        {
            //We were expecting an endsym after all of those statements...
            error(19, curToken);
        }
        else
        {
        	curToken++;
        }
    }
    else if (getTokenType(curToken) == ifsym)
    {
        curToken++;

		//Parse the condition!
		int pass[SET_SIZE];
		setInit(pass);
		pass[0] = thensym;
		setUnion(pass, pass, stopset);
		setUnion(pass, pass, followCondition);

        // rc-1 will hold the condition's result
        condition(pass);

        int jpc = cx;
        emit(8, rc-1, 0, 0);

        //Once we've checked on the condition we are DONE with that result!
        rc -= 1;

        if (getTokenType(curToken) != thensym)
        {
            //should have been a then after the condition of the if...
            error(16, curToken);
        }
        else
        {
        	curToken++;
        }

        //then statement...
        setInit(pass);
        pass[0] = elsesym;
        setUnion(pass, pass, stopset);
        setUnion(pass, pass, followStatement);

        statement(pass);

        code_array[jpc].m = cx;

        //Now there could be an else...
		if (getTokenType(curToken) == elsesym)
		{
			//There is an else!

			//So, we need to have something to jump past the else if the original condition was true...
			int jmp = cx;
			emit(7, 0, 0, 0);

			//We need to update our previous jump stuff to jump past that new jump statement, too!
			code_array[jpc].m = cx;

			//Read the statement...
			curToken++;
			statement(pass);

			//Update jmp
			code_array[jmp].m = cx;
		}
    }
    else if (getTokenType(curToken) == whilesym)
    {
        curToken++;

        int checkStart = cx;

        int pass[SET_SIZE];
        setInit(pass);
        pass[0] = dosym;
        setUnion(pass, pass, stopset);
        setUnion(pass, pass, followCondition);

        // rc-1 will hold the condition's result
        condition(pass);

        int jpc = cx;
        emit(8, rc-1, 0, 0);

        //Once we've checked on the condition we are DONE with that result!
        rc -= 1;

        if (getTokenType(curToken) != dosym)
        {
            //Expected a do after condition for while
            error(18, curToken);
        }
        else
        {
        	curToken++;
        }

        setInit(pass);
        setUnion(pass, pass, stopset);
        setUnion(pass, pass, followStatement);

        statement(pass);

        emit(7, 0, 0, checkStart);

        code_array[jpc].m = cx;
    }
    else if (getTokenType(curToken) == readsym)
    {
    	curToken++;

    	//Now, I expect an identifier!
    	if (getTokenType(curToken) != identsym)
    	{
    		//No identifier whaaat?!?!?
    		error(30, curToken);
    	}
    	else
    	{
			//Okay, we have an identifier!
			curToken++;

			//Does the identifier exist?
			int place = findSymbolByName(tokenList[curToken]);
			if (place == -1)
			{
				//Undeclared identifier!
				error(11, curToken);
			}
			else
			{
				if (symbolTable[place].kind != 2)
				{
					//Can't read into a procedure or constant!
					error(31, curToken);
				}

				if (ENFORCE_VARIABLE_ASSIGNMENT)
				{
					//Inform the world that this variable has been assigned!
					symbolTable[place].val = 1;
				}

				//Okay, it exists, and has no issues! We need to generate code to read into this symbol...
				emit(10, rc, 0, 2); // Read input into register rc
				int levelsDown = lexLevel-symbolTable[place].level;
				emit(4, rc, levelsDown, symbolTable[place].addr); // Store the value now in register rc into the address of the ident

			}

			curToken++;
    	}
    }
    else if (getTokenType(curToken) == writesym)
    {
    	curToken++;

		int pass[SET_SIZE];
		setInit(pass);
		setUnion(pass, pass, stopset);
		setUnion(pass, pass, followExpression);

    	//Now, I expect an expression! Result will be in rc-1
    	expression(pass);

    	emit(9, rc-1, 0, 1); // Print what is in rc-1 to the screen

    	//We are done with the value in rc-1, so remove it!
    	rc--;
    }
    else if (getTokenType(curToken) == callsym)
    {
    	curToken++;

    	//Better be an identifier!
    	if (getTokenType(curToken) != identsym)
    	{
    		//Call requires ident...
    		error(14, curToken);
    	}
    	else
    	{
			curToken++;

			//Is the identifier a procedure identifier?
			int place = findSymbolByName(tokenList[curToken]);
			if (place == -1)
			{
				//Could not find symbol
		    	error(11, curToken);
			}
			else
			{
				if (symbolTable[place].kind != 3)
				{
					//Call to var or const is useless..
					error(15, curToken);
				}

				//Identifier is a procedure and exists!
				int levelsDown = lexLevel-symbolTable[place].level;
				emit(5, 0, levelsDown, symbolTable[place].addr);
			}

			curToken++;
    	}
    }

    test(stopset, stopset, 19);
}

void condition(int * stopset)
{
	//No first set checking here because expression will catch it...

    if (getTokenType(curToken) == oddsym)
    {
        curToken++;

        int pass[SET_SIZE];
        setInit(pass);
        setUnion(pass, pass, stopset);
        setUnion(pass, pass, followExpression);

        // rc-1 holds the result of expression
        expression(pass);

        emit(17, rc-1, 0, 0);
    }
    else
    {
    	int pass[SET_SIZE];
        setInit(pass);
        pass[0] = numbersym;
        setUnion(pass, pass, stopset);
        setUnion(pass, pass, followExpression);

        expression(pass);

		int relop = 0;
        if (getTokenType(curToken) != eqlsym && getTokenType(curToken) != neqsym && getTokenType(curToken) != lesssym && getTokenType(curToken) != leqsym && getTokenType(curToken) !=gtrsym && getTokenType(curToken) != geqsym)
        {
            //Looking for relational symbol
            error(20, curToken);
        }
        else
        {
        	relop = getTokenType(curToken);
        	curToken++;
        }

        setInit(pass);
        setUnion(pass, pass, stopset);
        setUnion(pass, pass, followExpression);

        expression(pass);

        //At this point, rc-2 has the first expression, and rc-1 has the second expression

        if (relop == eqlsym)
        {
        	emit(19, rc-2, rc-2, rc-1);
        }
        else if (relop == neqsym)
        {
        	emit(20, rc-2, rc-2, rc-1);
        }
        else if (relop == lesssym)
        {
        	emit(21, rc-2, rc-2, rc-1);
        }
        else if (relop == leqsym)
        {
        	emit(22, rc-2, rc-2, rc-1);
        }
        else if (relop == gtrsym)
        {
        	emit(23, rc-2, rc-2, rc-1);
        }
        else if (relop == geqsym)
        {
        	emit(24, rc-2, rc-2, rc-1);
        }
        rc = rc-1;
    }
}

void expression(int * stopset)
{
	//Assume it's just a normal and positive expression...
    int adop = plussym;
    if (getTokenType(curToken) == plussym || getTokenType(curToken) == minussym)
    {
    	//If it was a minus sym, then here we'd set adop to minussym.
        adop = getTokenType(curToken);
        curToken++;
    }

	int pass[SET_SIZE];
	setInit(pass);
	setUnion(pass, pass, followTerm);
	setUnion(pass, pass, followExpression);
	setUnion(pass, pass, stopset);

    //Load a term! After this a term will be in rc-1.
    term(pass);

    if (adop == minussym)
    {
    	emit(12, rc-1, 0, 0);
    }

    while (getTokenType(curToken) == plussym || getTokenType(curToken) == minussym)
    {
    	//Remember the operation...
    	int operation = getTokenType(curToken);

        curToken++;

    	//Load the next term...
    	int pass[SET_SIZE];
		setInit(pass);
		setUnion(pass, pass, followTerm);
		setUnion(pass, pass, followExpression);
		setUnion(pass, pass, stopset);
        term(pass);

        //Apply the operation!
        if (operation == plussym)
        	emit(13, rc-2, rc-2, rc-1);
        else
        	emit(14, rc-2, rc-2, rc-1);
        rc = rc-1;

    }
}

void term(int * stopset)
{
    int pass[SET_SIZE];
    setInit(pass);
    setUnion(pass, pass, stopset);
    setUnion(pass, pass, followFactor);

    //This will get us a factor into register rc-1.
    factor(pass);

    //Do we have more terms factors?
    while (getTokenType(curToken) == multsym || getTokenType(curToken) == slashsym)
    {
    	//Remember the operation...
    	int operation = getTokenType(curToken);

    	//Load the next factor...
        curToken++;

        int pass[SET_SIZE];
        setInit(pass);
        setUnion(pass, pass, stopset);
        setUnion(pass, pass, followFactor);
        factor(pass);

        //Perform the operation on rc-2 and rc-1 and store into rc-2 and set rc to rc-1!
        if (operation == multsym)
        	emit(15, rc-2, rc-2, rc-1);
        else
        	emit(16, rc-2, rc-2, rc-1);
        rc = rc-1;

    }
}

void factor(int * stopset)
{
	test(firstFactor, stopset, 28);
    if (getTokenType(curToken) == identsym)
    {
        curToken++;
        //Now load whatever that identifier is onto the "register stack"

        //Make sure the identifier is valid...
        int place = findSymbolByName(tokenList[curToken]);
        if (place == -1)
        {
        	//Undeclared identifier!
        	error(11, curToken);
        }
        else
        {
		    if (symbolTable[place].kind != 1 && symbolTable[place].kind != 2)
		    {
		    	//Factor can't be a procedure!
		    	error(21, curToken);
		    }
		    else
		    {
				if (ENFORCE_VARIABLE_ASSIGNMENT)
				{
					//Enfoce that, if this is a variable, it must have been assigned to!
					if (symbolTable[place].kind == 2 && symbolTable[place].val != 1)
					{
						//Use of unassigned variable!
						error(33, curToken);
					}
				}

				//Ok, valid identifier, so now emit away!
				int levelsDown = lexLevel-symbolTable[place].level;
				emit(3, rc, levelsDown, symbolTable[place].addr); // Load into register rc the value at address of the identifier...
				rc++;									 // Let the world know that we used a register
				rcCheck();
		    }
        }
        curToken++;
    }
    else if (getTokenType(curToken) == numbersym)
    {
        curToken++;

        if (strlen(tokenList[curToken]) > MAX_NUMBER_SIZE)
        {
        	//Number too large!
        	error(25, curToken);
        }
        else
		{
		    //Get the number's literal value...
		    int literalValue = atoi(tokenList[curToken]);

		    emit(1, rc, 0, literalValue); // Load immediately into register rc literalValue
		    rc++;						  // Let the world know that we used a register
		    rcCheck();

		    curToken++;
        }
    }
    else if (getTokenType(curToken) == lparentsym)
    {
        curToken++;

		int pass[SET_SIZE];
		setInit(pass);
		pass[0] = rparentsym;
		setUnion(pass, pass, stopset);
		setUnion(pass, pass, followExpression);

        //Expression should leave us one register value with the answer!
        expression(pass);

        if (getTokenType(curToken) != rparentsym)
        {
            //Expect )
            error(22, curToken);
        }
        else
        {
        	curToken++;
        }
    }

    //Enforce the next token is a stop symbol
    test(stopset, stopset, 23);
}



// --------------------------------------------------End Parser/Code Generator Code--------------------------------------------------

// --------------------------------------------------Begin Main Code--------------------------------------------------

/*
	I expect this program to take an input file of lexemes, parse it, and print out
	a file containing whether or not parsing succeeded, and then the generated code.
 */


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Invalid arguments to parser code generator!\nUSAGE: ./parserCodeGenerator [lexical analyzer file] [code output file]\n");
		return 1;
	}

	populateSets();
    openFiles(argv[1], argv[2]);
    readInputFile();
    populateTokenList();
    clearSymbolTable();
    clearCodeArray();
    doTheAwesomeParsingAndCodeGenerating();
	writeCodeArray();

    return 0;
}

// --------------------------------------------------End Main Code--------------------------------------------------
