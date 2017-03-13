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

// --------------------------------------------------Begin Random Additional Code--------------------------------------------------

/*
	This method halts the program execution after printing an error 
	with code [code].
*/
void error(int code, int token)
{
	printf("Error on token %d: ", token);
	if (code == 1)
		printf("Use of = instead of :=.\n");
	if (code == 2)
		printf("= must be followed by a number.\n");
	if (code == 3)
		printf("Identifier must be followed by =.\n");
	if (code == 4)
		printf("const, var, procedure must be followed by identifier.\n");
	if (code == 5)
		printf("Semicolon or comma missing.\n");
	if (code == 6)
		printf("Incorrect symbol after procedure declaration.\n");
	if (code == 7)
		printf("Statement expected.\n");
	if (code == 8)
		printf("Incorrect symbol after ststement part in block.\n");
	if (code == 9)
		printf("Period expected.\n");
	if (code == 10)
		printf("Semicolon between statements missing.\n");
	if (code == 11)
		printf("Undeclared identifier.\n");
	if (code == 12)
		printf("Assignment to constant or procedure is not allowed.\n");
	if (code == 13)
		printf("Assignment operator expected.\n");
	if (code == 14)
		printf("call must be followed by an identifier.\n");
	if (code == 15)
		printf("Call of a constant or a variable is meaningless.\n");
	if (code == 16)
		printf("then expected.\n");
	if (code == 17)
		printf("Semicolon expected.\n"); // I modified this one to not include "or }" because that makes no sense to me.
	if (code == 18)
		printf("do expected.\n");
	if (code == 19)
		printf("Incorrect symbol following statement.\n");
	if (code == 20)
		printf("Relational operator expected.\n");
	if (code == 21)
		printf("Expression must not contain a procedure identifier.\n");
	if (code == 22)
		printf("Right parenthesis missing.\n");
	if (code == 23)
		printf("The preceding factor cannot begin with this symbol.\n");
	if (code == 24)
		printf("An expression cannot begin with this symbol.\n");
	if (code == 25)
		printf("This number is too large.\n");
	if (code == 26)
		printf("The input file is too large.\n");
	if (code == 27)
		printf("Call must be followed by an identifier for a procedure.");
	
	exit(1);
}

/*
	This method does what it sounds like. It appends one character, [newChar], to
	the end of the passed in string, [string]. Assumes the passed in string is
	large enough to accomodate the extra character.
*/
void appendCharToString(char * string, char newChar)
{
	int len = strlen(string);
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
void openFiles()
{
	inputFile = fopen("laOutput.txt", "r");
	outputFile = fopen("output.txt", "w");

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
	int inputSize = ftell(inputFile)-1;
	
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

	int len = strlen(lexemeList);
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
	This method puts the symbol [s] in the first available spot in the symbol
	table. Returns 1 if there was a spot, and 0 if the symbol table was full.
*/
int addSymbol(symbol s)
{
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

void doTheAwesomeParsingAndCodeGenerating();
void program();
void block();
void statement();

void doTheAwesomeParsingAndCodeGenerating()
{
	program();
}

void program()
{
	block();
	//If current token is NOT a period
	if (getTokenType(curToken) != periodsym)
	{
		//Program must end with a period
		error(9, curToken);
	}
	curToken++;
}

//TODO: ask about this structure of unchained ifs
void block()
{
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
			//Double skip because following identsym is the identifier name itself...
			curToken++;
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
			//Double skip because following numbersym is the number itself...
			curToken++;
			curToken++;
		}
		while(getTokenType(curToken) == commasym);
		
		//Now we expect a semicolon...
		if (getTokenType(curToken) != semicolonsym)
		{
			//Missing a semicolon!
			error(5, curToken);
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
			//Double skip because following the identsym is the identifier itself...
			curToken++;
			curToken++;
		}
		while(getTokenType(curToken) == commasym);
		
		//Now we expect a semicolon...
		if (getTokenType(curToken) != semicolonsym)
		{
			//Missing a semicolon!
			error(5, curToken);
		}
	}
	
	while(getTokenType(curToken) == procsym)
	{
		curToken++;
		if (getTokenType(curToken) != identsym)
		{
			//Expected an identifier after beginning of procedure!
			error(4, curToken);
		}
		//Double skip because following the identsym is the identifier itself...
		curToken++;
		curToken++;
		if (getTokenType(curToken) != semicolonsym)
		{
			//Expected semicolon after procedure declaration!
			error(17, curToken);
		}
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
	
	curToken++;
}

void statement()
{
	//Statement could just be like x := 6;
	if (getTokenType(curToken) == identsym)
	{
		//Double skip because after identsym is the identifier itself...
		curToken++;
		curToken++;
		if (getTokenType(curToken) != becomesym)
		{
			//Must be assignment statement
			error(13, curToken);
		}
		
		curToken++;
		
		//Parse an expression...
		expression();
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
		condition();
		if (getTokenType(curToken) != thensym)
		{
			//should have been a then after the condition of the if...
			error(16, curToken);
		}
		curToken++;
		statement();	
	}
	else if (getTokenType(curToken) == whilesym)
	{
		curToken++;
		condition();
		if (getTokenType(curToken) != dosym)
		{
			//Expected a do after condition for while
			error(18, curToken);
		}
		curToken++;
		statement();
	}
}



// --------------------------------------------------End Parser/Code Generator Code--------------------------------------------------

// --------------------------------------------------Begin Main Code--------------------------------------------------

/*
	I expect this program to take an input file of lexemes, parse it, and print out
	a file containing whether or not parsing succeeded, and then the generated code.
*/
int main()
{
	//TODO: Later, we can make the arguments determine the input and output file paths, but for now it's hardcoded as input.txt and output.txt.
	openFiles();
	readInputFile();
	populateTokenList();
	printTokenList();
	clearSymbolTable();
	doTheAwesomeParsingAndCodeGenerating();
	
	return 0;
}

// --------------------------------------------------End Main Code--------------------------------------------------
