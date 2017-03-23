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


// --------------------------------------------------Begin Random Additional Code--------------------------------------------------

void eprintf(char * message)
{
	FILE * errorFile = fopen("ef", "w");
	fprintf(errorFile, "An error occurred while parsing: %s\n", message);
	fclose(errorFile);
}

/*
	This method halts the program execution after printing an error
	with code [code].
 */
void error(int code, int token)
{
    //printf("Error on token %d: ", token);
    if (code == 1)
        eprintf("Use of = instead of :=.\n");
    if (code == 2)
        eprintf("= must be followed by a number.\n");
    if (code == 3)
        eprintf("Identifier must be followed by =.\n");
    if (code == 4)
        eprintf("const, var, procedure must be followed by identifier.\n");
    if (code == 5)
        eprintf("Semicolon or comma missing.\n");
    if (code == 6)
        eprintf("Incorrect symbol after procedure declaration.\n");
    if (code == 7)
        eprintf("Statement expected.\n");
    if (code == 8)
        eprintf("Incorrect symbol after ststement part in block.\n");
    if (code == 9)
        eprintf("Period expected.\n");
    if (code == 10)
        eprintf("Semicolon between statements missing.\n");
    if (code == 11)
        eprintf("Undeclared identifier.\n");
    if (code == 12)
        eprintf("Assignment to constant or procedure is not allowed.\n");
    if (code == 13)
        eprintf("Assignment operator expected.\n");
    if (code == 14)
        eprintf("call must be followed by an identifier.\n");
    if (code == 15)
        eprintf("Call of a constant or a variable is meaningless.\n");
    if (code == 16)
        eprintf("then expected.\n");
    if (code == 17)
        eprintf("Semicolon expected.\n"); // I modified this one to not include "or }" because that makes no sense to me.
    if (code == 18)
        eprintf("do expected.\n");
    if (code == 19)
        eprintf("Incorrect symbol following statement.\n");
    if (code == 20)
        eprintf("Relational operator expected.\n");
    if (code == 21)
        eprintf("Expression must not contain a procedure identifier.\n");
    if (code == 22)
        eprintf("Right parenthesis missing.\n");
    if (code == 23)
        eprintf("The preceding factor cannot begin with this symbol.\n");
    if (code == 24)
        eprintf("An expression cannot begin with this symbol.\n");
    if (code == 25)
        eprintf("This number is too large.\n");
    if (code == 26)
        eprintf("The input file is too large.\n");
    if (code == 27)
        eprintf("Call must be followed by an identifier for a procedure.\n");
    if (code == 28)
        eprintf("Expected a factor, did not find one!\n");
    if (code == 29)
    	eprintf("Too many symbols, or a conflicting symbol was found!\n");
    if (code == 30)
    	eprintf("Read and write require an identifier after them!\n");
    if (code == 31)
    	eprintf("You can only read into a variable!");
    if (code == 32)
    	eprintf("You can only write a variable or constant!");

    exit(1);
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
	that has the name of name. Returns -1 if the symbol does not
	exist in the symbol table.
 */
int findSymbolByName(char * name)
{
    for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++)
    {
        if (strcmp(name, symbolTable[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
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

// --------------------------------------------------Begin Parser/Code Generator Code--------------------------------------------------


/*
	This variable represents the next open register at all times.
*/
 int rc = 0;

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
int var_level = 1;
int addressCounter = 1;

void emit(int op, int r, int l, int m);
void doTheAwesomeParsingAndCodeGenerating();
void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();

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
    int start = curToken;
    block();
    //If current token is NOT a period
    if (getTokenType(curToken) != periodsym)
    {
        //Program must end with a period
        error(9, curToken);
    }
    printf("Got program from %d-%d\n", start, curToken);

    //SIO
    emit(11, 0, 0, 3);
}

//TODO: ask about this structure of unchained ifs
void block()
{
    symbol s;
    int start = curToken;
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
            curToken++;
            if (getTokenType(curToken) != numbersym)
            {
                //Expected a number!
                error(2, curToken);
            }
            
            curToken++;
			//Symbol table value
            s.val = atoi(tokenList[curToken]);
            s.level = 1;
            s.addr = addressCounter;
            addressCounter++;
            curToken++;
			if (!addSymbol(s))
			{
				//Symbol table full or conflicting symbol!
            	error(29, curToken);
            }
            
            emit(6, 0, 0, 1); // increment our stack size in the vm by 1 to allocate the space for this thing..
            emit(1, rc, 0, s.val);  // load into register!
            emit(4, rc, 0, s.addr); // store onto vm stack!
        }
        while(getTokenType(curToken) == commasym);

        //Now we expect a semicolon...
        if (getTokenType(curToken) != semicolonsym)
        {
            //Missing a semicolon!
            error(5, curToken);
        }
        curToken++;
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
            curToken++;
			//Symbol table kind
			s.kind = 2;
			//Symbol table identifier
			strcpy(s.name, tokenList[curToken]);
			//Symbol table level
			s.level = 1;
			s.val = 0; //Because they're variables which are not assigned initially!
			s.addr = addressCounter;
			addressCounter++;
			if (!addSymbol(s))
            {
            	//Symbol table full or conflicting symbol!
            	error(29, curToken);
            }
            curToken++;
            
            //Now I have added a new symbol to my table and need to plop it into the vm stack...
            emit(6, 0, 0, 1); // increment our stack size in the vm by 1 to allocate the space for this thing..
            emit(1, rc, 0, s.val); // load into register!
            emit(4, rc, 0, s.addr); // store whats in register into vm stack!
        }
        while(getTokenType(curToken) == commasym);
	

        //Now we expect a semicolon...
        if (getTokenType(curToken) != semicolonsym)
        {
            //Missing a semicolon!
            error(5, curToken);
        }
        curToken++;
    }

	//TODO: Procedures not yet supported
    while(getTokenType(curToken) == procsym)
    {
        curToken++;
        if (getTokenType(curToken) != identsym)
        {
            //Expected an identifier after beginning of procedure!
            error(4, curToken);
        }
		//Symbol table type
		s.kind = 3;
        //Double skip because following the identsym is the identifier itself...
        curToken++;
		//Symbol table identifier
		strcpy(s.name, tokenList[curToken]);
        curToken++;
        if (getTokenType(curToken) != semicolonsym)
        {
            //Expected semicolon after procedure declaration!
            error(17, curToken);
        }
		if (!addSymbol(s))
			printf("Symbol table is full.\n");
        curToken++;

        /*
         TODO: ask about this calling block and looking for semicolon after a
         procedure line. Doesn't that imply that a procedure could just be like

         procedure MULT;
         var x;;

         Which looks invalid to me. Doesn't block need to consider begin and end as well?

         Edit: I've kind of resolved this alone. The fact that block calls statement later implies
         that there is at least one statement after variable declaration, procedure declaration, or
         whatever. Thus, it makes sense to check for semicolon after block because a procedure is
         a procedure declaration, then some other declarations, then a statement, then a semicolon.
         */

        //Now, after the procedure declaration there should be a block of code...
        block();

        //After every procedure is an additional semicolon...
        if (getTokenType(curToken) != semicolonsym)
        {
            error(17, curToken);
        }
        curToken++;
    }

    //And there MUST be a statement next... That is, a block must have at least one statement in addition to declarations...
    statement();

    printf("Got block from %d-%d\n", start, curToken);
}

void statement()
{
    int start = curToken;
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
        if (symbolTable[place].kind != 2)
        {
        	//Cannot assign to procedure or constant
        	error(12, curToken);
        }
        
        curToken++;
        
        if (getTokenType(curToken) != becomesym)
        {
            //Must be assignment statement
            error(13, curToken);
        }
        
        

        curToken++;

        //Parse an expression...
        expression();
        
        //Put the resulting expression into the variable
        emit(4, rc-1, 0, symbolTable[place].addr);
    }
    else if (getTokenType(curToken) == callsym)
    {
        curToken++;
        if (getTokenType(curToken) != identsym)
        {
            error(27, curToken);
        }
        //Double increment because after identsym is the identifier itself...
        curToken++;
        curToken++;
    }
    else if (getTokenType(curToken) == beginsym)
    {
        curToken++;
        statement();
        while(getTokenType(curToken) == semicolonsym)
        {
            curToken++;
            statement();
        }
        if (getTokenType(curToken) != endsym)
        {
            //We were expecting an endsym after all of those statements...
            error(19, curToken);
        }
        curToken++;
    }
    else if (getTokenType(curToken) == ifsym)
    {
        curToken++;
        
        // rc-1 will hold the condition's result
        condition();
        
        int jpc = cx;
        emit(8, rc-1, 0, 0);
        
        if (getTokenType(curToken) != thensym)
        {
            //should have been a then after the condition of the if...
            error(16, curToken);
        }
        curToken++;
        statement();
        
        code_array[jpc].m = cx;
    }
    else if (getTokenType(curToken) == whilesym)
    {
        curToken++;
        
        int checkStart = cx;
        
        // rc-1 will hold the condition's result
        condition();
        
        int jpc = cx;
        emit(8, rc-1, 0, 0);
        
        if (getTokenType(curToken) != dosym)
        {
            //Expected a do after condition for while
            error(18, curToken);
        }
        curToken++;
        statement();
        
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
    	
    	//Okay, we have an identifier!
    	curToken++;
    	
    	//Does the identifier exist?
    	int place = findSymbolByName(tokenList[curToken]);
    	if (place == -1)
    	{
    		//Undeclared identifier!
    		error(11, curToken);
    	}
    	if (symbolTable[place].kind != 2)
    	{
    		//Can't read into a procedure or constant!
    		error(31, curToken);
    	}
    	
    	//Okay, it exists, and has no issues! We need to generate code to read into this symbol...
    	emit(10, rc, 0, 2); // Read input into register rc 
    	emit(4, rc, 0, symbolTable[place].addr); // Store the value now in register rc into the address of the ident
    	
    	curToken++;
    }
    else if (getTokenType(curToken) == writesym)
    {
    	curToken++;
    	
    	//Now, I expect an identifier!
    	if (getTokenType(curToken) != identsym)
    	{
    		//No identifier whaaat?!?!?
    		error(30, curToken);
    	}
    	
    	//Okay, we have an identifier!
    	curToken++;
    	
    	//Does the identifier exist?
    	int place = findSymbolByName(tokenList[curToken]);
    	if (place == -1)
    	{
    		//Undeclared identifier!
    		error(11, curToken);
    	}
    	if (symbolTable[place].kind != 2 && symbolTable[place].kind != 1)
    	{
    		//Can't write out a procedure!
    		error(32, curToken);
    	}
    	
    	//Okay, it exists, and has no issues! We need to generate code to write to the screen this symbol...
    	emit(3, rc, 0, symbolTable[place].addr); // Load into rc what is at the address of the symbol we are talking about
    	emit(9, rc, 0, 1); // Print what is in rc to the screen
    	
    	curToken++;
    }
    printf("Got statement from %d-%d\n", start, curToken);
}

