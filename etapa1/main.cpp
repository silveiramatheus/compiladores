// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Função principal

// Include input-output functions
#include <iostream>

// If any token is not defined, include the tokens header
#ifndef KW_CHAR
#include "tokens.h"
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

int main(int argc, char** argv)
{
    // Read the tokens
    while (isRunning())
    {
        switch (yylex())
        {
            // EOF should do nothing (running will be set to false)
            case EOF:           break;

            // Keywords
            case KW_CHAR:       fprintf(stderr, "KW_CHAR "); break;
            case KW_INT:        fprintf(stderr, "KW_INT "); break;
            case KW_IF:         fprintf(stderr, "KW_IF "); break;
            case KW_THEN:       fprintf(stderr, "KW_THEN "); break;
            case KW_ELSE:       fprintf(stderr, "KW_ELSE "); break;
            case KW_WHILE:      fprintf(stderr, "KW_WHILE "); break;
            case KW_READ:       fprintf(stderr, "KW_READ "); break;
            case KW_PRINT:      fprintf(stderr, "KW_PRINT "); break;
            case KW_RETURN:     fprintf(stderr, "KW_RETURN "); break;

            // Special characters
            case ',':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case ';':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case ':':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '(':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case ')':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '[':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case ']':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '{':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '}':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '=':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '+':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '-':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '*':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '/':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '%':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '<':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '>':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '&':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '|':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;
            case '~':           fprintf(stderr, "SPECIAL(%s) ", yytext); break;

            // Identifiers
            case TK_IDENTIFIER: fprintf(stderr, "TK_IDENTIFIER "); break;

            // Literals
            case LIT_INT:       fprintf(stderr, "LIT_INT "); break;
            case LIT_CHAR:      fprintf(stderr, "LIT_CHAR "); break;
            case LIT_STRING:    fprintf(stderr, "LIT_STRING "); break;

            // Errors
            case TOKEN_ERROR:   fprintf(stderr, "TOKEN_ERROR "); break;
            default:            fprintf(stderr, "Undefined "); break;
        }
    }

    // Print resulting symbols table
    fprintf(stderr, "\n");
    printSymbols();

    // Print line count
    fprintf(stderr, "Lines: %d\n", getLineNumber());

    // End point
    return 0;
}