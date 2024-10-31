#ifndef KIRAZ_AST_OPERATOR_H
#define KIRAZ_AST_OPERATOR_H

#include <cassert>

#include <kiraz/Node.h>

namespace ast {
class OpBinary : public Node {
protected:
    explicit OpBinary(int op, const Node::Ptr &left, const Node::Ptr &right)
        : Node(op), m_left(left), m_right(right) {
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
            opstr = "Equals"; 
        break;
        
        case OP_GT: 
            opstr = "GT"; 
            break;
        
        case OP_LT: 
            opstr = "LT"; 
            break;
        
        case OP_GE: 
            opstr = "GE"; 
            break;
        
        case OP_LE: 
            opstr = "LE"; 
            break;
        
        default:
            break;
        }

        return fmt::format("{}({},{})", opstr, get_left()->as_string(), get_right()->as_string());
    }

private:
    Node::Ptr m_left, m_right;
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

class OpReturn : public Node {
private:
    Node::Ptr m_returnValue;

public:
    OpReturn(const Node::Ptr &returnValue) 
        : Node(OP_RETURNS), m_returnValue(returnValue) {
            assert(returnValue);
        }

    auto get_return_value() const { return m_returnValue; }

    std::string as_string() const override {
        return fmt::format("Return({})", m_returnValue->as_string());
    }
};

}

#endif // KIRAZ_AST_OPERATOR_H
