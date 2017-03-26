----------------------------General Program Usage----------------------------

This program compiles code written in tiny PL/0. To use this program,
first compile it. Below is an example of how to compile the program
on a linux system:

gcc -o compile compile.c

Once you compile the program, then the program can be run. Note that 
you must place every file the program needs in the same directory as
the compile executable. Below is an example of how to run the program
on a linux system:

./compile -l -a -v

Upon running the program, it will first compile what it needs to run: the
lexical analyzer, the parser and code generator, and the vm. After this, it
will use the three parts in the order given above to compile the program
that is stored in a file named input.txt that is in the same directory as the
program itself. If the compilation is successful, then the program will be
run on the included vm. If the code in input.txt is invalid for some reason,
then the compilation will not succeed and an appropriate error message will
be written to the screen.

This program accepts arguments, which were shown in the example command above.
Below are descriptions of what the arguments do:

-l		Prints the output from the lexical analyzer to the screen.
-a		Prints the generated assembly code to the screen.
-v		Prints the virtual machine output to the screen after it runs.

-----------------------------Additional Included Files----------------------------


------------------------------Additional Information------------------------------

If, for some reason, you want to use the parser code generator alone, you can do
so by compiling parserCodeGenerator.c by itself. Below is an example of how to
compile parserCodeGenerator.c on a linux system:

gcc -o parserCodeGenerator parserCodeGenerator.c

Once compiled, you can run it using an input file which must contain, on the last
line, the lexeme list to be used. Below is an example of how to run the parser code
generator by itself:

./parserCodeGenerator [input file name] [output file name]

Where "[input file name]" is replaced with the name of the file meeting the criteria
listed above, and "[output file name]" is the name of the file you want the output
(assembly code) to be placed in. If the parser code generator finds an error, execution
will halt and a file named "ef" will contain the error information.
