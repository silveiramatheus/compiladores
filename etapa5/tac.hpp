#pragma once

#include <vector>
#include <map>
#include <string>

struct AST;
struct Symbol;

enum TAC_Type
{
    TAC_SYMBOL,
    TAC_ADD,
    TAC_SUB,
    TAC_MUL,
    TAC_DIV,
    TAC_COPY,
    TAC_LABEL,
    TAC_IFZ,
    TAC_AND,
    TAC_OR,
    TAC_BIG,
    TAC_LES,
    TAC_EQ,
    TAC_JUMP,
    TAC_NOT,
    TAC_READVEC,
    TAC_CALL,
    TAC_BEGINFUN,
    TAC_ENDFUN,
    TAC_ARG,
    TAC_RET,
    TAC_PRINT,
    TAC_READ,
    TAC_VECDEF,
    TAC_VECWRITE,
    TAC_VARDEF
};

struct TAC
{
    TAC_Type type;

    Symbol* res;
    Symbol* op1;
    Symbol* op2;

    // Used for function arguments
    int index = -1;

    // Used for variable definitions
    AST* valueNode = nullptr;

    TAC
    (
        TAC_Type _type,
        Symbol* _res = nullptr,
        Symbol* _op1 = nullptr,
        Symbol* _op2 = nullptr
    )
    {
        type = _type,
        res = _res;
        op1 = _op1;
        op2 = _op2;
    }

    void print();
    static void printList(std::vector<TAC> tacList);

    static std::vector<TAC> generateCode(AST* node);
    static std::vector<TAC> tacJoin(std::vector<AST*> children, std::map<AST*, std::vector<TAC>> codes);
    static std::vector<TAC> makeIfThen(std::vector<TAC> conditionCode, std::vector<TAC> thenCode);
    static std::vector<TAC> makeIfThenElse(std::vector<TAC> conditionCode, std::vector<TAC> thenCode, std::vector<TAC> elseCode);
    static std::vector<TAC> makeWhile(std::vector<TAC> conditionCode, std::vector<TAC> codeBlock);

    TAC withIndex(int _index) { index = _index; return *this; }
    TAC withValueNode(AST* node) { valueNode = node; return *this; }
};