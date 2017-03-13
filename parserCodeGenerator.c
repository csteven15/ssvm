#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_INPUT_SIZE 32768

/*
	This method halts the program execution after printing an error 
	with code [code].
*/
void error(int code)
{
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
	
	exit(1);
}

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



// --------------------------------------------------Begin Parser/Code Generator Code--------------------------------------------------

void parseLexemes()
{
	
}

// --------------------------------------------------End Parser/Code Generator Code--------------------------------------------------

/*
	I expect this program to take an input file of lexemes, parse it, and print out
	a file containing whether or not parsing succeeded, and then the generated code.
*/
int main()
{
	//TODO: Later, we can make the arguments determine the input and output file paths, but for now it's hardcoded as input.txt and output.txt.
	openFiles();
	readInputFile();
	parseLexemes();
	
	return 0;
}
