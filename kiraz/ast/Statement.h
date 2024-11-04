#ifndef KIRAZ_AST_STATEMENT_H
#define KIRAZ_AST_STATEMENT_H

#include <cassert>
#include <string>
#include <kiraz/Node.h>
#include <iostream>

namespace ast {

class Statement : public Node {
protected:
    explicit Statement(int type) : Node(type) {}
};

// TODO change types
class TypeNode : public Node {
private:
    enum enum_type {
        none_type,
        int_type,
        uint_type,
        long_long_type,
        ulong_long_type,
        int128_type,
        uint128_type,
        bool_type,
        char_type,
        last_integer_type = char_type,
        float_type,
        double_type,
        long_double_type,
        last_numeric_type = long_double_type,
        cstring_type,
        string_type,
        pointer_type,
        custom_type
    } m_enumType;

    std::string m_type;

public:
    TypeNode(const std::string &type) 
        : Node(IDENTIFIER), m_type(type) {
        
        initialize_enum_from_string(type);
    }

    void initialize_enum_from_string(const std::string &type) {
        if (type == "int") m_enumType = int_type;
        else if (type == "uint") m_enumType = uint_type;
        else if (type == "long") m_enumType = long_long_type;
        else if (type == "ulong") m_enumType = ulong_long_type;
        else if (type == "int128") m_enumType = int128_type;
        else if (type == "uint128") m_enumType = uint128_type;
        else if (type == "bool") m_enumType = bool_type;
        else if (type == "char") m_enumType = char_type;
        else if (type == "float") m_enumType = float_type;
        else if (type == "double") m_enumType = double_type;
        else if (type == "long_double") m_enumType = long_double_type;
        else if (type == "cstring") m_enumType = cstring_type;
        else if (type == "string") m_enumType = string_type;
        else if (type == "pointer") m_enumType = pointer_type;
        else if (type == "custom") m_enumType = custom_type;
        else m_enumType = none_type;
    }

    auto get_type() const { return m_type; }

    bool is_valid_type() const {
        return m_enumType != none_type;
    }

    std::string as_string() const override {
        return m_type;
    }
};

class Parameter : public Node {
private:
    Node::Ptr m_name;
    Node::Ptr m_type;

public:
    Parameter(const Node::Ptr &name, const Node::Ptr &type)
        : Node(IDENTIFIER), 
          m_name(name), 
          m_type(type) 
    {
        assert(name);
        assert(type);
    }

    auto get_type() const { return m_type; }
    auto get_name() const { return m_name; }

    std::string as_string() const override {
        return fmt::format("Parameter(name={}, type={})", m_name->as_string(), m_type->as_string());
    }
};

class FunctionStatement : public Node {
private:
    Node::Ptr m_name;
    Node::Ptr m_returnType;
    std::vector<Node::Ptr> m_parameters;
    Node::Ptr m_body;

public:
    FunctionStatement(Node::Ptr name, Node::Ptr returnType, std::vector<Node::Ptr> parameters, Node::Ptr body)
        : Node(KW_FUNC), 
          m_name(name), 
          m_returnType(returnType),
          m_parameters(parameters), 
          m_body(body) {}

    auto getName() const { return m_name; }
    auto getReturnType() const { return m_returnType; }
    auto getParameters() const { return m_parameters; }
    auto getBody() const { return m_body; }

    std::string as_string() const override {
        std::string param_list = "[";
        for (const auto &param : m_parameters) {
            param_list += param->as_string() + ", ";
        }
        if (!m_parameters.empty()) {
            param_list = param_list.substr(0, param_list.size() - 2);
        }
        param_list += "]";

        return fmt::format("Function(\n  name={},\n  return_type={},\n  parameters={},\n  body={})", 
                           m_name->as_string(),
                           m_returnType ? m_returnType->as_string() : "void",
                           param_list, 
                           m_body ? m_body->as_string() : "null");
    }
};

class LetStatement : public Statement {
private:
    Node::Ptr m_identifier;
    Node::Ptr m_type;
    Node::Ptr m_value;

public:
    LetStatement(const Node::Ptr &identifier, const Node::Ptr &value)
        : Statement(KW_LET), m_identifier(identifier), m_type(nullptr), m_value(value) {
            assert(identifier);
            assert(value);
        }

    LetStatement(const Node::Ptr &identifier, const Node::Ptr &type, const Node::Ptr &value)
        : Statement(KW_LET), m_identifier(identifier), m_type(type), m_value(value) {
            assert(identifier);
            assert(value);
            if (type) {
                auto typeNode = Node::add<ast::TypeNode>(type->as_string());
                assert(typeNode && typeNode->is_valid_type());
            } else {
                m_type = nullptr;
            }
        }

    auto get_identifier() const { return m_identifier; }
    auto get_type() const { return m_type; }
    auto get_value() const { return m_value; }

    std::string as_string() const override {
        std::string output = fmt::format("Let(\n  n={},", m_identifier->as_string());
        
        if (m_type) {
            output += fmt::format("\n  t={},", m_type->as_string());
        }

        output += fmt::format("\n  i={})", m_value ? m_value->as_string() : "none");

        return output;
    }
};

class AssignmentStatement : public Statement {
private:
    Node::Ptr m_left;
    Node::Ptr m_type;
    Node::Ptr m_right;

public:
    AssignmentStatement(const Node::Ptr &left, const Node::Ptr &right)
        : Statement(OP_ASSIGN), m_left(left), m_type(nullptr), m_right(right) {
            assert(left);
            assert(right);
        }
    AssignmentStatement(const Node::Ptr &left, const Node::Ptr &type, const Node::Ptr &right)
        : Statement(OP_ASSIGN), m_left(left), m_type(type), m_right(right) {
            assert(left);
            assert(right);
            if (type) {
                auto typeNode = Node::add<ast::TypeNode>(type->as_string());
                assert(typeNode && typeNode->is_valid_type());
            } else {
                m_type = nullptr;
            }
        }

    auto get_left() const { return m_left; }
    auto get_right() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("Assign(\n  l={},\n  r={})", 
                           m_left->as_string(), 
                           m_right->as_string());
    }
};

}

#endif // KIRAZ_AST_STATEMENT_H
