// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Tabela de símbolos

#include "symbols.hpp"
#include "ast.hpp"
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
    fprintf(stderr, "%-35s\t%-35s\t%-35s\n", "==== SYMBOL ====", "==== TYPE ====", "==== DATA TYPE ====");
    // Iterate through the map and print the elements
    map<string, Symbol*>::iterator iterator = symbolsTable.begin();
    while (iterator != symbolsTable.end()) {
        fprintf(
            stderr,
            "%-35s\t%-35s\t%-35s\n",
            iterator->first.c_str(),
            getSymbolTypeName(iterator->second->type).c_str(),
            getSymbolDataTypeName(iterator->second->dataType).c_str()
        );
        ++iterator;
    }
}

string getSymbolTypeName(int type)
{
    switch (type)
    {
        case SYMBOL_LIT_INT:            return "Integer Literal";
        case SYMBOL_LIT_CHAR:           return "Char Literal";
        case SYMBOL_LIT_STRING:         return "String Literal";
        case SYMBOL_IDENTIFIER:         return "Identifier";
        case SYMBOL_VARIABLE:           return "Variable";
        case SYMBOL_FUNCTION:           return "Function";
        case SYMBOL_VECTOR:             return "Vector";
        default:                        return "UNKNOWN";
    }
}

string getSymbolDataTypeName(int type)
{
    switch (type)
    {
        case DATATYPE_CHAR:             return "Char";
        case DATATYPE_INT:              return "Integer";
        case DATATYPE_STRING:           return "String";
        default:                        return "UNKNOWN";
    }
}

int checkUndeclaredSymbols()
{
    int undeclaredNum = 0;

    // Iterate through the map
    map<string, Symbol*>::iterator iterator = symbolsTable.begin();
    while (iterator != symbolsTable.end())
    {
        if (iterator->second->type == SYMBOL_IDENTIFIER)
        {
            fprintf(stderr, "Semantic error: identifier %s undeclared \n", iterator->first.c_str());
            ++undeclaredNum;
        }

        ++iterator;
    }

    return undeclaredNum;
}

Symbol* makeTemp()
{
    static int serial = -1;
    ++serial;
    return insertSymbol(SYMBOL_VARIABLE, "TEMP_SYMBOL[" + std::to_string(serial) + "]");
}

Symbol* makeLabel()
{
    static int serial = -1;
    ++serial;
    return insertSymbol(SYMBOL_VARIABLE, "LABEL[" + std::to_string(serial) + "]");
}