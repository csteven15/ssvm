/*
	COP3402 Homework #3: Parser & Code Generator Assignment
	3/23/2017
	Philip Rodriguez & Steven Chen
	ph644520 & st140537
*/

#include <stdio.h>
#include <string.h>

//Set constant values (2F)
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

//Our structure for an instruction in code!
typedef struct instruction {
	int op;
	int r;
	int l;
	int m;
} instruction;

//Files!
FILE * codeFile;
FILE * outputFile;

//Halt flag
int halt;

//CPU Registers
int bp, sp, pc;
instruction ir;

//Our register file
int r[16];

//The stack (which apparently starts at location 1)
int stack[MAX_STACK_HEIGHT];

//A place for the code!
int codeLength;
instruction code[MAX_CODE_LENGTH];

//A map from opcode to string operation
char opmap[25][16];

//Some stuff for keeping track of where activation record bars belong
int arbl[MAX_STACK_HEIGHT];
int arblp = -1;

void addBar(int sp)
{
	arblp++;
	arbl[arblp] = sp;
}

void removeBar()
{
	arblp--;
	if (arblp < -1)
		arblp = -1;
}

//Base method given by assignment sheet (10A)
int base(int l, int base)
{
	int b1;
	b1 = base;
	while(l > 0)
	{
		b1 = stack[b1+1];
		l--;
	}
	return b1;
}

