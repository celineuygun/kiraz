%{
#include "main.h"
#include "lexer.hpp"

#include <kiraz/ast/Operator.h>
#include <kiraz/ast/Literal.h>

#include <kiraz/token/Literal.h>

int yyerror(const char *msg);
extern std::shared_ptr<Token> curtoken;
extern int yylineno;
%}

%token    OP_LPAREN
%token    OP_RPAREN

%token    OP_PLUS
%token    OP_MINUS
%token    OP_DIVF
%token    OP_MULT

%token    L_INTEGER

%left     OP_PLUS OP_MINUS
%left     OP_MULT OP_DIVF

%token    REJECTED
%%

stmt
    : OP_LPAREN stmt OP_RPAREN { $$ = $2; }
    | addsub
    ;

addsub
    : muldiv
    | stmt OP_PLUS stmt { $$ = Node::add<ast::OpAdd>($1, $3);}
    | stmt OP_MINUS stmt { $$ = Node::add<ast::OpSub>($1, $3);}
    ;

muldiv
    : posneg
    | stmt OP_MULT stmt { $$ = Node::add<ast::OpMult>($1, $3);}
    | stmt OP_DIVF stmt { $$ = Node::add<ast::OpDivF>($1, $3);}
    ;

posneg
    : L_INTEGER     { $$ = Node::add<ast::Integer>(curtoken);}
    | OP_PLUS stmt  { $$ = Node::add<ast::SignedNode>(OP_PLUS, $2);}
    | OP_MINUS stmt  { $$ = Node::add<ast::SignedNode>(OP_MINUS, $2);}
    ;

%%

int yyerror(const char *s) {
    if (curtoken) {
        fmt::print("** Parser Error at {}:{} at token: {}\n",
            yylineno, Token::colno, curtoken->as_string());
    }
    else {
        fmt::print("** Parser Error at {}:{}, null token\n",
            yylineno, Token::colno);
    }
    Token::colno = 0;
    Node::reset_root();
    return 1;
}
