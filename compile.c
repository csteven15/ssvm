/*
	COP3402 Homework #4: Parser & Code Generator Assignment
	4/14/2017
	Philip Rodriguez & Steven Chen
	ph644520 & st140537
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Returns true if argv contains the string value.
int contains(int argc, char** argv, char * value)
{
	for(int i = 0; i < argc; i++)
	{
		if (strcmp(value, argv[i]) == 0)
			return 1;
	}
	return 0;
}

/*
	This method will read all contents of the file
	at [filePath] into the character array readBuffer.
*/
char readBuffer[262144];
void readFile(char * filePath)
{
	FILE * file;
	file = fopen(filePath, "r");
	

	fseek(file, 0, SEEK_END);
	int inputSize = ftell(file)-1;
	fseek(file, 0, SEEK_SET);
	int i;
	for(i = 0; i < inputSize; i++)
	{
		fscanf(file, "%c", &readBuffer[i]);
	}
	readBuffer[i] = '\0';
	
	fclose(file);
}

/*
	This method will check to see if the file named
	"ef" exists, and if it does, then it will print
	its line of content and then terminate the program.
*/
void checkError()
{
	FILE * errorFile;
	errorFile = fopen("ef", "r");
	if (errorFile)
	{
		//It exists! There is an error!
		fclose(errorFile);
		readFile("ef");
		printf("%s", readBuffer);
		exit(1);
	}
	else
	{
		//No error yet!
	}
}

int main(int argc, char** argv)
{
	char * inputFileName = "input.txt";
	
	//Clean up our workspace...
	//printf("Cleaning workspace...\n");
	system("rm -f ef");
	system("rm -f lao");
	system("rm -f pcgo");
	system("rm -f vmo");
	//printf("Workspace cleaned!\n");
	
	//Compile everything we'll need!
	//printf("Compiling required things...\n");
	system("gcc -o lexicalAnalyzer lexicalAnalyzer.c");
	system("gcc -o parserCodeGenerator parserCodeGenerator.c");
	system("gcc -o vm vm.c");
	//printf("Required things compiled!\n");
	
	//Now, run everything in order and such! Print output as needed...
	
	printf("Running lexical analyzer...\n");
	char cmd[1024];
	sprintf(cmd, "./lexicalAnalyzer %s lao", inputFileName);
	system(cmd);
	checkError();
	
	if (contains(argc, argv, "-l"))
	{
		readFile("lao");
		printf("Lexical Analyzer Output:\n%s\n", readBuffer);
	}

	printf("\nRunning parser code generator...\n");
	system("./parserCodeGenerator lao pcgo");
	checkError();
	
	//No errors occurred if we are here, so say that!
	printf("No errors, program is syntactically correct!\n");
	
	if (contains(argc, argv, "-a"))
	{
		readFile("pcgo");
		printf("Parser/Code Generator Output:\n%s\n", readBuffer);
	}

	printf("\nRunning vm...\n");
	system("./vm pcgo vmo");
	checkError();
	
	if (contains(argc, argv, "-v"))
	{
		readFile("vmo");
		printf("Virtual Machine Output:\n%s\n", readBuffer);
	}
	
	
	printf("\n\nCompile program finished successfully.\n");
	return 0;
}
