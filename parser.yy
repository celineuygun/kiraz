%{
#include "main.h"
#include "lexer.hpp"
#include <iostream>

#include <kiraz/ast/Identifier.h>
#include <kiraz/ast/Literal.h>
#include <kiraz/ast/Operator.h>
#include <kiraz/ast/Statement.h>

#include <kiraz/token/Boolean.h>
#include <kiraz/token/Identifier.h>
#include <kiraz/token/Keyword.h>
#include <kiraz/token/Literal.h>
#include <kiraz/token/Operator.h>

int yyerror(const char *msg);
extern std::shared_ptr<Token> curtoken;
extern int yylineno;
extern std::string identifier_txt;
%}

%token    IDENTIFIER

%token    OP_RETURNS
%token    OP_EQUALS
%token    OP_ASSIGN
%token    OP_GT
%token    OP_LT
%token    OP_GE
%token    OP_LE
%token    OP_LPAREN
%token    OP_RPAREN
%token    OP_LBRACE
%token    OP_RBRACE
%token    OP_COLON
%token    OP_PLUS
%token    OP_MINUS
%token    OP_MULT
%token    OP_DIVF
%token    OP_COMMA
%token    OP_NEWLINE
%token    OP_SCOLON
%token    OP_DOT
%token    OP_NOT

%token    L_INTEGER
%token    L_STRING
%token    L_BOOLEAN
%token    L_TRUE
%token    L_FALSE

%token    KW_IF
%token    KW_FUNC
%token    KW_WHILE
%token    KW_CLASS
%token    KW_IMPORT
%token    KW_LET

%left     OP_PLUS OP_MINUS
%left     OP_MULT OP_DIVF
%right    OP_ASSIGN

%token    REJECTED
%%

prog
    : stmts
    ;

stmts
    : stmt stmts
    | /* empty */
    ;

stmt
    : decl_stmt
    | assign_stmt
    | expr_stmt
    ;

decl_stmt
    : IDENTIFIER type_decl OP_SCOLON
    ;

type_decl
    : OP_COLON dtype
    ;

dtype
    : L_INTEGER    // TODO implement type checking
    ;

assign_stmt
    : IDENTIFIER OP_ASSIGN expr OP_SCOLON
    {
        auto identifier = Node::add<ast::Identifier>(identifier_txt); 
        $$ = Node::add<ast::AssignmentStatement>(
            identifier,
            $3
        );
    }
    ;

expr_stmt
    : expr OP_SCOLON           { $$ = $1; }
    ;

expr
    : IDENTIFIER                { $$ = Node::add<ast::Identifier>(identifier_txt); }
    | L_INTEGER                 { $$ = Node::add<ast::Integer>(curtoken); }
    | expr OP_PLUS expr         { $$ = Node::add<ast::OpAdd>($1, $3); }
    | expr OP_MINUS expr        { $$ = Node::add<ast::OpSub>($1, $3); }
    | expr OP_MULT expr         { $$ = Node::add<ast::OpMult>($1, $3);}
    | expr OP_DIVF expr         { $$ = Node::add<ast::OpDivF>($1, $3);}
    | OP_LPAREN expr OP_RPAREN  { $$ = $2; }
    | OP_PLUS expr              { $$ = Node::add<ast::SignedNode>(OP_PLUS, $2);}
    | OP_MINUS expr             { $$ = Node::add<ast::SignedNode>(OP_MINUS, $2);}
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
