%{
    #include "tokens.h"
    #include "symbols.hpp"

    string accumulatedString;
    int stringError = 0;
    int lineCount = 1;
    int running = 1;
%}
%option noyywrap
%option noinput
%option nounput
%x STRING
%x COMMENT
%x MULTI_LINE_COMMENT

%%
[\n]                            { lineCount++; }
"char"                          { return KW_CHAR; }
"int"                           { return KW_INT; }
"if"                            { return KW_IF; }
"then"                          { return KW_THEN; }
"else"                          { return KW_ELSE; }
"while"                         { return KW_WHILE; }
"read"                          { return KW_READ; }
"print"                         { return KW_PRINT; }
"return"                        { return KW_RETURN; }
[,;:\(\)\[\]\{\}=+\-*/%\<\>&|~] { return yytext[0]; }
[a-zA-Z0-9]+[_][a-zA-Z0-9]*     { return TOKEN_ERROR; }
[a-zA-Z0-9]+                    { insertSymbol(TK_IDENTIFIER, yytext); return TK_IDENTIFIER; }
[#][0-9]+                       { insertSymbol(LIT_INT, yytext); return LIT_INT; }
['][a-zA-Z][']                  { insertSymbol(LIT_CHAR, yytext); return LIT_CHAR; }
["]                             { accumulatedString = "\""; stringError = 0; BEGIN(STRING); }
<STRING>[\n]                    { lineCount++; stringError = 1; }
<STRING><<EOF>>                 { BEGIN(INITIAL); return TOKEN_ERROR; }
<STRING>[\\]["]                 { accumulatedString.append(yytext); }
<STRING>[\][n]                  { accumulatedString.append(yytext); }
<STRING>["]                     { accumulatedString.append(yytext); insertSymbol(LIT_STRING, accumulatedString); if (stringError) return TOKEN_ERROR; else {BEGIN(INITIAL); return LIT_STRING; } }
<STRING>.                       { accumulatedString.append(yytext); }
"//"                            { BEGIN(COMMENT); }
<COMMENT>[\n]                   { lineCount++; BEGIN(INITIAL); }
<COMMENT>.                      {}
"/**"                           { BEGIN(MULTI_LINE_COMMENT); }
<MULTI_LINE_COMMENT><<EOF>>     { BEGIN(INITIAL); return TOKEN_ERROR; }
<MULTI_LINE_COMMENT>[\n]        { lineCount++; }
<MULTI_LINE_COMMENT>"**/"       { BEGIN(INITIAL); }
<MULTI_LINE_COMMENT>.           {}
.                               {}
<<EOF>>                         { running = 0; return EOF; }
%%

#include "main.cpp"

// Required by specification
void initMe()
{

}

// Required by specification
int isRunning(void)
{
    return running;
}

// Required by specification
int getLineNumber()
{
    return lineCount;
}