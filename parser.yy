%{
#include <stack>
#include "main.h"
#include "lexer.hpp"

#include <kiraz/ast/Boolean.h>
#include <kiraz/ast/BuiltinTypes.h>
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

std::shared_ptr<ast::ParameterList> parameters = nullptr;
std::vector<std::shared_ptr<ast::Parameter>> param_vector;

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
%token    OP_AND
%token    OP_OR


%token    L_INTEGER
%token    L_STRING
%token    L_BOOLEAN
%token    L_TRUE
%token    L_FALSE

%token    KW_IF
%token    KW_ELSE
%token    KW_RETURN
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
        if($1) { $$ = Node::add<ast::Module>($1); }
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
    | return_stmt
    | let_stmt
    | assign_stmt
    | if_stmt
    | expr_stmt
    | while_stmt
    | OP_LPAREN stmt OP_RPAREN { $$ = $2; }
    ;

import_stmt
    : KW_IMPORT identifier OP_SCOLON
    {
        $$ = Node::add<ast::ImportStatement>($2);
    }
    ;

type_decl
    : OP_COLON identifier { $$ = $2; }
    ;

class_stmt
    : KW_CLASS identifier OP_LBRACE stmts OP_RBRACE OP_SCOLON
    {
        $$ = Node::add<ast::ClassStatement>($2, $4);
    }
    | KW_CLASS identifier OP_COLON identifier OP_LBRACE stmts OP_RBRACE OP_SCOLON
    {
        $$ = Node::add<ast::ClassStatement>($2, $6, $4);
    }
    ;

func_stmt
    : KW_FUNC identifier OP_LPAREN param_list OP_RPAREN type_decl OP_LBRACE stmts OP_RBRACE OP_SCOLON
    {
        auto func = Node::add<ast::FunctionStatement>($2, $4, $6, $8);
        if(func) {
            if(parameters) {
                parameters->set_stmt(func);
                for (auto& param : param_vector) {
                    param->set_stmt(func);
                }
                param_vector.clear();
            }
        }
        $$ = func;
    }
    ;
    
param_list
    : /* empty */                   { $$ = nullptr; }
    | param
    { 
        auto params = Node::add<ast::ParameterList>($1, nullptr);
        parameters = params;
        $$ = params;
    }
    | param_list OP_COMMA param 
    { 
        auto params = Node::add<ast::ParameterList>($1, $3);
        parameters = params;
        $$ = params;
    }
    ;

param
    : identifier type_decl
    {
        auto param = Node::add<ast::Parameter>($1, $2);
        param_vector.push_back(param);
        $$ = param;
    }
    | atom
    {
        auto param = Node::add<ast::Parameter>($1, nullptr);
        param_vector.push_back(param);
        $$ = param;
    }
    ;

return_stmt
    : KW_RETURN expr OP_SCOLON
    {
        $$ = Node::add<ast::ReturnStatement>($2);
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

call_expr
    : dot_expr OP_LPAREN param_list OP_RPAREN
    {
        $$ = Node::add<ast::CallStatement>($1, $3);
    }
    | identifier OP_LPAREN param_list OP_RPAREN
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

if_stmt
    : if OP_SCOLON          { $$ = $1; }
    ;

if
    : KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmts OP_RBRACE
    {
        $$ = Node::add<ast::IfStatement>($3, $6, nullptr);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmts OP_RBRACE KW_ELSE OP_LBRACE stmts OP_RBRACE
    {
        $$ = Node::add<ast::IfStatement>($3, $6, $10);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmts OP_RBRACE KW_ELSE if
    {
        $$ = Node::add<ast::IfStatement>($3, $6, $9); 
    }
    ;

while_stmt
    : KW_WHILE OP_LPAREN expr OP_RPAREN OP_LBRACE stmts OP_RBRACE OP_SCOLON
    {
        $$ = Node::add<ast::WhileStatement>($3, $6);
    }
    ;

expr_stmt
    : expr OP_SCOLON          { $$ = $1; }
    ;

expr
    : additive
    | logical
    | call_expr
    | dot_expr
    | expr OP_EQUALS additive { $$ = Node::add<ast::OpEquals>($1, $3); }
    | expr OP_GT additive     { $$ = Node::add<ast::OpGT>($1, $3); }
    | expr OP_GE additive     { $$ = Node::add<ast::OpGE>($1, $3); }
    | expr OP_LT additive     { $$ = Node::add<ast::OpLT>($1, $3); }
    | expr OP_LE additive     { $$ = Node::add<ast::OpLE>($1, $3); }
    ;

logical
    : OP_AND OP_LPAREN expr OP_COMMA expr OP_RPAREN { $$ = ast::BuiltinManager::AndFunction($3, $5); }
    | OP_OR OP_LPAREN expr OP_COMMA expr OP_RPAREN  { $$ = ast::BuiltinManager::OrFunction($3, $5); }
    | OP_NOT OP_LPAREN expr OP_RPAREN               { $$ = ast::BuiltinManager::NotFunction($3); }
    ;
    ;

additive
    : additive OP_PLUS multiplicative  { $$ = Node::add<ast::OpAdd>($1, $3); }
    | additive OP_MINUS multiplicative 
    { 
        $$ = Node::add<ast::OpSub>($1, $3); 
    }
    | multiplicative
    ;

multiplicative
    : multiplicative OP_MULT negative  { $$ = Node::add<ast::OpMult>($1, $3); }
    | multiplicative OP_DIVF negative  { $$ = Node::add<ast::OpDivF>($1, $3); }
    | negative
    ;

negative
    : OP_MINUS atom           { $$ = Node::add<ast::SignedNode>(OP_MINUS, $2); }
    | OP_MINUS OP_MINUS atom  
    {
        yyerror("Invalid use of multiple negation operators"); 
        Node::reset_root();
        YYERROR;
    }
    | atom
    ;

atom
    : identifier               { $$ = $1; }
    | L_INTEGER                { $$ = Node::add<ast::Integer>(curtoken); }
    | L_STRING                 { $$ = Node::add<ast::String>(curtoken); }
    | L_TRUE                   { $$ = Node::add<ast::Boolean>(L_TRUE); }
    | L_FALSE                  { $$ = Node::add<ast::Boolean>(L_FALSE); }
    | OP_LPAREN expr OP_RPAREN { $$ = $2; }
    ;

identifier
    : IDENTIFIER               { $$ = Node::add<ast::Identifier>(curtoken); }
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
