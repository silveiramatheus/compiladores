#include "tac.hpp"
#include "symbols.hpp"
#include "ast.hpp"

std::string TAC_Type_Name[] =
{
    "TAC_SYMBOL",
    "TAC_ADD",
    "TAC_SUB",
    "TAC_MUL",
    "TAC_DIV",
    "TAC_COPY",
    "TAC_LABEL",
    "TAC_IFZ",
    "TAC_AND",
    "TAC_OR",
    "TAC_BIG",
    "TAC_LES",
    "TAC_EQ",
    "TAC_JUMP",
    "TAC_NOT",
    "TAC_READVEC",
    "TAC_CALL",
    "TAC_BEGINFUN",
    "TAC_ENDFUN",
    "TAC_ARG",
    "TAC_RET",
    "TAC_PRINT",
    "TAC_READ",
    "TAC_VECDEF",
    "TAC_VECWRITE",
    "TAC_VARDEF"
};

std::map<int, TAC_Type> ast_to_tac = 
{
    { AST_ADD, TAC_ADD },
    { AST_SUB, TAC_SUB },
    { AST_MUL, TAC_MUL },
    { AST_DIV, TAC_DIV },
    { AST_AND, TAC_AND },
    {  AST_OR, TAC_OR  },
    { AST_BIG, TAC_BIG },
    { AST_LES, TAC_LES },
    {  AST_EQ, TAC_EQ  },
};

std::map<AST*, int> listIndex;

void TAC::print()
{
    // No need to print TAC_SYMBOL.
    if (type == TAC_SYMBOL)
        return;

    // No need to print variable definitions
    if (type == TAC_VARDEF || type == TAC_VECDEF)
        return;

    fprintf(stderr, "TAC");
    fprintf(stderr, "(%s", TAC_Type_Name[type].c_str());
    fprintf(stderr, ",%s", res? res->text.c_str() : "_");
    fprintf(stderr, ",%s", op1? op1->text.c_str() : "_");
    fprintf(stderr, ",%s", op2? op2->text.c_str() : "_");
    fprintf(stderr, ")");

    if (type == TAC_ARG)
        fprintf(stderr, " --- parameter %d for function \"%s\"", index, parentFunction->text.c_str());

    fprintf(stderr, "\n");
}

void TAC::printList(std::vector<TAC> tacList)
{
    for (size_t i = 0; i < tacList.size(); ++i)
        tacList[i].print();
}

std::vector<TAC> TAC::generateCode(AST* node, Symbol* functionCallContext)
{
    std::vector<TAC> output;

    if (!node)
        return output;

    // Check if this is a function call node
    if (node->type == AST_CALL)
        functionCallContext = node->symbol;

    // Generate code for all children
    std::map<AST*, std::vector<TAC>> childrenCode;
    for (AST* child : node->children)
    {
        childrenCode.emplace(child, generateCode(child, functionCallContext));
    }
    #define CHILD_CODE(n) childrenCode[node->children[n]]

    // Generate code for this node
    switch (node->type)
    {
        // Symbols
    case AST_SYM:
        output = { TAC(TAC_SYMBOL, node->symbol) };
        break;

        // Operations with two operands
    case AST_ADD:
    case AST_SUB:
    case AST_MUL:
    case AST_DIV:
    case AST_AND:
    case AST_OR:
    case AST_BIG:
    case AST_LES:
    case AST_EQ:
        output = tacJoin(node->children, childrenCode);
        output.push_back(TAC(ast_to_tac[node->type], makeTemp(), CHILD_CODE(0).back().res, CHILD_CODE(1).back().res));
        break;
    
        // Negation
    case AST_NOT:
        output = tacJoin(node->children, childrenCode);
        output.push_back(TAC(TAC_NOT, makeTemp(), CHILD_CODE(0).back().res));
        break;

        // Variable definitions
    case AST_VARDEF:
        output.push_back(TAC(TAC_VARDEF, node->symbol).withValueNode(node->children[1]));
        break;

        // Vector definition
    case AST_VECTORDEF:
        output.push_back(TAC(TAC_VECDEF, node->symbol).withValueNode(node->children[1]));
        break;

        // Function definition
    case AST_FUNDEF:
        output = CHILD_CODE(2); // function body
        output.insert(output.begin(), TAC(TAC_BEGINFUN, node->symbol)); // begin function
        output.push_back(TAC(TAC_ENDFUN, node->symbol));    // end function
        break;

        // Assign
    case AST_ASSIGN:
        output = CHILD_CODE(0);
        output.push_back(TAC(TAC_COPY, node->symbol, CHILD_CODE(0).back().res));
        break;

        // Assign to vector
    case AST_ASSIGN_INDEXED:
        output = tacJoin(node->children, childrenCode);
        output.push_back(TAC(TAC_VECWRITE, node->symbol, CHILD_CODE(0).back().res, CHILD_CODE(1).back().res));
        break;

        // If-then else
    case AST_IF:
        if (node->children.size() == 2)
            output = makeIfThen(CHILD_CODE(0), CHILD_CODE(1));
        else
            output = makeIfThenElse(CHILD_CODE(0), CHILD_CODE(1), CHILD_CODE(2));
        break;

        // While
    case AST_WHILE:
        output = makeWhile(CHILD_CODE(0), CHILD_CODE(1));
        break;

        // Indexed expression
    case AST_IND:
        output = tacJoin(node->children, childrenCode);
        output.push_back(TAC(TAC_READVEC, makeTemp(), node->symbol, CHILD_CODE(0).back().res));
        break;

        // Function call
    case AST_CALL:
        output = tacJoin(node->children, childrenCode);
        output.push_back(TAC(TAC_CALL, makeTemp(), node->symbol));
        break;

        // List of expressions (used for arguments in function calls)
    case AST_LEXP:
        output = makeFunctionArguments(CHILD_CODE(0), functionCallContext, CHILD_CODE(1));
        break;

        // Return from function
    case AST_RETURN:
        output = tacJoin(node->children, childrenCode);
        output.push_back(TAC(TAC_RET, CHILD_CODE(0).back().res));
        break;

        // Print
    case AST_LPRINT:
        output = CHILD_CODE(0);
        output.push_back(TAC(TAC_PRINT, CHILD_CODE(0).back().res));
        for (TAC& tac : CHILD_CODE(1))
            output.push_back(tac);
        break;

        // Read
    case AST_READ:
        output = tacJoin(node->children, childrenCode);
        output.push_back(TAC(TAC_READ, node->symbol));
        break;

    default:
        // Return the union of code for all sub-trees
        output = tacJoin(node->children, childrenCode);
        break;
    }

    return output;
}

