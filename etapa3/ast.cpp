#include "ast.hpp"
#include "symbols.hpp"
#include <string>








std::string astTypeName[] =
{
	"AST_VOID",	// Not important
	"AST_SYM",	// A symbol, which is an expression
	"AST_ADD",
	"AST_SUB",
	"AST_MUL",
	"AST_DIV",
	"AST_NOT",
	"AST_AND",
	"AST_OR",
	"AST_LES",
	"AST_BIG",
	"AST_EQ",
	"AST_IND",
	"AST_CALL",
	"AST_LEXP",
	"AST_LCMD",
	"AST_ASSIGN",
	"AST_IF",
	"AST_WHILE",
	"AST_READ",
	"AST_PRINT",
	"AST_RETURN",
	"AST_FUNDEF",
	"AST_VARDEF",
	"AST_LDEF",
	"AST_TYPEINT",
	"AST_TYPECHAR"
};

AST* AST::createNode(int type, std::vector<AST*> children, Symbol* symbol)
{
	return new AST(type, children, symbol);
}

void AST::print(AST* node, int level)
{
	if (!node)	return;

	for (int l = 0; l < level; ++l)
		fprintf(stderr, "   ");

	fprintf(
		stderr, "AST(%s,%s)\n",
		astTypeName[node->type].c_str(),
		node->symbol? node->symbol->text.c_str() : ""
	);

	for (auto child : node->children)
		print(child, level + 1);
}
