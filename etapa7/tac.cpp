#include "tac.hpp"
#include "symbols.hpp"
#include "ast.hpp"
#include <sstream>
#include <algorithm>

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

bool TAC::shouldPrint()
{
    // No need to print TAC_SYMBOL.
    if (type == TAC_SYMBOL)
        return false;

    // No need to print variable definitions
    if (type == TAC_VARDEF || type == TAC_VECDEF)
        return false;

    return true;
}

bool TAC::print()
{
    if (!shouldPrint())
        return false;

    fprintf(stderr, "TAC");
    fprintf(stderr, "(%s", TAC_Type_Name[type].c_str());
    fprintf(stderr, ",%s", res? res->text.c_str() : "_");
    fprintf(stderr, ",%s", op1? op1->text.c_str() : "_");
    fprintf(stderr, ",%s", op2? op2->text.c_str() : "_");
    fprintf(stderr, ")");

    if (type == TAC_ARG)
        fprintf(stderr, " --- parameter %d for function \"%s\"", index, parentFunction->text.c_str());

    fprintf(stderr, "\n");

    return true;
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

std::vector<TAC::BasicBlock> TAC::findBasicBlocks(std::vector<TAC>& TAC_list)
{
    std::vector<TAC::BasicBlock> output;
    int blockStart = -1;
    auto foundBasicBlock = [&](int _start, int _end)
    {
        if (_end > _start)
        {
            output.push_back(BasicBlock(_start, _end));
        }
        blockStart = -1;
    };
    auto isBranchTarget = [](TAC& tac)
    {
        return
            tac.type == TAC_LABEL ||
            tac.type == TAC_BEGINFUN;
    };
    auto isBranch = [](TAC& tac)
    {
        return
            tac.type == TAC_IFZ ||
            tac.type == TAC_JUMP ||
            tac.type == TAC_ENDFUN ||
            tac.type == TAC_RET ||
            tac.type == TAC_CALL;
    };

    for (size_t i = 0; i < TAC_list.size(); ++i)
    {
        TAC& tac = TAC_list[i];

        // Looking for a basic block start?
        if (blockStart == -1)
        {
            if (!isBranch(tac))
            {
                blockStart = i;
                continue;
            }
        }

        // Looking for where it ends?
        else
        {
            if (isBranch(tac))
            {
                foundBasicBlock(blockStart, i);
                continue;
            }
            else if (isBranchTarget(tac))
            {
                foundBasicBlock(blockStart, i-1);
                blockStart = i;
                continue;
            }
            else if (i == TAC_list.size() - 1)
            {
                foundBasicBlock(blockStart, i);
                continue;
            }
        }
    }

    return output;
}

void TAC::printBasicBlocks(std::vector<BasicBlock>& basicBlocks, std::vector<TAC>& TAC_list)
{
    for (size_t i = 0; i < basicBlocks.size(); ++i)
    {
        BasicBlock& basicBlock = basicBlocks[i];
        fprintf(stderr, "Basic Block %ld:\n", i+1);

        for (int j = basicBlock.start; j <= basicBlock.end; ++j)
        {
            if (TAC_list[j].shouldPrint())
            {
                fprintf(stderr, "%5d: ", j);
                TAC_list[j].print();
            }
        }
    }
}

void TAC::optimize_constantFolding(std::vector<BasicBlock>& basicBlocks, std::vector<TAC>& TAC_list, int& outRemovedInstructions)
{
    // We will store all the useless instructions that only use constants
    std::vector<int> constantOps;

    // For each Basic Block...
    for (size_t i = 0; i < basicBlocks.size(); ++i)
    {
        BasicBlock& basicBlock = basicBlocks[i];

        // And the symbols we will use as substitutes
        std::map<Symbol*, Symbol*> symbolSubstitutions;

        // We keep track if there's any constant folding left to be done
        bool constantOpsChanged = true;
        while (constantOpsChanged)
        {
            constantOpsChanged = false;

            // For each instruction in the basic block...
            for (int j = basicBlock.start; j <= basicBlock.end; ++j)
            {
                TAC* tac = &TAC_list[j];

                // If this instruction has already been identified as useless, skip it
                if (std::find(constantOps.begin(), constantOps.end(), j) != constantOps.end())
                {
                    continue;
                }

                // If the result goes into a temporary...
                if (tac->res != nullptr && tac->res->isTemp)
                {
                    int res = 0;

                    // And both operands are numeric literals...
                    if (tac->op1 != nullptr && tac->op1->isNumericLiteral() && tac->op2 != nullptr && tac->op2->isNumericLiteral())
                    {
                        int op1 = tac->op1->toInt();
                        int op2 = tac->op2->toInt();

                        switch (tac->type)
                        {
                        case TAC_ADD:
                            res = op1 + op2;
                            break;
                        case TAC_SUB:
                            res = op1 - op2;
                            break;
                        case TAC_MUL:
                            res = op1 * op2;
                            break;
                        case TAC_DIV:
                            res = op1 / op2;
                            break;
                        case TAC_AND:
                            res = op1 & op2;
                            break;
                        case TAC_OR:
                            res = op1 | op2;
                            break;
                        case TAC_BIG:
                            res = op1 > op2;
                            break;
                        case TAC_LES:
                            res = op1 < op2;
                            break;
                        case TAC_EQ:
                            res = op1 == op2;
                            break;
                        default:
                            continue;
                            break;
                        }
                    }

                    // Or it's a NOT operation with a literal...
                    else if (tac->type == TAC_NOT && tac->op1->isNumericLiteral())
                    {
                        res = !tac->op1->toInt();
                    }

                    else
                        continue;

                    // We create a new symbol for this result
                    std::ostringstream stringStream;
                    stringStream << "#" << res;
                    Symbol* newSymbol = insertSymbol(SYMBOL_LIT_INT, stringStream.str());

                    // And mark it as the substitute for this temp
                    symbolSubstitutions[tac->res] = newSymbol;
                    constantOpsChanged = true;

                    // This operation is now useless
                    constantOps.push_back(j);
                    ++outRemovedInstructions;
                }
            }

            // Apply the substitutions on each instruction in the basic block...
            for (int j = basicBlock.start; j <= basicBlock.end; ++j)
            {
                TAC* tac = &TAC_list[j];

                // Replace res
                if (Symbol* replacement = symbolSubstitutions[tac->res])
                    tac->res = replacement;

                // Replace op1
                if (Symbol* replacement = symbolSubstitutions[tac->op1])
                    tac->op1 = replacement;

                // Replace op2
                if (Symbol* replacement = symbolSubstitutions[tac->op2])
                    tac->op2 = replacement;
            }
        }
    }

    // Now, we will delete all the useless instructions from the code
    for (size_t i = 0; i < constantOps.size(); ++i)
    {
        int constantOp = constantOps[i];
        TAC_list.erase(TAC_list.begin() + constantOp);

        // Adjust indices
        for (size_t j = i + 1; j < constantOps.size(); ++j)
            if (constantOps[j] > constantOp)
                --constantOps[j];
    }

    // Get all the symbols in the table
    std::vector<Symbol*> allSymbols;
    for (std::pair<std::string, Symbol*> pair : getSymbolsTable())
        allSymbols.push_back(pair.second);

    // Check which symbols are being used
    std::map<Symbol*, bool> isSymbolUsed;
    for (TAC& tac : TAC_list)
    {
        if (tac.res)
            isSymbolUsed[tac.res] = true;
        if (tac.op1)
            isSymbolUsed[tac.op1] = true;
        if (tac.op2)
            isSymbolUsed[tac.op2] = true;
    }

    // Now delete all the unused symbols from the table
    for (Symbol* symbol : allSymbols)
    {
        if (!isSymbolUsed[symbol])
        {
            removeSymbol(symbol->text);
        }
    }
}