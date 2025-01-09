#include "asm.hpp"

int stringLabelCounter = 0;
std::map<std::string, std::string> stringLiterals;

// Função para gerar rótulos para strings literais
std::string getStringLabel() {
    std::string label;
    if (stringLabelCounter == 0)
    {
        label = "L_.str";
    }
    else {
        label = "L_.str" + std::to_string(stringLabelCounter);
    }
    stringLabelCounter++;
    return label;
}


void printTempSymbols(FILE* fout, std::vector<TAC> tacList)
{
    for (const auto& tac : tacList) {
        if (tac.res && tac.res->text.find("TEMP_SYMBOL") == 0) {
            fprintf(fout, "\t.globl _%s\n", tac.res->text.c_str());
            fprintf(fout, "_%s:\n\t.long\t0\n", tac.res->text.c_str());
        }
    }
}

std::string removeHash(const std::string& str) {
    // Verifica se a string começa com '#'
    if (!str.empty() && str[0] == '#') {
        return str.substr(1);
    }
    return str;
}

void generateDataSection(FILE* fout, AST* ast)
{
    if (!ast || !fout) {
        return;
    }

    // Verifica se o nó atual tem filhos e processa de acordo
    if (ast->children.size() > 0) {
        switch (ast->type)
        {
            case AST_VARDEF:
                // Verifica se o nó representa uma variável de tipo inteiro ou caractere
                if (ast->children.size() > 1) {
                    if (ast->symbol && ast->children[1]->symbol) {
                        if (ast->children[0]->type == AST_TYPEINT && ast->children[1] && ast->children[0]) {
                            std::string inteiroSemHash = removeHash(ast->children[1]->symbol->text.c_str());
                            fprintf(fout, "\t.globl _%s\n", ast->symbol->text.c_str());
                            fprintf(fout, "_%s:\n\t.long\t%s\n\n", ast->symbol->text.c_str(), inteiroSemHash.c_str());
                        }
                        else if (ast->children[0]->type == AST_TYPECHAR && ast->children[1] && ast->children[0]) {
                            char charValue = ast->children[1]->symbol->text[1];
                            int asciiValue = static_cast<int>(charValue); 

                            fprintf(fout, "\t.globl _%s\n", ast->symbol->text.c_str());
                            fprintf(fout, "_%s:\n\t.byte\t%d\n\n", ast->symbol->text.c_str(), asciiValue);  
                        }
                    }
                }
                break;

            case AST_VECTORDEF:
                // Verifica se o AST_VECTORDEF tem 2 ou 3 filhos
                if (ast->children.size() == 2) {
                    // Se tiver 2 filhos, gera o vetor com zeros (sem inicialização)
                    fprintf(fout, "\t.globl _%s\n", ast->symbol->text.c_str());
                    fprintf(fout, "_%s:\n", ast->symbol->text.c_str());
                    // Supondo que o tamanho do vetor está no primeiro filho
                    int size = std::stoi((removeHash(ast->children[1]->symbol->text.c_str()))); // Tamanho do vetor
                    for (int i = 0; i < size; ++i) {
                        fprintf(fout, "\t.long\t0\n"); // Inicializa com zero
                    }
                } 
                else if (ast->children.size() == 3) {
                    // Se tiver 3 filhos, gera o vetor com valores da lista de inicialização
                    fprintf(fout, "\t.globl _%s\n", ast->symbol->text.c_str());
                    fprintf(fout, "_%s:\n", ast->symbol->text.c_str());
                    AST* listNode = ast->children[2]; // O terceiro filho é a lista de inicialização

                    // Percorre a lista de literais (AST_LLITERAL)
                    while (listNode != nullptr) {
                        if (listNode->children.size() > 0) {
                            std::string value = removeHash(listNode->children[0]->symbol->text); // Valor do literal
                            fprintf(fout, "\t.long\t%s\n", value.c_str());
                            listNode = listNode->children[1]; // Avança para o próximo elemento da lista
                        }
                        else {
                            break;
                        }
                    }
                }
                break;

            default:
                break;
        }
    }

    // Recursão para os filhos após gerar os dados
    for (auto child : ast->children) {
        if (child) { // Verifica se o filho não é nulo
            generateDataSection(fout, child);
        }
    }
}