std::vector<TAC> TAC::tacJoin(std::vector<AST*> children, std::map<AST*, std::vector<TAC>> codes)
{
    std::vector<TAC> output;

    for (AST* child : children)
    {
        for (TAC& tac : codes[child])
        {
            output.push_back(tac);
        }
    }

    return output;
}

std::vector<TAC> TAC::makeIfThen(std::vector<TAC> conditionCode, std::vector<TAC> thenCode)
{
    // Create the jump label
    Symbol* label = makeLabel();

    std::vector<TAC> output;
    for (TAC& tac : conditionCode)
        output.push_back(tac);  // 1. Run condition code
    output.push_back(TAC(TAC_IFZ, label, conditionCode.back().res)); // 2. Jump if false
    for (TAC& tac : thenCode)
        output.push_back(tac);  // 3. Run 'then' code
    output.push_back(TAC(TAC_LABEL, label));    // 4. Label after 'then' code

    return output;
}

std::vector<TAC> TAC::makeIfThenElse(std::vector<TAC> conditionCode, std::vector<TAC> thenCode, std::vector<TAC> elseCode)
{
    // Create the jump labels
    Symbol* elseLabel = makeLabel();
    Symbol* afterElseLabel = makeLabel();

    std::vector<TAC> output;
    for (TAC& tac : conditionCode)
        output.push_back(tac);  // 1. Run condition code
    output.push_back(TAC(TAC_IFZ, elseLabel, conditionCode.back().res)); // 2. Jump to else if false
    for (TAC& tac : thenCode)
        output.push_back(tac);  // 3. Run 'then' code
    output.push_back(TAC(TAC_JUMP, afterElseLabel));    // 4. Jump to the code after 'else'
    output.push_back(TAC(TAC_LABEL, elseLabel));    // 5. Label at the start of 'else' code
    for (TAC& tac : elseCode)
        output.push_back(tac);  // 6. Run 'else' code
    output.push_back(TAC(TAC_LABEL, afterElseLabel));    // 7. Label after the 'else' code

    return output;
}

std::vector<TAC> TAC::makeWhile(std::vector<TAC> conditionCode, std::vector<TAC> codeBlock)
{
    // Create the jump labels
    Symbol* whileStartLabel = makeLabel();
    Symbol* whileEndLabel = makeLabel();

    std::vector<TAC> output;
    output.push_back(TAC(TAC_LABEL, whileStartLabel));    // 1. Label before testing the condition
    for (TAC& tac : conditionCode)
        output.push_back(tac);  // 2. Run condition code
    output.push_back(TAC(TAC_IFZ, whileEndLabel, conditionCode.back().res)); // 3. Jump to after while if false
    for (TAC& tac : codeBlock)
        output.push_back(tac);  // 4. Run code block
    output.push_back(TAC(TAC_JUMP, whileStartLabel));    // 5. Jump to the code before while
    output.push_back(TAC(TAC_LABEL, whileEndLabel));  // 6. Label after the while instruction

    return output;
}

std::vector<TAC> TAC::makeFunctionArguments(std::vector<TAC> argCode, Symbol* function, std::vector<TAC> tailCode)
{
    std::vector<TAC> output = argCode;

    int argIndex = 0;
    output.push_back(TAC(TAC_ARG, argCode.back().res).withParentFunction(function, argIndex));
    for (TAC& tac : tailCode)
    {
        if (tac.type == TAC_ARG)
        {
            ++argIndex;
            output.push_back(tac.withParentFunction(function, argIndex));
        }
        else
        {
            output.push_back(tac);
        }
    }

    return output;
}