void condition()
{
    int start = curToken;
    if (getTokenType(curToken) == oddsym)
    {
        curToken++;
        
        // rc-1 holds the result of expression
        expression();
        
        emit(17, rc-1, 0, 0);
    }
    else
    {
        expression();
        
        if (getTokenType(curToken) != eqlsym && getTokenType(curToken) != neqsym && getTokenType(curToken) != lesssym && getTokenType(curToken) != leqsym && getTokenType(curToken) !=gtrsym && getTokenType(curToken) != geqsym)
        {
            //Looking for relational symbol
            error(20, curToken);
        }
        int relop = getTokenType(curToken);
        curToken++;
        
        expression();
        
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
    printf("Got condition from %d-%d\n", start, curToken);
}

void expression()
{
    int start = curToken;
	//Assume it's just a normal and positive expression...
    int adop = plussym;
    if (getTokenType(curToken) == plussym || getTokenType(curToken) == minussym)
    {
    	//If it was a minus sym, then here we'd set adop to minussym.
        adop = getTokenType(curToken);
        curToken++;
    }
    
    //Load a term! After this a term will be in rc-1.
    term();
    
    if (adop == minussym)
    {
    	emit(12, rc-1, 0, 0);
    }
    
    while (getTokenType(curToken) == plussym || getTokenType(curToken) == minussym)
    {
    	//Remember the operation...
    	int operation = getTokenType(curToken);
    	
    	//Load the next term...
        curToken++;
        term();
        
        //Apply the operation!
        if (operation == plussym)
        	emit(13, rc-2, rc-2, rc-1);
        else
        	emit(14, rc-2, rc-2, rc-1);
        rc = rc-1;
        	
    }
    printf("Got expression from %d-%d\n", start, curToken);
}

void term()
{
    int start = curToken;
    
    //This will get us a factor into register rc-1.
    factor();
    
    //Do we have more terms factors?
    while (getTokenType(curToken) == multsym || getTokenType(curToken) == slashsym)
    {
    	//Remember the operation...
    	int operation = getTokenType(curToken);
    	
    	//Load the next factor...
        curToken++;
        factor();
        
        //Perform the operation on rc-2 and rc-1 and store into rc-2 and set rc to rc-1!
        if (operation == multsym)
        	emit(15, rc-2, rc-2, rc-1);
        else
        	emit(16, rc-2, rc-2, rc-1);
        rc = rc-1;
        
    }
    
    printf("Got term from %d-%d\n", start, curToken);
}

void factor()
{
    int start = curToken;
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
        if (symbolTable[place].kind != 1 && symbolTable[place].kind != 2)
        {
        	//Factor can't be a procedure!
        	error(21, curToken);
        }
        
        //Ok, valid identifier, so now emit away!
        emit(3, rc, 0, symbolTable[place].addr); // Load into register rc the value at address of the identifier...
        rc++;									 // Let the world know that we used a register
        
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
        
        //Get the number's literal value...
        int literalValue = atoi(tokenList[curToken]);
        
        emit(1, rc, 0, literalValue); // Load immediately into register rc literalValue
        rc++;						  // Let the world know that we used a register
        
        
        curToken++;
    }
    else if (getTokenType(curToken) == lparentsym)
    {
        curToken++;
        
        //Expression should leave us one register value with the answer!
        expression();
        
        if (getTokenType(curToken) != rparentsym)
        {
            //Expect )
            error(22, curToken);
        }
        curToken++;
    }
    else
    {
        //Expected a factor, did not find one.
        error(28, curToken);
    }
    printf("Got factor from %d-%d\n", start, curToken);
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
	
    openFiles(argv[1], argv[2]);
    readInputFile();
    populateTokenList();
    printTokenList();
    clearSymbolTable();
    clearCodeArray();
    doTheAwesomeParsingAndCodeGenerating();
	printSymbolTable();
	printCodeArray();
	writeCodeArray();

    return 0;
}

// --------------------------------------------------End Main Code--------------------------------------------------
