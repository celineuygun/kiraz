#ifndef KIRAZ_AST_OPERATOR_H
#define KIRAZ_AST_OPERATOR_H

#include <cassert>

#include <kiraz/Node.h>

namespace ast {
class BuiltinManager;

class Operator : public Node {
protected:
    explicit Operator(int op) {}
};

class OpUnary : public Operator {
protected:
    Node::Ptr m_operand;

public:
    explicit OpUnary(int op, Node::Ptr &operand)
        : Operator(op), m_operand(operand) {
            assert(operand);
        }

    auto get_operand() const { return m_operand; }

    std::string as_string() const override {
        return fmt::format("{}({})", operator_name(), m_operand->as_string());
    }

protected:
    virtual std::string operator_name() const = 0;
};

class OpBinary : public Operator {
protected:
    explicit OpBinary(int op, const Node::Ptr &left, const Node::Ptr &right)
        : Operator(op), m_left(left), m_right(right) {
            assert(left);
            assert(right);
        }

    virtual std::string operator_name() const = 0;

public:
    auto get_left() const {return m_left;}
    auto get_right() const {return m_right;}

    std::string as_string() const override {
        return fmt::format("{}(l={}, r={})", operator_name(), m_left->as_string(), m_right->as_string());
    }

private:
    Node::Ptr m_left, m_right;
};

class OpNot : public OpUnary {
public:
    explicit OpNot(Node::Ptr &operand);

protected:
    std::string operator_name() const override { return "Not"; }
};

class OpAdd : public OpBinary {
public:
    OpAdd(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_PLUS, left, right) {}

protected:
    std::string operator_name() const override { return "Add"; }
};

class OpSub : public OpBinary {
public:
    OpSub(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_MINUS, left, right) {}

protected:
    std::string operator_name() const override { return "Sub"; }
};

class OpMult : public OpBinary {
public:
    OpMult(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_MULT, left, right) {}

protected:
    std::string operator_name() const override { return "Mult"; }
};

class OpDivF : public OpBinary {
public:
    OpDivF(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_DIVF, left, right) {}

protected:
    std::string operator_name() const override { return "DivF"; }
};

class OpEquals : public OpBinary {
public:
    OpEquals(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_EQUALS, left, right) {}

protected:
    std::string operator_name() const override { return "OpEq"; }
};

class OpGT : public OpBinary {
public:
    OpGT(const Node::Ptr &left, const Node::Ptr &right);

protected:
    std::string operator_name() const override { return "OpGt"; }
};

class OpLT : public OpBinary {
public:
    OpLT(const Node::Ptr &left, const Node::Ptr &right);

protected:
    std::string operator_name() const override { return "OpLt"; }
};

class OpGE : public OpBinary {
public:
    OpGE(const Node::Ptr &left, const Node::Ptr &right);

protected:
    std::string operator_name() const override { return "OpGe"; }
};

class OpLE : public OpBinary {
public:
    OpLE(const Node::Ptr &left, const Node::Ptr &right);

protected:
    std::string operator_name() const override { return "OpLe"; }
};

class OpAssign : public Node {
private:
    Node::Ptr m_left;
    Node::Ptr m_right;

public:
    OpAssign(const Node::Ptr &left, const Node::Ptr &right)
        : m_left(left), m_right(right) {
            assert(left);
            assert(right);
        }

    auto get_left() const { return m_left; }
    auto get_right() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("Assign({}, {})", m_left->as_string(), m_right->as_string());
    }
};

class OpDot : public Node {
private:
    Node::Ptr m_left;
    Node::Ptr m_right;

public:
    OpDot(Node::Ptr &left, Node::Ptr &right)
        : m_left(left), m_right(right) {
        }

    auto get_left() const { return m_left; }
    auto get_right() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("Dot(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpAnd : public Node {
private:
    Node::Ptr m_left;
    Node::Ptr m_right;

public:
    OpAnd(Node::Ptr &left, Node::Ptr &right);

    auto get_left() const { return m_left; }
    auto get_right() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("And(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpOr : public Node {
private:
    Node::Ptr m_left;
    Node::Ptr m_right;

public:
    OpOr(Node::Ptr &left, Node::Ptr &right);

    auto get_left() const { return m_left; }
    auto get_right() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("Or(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

}

#endif // KIRAZ_AST_OPERATOR_H
