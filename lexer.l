
%option noyywrap
%option yylineno

%{
// https://stackoverflow.com/questions/9611682/flexlexer-support-for-unicode/9617585#9617585
#include "main.h"
#include <iostream>
#include <kiraz/token/Boolean.h>
#include <kiraz/token/Identifier.h>
#include <kiraz/token/Keyword.h>
#include <kiraz/token/Literal.h>
#include <kiraz/token/Operator.h>

static auto &colno = Token::colno;
using namespace token;
extern std::shared_ptr<Token> curtoken;

std::stack<std::string> identifierStack;
%}


%%

"int"        { colno += yyleng; return INT; }
"uint"       { colno += yyleng; return UINT; }
"long"       { colno += yyleng; return LONG; }
"ulong"      { colno += yyleng; return ULONG; }
"int128"     { colno += yyleng; return INT128; }
"uint128"    { colno += yyleng; return UINT128; }
"bool"       { colno += yyleng; return BOOL; }
"char"       { colno += yyleng; return CHAR; }
"float"      { colno += yyleng; return FLOAT; }
"double"     { colno += yyleng; return DOUBLE; }
"cstring"    { colno += yyleng; return CSTRING; }
"string"     { colno += yyleng; return STRING; }
"pointer"    { colno += yyleng; return POINTER; }
"custom"     { colno += yyleng; return CUSTOM; }


"import"                    { colno += yyleng; curtoken = Token::New<KeywordImport>(); return KW_IMPORT; }
"func"                      { colno += yyleng; curtoken = Token::New<KeywordFunc>(); return KW_FUNC; }
"if"                        { colno += yyleng; curtoken = Token::New<KeywordIf>(); return KW_IF; }
"while"                     { colno += yyleng; curtoken = Token::New<KeywordWhile>(); return KW_WHILE; }
"let"                       { colno += yyleng; curtoken = Token::New<KeywordLet>(); return KW_LET; }
"class"                     { colno += yyleng; curtoken = Token::New<KeywordClass>(); return KW_CLASS; }
"true"                      { colno += yyleng; curtoken = Token::New<Boolean>(true); return L_TRUE; }
"false"                     { colno += yyleng; curtoken = Token::New<Boolean>(false); return L_FALSE; }

[0-9]+                      { colno += yyleng; curtoken = Token::New<Integer>(10, yytext); return L_INTEGER; }

\"[^\"]+\"                  { colno += yyleng; curtoken = Token::New<String>(yytext); return L_STRING; }

"->"                        { colno += yyleng; curtoken = Token::New<OpReturns>(); return OP_RETURNS; }
"=="                        { colno += yyleng; curtoken = Token::New<OpEquals>(); return OP_EQUALS; }
"="                         { colno += yyleng; curtoken = Token::New<OpAssign>(); return OP_ASSIGN; }
">"                         { colno += yyleng; curtoken = Token::New<OpGt>(); return OP_GT; }
"<"                         { colno += yyleng; curtoken = Token::New<OpLt>(); return OP_LT; }
">="                        { colno += yyleng; curtoken = Token::New<OpGe>(); return OP_GE; }
"<="                        { colno += yyleng; curtoken = Token::New<OpLe>(); return OP_LE; }
"("                         { colno += yyleng; curtoken = Token::New<OpLparen>(); return OP_LPAREN; }
")"                         { colno += yyleng; curtoken = Token::New<OpRparen>(); return OP_RPAREN; }
"{"                         { colno += yyleng; curtoken = Token::New<OpLbrace>(); return OP_LBRACE; }
"}"                         { colno += yyleng; curtoken = Token::New<OpRbrace>(); return OP_RBRACE; }
"+"                         { colno += yyleng; curtoken = Token::New<OpPlus>(); return OP_PLUS; }
"-"                         { colno += yyleng; curtoken = Token::New<OpMinus>(); return OP_MINUS; }
"*"                         { colno += yyleng; curtoken = Token::New<OpMult>(); return OP_MULT; }
"/"                         { colno += yyleng; curtoken = Token::New<OpDivF>(); return OP_DIVF; }
","                         { colno += yyleng; curtoken = Token::New<OpComma>(); return OP_COMMA; }
":"                         { colno += yyleng; curtoken = Token::New<OpColon>(); return OP_COLON; }
";"                         { colno += yyleng; curtoken = Token::New<OpScolon>(); return OP_SCOLON; }
"."                         { colno += yyleng; curtoken = Token::New<OpDot>(); return OP_DOT; }
"!"                         { colno += yyleng; curtoken = Token::New<OpNot>(); return OP_NOT; }

[a-zA-Z_][0-9a-zA-Z_]*      { colno += yyleng; identifierStack.push(yytext); curtoken = Token::New<Identifier>(yytext); return IDENTIFIER; }

[ \n\t]+                    { colno += yyleng; }
.                           { colno += yyleng; curtoken = Token::New<Rejected>(yytext); return YYUNDEF; }
