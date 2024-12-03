// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Tabela de símbolos

#define SYMBOL_LIT_INT      1
#define SYMBOL_LIT_CHAR     2
#define SYMBOL_LIT_STRING   3
#define SYMBOL_IDENTIFIER   7
#define SYMBOL_VARIABLE     8
#define SYMBOL_FUNCTION     9
#define SYMBOL_VECTOR       10

#define DATATYPE_NONE       0
#define DATATYPE_INT        1
#define DATATYPE_CHAR       2
#define DATATYPE_STRING     3
#define DATATYPE_BOOL       4

#ifndef SYMBOLS_HEADER
#define SYMBOLS_HEADER

#include <string>

using namespace std;

struct AST;

struct Symbol
{
    Symbol(int _type, string _text)
    {
        type = _type;
        text = _text;
        dataType = DATATYPE_NONE;   // this will be determined in the semantic verifications
        functionDeclarationNode = nullptr;
    }

    int type;
    int dataType;
    AST* functionDeclarationNode;
    string text;
};

Symbol* insertSymbol(int type, string text);

void printSymbols();

string getSymbolTypeName(int type);

string getSymbolDataTypeName(int type);

int checkUndeclaredSymbols();

Symbol* makeTemp();

Symbol* makeLabel();

#endif