void generateAsm(std::vector<TAC> tacList, AST* node)
{
    //TAC *tac;
    FILE *fout;

    fout = fopen("out.s", "w");
    if (!fout)
    {
        std::cerr << "Erro ao abrir o arquivo para escrita.\n";
        return;
    }

    std::map<std::string, std::string> stringLiterals;
    std::map<std::string, std::string> formatLiterals;

    // Gerar seção de código
    fprintf(fout, "\t.section	__TEXT,__text,regular,pure_instructions\n");
    for (auto& tac : tacList)
    {
        std::string formatLabel = "";
        std::string label = "";
        switch (tac.type)
        {
        case TAC_ADD:
            if (tac.res && tac.op1 && tac.op2) 
            {
                fprintf(fout, "\n## TAC_ADD\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\taddl\t_%s(%%rip), %%eax\n", tac.op2->text.c_str()); 
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str()); 
            }
            break;

        case TAC_SUB:
            if (tac.res && tac.op1 && tac.op2) 
            {
                fprintf(fout, "\n## TAC_SUB\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\tsubl\t_%s(%%rip), %%eax\n", tac.op2->text.c_str()); 
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str()); 
            }
            break;

        case TAC_MUL:
            if (tac.res && tac.op1 && tac.op2) 
            {
                fprintf(fout, "\n## TAC_MUL\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\timull\t_%s(%%rip), %%eax\n", tac.op2->text.c_str());
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str());
            }
            break;

        case TAC_DIV:
            if (tac.res && tac.op1 && tac.op2) 
            {
                fprintf(fout, "\n## TAC_DIV\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\tcltd\n");
                fprintf(fout, "\tidivl\t_%s(%%rip)\n", tac.op2->text.c_str());
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str());
            }
            break;

        case TAC_COPY:
            if (tac.res && tac.op1) 
            {
                fprintf(fout, "\n## TAC_COPY\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str());
            }
            break;

        case TAC_LABEL:
            fprintf(fout, "\n## TAC_LABEL\n");
            fprintf(fout, "._%s:\n", tac.res->text.c_str());
            break;

        case TAC_EQ:
            if (tac.res && tac.op1 && tac.op2) 
            {
                fprintf(fout, "\n## TAC_EQ\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\tcmpl\t_%s(%%rip), %%eax\n", tac.op2->text.c_str());
                fprintf(fout, "\tsete\t%%al\n");
                fprintf(fout, "\tandb\t$1, %%al\n");
                fprintf(fout, "\tmovzbl\t%%al, %%eax\n");
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str());
            }
            break;

        case TAC_BIG:
            if (tac.res && tac.op1 && tac.op2) 
            {
                fprintf(fout, "\n## TAC_BIG\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\tcmpl\t_%s(%%rip), %%eax\n", tac.op2->text.c_str());
                fprintf(fout, "\tsetg\t%%al\n");
                fprintf(fout, "\tandb\t$1, %%al\n");
                fprintf(fout, "\tmovzbl\t%%al, %%eax\n");
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str());
            }
            break;

        case TAC_LES:
            if (tac.res && tac.op1 && tac.op2) 
            {
                fprintf(fout, "## TAC_LES\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\tcmpl\t_%s(%%rip), %%eax\n", tac.op2->text.c_str());
                fprintf(fout, "\tsetl\t%%al\n");
                fprintf(fout, "\tandb\t$1, %%al\n");
                fprintf(fout, "\tmovzbl\t%%al, %%eax\n");
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str());
            }
            break;

        case TAC_IFZ:
            if (tac.res && tac.op1) 
            {
                fprintf(fout, "\n## TAC_IFZ\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax", tac.op1->text.c_str());
                fprintf(fout, "\ttestl\t%%eax, %%eax\n");
                fprintf(fout, "\tjz\t._%s\n", tac.res->text.c_str());
            }
            break;

        case TAC_JUMP:
            fprintf(fout, "\n## TAC_JUMP\n");
            fprintf(fout, "\tjmp\t._%s\n", tac.res->text.c_str());
            break;

        case TAC_RET:
            fprintf(fout, "\n## TAC_RET\n");
            fprintf(fout, "\tpopq\t%%rbp\n");
            fprintf(fout, "\tretq\n");    
            break;

        case TAC_NOT:
            if (tac.res && tac.op1) 
            {
                fprintf(fout, "\n## TAC_NOT\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac.op1->text.c_str());
                fprintf(fout, "\txorl\t$1, %%eax\n");
                fprintf(fout, "\tmovzbl\t%%al, %%eax\n");
                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac.res->text.c_str());
            }
            break;
     
        case TAC_BEGINFUN:
            fprintf(fout, "\n## TAC_BEGINFUN\n");
            fprintf(fout, "\t.global\t_%s\n", tac.res->text.c_str());
            fprintf(fout, " _%s:\n", tac.res->text.c_str());
            fprintf(fout, "\tpushq\t%%rbp\n");
            fprintf(fout, "\tmovq\t%%rsp, %%rbp\n");
            break;
        
        case TAC_ENDFUN:
            fprintf(fout, "\n## TAC_ENDFUN\n");
            fprintf(fout, "\txorl\t%%eax, %%eax\n");
            fprintf(fout, "\tpopq\t%%rbp\n");
            fprintf(fout, "\tretq\n");
            break;

        case TAC_PRINT:
            if (tac.res->dataType == SYMBOL_LIT_INT)
            {
                formatLabel = getStringLabel();
                formatLiterals[formatLabel] = "\"%d\\0\"";

                fprintf(fout, "\n## TAC_PRINT INT\n");
                fprintf(fout, "\tmovl\t_%s(%%rip), %%esi\n", tac.res->text.c_str());
                fprintf(fout, "\tleaq\t%s(%%rip), %%rdi\n", formatLabel.c_str());
                fprintf(fout, "\tmovb\t$0, %%al\n");
                fprintf(fout, "\tcallq\t_printf\n");
            }
            else if (tac.res->dataType == SYMBOL_LIT_CHAR)
            {
                formatLabel = getStringLabel();
                formatLiterals[formatLabel] = "\"%c\\0\"";

                fprintf(fout, "\n## TAC_PRINT CHAR\n");
                fprintf(fout, "\tmovzbl\t_%s(%%rip), %%esi\n", tac.res->text.c_str());
                fprintf(fout, "\tleaq\t%s(%%rip), %%rdi\n", formatLabel.c_str());
                fprintf(fout, "\tmovb\t$0, %%al\n");
                fprintf(fout, "\tcallq\t_printf\n");
            }
            else
            {
                label = getStringLabel();
                stringLiterals[label] = tac.res->text;

                fprintf(fout, "\n## TAC_PRINT STRING\n");
                fprintf(fout, "\tleaq\t%s(%%rip), %%rdi\n", label.c_str());
                fprintf(fout, "\tmovb\t$0, %%al\n");
                fprintf(fout, "\tcallq\t_printf\n");
            }
            break;

        case TAC_READ:
            label = getStringLabel();
            stringLiterals[label] = tac.res->dataType == SYMBOL_LIT_INT ? "\"%d\"" : "\"%c\"";
            fprintf(fout, "\n## TAC_READ\n");
            fprintf(fout, "\tleaq\t%s(%%rip), %%rdi\n", label.c_str());
            fprintf(fout, "\tleaq\t_%s(%%rip), %%rsi\n", tac.res->text.c_str());
            fprintf(fout, "\tmovb\t$0, %%al\n");
            fprintf(fout, "\tcallq\t_scanf\n");
            fprintf(fout, "%s:\n", label.c_str());    
            break;
            
        default:
            break;
        }
    }

    // Gerar seção de data
    fprintf(fout, "\n\t.section\t__DATA,__data\n");
    generateDataSection(fout, node);
    printTempSymbols(fout, tacList);

    // Adicionar rótulos de formato na seção de dados
    for (const auto& [label, value] : formatLiterals)
    {
        fprintf(fout, "%s:\n", label.c_str());
        fprintf(fout, "\t.asciz %s\n", value.c_str());
    }

    // Adicionar literais de string na seção de dados
    for (const auto& [label, value] : stringLiterals)
    {
        fprintf(fout, "%s:\n", label.c_str());
        fprintf(fout, "\t.asciz %s\n", value.c_str()); 
    }


    fclose(fout);
}


