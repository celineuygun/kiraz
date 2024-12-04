#ifndef KIRAZ_AST_OPERATOR_H
#define KIRAZ_AST_OPERATOR_H

#include <cassert>

#include <kiraz/Node.h>

namespace ast {

class Operator : public Node {
protected:
    explicit Operator(int op) : Node(op) {}
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

public:
    auto get_left() const {return m_left;}
    auto get_right() const {return m_right;}

    std::string as_string() const override {
        assert(get_left());
        assert(get_right());

        std::string opstr;
        switch (get_id())
        {
        case OP_PLUS:
            opstr = "Add";
            break;
        
        case OP_MINUS:
            opstr = "Sub";
            break;
        
        case OP_MULT:
            opstr = "Mult";
            break;
        
        case OP_DIVF:
            opstr = "DivF";
            break;

        case OP_EQUALS:
            opstr = "OpEq"; 
        break;
        
        case OP_GT: 
            opstr = "OpGt"; 
            break;
        
        case OP_LT: 
            opstr = "OpLt"; 
            break;
        
        case OP_GE: 
            opstr = "OpGe"; 
            break;
        
        case OP_LE: 
            opstr = "OpLe"; 
            break;

        case OP_AND: 
            opstr = "OpAnd"; 
            break;

        case OP_OR: 
            opstr = "OpOr"; 
            break;
        
        default:
            break;
        }

        return fmt::format("{}(l={}, r={})", opstr, get_left()->as_string(), get_right()->as_string());
    }

private:
    Node::Ptr m_left, m_right;
};

class OpNot : public OpUnary {
public:
    explicit OpNot(Node::Ptr &operand) : OpUnary(OP_NOT, operand) {}

protected:
    std::string operator_name() const override { return "Not"; }
};

class OpAdd : public OpBinary {
public:
    OpAdd(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_PLUS, left, right) {}
};

class OpSub : public OpBinary {
public:
    OpSub(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_MINUS, left, right) {}
};

class OpMult : public OpBinary {
public:
    OpMult(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_MULT, left, right) {}
};

class OpDivF : public OpBinary {
public:
    OpDivF(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_DIVF, left, right) {}
};

class OpEquals : public OpBinary {
public:
    OpEquals(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_EQUALS, left, right) {}
};

class OpGT : public OpBinary {
public:
    OpGT(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_GT, left, right) {}
};

class OpLT : public OpBinary {
public:
    OpLT(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_LT, left, right) {}
};

class OpGE : public OpBinary {
public:
    OpGE(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_GE, left, right) {}
};

class OpLE : public OpBinary {
public:
    OpLE(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_LE, left, right) {}
};

class OpAssign : public Node {
private:
    Node::Ptr m_left;
    Node::Ptr m_right;

public:
    OpAssign(const Node::Ptr &left, const Node::Ptr &right)
        : Node(OP_ASSIGN), m_left(left), m_right(right) {
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
        : Node(OP_DOT), m_left(left), m_right(right) {
            assert(left);
            assert(right);
        }

    auto getLeft() const { return m_left; }
    auto getRight() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("Dot(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpAnd : public Node {
private:
    Node::Ptr m_left;
    Node::Ptr m_right;

public:
    OpAnd(Node::Ptr &left, Node::Ptr &right)
        : Node(OP_AND), m_left(left), m_right(right) {
            assert(left);
            assert(right);
        }

    auto getLeft() const { return m_left; }
    auto getRight() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("And(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpOr : public Node {
private:
    Node::Ptr m_left;
    Node::Ptr m_right;

public:
    OpOr(Node::Ptr &left, Node::Ptr &right)
        : Node(OP_OR), m_left(left), m_right(right) {
            assert(left);
            assert(right);
        }

    auto getLeft() const { return m_left; }
    auto getRight() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("Or(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

}

#endif // KIRAZ_AST_OPERATOR_H
