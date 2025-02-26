
%option noyywrap
%option yylineno

%{
// https://stackoverflow.com/questions/9611682/flexlexer-support-for-unicode/9617585#9617585
#include "main.h"
#include <kiraz/token/Boolean.h>
#include <kiraz/token/Identifier.h>
#include <kiraz/token/Keyword.h>
#include <kiraz/token/Literal.h>
#include <kiraz/token/Operator.h>

static auto &colno = Token::colno;
using namespace token;
extern std::shared_ptr<Token> curtoken;
%}


%%

"import"                    { colno += yyleng; curtoken = Token::New<KeywordImport>(); return KW_IMPORT; }
"func"                      { colno += yyleng; curtoken = Token::New<KeywordFunc>(); return KW_FUNC; }
"return"                    { colno += yyleng; curtoken = Token::New<KeywordReturn>(); return KW_RETURN; }
"if"                        { colno += yyleng; curtoken = Token::New<KeywordIf>(); return KW_IF; }
"else"                      { colno += yyleng; curtoken = Token::New<KeywordIf>(); return KW_ELSE; }
"while"                     { colno += yyleng; curtoken = Token::New<KeywordWhile>(); return KW_WHILE; }
"let"                       { colno += yyleng; curtoken = Token::New<KeywordLet>(); return KW_LET; }
"class"                     { colno += yyleng; curtoken = Token::New<KeywordClass>(); return KW_CLASS; }
"true"                      { colno += yyleng; curtoken = Token::New<Boolean>(true); return L_TRUE; }
"false"                     { colno += yyleng; curtoken = Token::New<Boolean>(false); return L_FALSE; }

"and"                       { colno += yyleng; curtoken = Token::New<OpAnd>(); return OP_AND; }
"or"                        { colno += yyleng; curtoken = Token::New<OpOr>(); return OP_OR; }
"not"                       { colno += yyleng; curtoken = Token::New<OpNot>(); return OP_NOT; }

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

[a-zA-Z_][0-9a-zA-Z_]*      { colno += yyleng; curtoken = Token::New<Identifier>(yytext); return IDENTIFIER; }

[ \t]+                      { colno += yyleng; }
[\\n]+                      { colno = 0; yylineno++; }
.                           { colno += yyleng; curtoken = Token::New<Rejected>(yytext); return YYUNDEF; }
