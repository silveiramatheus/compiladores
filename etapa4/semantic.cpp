#include "semantic.hpp"
#include "ast.hpp"
#include "symbols.hpp"
#include <map>

int semanticErrors = 0;

std::map<int, int> astDataTypeMap = 
{
    { AST_TYPEINT, DATATYPE_INT },
    { AST_TYPECHAR, DATATYPE_CHAR }
};

std::map<int, int> symbolTypeDataTypeMap = 
{
    { SYMBOL_LIT_CHAR, DATATYPE_CHAR },
    { SYMBOL_LIT_INT, DATATYPE_INT },
    { SYMBOL_LIT_STRING, DATATYPE_STRING }
};

std::map<int, int> astDefSymbolTypeMap = 
{
    { AST_VARDEF, SYMBOL_VARIABLE },
    { AST_FUNDEF, SYMBOL_FUNCTION },
    { AST_PARAM, SYMBOL_VARIABLE },
    { AST_VECTORDEF, SYMBOL_VECTOR }
};

bool isLiteral(int symbolType)
{
    return
        symbolType == SYMBOL_LIT_INT ||
        symbolType == SYMBOL_LIT_CHAR ||
        symbolType == SYMBOL_LIT_STRING;
}

int getAstListSize(AST* firstElement)
{
    int size = 0;
    if (firstElement != nullptr)
    {
        ++size;
        size += getAstListSize(firstElement->children[1]);
    }
    return size;
}

int getLiteralNumericValue(AST* node)
{
    if (node->symbol == nullptr)
        return 0;

    // Integer
    if (node->symbol->type == SYMBOL_LIT_INT)
    {
        std::string text = node->symbol->text;
        text.replace(0, 1, ""); // remove #
        return std::stoi(text);
    }

    // Char
    if (node->symbol->type == SYMBOL_LIT_CHAR)
    {
        return node->symbol->text[1];
    }

    return 0;
}

void checkAndSetDeclarations(AST* node)
{
    // check end of tree
    if (node == nullptr)
        return;

    switch (node->type)
    {
        // Declarations
    case AST_VARDEF:
    case AST_FUNDEF:
    case AST_PARAM:
    case AST_VECTORDEF:
        if (node->symbol->type != SYMBOL_IDENTIFIER)
        {
            fprintf(stderr, "Semantic error: %s already declared.\n", node->symbol->text.c_str());
            ++semanticErrors;
        }
        else
        {
            node->symbol->type = astDefSymbolTypeMap[node->type];
            node->symbol->dataType = astDataTypeMap[node->children[0]->type];

            // Store a reference to the function declaration node for verifying the parameters when called
            if (node->type == AST_FUNDEF)
            {
                node->symbol->functionDeclarationNode = node;
            }

            // Verify the vector size
            if (node->type == AST_VECTORDEF)
            {
                // Initialized on declaration
                if (node->children.size() == 3)
                {
                    // Check if the initializer list is larger than the specified size
                    if (getAstListSize(node->children[2]) > getLiteralNumericValue(node->children[1]))
                    {
                        fprintf(stderr, "Semantic error: initializer list is too big for %s.\n", node->symbol->text.c_str());
                        ++semanticErrors;
                    }
                }
            }
        }
        break;

        // Literals
    case AST_SYM:
        if (isLiteral(node->symbol->type))
        {
            node->symbol->dataType = symbolTypeDataTypeMap[node->symbol->type];
        }
        break;
    }
    
    // process child nodes
   	for (auto child : node->children)
        checkAndSetDeclarations(child);
}

void checkUndeclared()
{
    semanticErrors += checkUndeclaredSymbols();
}

bool isArithmeticOperand(int astNodeType)
{
    return
        astNodeType == AST_ADD ||
        astNodeType == AST_SUB ||
        astNodeType == AST_MUL ||
        astNodeType == AST_DIV;
}

bool isScalarDataType(int dataType)
{
    return
        dataType == DATATYPE_CHAR ||
        dataType == DATATYPE_INT;
}

bool isScalarNode(AST* node)
{
    if (node == nullptr)
        return false;

    int& nodeType = node->type;

    // Operations
    if (isArithmeticOperand(nodeType))
    {
        return true;
    }

    if (node->symbol != nullptr)
    {
        int& symbolType = node->symbol->type;
        int& dataType = node->symbol->dataType;
        
        // Function calls
        if (symbolType == SYMBOL_FUNCTION)
        {
            return nodeType == AST_CALL && isScalarDataType(dataType);
        }

        // Vector dereferencing
        else if (symbolType == SYMBOL_VECTOR)
        {
            return nodeType == AST_IND && isScalarDataType(dataType);
        }

        // Other symbol types
        else
        {
            return isScalarDataType(dataType);
        }
    }

    return false;
}

bool isBooleanNode(AST* node)
{
    return
        node->type == AST_AND ||
        node->type == AST_OR ||
        node->type == AST_LES ||
        node->type == AST_BIG ||
        node->type == AST_EQ;
}

