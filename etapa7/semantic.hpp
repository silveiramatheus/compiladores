// UFRGS - Compiladores - Professor Marcelo Johann - 2024/2
// Matheus Silveira e Tiago Binz
// Tabela de símbolos

#pragma once

struct AST;

extern int semanticErrors;

void checkAndSetDeclarations(AST* node);
void checkUndeclared();
void checkOperands(AST* node);
void checkSymbolUsages(AST* node);
void checkAssignations(AST* node);
void checkFunctionCalls(AST* node);
int getSemanticErrors();
