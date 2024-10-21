// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Função principal

// Include input-output functions
#include <iostream>

// If any token is not defined, include the tokens header
#ifndef KW_CHAR

#endif

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

int yyparse();

int main(int argc, char** argv)
{
    
    yyparse();

    // Print resulting symbols table
    fprintf(stderr, "\n");
    printSymbols();

    // Print line count
    fprintf(stderr, "Lines: %d\n", getLineNumber());

    exit(0);

    // End point
    return 0;
}