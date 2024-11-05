// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Tabela de símbolos

#ifndef SYMBOLS_HEADER
#define SYMBOLS_HEADER

#include <string>

using namespace std;

struct Symbol
{
    Symbol(int _type, string _text)
    {
        type = _type;
        text = _text;
    }

    int type;
    string text;
};

Symbol* insertSymbol(int type, string text);

void printSymbols();

string getSymbolTypeName(int type);

#endif