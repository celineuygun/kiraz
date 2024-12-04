#ifndef KIRAZ_TOKEN_OPERATOR_H
#define KIRAZ_TOKEN_OPERATOR_H

#include <kiraz/Token.h>
#include <string>

namespace token {
class Operator : public Token {
public:
    Operator(int type) : Token(type) {}
};

class OpReturns : public Operator {
public:
    OpReturns() : Operator(OP_RETURNS) {}

    std::string as_string() const override {return "OP_RETURNS";}
};

class OpEquals : public Operator {
public:
    OpEquals() : Operator(OP_EQUALS) {}

    std::string as_string() const override {return "OP_EQUALS";}
};

class OpAssign : public Operator {
public:
    OpAssign() : Operator(OP_ASSIGN) {}

    std::string as_string() const override {return "OP_ASSIGN";}
};

class OpGt : public Operator {
public:
    OpGt() : Operator(OP_GT) {}

    std::string as_string() const override {return "OP_GT";}
};

class OpLt : public Operator {
public:
    OpLt() : Operator(OP_LT) {}

    std::string as_string() const override {return "OP_LT";}
};

class OpGe : public Operator {
public:
    OpGe() : Operator(OP_GE) {}

    std::string as_string() const override {return "OP_GE";}
};

class OpLe : public Operator {
public:
    OpLe() : Operator(OP_LE) {}

    std::string as_string() const override {return "OP_LE";}
};

class OpLparen : public Operator {
public:
    OpLparen() : Operator(OP_LPAREN) {}

    std::string as_string() const override {return "OP_LPAREN";}
};

class OpRparen : public Operator {
public:
    OpRparen() : Operator(OP_RPAREN) {}

    std::string as_string() const override {return "OP_RPAREN";}
};

class OpLbrace : public Operator {
public:
    OpLbrace() : Operator(OP_LBRACE) {}

    std::string as_string() const override {return "OP_LBRACE";}
};

class OpRbrace : public Operator {
public:
    OpRbrace() : Operator(OP_RBRACE) {}

    std::string as_string() const override {return "OP_RBRACE";}
};

class OpPlus : public Operator {
public:
    OpPlus() : Operator(OP_PLUS) {}

    std::string as_string() const override {return "OP_PLUS";}
};

class OpMinus : public Operator {
public:
    OpMinus() : Operator(OP_MINUS) {}

    std::string as_string() const override {return "OP_MINUS";}
};

class OpMult : public Operator {
public:
    OpMult() : Operator(OP_MULT) {}

    std::string as_string() const override {return "OP_MULT";}
};

class OpDivF : public Operator {
public:
    OpDivF() : Operator(OP_DIVF) {}

    std::string as_string() const override {return "OP_DIVF";}
};

class OpComma : public Operator {
public:
    OpComma() : Operator(OP_COMMA) {}

    std::string as_string() const override {return "OP_COMMA";}
};

class OpNewline : public Operator {
public:
    OpNewline() : Operator(OP_NEWLINE) {}

    std::string as_string() const override {return "OP_NEWLINE";}
};

class OpColon : public Operator {
public:
    OpColon() : Operator(OP_COLON) {}

    std::string as_string() const override {return "OP_COLON";}
};

class OpScolon : public Operator {
public:
    OpScolon() : Operator(OP_SCOLON) {}

    std::string as_string() const override {return "OP_SCOLON";}
};

class OpDot : public Operator {
public:
    OpDot() : Operator(OP_DOT) {}

    std::string as_string() const override {return "OP_DOT";}
};

class OpNot : public Operator {
public:
    OpNot() : Operator(OP_NOT) {}

    std::string as_string() const override {return "OP_NOT";}
};

class OpAnd: public Operator {
public:
    OpAnd() : Operator(OP_AND) {}

    std::string as_string() const override {return "OP_AND";}
};

class OpOr: public Operator {
public:
    OpOr() : Operator(OP_OR) {}

    std::string as_string() const override {return "OP_OR";}
};


}

#endif
