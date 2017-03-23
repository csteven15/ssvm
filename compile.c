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

void checkError()
{
	FILE * errorFile;
	errorFile = fopen("ef", "r");
	if (errorFile)
	{
		//It exists! There is an error!
		char message[1024];
		fgets(message, 1024, errorFile);
		printf("%s", message);
		fclose(errorFile);
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
	printf("Cleaning workspace...\n");
	system("rm -f ef");
	system("rm -f lao");
	system("rm -f pcgo");
	system("rm -f vmo");
	printf("Workspace cleaned!\n");
	
	//Compile everything we'll need!
	printf("Compiling required things...\n");
	system("gcc -o lexicalAnalyzer lexicalAnalyzer.c");
	system("gcc -o parserCodeGenerator parserCodeGenerator.c");
	system("gcc -o vm vm.c");
	printf("Required things compiled!\n");

	//Determine what needs to be run to print the desired output...
	int runLA = 0, runPCG = 0, runVM = 0;
	if (contains(argc, argv, "-l"))
	{
		runLA = 1;
	}
	if (contains(argc, argv, "-a"))
	{
		runLA = 1;
		runPCG = 1;
	}
	if (contains(argc, argv, "-v"))
	{
		runLA = 1;
		runPCG = 1;
		runVM = 1;
	}
	
	//now, run what we said we'd run!
	if (runLA)
	{
		printf("Running lexical analyzer...\n");
		char cmd[1024];
		sprintf(cmd, "./lexicalAnalyzer %s lao", inputFileName);
		system(cmd);
		checkError();
	}
	if (runPCG)
	{
		printf("Running parser code generator...\n");
		system("./parserCodeGenerator lao pcgo");
		checkError();
	}
	if (runVM)
	{
		printf("Running vm...\n");
		system("./vm pcgo vmo");
		checkError();
	}
	
	//Now, print the appropriate output!
	if (contains(argc, argv, "-l"))
	{
		//Print la stuff
	}
	if (contains(argc, argv, "-a"))
	{
		//Print parser stuff
	}
	if (contains(argc, argv, "-v"))
	{
		//Print vm stuff
	}
	
	
	printf("\n\nCompile program finished successfully.\n");
	return 0;
}
