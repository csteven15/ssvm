#include<stdio.h>
#include<stdlib.h>

#define MAX_INPUT_SIZE 32768

//A method to halt execution after presenting the
// error corresponding to the code [code].
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

void openFiles()
{
	inputFile = fopen("input.txt", "r");
	outputFile = fopen("output.txt", "w");
}

//The actual input file data handling...
char lexemeList[MAX_INPUT_SIZE];

void readInputFile()
{
	//TODO: find the lexeme list and read only the lexeme list data into lexemeList. This information should be exclusively on the last line of the output from lexicalAnalyzer.
}

int main()
{
	//TODO: Later, we can make the arguments determine the input and output file paths, but for now it's hardcoded as input.txt and output.txt.
	openFiles();
	readInputFile();
	return 0;
}