void checkOperands(AST* node)
{
    // check end of tree
    if (node == nullptr)
        return;

    // process child nodes
   	for (auto child : node->children)
        checkOperands(child);

    switch (node->type)
    {
        // Arithmetic operations and number comparisons
    case AST_ADD:
    case AST_SUB:
    case AST_MUL:
    case AST_DIV:
    case AST_LES:
    case AST_BIG:
    case AST_EQ:
        if (!isScalarNode(node->children[0]))
        {
            fprintf(stderr, "Semantic error: invalid left operand in %c\n", node->operatorSymbol);
            ++semanticErrors;
        }
        if (!isScalarNode(node->children[1]))
        {
            fprintf(stderr, "Semantic error: invalid right operand in %c\n", node->operatorSymbol);
            ++semanticErrors;
        }
        break;

        // Boolean operations
    case AST_NOT:
        if (!isBooleanNode(node->children[0]))
        {
            fprintf(stderr, "Semantic error: cannot negate expression\n");
            ++semanticErrors;
        }
        break;
    case AST_AND:
    case AST_OR:
        if (!isBooleanNode(node->children[0]))
        {
            fprintf(stderr, "Semantic error: invalid left boolean operator\n");
            ++semanticErrors;
        }
        if (!isBooleanNode(node->children[1]))
        {
            fprintf(stderr, "Semantic error: invalid right boolean operator\n");
            ++semanticErrors;
        }
        break;
    }
}

void checkSymbolUsages(AST* node)
{
    // check end of tree
    if (node == nullptr)
        return;

    // process child nodes
   	for (auto child : node->children)
        checkSymbolUsages(child);

    if (node->symbol == nullptr)
        return;
    
    if (node->type == AST_CALL && node->symbol->type != SYMBOL_FUNCTION)
    {
        fprintf(stderr, "Semantic error: Cannot treat %s as a function\n", node->symbol->text.c_str());
        ++semanticErrors;
    }

    if (node->type == AST_IND && node->symbol->type != SYMBOL_VECTOR)
    {
        fprintf(stderr, "Semantic error: Cannot treat %s as a vector\n", node->symbol->text.c_str());
        ++semanticErrors;
    }

    if ((node->type == AST_IND || node->type == AST_ASSIGN_INDEXED ) && !isScalarNode(node->children[0]))
    {
        fprintf(stderr, "Semantic error: trying to index %s with a non-scalar expression\n", node->symbol->text.c_str());
        ++semanticErrors;
    }
}

void checkAssignations(AST* node)
{
    // check end of tree
    if (node == nullptr)
        return;

    // process child nodes
   	for (auto child : node->children)
        checkAssignations(child);

    switch (node->type)
    {
        // Assign operation
    case AST_ASSIGN:
        if (!isScalarNode(node->children[0]))
        {
            fprintf(stderr, "Semantic error: Cannot assign %s with expression\n",
                node->symbol->text.c_str()
            );
            ++semanticErrors;
        }

        // Assigning vector without []
        if (node->symbol->type == SYMBOL_VECTOR)
        {
            fprintf(stderr, "Semantic error: Cannot assign %s with %s. Are you forgetting []?\n",
                node->symbol->text.c_str(),
                node->children[0]->symbol->text.c_str()
            );
            ++semanticErrors;
        }

        // Assigning to function
        if (node->symbol->type == SYMBOL_FUNCTION)
        {
            fprintf(stderr, "Semantic error: Cannot assign to function %s\n",
                node->symbol->text.c_str()
            );
            ++semanticErrors;
        }
        break;

        // Assign to vector operation
    case AST_ASSIGN_INDEXED:
        if (!isScalarNode(node->children[1]))
        {
            fprintf(stderr, "Semantic error: Cannot assign %s with expression\n",
                node->symbol->text.c_str()
            );
            ++semanticErrors;
        }

        // Trying to index something that is not a vector on the left side
        if (node->symbol->type != SYMBOL_VECTOR)
        {
            fprintf(stderr, "Semantic error: %s is not a vector\n",
                node->symbol->text.c_str()
            );
            ++semanticErrors;
        }
        break;
    }
}

bool areAllAstListElementsScalar(AST* firstElement)
{
    if (firstElement == nullptr)
        return true;

    return isScalarNode(firstElement->children[0]) && areAllAstListElementsScalar(firstElement->children[1]);
}

void checkFunctionCalls(AST* node)
{
    // check end of tree
    if (node == nullptr)
        return;

    // process child nodes
   	for (auto child : node->children)
        checkFunctionCalls(child);

    // Check parameters in calls
    if (node->type == AST_CALL)
    {
        if (node->symbol->functionDeclarationNode != nullptr)
        {
            // Check the number of parameters
            if (getAstListSize(node->symbol->functionDeclarationNode->children[1]) != getAstListSize(node->children[0]))
            {
                fprintf(stderr, "Semantic error: incorrect number of parameters for %s\n", node->symbol->text.c_str());
                ++semanticErrors;
            }

            // Check if they are all int or char
            if (!areAllAstListElementsScalar(node->children[0]))
            {
                fprintf(stderr, "Semantic error: incorrect parameter type passed into %s\n", node->symbol->text.c_str());
                ++semanticErrors;
            }
        }
    }

    // Check return type
    if (node->type == AST_RETURN)
    {
        if (!isScalarNode(node->children[0]))
        {
            fprintf(stderr, "Semantic error: incorrect return type\n");
            ++semanticErrors;
        }
    }
}

int getSemanticErrors()
{
    return semanticErrors;
}