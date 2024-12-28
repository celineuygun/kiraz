#ifndef KIRAZ_AST_LITERAL_H
#define KIRAZ_AST_LITERAL_H

#include <kiraz/Node.h>
#include "BuiltinTypes.h"

namespace ast {
class Integer : public Node {
public:
    Integer(Token::Ptr);

    std::string as_string() const override {return fmt::format("Int({})", m_value);}
    // std::string as_string() const override { return fmt::format("Int({}, {})", m_base, m_value); }

    Node::Ptr gen_wat(WasmContext &ctx) override {
        ctx.body() << "  i64.const " << m_value << "\n";
        return nullptr;
    }
private:
    int64_t m_value;
    int64_t m_base;
};

class SignedNode : public Node {
public:
    SignedNode(int op, Node::Cptr operand)
        : m_operator(op), m_operand(operand) {}

    std::string as_string() const override {
        //return fmt::format("Signed({}, {})", m_operator, m_operand->as_string());
        return fmt::format("Signed({}, {})", operator_to_string(m_operator), m_operand->as_string());
    }

private:
    int m_operator;
    Node::Cptr m_operand;

    std::string operator_to_string(int op) const {
        switch (op) {
            case OP_PLUS: return "OP_PLUS";
            case OP_MINUS: return "OP_MINUS";
            default: return fmt::format("UnknownOperator({})", op);
        }
    }
};

class String : public Node {
public:
    String(Token::Ptr t);

    std::string as_string() const override { 
        return fmt::format("Str({})", m_value); 
    }

private:
    std::string m_value;
};
}

#endif
