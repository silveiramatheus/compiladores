#include "ast.hpp"
#include "symbols.hpp"
#include <string>
#include <fstream>

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
	"AST_TYPECHAR",
	"AST_PARAM",
	"AST_LPARAM",
	"AST_BLOCK",
	"AST_ASSIGN_INDEXED",
	"AST_VECTORDEF",
	"AST_LLITERAL",
	"AST_LPRINT"
};

AST* AST::createNode(int type, std::vector<AST*> children, Symbol* symbol, char operatorSymbol)
{
	return new AST(type, children, symbol, operatorSymbol);
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

void AST::decompileToFile(AST* node, std::string fileName)
{
	ofstream output;
	output.open(fileName);
	decompile(node, output);
	output.close();
}

void AST::decompile(AST* node, std::ostream& output)
{
	if (!node) return;

	switch (node->type)
	{
		// List of definitions
	case AST_LDEF:
		for (size_t i = 0; i < node->children.size(); i++)
			decompile(node->children[i], output);
		break;

		// Variable definition
	case AST_VARDEF:
		decompile(node->children[0], output);		// Variable type
		output << " ";
		output << node->symbol->text.c_str();		// Variable name
		output << " = ";
		decompile(node->children[1], output);		// Variable value
		output << ";\n";
		break;

		// Vector definition
	case AST_VECTORDEF:
		decompile(node->children[0], output);		// Vector type
		output << " ";
		output << node->symbol->text.c_str();		// Vector name
		output << "[";
		decompile(node->children[1], output);		// Vector size
		output << "]";

		// Initialized
		if (node->children.size() >= 3)
		{
			output << " = ";
			decompile(node->children[2], output);	// Vector elements
		}
		output << ";\n";
		break;

		// List of literals
	case AST_LLITERAL:
		decompile(node->children[0], output);
		if (node->children[1])
		{
			output << " ";
			decompile(node->children[1], output);
		}
		break;

		// Symbol
	case AST_SYM:
		output << node->symbol->text.c_str();
		break;

		// Indexed expression
	case AST_IND:
		output << node->symbol->text.c_str();	// Vector name
		output << "[";
		decompile(node->children[0], output);	// Index
		output << "]";
		break;

		// int (type token)
	case AST_TYPEINT:
		output << "int";
		break;

		// char (type token)
	case AST_TYPECHAR:
		output << "char";
		break;

		// Function definition
	case AST_FUNDEF:
		decompile(node->children[0], output);	// Function type
		output << " ";
		output << node->symbol->text.c_str();	// Function name
		output << "(";
		decompile(node->children[1], output);	// Function parameters
		output << ")\n";
		decompile(node->children[2], output);	// Function body
		break;

		// Function parameter list
	case AST_LPARAM:
		if (node->children.size() > 0)
		{
			decompile(node->children[0], output);		// This parameter
			if (node->children[1])
			{
				output << ", ";
				decompile(node->children[1], output);	// Next parameter
			}
		}
		break;

		// Function parameter
	case AST_PARAM:
		decompile(node->children[0], output);		// Parameter type
		output << " ";
		output << node->symbol->text.c_str();		// Parameter name
		break;

		// Command block
	case AST_BLOCK:
		output << "{\n";
		decompile(node->children[0], output);
		output << "}\n";
		break;

		// Command list
	case AST_LCMD:
		decompile(node->children[0], output);
		decompile(node->children[1], output);
		break;

		// Command: assign
	case AST_ASSIGN:
		output << node->symbol->text.c_str();	// Variable name
		output << " = ";
		decompile(node->children[0], output);	// New value
		output << ";\n";
		break;

		// Command: assign indexed
	case AST_ASSIGN_INDEXED:
		output << node->symbol->text.c_str();	// Variable name
		output << "[";
		decompile(node->children[0], output);	// Index expression
		output << "] = ";
		decompile(node->children[1], output);	// New value
		output << ";\n";
		break;

		// If-statement
	case AST_IF:
		output << "if(";
		decompile(node->children[0], output);	// If condition
		output << ") then\n";
		decompile(node->children[1], output);	// then block

		// Contains else keyword
		if (node->children.size() >= 3)
		{
			output << "else\n";
			decompile(node->children[2], output);	// else block
		}
		break;

		// While-statement
	case AST_WHILE:
		output << "while(";
		decompile(node->children[0], output);
		output << ")\n";
		decompile(node->children[1], output);
		break;

		// Read
	case AST_READ:
		output << "read ";
		output << node->symbol->text.c_str();
		output << ";\n";
		break;

		// Print
	case AST_PRINT:
		output << "print ";
		decompile(node->children[0], output);
		output << ";\n";
		break;

		// Print list
	case AST_LPRINT:
		decompile(node->children[0], output);
		if (node->children[1])
		{
			output << " ";
			decompile(node->children[1], output);
		}
		break;

		// Return command
	case AST_RETURN:
		output << "return ";
		decompile(node->children[0], output);
		output << ";\n";
		break;

		// Operations (with two operands)
	case AST_ADD:
	case AST_MUL:
	case AST_DIV:
	case AST_SUB:
	case AST_AND:
	case AST_OR:
	case AST_BIG:
	case AST_LES:
	case AST_EQ:
		output << "(";
		decompile(node->children[0], output);	// op1
		output << ")";
		output << " " << node->operatorSymbol << " ";	// operation
		output << "(";
		decompile(node->children[1], output);	// op2
		output << ")";
		break;

		// Negation
		case AST_NOT:
			output << node->operatorSymbol;
			output << "(";
			decompile(node->children[0], output);
			output << ")";
			break;

		// Call function
	case AST_CALL:
		output << node->symbol->text.c_str();
		output << "(";
		decompile(node->children[0], output);
		output << ")";
		break;

		// Expressions list
	case AST_LEXP:
		decompile(node->children[0], output);
		if (node->children[1])
		{
			output << ", ";
			decompile(node->children[1], output);
		}
		break;

	default:
		break;
	}
}
