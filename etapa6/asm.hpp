#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include "tac.hpp"
#include "ast.hpp"
#include "symbols.hpp"

void generateAsm(std::vector<TAC> tacList, AST* node);
void generateDataSection(FILE* fout, AST* node);
std::string removeHash(const std::string& str);
void printTempSymbols(FILE* fout, std::vector<TAC> tacList);
void generateTextSectionString();