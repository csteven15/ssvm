/*

Philip Rodriguez
COP3402 HW2
Lexical Analyzer
2/19/2016

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_NUMBER_LENGTH 5
#define MAX_IDENTIFIER_LENGTH 11

#define MAX_CODE_LENGTH 32768


//~~~Special things~~~

//Internal representation stuff
int nulsym = 1, identsym = 2, numbersym = 3, plussym = 4,
minussym = 5, multsym = 6, slashsym = 7, oddsym = 8, eqlsym = 9,
neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
periodsym = 19, becomesym = 20, beginsym = 21, endsym = 22, ifsym = 23,
thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33;


char symbols[] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};
char reserved[14][32] = {
"const",
"var",
"procedure",
"call",
"begin",
"end",
"if",
"then",
"else",
"while",
"do",
"read",
"write",
"odd"
};

int reservedIndex(char * identifier)
{
	for(int i = 0; i < 14; i++)
	{
		if (strcmp(reserved[i], identifier) == 0)
		{
			return i;
		}
	}
	return -1;
}

int mapReserved(int spotInReserved)
{
	if (spotInReserved == 0)
		return constsym;
	if (spotInReserved == 1)
		return varsym;
	if (spotInReserved == 2)
		return procsym;
	if (spotInReserved == 3)
		return callsym;
	if (spotInReserved == 4)
		return beginsym;
	if (spotInReserved == 5)
		return endsym;
	if (spotInReserved == 6)
		return ifsym;
	if (spotInReserved == 7)
		return thensym;
	if (spotInReserved == 8)
		return elsesym;
	if (spotInReserved == 9)
		return whilesym;
	if (spotInReserved == 10)
		return dosym;
	if (spotInReserved == 11)
		return readsym;
	if (spotInReserved == 12)
		return writesym;
	if (spotInReserved == 13)
		return oddsym;
    return -1;
}

//TODO nullsym = 1?????
int mapSymbol(char * symbol)
{
	if (strcmp(symbol, "+") == 0)
		return plussym;
	if (strcmp(symbol, "-") == 0)
		return minussym;
	if (strcmp(symbol, "*") == 0)
		return multsym;
	if (strcmp(symbol, "/") == 0)
		return slashsym;
	if (strcmp(symbol, "(") == 0)
		return lparentsym;
	if (strcmp(symbol, ")") == 0)
		return rparentsym;
	if (strcmp(symbol, "=") == 0)
		return eqlsym;
	if (strcmp(symbol, ",") == 0)
		return commasym;
	if (strcmp(symbol, ".") == 0)
		return periodsym;
	if (strcmp(symbol, "<") == 0)
		return lessym;
	if (strcmp(symbol, ">") == 0)
		return gtrsym;
	if (strcmp(symbol, ";") == 0)
		return semicolonsym;

	if (strcmp(symbol, "<>") == 0)
		return neqsym;
	if (strcmp(symbol, "<=") == 0)
		return leqsym;
	if (strcmp(symbol, ">=") == 0)
		return geqsym;
	if (strcmp(symbol, ":=") == 0)
		return becomesym;
    return -1;
}

//~~~Character Manipulation Methods~~~

//Returns 1 iff theChar is a uppercase letter, 0 otherwise.
int isUpper(char theChar)
{
	return theChar >= 65 && theChar <= 90;
}

//Returns 1 iff theChar is a lowercase letter, 0 otherwise.
int isLower(char theChar)
{
	return theChar >= 97 && theChar <= 122;
}

//Returns 1 iff theChar is an alphabetic letter, 0 otherwise.
int isAlpha(char theChar)
{
	return isLower(theChar) || isUpper(theChar);
}

//Returns 1 iff theChar is a digit 0-9, 0 otherwise.
int isDigit(char theChar)
{
	return theChar >= 48 && theChar <= 57;
}

//Returns 1 iff theChar is either a digit or an alphabetic letter, 0 otherwise.
int isAlphanumeric(char theChar)
{
	return isAlpha(theChar) || isDigit(theChar);
}

//Returns 1 iff theChar is a valid whitespace or invisible character, 0 otherwise.
int isInvisible(char theChar)
{
	return theChar == 9 || theChar == 10 || theChar == 13 || theChar == 32;
}

int isSymbol(char theChar)
{
	for(int i = 0; i < 13; i++)
	{
		if (symbols[i] == theChar)
			return 1;
	}
	return 0;
}

int isValid(char theChar)
{
	return isAlphanumeric(theChar) || isSymbol(theChar) || isInvisible(theChar) || theChar == '\0';
}

//Allows a user to test the character manipulation methods above
void runCharacterTester()
{
	char dump;
	while(1)
	{
		printf("Enter a character to test:");
		char theChar;
		scanf("%c", &theChar);
		scanf("%c", &dump);
		printf("%c isUpper is %d\n", theChar, isUpper(theChar));
		printf("%c isLower is %d\n", theChar, isLower(theChar));
		printf("%c isAlpha is %d\n", theChar, isAlpha(theChar));
		printf("%c isDigit is %d\n", theChar, isDigit(theChar));
		printf("%c isAlphanumeric is %d\n", theChar, isAlphanumeric(theChar));
		printf("%c isInvisible is %d\n", theChar, isInvisible(theChar));
		printf("%c isSymbol is %d\n\n", theChar, isSymbol(theChar));
	}
}

//~~~Error state stuff~~~

void throwError(char * message)
{
	printf("An error with the input caused the process to fail: %s\n", message);
	exit(0);
}

//~~~File handling stuff~~~

//Our input file
int ip = 0;
char inputChars[MAX_CODE_LENGTH];

//The output file
FILE * outFile;

//File operations
char getChar(int ignoreValidity)
{
	char nextChar = inputChars[ip];
	ip++;
	if (!ignoreValidity && !isValid(nextChar))
	{
		throwError("Invalid character encountered!");
	}
	return nextChar;
}

void ungetChar()
{
	ip--;
}

//The buffer
int bp;
char buffer[16];
void clearBuffer()
{
	bp = 0;
	for(int i = 0; i < 16; i++)
	{
		buffer[i] = '\0';
	}
}

void addToBuffer(char theChar)
{
	if (bp < 16)
	{
		buffer[bp] = theChar;
		bp++;
	}
}

void openFiles(char * inputFile, char * outputFile)
{
	FILE * inFile = fopen(inputFile, "r");
	outFile = fopen(outputFile, "w");

	fseek(inFile, 0, SEEK_END);
	int inputSize = ftell(inFile)-1;
	fseek(inFile, 0, SEEK_SET);
	int i;
	for(i = 0; i < inputSize; i++)
	{
		fscanf(inFile, "%c", &inputChars[i]);
	}
	inputChars[i] = '\0';
}

//~~~Text processing~~~
char lexemeTable[MAX_CODE_LENGTH];
char lexemeList[MAX_CODE_LENGTH];

void clearLexemeOutput()
{
	for(int i = 0; i < MAX_CODE_LENGTH; i++)
	{
		lexemeTable[i] = '\0';
		lexemeList[i] = '\0';
	}
	strcat(lexemeTable, "Lexeme Table:\nlexeme       token type\n");
	strcat(lexemeList, "Lexeme List:\n");
}

void insertToLexemeTable(char * lexeme, int tokenType)
{
	char temp[64];
	char spaces[64] = {'\0'};
	while(strlen(lexeme) + strlen(spaces) < 13)
		strcat(spaces, " ");
	sprintf(temp, "%s%s%d\n", lexeme, spaces, tokenType);
	strcat(lexemeTable, temp);
}

void insertIntToLexemeList(int num)
{
	char temp[64];
	sprintf(temp, "%d ", num);
	strcat(lexemeList, temp);
}

void insertStrToLexemeList(char * identifier)
{
	char temp[64];
	sprintf(temp, "%s ", identifier);
	strcat(lexemeList, temp);
}

void processIdentifier(char * identifier)
{
	printf("Processing identifier \"%s\"\n", identifier);
	int index = reservedIndex(identifier);
	if (index > -1)
	{
		//It's reserved!
		int mapping = mapReserved(index);
		insertIntToLexemeList(mapping);
		insertToLexemeTable(identifier, mapping);
	}
	else
	{
		//Not reserved!
		insertIntToLexemeList(2);
		insertStrToLexemeList(identifier);
		insertToLexemeTable(identifier, 2);
	}
}

void processNumber(char * num)
{
	printf("Processing number \"%s\"\n", num);
	insertToLexemeTable(num, 3);
	insertIntToLexemeList(3);
	insertStrToLexemeList(num);
}

void processSymbol(char * sym)
{
	printf("Processing symbol \"%s\"\n", sym);
	insertToLexemeTable(sym, mapSymbol(sym));
	insertIntToLexemeList(mapSymbol(sym));
}

void processText()
{
	printf("Beginning lexical analysis...\n");

	//Clear out the output arrays...
	clearLexemeOutput();

	//Run through the unput characters...
	char nextChar = ' ';
	while(nextChar != '\0')
	{
		clearBuffer();
		while(isInvisible(nextChar = getChar(0)))
		{
			//Trash the invisible characters
		}

		//It's not invisible if we are here!
		if (isAlpha(nextChar))
		{
			addToBuffer(nextChar);
			while(isAlphanumeric(nextChar = getChar(0)))
			{
				addToBuffer(nextChar);
				if (strlen(buffer) > MAX_IDENTIFIER_LENGTH)
				{
					//Invalid identifier length
					throwError("Identifier too long!");
				}
			}
			ungetChar();
			//Process identifier in buffer
			processIdentifier(buffer);
		}
		else if (isDigit(nextChar))
		{
			addToBuffer(nextChar);
			while(isDigit(nextChar = getChar(0)))
			{
				addToBuffer(nextChar);
				if (strlen(buffer) > MAX_NUMBER_LENGTH)
				{
					//Invalid number length
					throwError("Number too long!");
				}
			}
			//Was this number followed by a letter?
			if (isAlpha(nextChar))
			{
				throwError("Variable does not start with letter!");
			}
			//It was not followed by a letter.. so we are okay!
			ungetChar();
			//Process number in buffer.
			processNumber(buffer);
		}
		else if (isSymbol(nextChar))
		{
			addToBuffer(nextChar);
			if (nextChar == '+' || nextChar == '-' || nextChar == '*' || nextChar == '(' || nextChar == ')' || nextChar == '=' || nextChar == ',' || nextChar == '.' || nextChar == ';')
			{
				//Process just that symbol itself!
				processSymbol(buffer);
			}
			else if (nextChar == '/')
			{
				nextChar = getChar(0);
				if (nextChar == '*')
				{
					state5:
					while((nextChar = getChar(1)) != '*')
					{
						//Dump comment...
					}
					while((nextChar = getChar(1)) == '*')
					{
						//Dump comment...
					}
					if (nextChar == '/')
					{
						//Done with comment!
					}
					else
					{
						//Still in comment from * to another char...
						goto state5;
					}
				}
				else
				{
					//Process as a divide symbol...
					ungetChar();
					processSymbol(buffer);
				}
			}
			else if (nextChar == '<')
			{
				nextChar = getChar(0);
				if (nextChar == '=')
				{
					addToBuffer(nextChar);
					processSymbol(buffer);
				}
				else if (nextChar == '>')
				{
					addToBuffer(nextChar);
					processSymbol(buffer);
				}
				else
				{
					//Process less only
					ungetChar();
					processSymbol(buffer);
				}
			}
			else if (nextChar == '>')
			{
				nextChar = getChar(0);
				if (nextChar == '=')
				{
					addToBuffer(nextChar);
					processSymbol(buffer);
				}
				else
				{
					//Process greater than only
					ungetChar();
					processSymbol(buffer);
				}
			}
			else if (nextChar == ':')
			{
				//We must have an equal following it!
				nextChar = getChar(0);
				if (nextChar == '=')
				{
					addToBuffer(nextChar);
					processSymbol(buffer);
				}
				else
				{
					throwError("Invalid symbol!");
				}
			}

		}
		else if (nextChar == '\0')
		{
			break;
		}
		else
		{
			//Invalid state
			throwError("Invalid state!");
		}
	}
	printf("Reached end of file. See output.txt for output!\n");

	//Print results to output file...
	fprintf(outFile, "%s\n", lexemeTable);
	fprintf(outFile, "%s", lexemeList);
}

void echoInput()
{
	fprintf(outFile, "Source Program:\n%s\n\n", inputChars);
}

int main(int argc, char ** argv)
{
	if (argc < 3)
	{
		printf("Invalid arguments for lexical analyzer!\nUSAGE: ./lexicalAnalyzer [input file] [output file]\n");
		return 1;
	}
	openFiles(argv[1], argv[2]);
	echoInput();
	processText();
	return 0;
}
