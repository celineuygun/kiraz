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

extern std::stack<std::string> identifierStack;
std::stack<std::vector<Node::Ptr>> parameterStack;
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

%token INT UINT LONG ULONG INT128 UINT128 BOOL CHAR FLOAT 
%token DOUBLE CSTRING STRING POINTER CUSTOM 

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
    : let_stmt
    | assign_stmt
    | expr_stmt
    ;

type_decl
    : OP_COLON dtype    { $$ = $2; }
    ;

dtype
    : INT         { $$ = Node::add<ast::TypeNode>("int"); }
    | UINT        { $$ = Node::add<ast::TypeNode>("uint"); }
    | LONG        { $$ = Node::add<ast::TypeNode>("long"); }
    | ULONG       { $$ = Node::add<ast::TypeNode>("ulong"); }
    | INT128      { $$ = Node::add<ast::TypeNode>("int128"); }
    | UINT128     { $$ = Node::add<ast::TypeNode>("uint128"); }
    | BOOL        { $$ = Node::add<ast::TypeNode>("bool"); }
    | CHAR        { $$ = Node::add<ast::TypeNode>("char"); }
    | FLOAT       { $$ = Node::add<ast::TypeNode>("float"); }
    | DOUBLE      { $$ = Node::add<ast::TypeNode>("double"); }
    | CSTRING     { $$ = Node::add<ast::TypeNode>("cstring"); }
    | STRING      { $$ = Node::add<ast::TypeNode>("string"); }
    | POINTER     { $$ = Node::add<ast::TypeNode>("pointer"); }
    | CUSTOM      { $$ = Node::add<ast::TypeNode>("custom"); }
    ;

let_stmt
    : KW_LET IDENTIFIER OP_ASSIGN expr OP_SCOLON
      { 
          auto identifier = Node::add<ast::Identifier>(identifierStack.top());
            identifierStack.pop();
          $$ = Node::add<ast::LetStatement>(identifier, $4);
      }
    | KW_LET IDENTIFIER type_decl OP_ASSIGN expr OP_SCOLON
      { 
          auto identifier = Node::add<ast::Identifier>(identifierStack.top());
          identifierStack.pop();
          $$ = Node::add<ast::LetStatement>(identifier, $3, $5);
      }
    ;

assign_stmt
    : IDENTIFIER OP_ASSIGN expr OP_SCOLON
    {
        auto identifier = Node::add<ast::Identifier>(identifierStack.top());
        identifierStack.pop();
        $$ = Node::add<ast::AssignmentStatement>(
            identifier,
            $3
        );
    }
    | IDENTIFIER type_decl OP_ASSIGN expr OP_SCOLON
    {
        auto identifier = Node::add<ast::Identifier>(identifierStack.top());
        identifierStack.pop();
        $$ = Node::add<ast::AssignmentStatement>(
            identifier,
            $2,
            $4
        );
    }
    ;

expr_stmt
    : expr OP_SCOLON           { $$ = $1; }
    ;

expr
    : IDENTIFIER                
    {   $$ = Node::add<ast::Identifier>(identifierStack.top());
        identifierStack.pop(); 
    }
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
