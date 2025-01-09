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
extern bool areThereSyntaxErrors;
void printAST();
void decompileAST(std::string fileName);
int runSemanticTests();
void generateCode(bool print, bool optimize);

int yyparse();

int main(int argc, char** argv)
{
    if (argc >= 2)
    {
        if ((yyin = fopen(argv[1],"r")) == 0) {
            printf("Cannot open file %s... \n",argv[1]);
            exit(1);
        }
        fprintf(stderr, "Input: %s\n", argv[1]);
    }

    // if (argc >= 3)
    // {
    //     if(!(fopen(argv[2], "w+"))){
    //         printf("Cannot open file %s... \n",argv[2]);
    //         exit(1);
    //     }
    //     fprintf(stderr, "Decompiled AST: %s\n", argv[2]);
    // }

    // Get extra parameters
    bool shouldOptimize = false;
    for (int i = 2; i < argc; ++i)
    {
        std::string argString = argv[i];
        if (argString.compare("-o") == 0)
            shouldOptimize = true;
    }

    // Parse the file
    fprintf(stderr, "==================== Syntax Analysis ====================\n");
    yyparse();
    if (areThereSyntaxErrors == false)
    {
        fprintf(stderr, "Ok!\n");
    }

    // Print the AST
    printAST();

    // Decompile the AST
    // if (argc >= 3)
    //     decompileAST(argv[2]);  // Second argument is the output file name

    // Semantic verifications
    if (runSemanticTests() > 0) // if we have any semantic errors
        exit(4);    // exit with the appropriate error code
    else
        fprintf(stderr, "Ok!\n");

    // Print resulting symbols table
    printSymbols();

    // If there are no syntax errors...
    if (areThereSyntaxErrors == false)
    {
        // Generate intermediate code
        generateCode(true, shouldOptimize);
    }

    // Print line count
    fprintf(stderr, "==================== Other Info ====================\n");
    fprintf(stderr, "Lines: %d\n", getLineNumber());

    // Success
    exit(0);

    // End point
    return 0;
}