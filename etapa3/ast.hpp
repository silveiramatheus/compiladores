// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Árvore Sintática Abstrata

#ifndef AST_HEADER
#define AST_HEADER

#include <vector>

class Symbol;

enum AST_Type
{
	AST_VOID,	// Not important
	AST_SYM,	// A symbol, which is an expression
	AST_ADD,
	AST_SUB,
	AST_MUL,
	AST_DIV,
	AST_NOT,
	AST_AND,
	AST_OR,
	AST_LES,
	AST_BIG,
	AST_EQ,
	AST_IND,
	AST_CALL,
	AST_LEXP,
	AST_LCMD,
	AST_ASSIGN,
	AST_IF,
	AST_WHILE,
	AST_READ,
	AST_PRINT,
	AST_RETURN,
	AST_FUNDEF,
	AST_VARDEF,
	AST_LDEF,
	AST_TYPEINT,
	AST_TYPECHAR
};

struct AST
{
	AST(int _type, std::vector<AST*> _children, Symbol* _symbol = nullptr)
	{
		type = _type;
		children = _children;
		symbol = _symbol;
	}
	int type;
	std::vector<AST*> children;
	Symbol* symbol;

	static AST* createNode(int type, std::vector<AST*> children, Symbol* symbol = nullptr);
	static void print(AST* node, int level);
};

#endif