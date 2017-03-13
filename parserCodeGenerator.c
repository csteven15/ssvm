#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
	Note that actual capacities listed below are one
	less due to null terminator requirements.
*/
#define MAX_INPUT_SIZE 32768
#define MAX_TOKEN_SIZE 64

// --------------------------------------------------Begin Random Additional Code--------------------------------------------------

/*
	This method halts the program execution after printing an error 
	with code [code].
*/
void error(int code)
{
	printf("ERROR: ");
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
		printf("Semicolon or } expected.\n");
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
		error(26);
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

// --------------------------------------------------End Token List Management Code--------------------------------------------------

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

void doTheAwesomeParsingAndCodeGenerating()
{
	program();
}

void program()
{
	block();
	//If current token is NOT a period
	if (getTokenType(curToken) != 19)
	{
		//Program must end witha period
		error(9);
	}
	curToken++;
}

void block()
{
	//Blah blah get block here...
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
	doTheAwesomeParsingAndCodeGenerating();
	
	return 0;
}

// --------------------------------------------------End Main Code--------------------------------------------------
