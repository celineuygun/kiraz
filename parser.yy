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

%token T I B A1 INT INT64 UINT LONG ULONG INT128 UINT128 BOOL CHAR FLOAT 
%token DOUBLE CSTRING STRING POINTER CUSTOM 

%token    KW_IF
%token    KW_FUNC
%token    KW_WHILE
%token    KW_CLASS
%token    KW_IMPORT
%token    KW_LET

%right    OP_ASSIGN
%left     OP_PLUS OP_MINUS
%left     OP_MULT OP_DIVF
%left     OP_EQUALS
%left     OP_GT OP_GE OP_LT OP_LE

%token    REJECTED

%start prog

%%

prog
    : stmts       
    {
        if($1) 
        {
            $$ = Node::add<ast::Module>($1);
        }
    }
    ;

stmts
    : stmt stmts  { $$ = Node::add<ast::StatementList>($1, $2); }
    | /* empty */ { $$ = nullptr; }
    | expr        { $$ = $1; }
    ;

stmt
    : import_stmt
    | class_stmt
    | func_stmt
    | let_stmt
    | assign_stmt
    | call_stmt
    | expr_stmt
    | OP_LPAREN stmt OP_RPAREN { $$ = $2; }
    ;

import_stmt
    : KW_IMPORT identifier OP_SCOLON
    {
        $$ = Node::add<ast::ImportStatement>($2);
    }
    ;

type_decl
    : OP_COLON dtype    { $$ = $2; }
    ;

dtype
    : T           { $$ = Node::add<ast::TypeNode>("T"); }
    | I           { $$ = Node::add<ast::TypeNode>("I"); }
    | B           { $$ = Node::add<ast::TypeNode>("B"); }
    | A1          { $$ = Node::add<ast::TypeNode>("A1"); }
    | INT         { $$ = Node::add<ast::TypeNode>("int"); }
    | INT64       { $$ = Node::add<ast::TypeNode>("Int64"); }
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

class_stmt
    : KW_CLASS identifier OP_LBRACE class_body OP_RBRACE OP_SCOLON
    {
        $$ = Node::add<ast::ClassStatement>($2, $4);
    }
    ;

class_body
    : class_member class_body { $$ = Node::add<ast::ClassBody>($1, $2); }
    | /* empty */             { $$ = nullptr; }
    ;

class_member
    : let_stmt
    | func_stmt
    ;

func_stmt
    : KW_FUNC identifier OP_LPAREN param_list OP_RPAREN type_decl OP_LBRACE stmts OP_RBRACE OP_SCOLON
    {
        $$ = Node::add<ast::FunctionStatement>($2, $4, $6, $8);
    }
    ;
    
param_list
    : /* empty */                   { $$ = nullptr; }
    | param                         { $$ = Node::add<ast::ParameterList>($1, nullptr); }
    | param_list OP_COMMA param     { $$ = Node::add<ast::ParameterList>($1, $3); }
    ;

param
    : identifier type_decl
      {
          $$ = Node::add<ast::Parameter>($1, $2); 
      }
    | identifier
      {
          $$ = Node::add<ast::Parameter>($1, nullptr); 
      }
    ;

let_stmt
    : KW_LET identifier OP_ASSIGN expr OP_SCOLON
      {
          $$ = Node::add<ast::LetStatement>($2, $4);
      }
    | KW_LET identifier type_decl OP_ASSIGN expr OP_SCOLON
      {
          $$ = Node::add<ast::LetStatement>($2, $3, $5);
      }
    | KW_LET identifier type_decl OP_SCOLON
      {
          $$ = Node::add<ast::LetStatement>($2, $3, nullptr);
      }
    ;

assign_stmt
    : identifier OP_ASSIGN expr OP_SCOLON
    {
        $$ = Node::add<ast::AssignmentStatement>(
            $1,
            $3
        );
    }
    | identifier type_decl OP_ASSIGN expr OP_SCOLON
    {
        $$ = Node::add<ast::AssignmentStatement>(
            $1,
            $2,
            $4
        );
    }
    ;

call_stmt
    : dot_expr OP_LPAREN param_list OP_RPAREN OP_SCOLON
    {
        $$ = Node::add<ast::CallStatement>($1, $3);
    }
    ;

dot_expr
    : identifier OP_DOT identifier 
    { 
        $$ = Node::add<ast::OpDot>($1, $3); 
    }
    | dot_expr OP_DOT identifier 
    { 
        $$ = Node::add<ast::OpDot>($1, $3); 
    }
    ;

expr_stmt
    : expr OP_SCOLON          { $$ = $1; }
    ;

expr
    : additive
    | expr OP_EQUALS additive { $$ = Node::add<ast::OpEquals>($1, $3); }
    | expr OP_GT additive     { $$ = Node::add<ast::OpGT>($1, $3); }
    | expr OP_GE additive     { $$ = Node::add<ast::OpGE>($1, $3); }
    | expr OP_LT additive     { $$ = Node::add<ast::OpLT>($1, $3); }
    | expr OP_LE additive     { $$ = Node::add<ast::OpLE>($1, $3); }
    ;

additive
    : additive OP_PLUS multiplicative  { $$ = Node::add<ast::OpAdd>($1, $3); }
    | additive OP_MINUS multiplicative { $$ = Node::add<ast::OpSub>($1, $3); }
    | multiplicative
    ;

multiplicative
    : multiplicative OP_MULT negative  { $$ = Node::add<ast::OpMult>($1, $3); }
    | multiplicative OP_DIVF negative  { $$ = Node::add<ast::OpDivF>($1, $3); }
    | negative
    ;

negative
    : OP_MINUS negative { $$ = Node::add<ast::SignedNode>(OP_MINUS, $2); }
    | atom
    ;

atom
    : identifier               { $$ = $1; }
    | L_INTEGER                { $$ = Node::add<ast::Integer>(curtoken); }
    | L_STRING                 { $$ = Node::add<ast::String>(curtoken); }
    | OP_LPAREN expr OP_RPAREN { $$ = $2; }
    ;

identifier
    : IDENTIFIER
    {
        auto identifier = Node::add<ast::Identifier>(identifierStack.top());
        identifierStack.pop();
        $$ = identifier;
    }
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
