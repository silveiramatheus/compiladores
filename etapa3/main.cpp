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
    // Parse the file
    yyparse();

    // Print the AST
    printAST();

    // Decompile the AST
    decompileAST(argv[1]);  // First argument is the output file name
    
    // Print resulting symbols table
    printSymbols();

    // Print line count
    fprintf(stderr, "==================== Other Info ====================\n");
    fprintf(stderr, "Lines: %d\n", getLineNumber());

    exit(0);

    // End point
    return 0;
}