// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Tabela de símbolos

#include "symbols.hpp"
#include "parser.tab.hpp"
#include <map>
#include <iostream>

map<string, Symbol*> symbolsTable; 

Symbol* insertSymbol(int type, string text)
{
    // If the symbol already exists in the table
    //Credits to https://www.geeksforgeeks.org/map-associative-containers-the-c-standard-template-library-stl/
    std::map<string, Symbol*>::iterator iterator = symbolsTable.find(text);
    if (iterator != symbolsTable.end() && iterator->second->type == type)
    {
        return iterator->second;
    }

    Symbol* s = new Symbol(type, text);
    symbolsTable[text] = s;
    return s;
}

// Credits to https://www.geeksforgeeks.org/map-associative-containers-the-c-standard-template-library-stl/
void printSymbols()
{
    // Table header
    fprintf(stderr, "==================== Symbols Table ====================\n");
    fprintf(stderr, "%-35s\t%s\n", "==== SYMBOL ====", "==== TYPE ====");
    // Iterate through the map and print the elements
    map<string, Symbol*>::iterator iterator = symbolsTable.begin();
    while (iterator != symbolsTable.end()) {
        fprintf(stderr, "%-35s\t%s\n", iterator->first.c_str(), getSymbolTypeName(iterator->second->type).c_str());
        ++iterator;
    }
}

string getSymbolTypeName(int type)
{
    switch (type)
    {
        case TK_IDENTIFIER: return "Identifier";
        case LIT_INT:       return "Integer Literal";
        case LIT_CHAR:      return "Char Literal";
        case LIT_STRING:    return "String Literal";
        default: return "UNKNOWN";
    }
}