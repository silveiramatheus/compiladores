// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Função principal

// Include input-output functions
#include <iostream>

// yylex header
int yylex();

// yytext reference
extern char* yytext;

// Variables from lex file
extern int running;
extern int lineCount;
extern FILE *yyin;

// From symbols.hpp
void printSymbols();

// From scanner.l
int isRunning(void);
int getLineNumber();

// from parser.ypp
void printAST();
void decompileAST(std::string fileName);

int yyparse();

int main(int argc, char** argv)
{
    if ((yyin = fopen(argv[1],"r")) == 0) {
        printf("Cannot open file %s... \n",argv[1]);
        exit(1);
    }

    if(!(fopen(argv[2], "w+"))){
        printf("Cannot open file %s... \n",argv[2]);
		exit(1);
	}

    if (argc == 3)
    {
        printf("Program: %s\nInput: %s \nOutput: %s\n", argv[0], argv[1], argv[2]);
    }

    // Parse the file
    yyparse();

    // Print the AST
    printAST();

    // Decompile the AST
    decompileAST(argv[2]);  // Second argument is the output file name
    
    // Print resulting symbols table
    printSymbols();

    // Print line count
    fprintf(stderr, "==================== Other Info ====================\n");
    fprintf(stderr, "Lines: %d\n", getLineNumber());

    exit(0);

    // End point
    return 0;
}