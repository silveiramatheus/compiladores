// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Árvore Sintática Abstrata

#ifndef AST_HEADER
#define AST_HEADER

#include <vector>
#include <string>

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
	AST_TYPECHAR,
	AST_PARAM,
	AST_LPARAM,
	AST_BLOCK,
	AST_ASSIGN_INDEXED,
	AST_VECTORDEF,
	AST_LLITERAL,
	AST_LPRINT
};

struct AST
{
	AST(int _type, std::vector<AST*> _children, Symbol* _symbol = nullptr, char _operatorSymbol = 0)
	{
		type = _type;
		children = _children;
		symbol = _symbol;
		operatorSymbol = _operatorSymbol;
	}
	int type;
	std::vector<AST*> children;
	Symbol* symbol;
	char operatorSymbol;	// Simplifies decompilation

	static AST* createNode(int type, std::vector<AST*> children, Symbol* symbol = nullptr, char operatorSymbol = 0);
	static void print(AST* node, int level);

	static void decompileToFile(AST* node, std::string fileName);
	static void decompile(AST* node, std::ostream& output);
};

#endif