//This prints one line of output for (7B)
void printMachineState(int instructionLine)
{
	instruction i = code[instructionLine];
	
	//Print out everything but the stack...
	fprintf(outputFile, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d", instructionLine, opmap[i.op], i.r, i.l, i.m, pc, bp, sp);
	
	//Now we need to print out the stack...
	//Since the stack starts at 1 according to the assignment sheet, just print from there...
	int barIterator = 0;
	for(int i = 1; i <= sp; i++)
	{
		fprintf(outputFile, "\t%d", stack[i]);
		
		//Do we need to print a bar here?
		if (barIterator <= arblp && arbl[barIterator] == i && i != sp)
		{
			fprintf(outputFile, "\t|");
			barIterator++;
		}
	}
	fprintf(outputFile, "\n");
}

//Perform the fetch cycle (2A)
void fetch()
{
	//Get instruction (2A)
	ir = code[pc];
	
	//Increment the program counter (2A)
	pc = pc + 1;
}

//Perform the execute cycle (2B)
void execute()
{
	//The line of the instruction we are about to execute is pc-1.
	int instructionLine = pc-1;

	if (ir.op == 0)
	{
		//Not defined!
	}
	else if (ir.op == 1)
	{
		// LIT: Load literal into register (3A/4A)
		r[ir.r] = ir.m;
	}
	else if (ir.op == 2)
	{
		// RTN: Return from a subroutine and restore caller environment (3B/4B)
		sp = bp-1;
		// Question: Why aren't these +2 and +3 respectively? Isn't an AR consisting of SL, DL, PC/RA, locals?
		// Answer: Because there's also a spot to return a value. The AR is like Ret, SL, DL, PC/RA, locals.
		bp = stack[sp+3]; 
		pc = stack[sp+4];
		
		//If sp is 0 after return, halt!
		if (sp == 0)
		{
			halt = 1;
		}
		
		//Remove topmost bar!
		removeBar();
	}
	else if (ir.op == 3)
	{
		// LOD: Load value from stack into register (3C/4C)
		r[ir.r] = stack[base(ir.l, bp) + ir.m];
	}
	else if (ir.op == 4)
	{
		// STO: Store value from register into stack (3D/4D)
		stack[base(ir.l, bp) + ir.m] = r[ir.r];
	}
	else if (ir.op == 5)
	{
		//Plop down a bar
		addBar(sp);
	
		// CAL: Method call (3E/4E)
		stack[sp+1] = 0;
		stack[sp+2] = base(ir.l, bp);
		stack[sp+3] = bp;
		stack[sp+4] = pc;
		bp = sp + 1;
		pc = ir.m;
	}
	else if (ir.op == 6)
	{
		// INC: increment (3F/4F)
		sp = sp + ir.m;
	}
	else if (ir.op == 7)
	{
		// JMP: jump (3G/4G)
		pc = ir.m;
	}
	else if (ir.op == 8)
	{
		// JPC: jump conditional (3H/4H)
		if (r[ir.r] == 0)
		{
			pc = ir.m;
		}
	}
	else if (ir.op == 9)
	{
		// SIO: Print what is at register (3I/4I)
		//Question: Should this print newline after content?
		printf("%d\n", r[ir.r]);
	}
	else if (ir.op == 10)
	{
		// SIO: Read input to register (3J/4J)
		//Question: Is this correct? Should we read in an integer using scanf?
		scanf("%d", &r[ir.r]);
	}
	else if (ir.op == 11)
	{
		// SIO: Set half flag to 1 (3K/4K)
		halt = 1;
	}
	else if (ir.op == 12)
	{
		// NEG
		r[ir.r] = -1 * r[ir.r];
	}
	else if (ir.op == 13)
	{
		//ADD
		r[ir.r] = r[ir.l] + r[ir.m];
	}
	else if (ir.op == 14)
	{
		// SUB
		r[ir.r] = r[ir.l] - r[ir.m];
	}
	else if (ir.op == 15)
	{
		// MUL
		r[ir.r] = r[ir.l] * r[ir.m];
	}
	else if (ir.op == 16)
	{
		// DIV
		r[ir.r] = r[ir.l] / r[ir.m];
	}
	else if (ir.op == 17)
	{
		// ODD
		r[ir.r] = r[ir.r] % 2;
	}
	else if (ir.op == 18)
	{
		// MOD
		r[ir.r] = r[ir.l] % r[ir.m];
	}
	else if (ir.op == 19)
	{
		// EQL
		r[ir.r] = r[ir.l] == r[ir.m];
	}
	else if (ir.op == 20)
	{
		// NEQ
		r[ir.r] = r[ir.l] != r[ir.m];
	}
	else if (ir.op == 21)
	{
		// LSS
		r[ir.r] = r[ir.l] < r[ir.m];
	}
	else if (ir.op == 22)
	{
		// LEQ
		r[ir.r] = r[ir.l] <= r[ir.m];
	}
	else if (ir.op == 23)
	{
		// GTR
		r[ir.r] = r[ir.l] > r[ir.m];
	}
	else if (ir.op == 24)
	{
		// GEQ
		r[ir.r] = r[ir.l] >= r[ir.m];
	}
	else
	{
		//Not defined!
	}
	
	//After execution, print the state! (2B)
	printMachineState(instructionLine);
}

// This handles (7A), printing the program in interpreted assembly with line numbers.
// It also prepares for (7B) by printing the heading for (7B).
void outputPartOne()
{
	fprintf(outputFile, "Line\tOP\tR\tL\tM\n");
	
	char opstr[16];
	for(int l = 0; l < codeLength; l++)
	{
		strcpy(opstr, opmap[code[l].op]);
		fprintf(outputFile, "%d\t%s\t%d\t%d\t%d\n", l, opstr, code[l].r, code[l].l, code[l].m);
	}
	
	fprintf(outputFile, "Initial Values\t\t\t\tpc\tbp\tsp\n");
}

//This populates the global 2D array opmap, and it should be called before opmap is used.
void populateOpMap()
{
	strcpy(opmap[0], "INVALID");
	strcpy(opmap[1], "lit");
	strcpy(opmap[2], "rtn");
	strcpy(opmap[3], "lod");
	strcpy(opmap[4], "sto");
	strcpy(opmap[5], "cal");
	strcpy(opmap[6], "inc");
	strcpy(opmap[7], "jmp");
	strcpy(opmap[8], "jpc");
	strcpy(opmap[9], "sio");
	strcpy(opmap[10], "sio");
	strcpy(opmap[11], "sio");
	strcpy(opmap[12], "neg");
	strcpy(opmap[13], "add");
	strcpy(opmap[14], "sub");
	strcpy(opmap[15], "mul");
	strcpy(opmap[16], "div");
	strcpy(opmap[17], "odd");
	strcpy(opmap[18], "mod");
	strcpy(opmap[19], "eql");
	strcpy(opmap[20], "neq");
	strcpy(opmap[21], "lss");
	strcpy(opmap[22], "leq");
	strcpy(opmap[23], "gtr");
	strcpy(opmap[24], "geq");
}

//Initialize the virtual machine!
void init(char * codePath, char * outputPath)
{
	//Open our output files. 
	codeFile = fopen(codePath, "r");
	outputFile = fopen(outputPath, "w");
	
	//Read in the code file!
	int cl = 0, opb, rb, lb, mb;
	while(fscanf(codeFile, "%d %d %d %d", &opb, &rb, &lb, &mb) == 4)
	{
		//We read a command successfully!
		code[cl].op = opb;
		code[cl].r = rb;
		code[cl].l = lb;
		code[cl].m = mb;
		cl++;
	}
	codeLength = cl;
	
	//Fulfill assignment spec (7A)
	populateOpMap();
	outputPartOne();
	
	//Initialize the CPU Registers (2C)
	sp = 0;
	bp = 1;
	pc = 0;
	ir.op = 0;
	ir.r = 0;
	ir.l = 0;
	ir.m = 0;
	
	//Initialize the stack values to 0 (2D)
	for(int i = 0; i < MAX_STACK_HEIGHT; i++)
	{
		stack[i] = 0;
	}
	
	//Initialize register file values to zero (2E)
	for(int i = 0; i < 16; i++)
	{
		r[i] = 0;
	}
	
	//Un-halt
	halt = 0;
	
	//We'll stop if we halt or go outside the program!
	//int iter = 0;//REMOVE DEBUG
	while (halt == 0 && pc < codeLength)// && iter < 2000)//REMOVE DEBUG (iter)
	{
		fetch();
		execute();
		//iter++; //REMOVE DEBUG
	}
}


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Invalid arguments to virtual machine!\nUSAGE: ./vm [code file] [output file]\n");
		return 1;
	}
    init(argv[1], argv[2]);
    return 0;
}
