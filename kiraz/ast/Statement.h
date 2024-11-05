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

class Module : public Statement {
private:
    Node::Ptr m_statements;

public:
    Module(Node::Ptr statements) : Statement(IDENTIFIER), m_statements(statements) {}

    std::string as_string() const override {
    return fmt::format("Module([{}])", m_statements ? m_statements->as_string() : "");
    }

};


// TODO change types
class TypeNode : public Statement {
private:
    enum enum_type {
        none_type,
        t_type,
        i_type,
        b_type,
        a1_type,
        int_type,
        int64_type,
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
        : Statement(IDENTIFIER), m_type(type) {
        
        initialize_enum_from_string(type);
    }

    void initialize_enum_from_string(const std::string &type) {
        if (type == "T") m_enumType = t_type;
        else if (type == "I") m_enumType = i_type;
        else if (type == "B") m_enumType = b_type;
        else if (type == "A1") m_enumType = a1_type;
        else if (type == "int") m_enumType = int_type;
        else if (type == "Int64") m_enumType = int64_type;
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
        return fmt::format("Id({})", m_type);
    }
};

class ImportStatement : public Statement {
private:
    Node::Ptr m_identifier;

public:
    ImportStatement(Node::Ptr identifier)
        : Statement(KW_IMPORT), m_identifier(identifier) {
        assert(identifier);
    }

    auto get_identifier() const { return m_identifier; }

    std::string as_string() const override {
        return fmt::format("Import({})", m_identifier->as_string());
    }
};

class CallStatement : public Statement {
private:
    Node::Ptr m_callee;
    Node::Ptr m_arguments;

public:
    CallStatement(Node::Ptr callee, Node::Ptr arguments)
        : Statement(IDENTIFIER), m_callee(callee), m_arguments(arguments) {}

    auto getCallee() const { return m_callee; }
    auto getArguments() const { return m_arguments; }

    std::string as_string() const override {
        std::string params_string;

        if (m_arguments) {
            params_string = fmt::format("FuncArgs([{}])", m_arguments->as_string());
        } else {
            params_string = "FuncArgs([])";
        }

        return fmt::format("Call(n={}, a={})", m_callee->as_string(), params_string);
    }

};


class ClassStatement : public Statement {
private:
    Node::Ptr m_name;
    Node::Ptr m_members; // Contains both methods and member variables

public:
    ClassStatement(Node::Ptr name, Node::Ptr members)
        : Statement(KW_CLASS), m_name(name), m_members(members) {}

    auto getName() const { return m_name; }
    auto getMembers() const { return m_members; }

    std::string as_string() const override {
        return fmt::format("Class(n={}, s=[{}])", 
                           m_name->as_string(), 
                           m_members ? m_members->as_string() : "");
    }
};

class ClassBody : public Statement {
private:
    Node::Ptr m_first;
    Node::Ptr m_next;

public:
    ClassBody(Node::Ptr first = nullptr, Node::Ptr next = nullptr)
        : Statement(IDENTIFIER), m_first(first), m_next(next) {}

    void setFirst(Node::Ptr first) { m_first = first; }
    void setNext(Node::Ptr next) { m_next = next; }

    auto getFirst() const { return m_first; }
    auto getNext() const { return m_next; }

    std::string as_string() const override {
        std::string result = "";

        if (m_first) {
            result += m_first->as_string();
        }

        if (m_next) {
            result += ", " + m_next->as_string();
        }

        return result;
    }
};

class Parameter : public Statement {
private:
    Node::Ptr m_name;
    Node::Ptr m_type;

public:
    Parameter(const Node::Ptr &name, const Node::Ptr &type)
        : Statement(IDENTIFIER), 
          m_name(name), 
          m_type(type) 
    {
        assert(name);
    }

    auto get_type() const { return m_type; }
    auto get_name() const { return m_name; }

    std::string as_string() const override {
        std::string result;
        if (m_type) {
            result = fmt::format("FArg(n={}, t={})", m_name->as_string(), m_type->as_string());
        }
    
        else {
            result = fmt::format("{}", m_name->as_string());
        }

        return result;
    }
};

class ParameterList : public Statement {
private:
    Node::Ptr m_first;
    Node::Ptr m_next;

public:
    ParameterList(Node::Ptr first, Node::Ptr next)
        : Statement(IDENTIFIER), m_first(first), m_next(next) {}

    auto get_first() const { return m_first; }
    auto get_next() const { return m_next; }

    std::string as_string() const override {
        std::string result = "";
        
        if (m_first) {
            result += m_first->as_string();
        }
        
        if (m_next) {
            result += ", " + m_next->as_string();
        }
        
        return result;
    }
};

class StatementList : public Statement {
private:
    Node::Ptr m_first;
    Node::Ptr m_next;

public:
    StatementList(Node::Ptr first, Node::Ptr next)
        : Statement(IDENTIFIER), m_first(first), m_next(next) {}

    auto get_first() const { return m_first; } 
    auto get_next() const { return m_next; }

    std::string as_string() const override {
        std::string result = "";

        if (m_first) {
            result += m_first->as_string();
        }

        if (m_next) {
            result += ", " + m_next->as_string();
        }

        return result;
    }
};


class FunctionStatement : public Statement {
private:
    Node::Ptr m_name;
    Node::Ptr m_returnType;
    Node::Ptr m_parameters;
    Node::Ptr m_body;

public:
    FunctionStatement(Node::Ptr name, Node::Ptr parameters, Node::Ptr returnType, Node::Ptr body)
        : Statement(KW_FUNC), 
          m_name(name), 
          m_returnType(returnType),
          m_parameters(parameters), 
          m_body(body) {}

    auto getName() const { return m_name; }
    auto getReturnType() const { return m_returnType; }
    auto getParameters() const { return m_parameters; }
    auto getBody() const { return m_body; }

     std::string as_string() const override {
        std::string params_string;

        if (m_parameters) {
            params_string = fmt::format("FuncArgs([{}])", m_parameters->as_string());
        } else {
            params_string = "[]";
        }

        return fmt::format("Func(n={}, a={}, r={}, s=[{}])", 
                           m_name->as_string(),
                           params_string,
                           m_returnType ? m_returnType->as_string() : "[]",
                           m_body ? m_body->as_string() : "");
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
            if (type) {
                //auto typeNode = Node::add<ast::TypeNode>(type->as_string());
                auto typeNode = std::static_pointer_cast<const ast::TypeNode>(type);
                assert(typeNode && typeNode->is_valid_type());
            } else {
                m_type = nullptr;
            }
        }

    auto get_identifier() const { return m_identifier; }
    auto get_type() const { return m_type; }
    auto get_value() const { return m_value; }

    std::string as_string() const override {
        std::string output = fmt::format("Let(n={}", m_identifier->as_string());
        
        if (m_type) {
            output += fmt::format(", t={}", m_type->as_string());
        }
        if (m_value) {
            output += fmt::format(", i={}", m_value ? m_value->as_string() : "none");
        }
        output += fmt::format(")");

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
                // auto typeNode = Node::add<ast::TypeNode>(type->as_string());
                auto typeNode = std::static_pointer_cast<const ast::TypeNode>(type);
                assert(typeNode && typeNode->is_valid_type());
            } else {
                m_type = nullptr;
            }
        }

    auto get_left() const { return m_left; }
    auto get_right() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("Assign(l={}, r={})", 
                           m_left->as_string(), 
                           m_right->as_string());
    }
};

class IfStatement : public Statement {
private:
    Node::Ptr m_condition;
    Node::Ptr m_thenBranch;
    Node::Ptr m_elseBranch;

public:
    IfStatement(Node::Ptr condition, Node::Ptr thenBranch, Node::Ptr elseBranch = nullptr)
        : Statement(IDENTIFIER), m_condition(condition), m_thenBranch(thenBranch), m_elseBranch(elseBranch) {
        assert(condition);
    }

    auto get_condition() const { return m_condition; }
    auto get_then_branch() const { return m_thenBranch; }
    auto get_else_branch() const { return m_elseBranch; }

    std::string as_string() const override {
        std::string thenString = m_thenBranch ? m_thenBranch->as_string() : "";
        
        std::string elseString = m_elseBranch ? m_elseBranch->as_string() : "";

        return fmt::format("If(?={}, then=[{}], else=[{}])", 
                           m_condition->as_string(), 
                           thenString, 
                           elseString);
    }
};

class WhileStatement : public Statement {
private:
    Node::Ptr m_condition;
    Node::Ptr m_repeatBranch;

public:
    WhileStatement(Node::Ptr condition, Node::Ptr repeatBranch = nullptr)
        : Statement(KW_WHILE), m_condition(condition), m_repeatBranch(repeatBranch) {
        assert(condition); 
    }

    auto get_condition() const { return m_condition; }
    auto get_repeat_branch() const { return m_repeatBranch; }

    std::string as_string() const override {
        std::string repeatString = m_repeatBranch ? m_repeatBranch->as_string() : "";

        return fmt::format("While(?={}, repeat=[{}])", 
                           m_condition->as_string(), 
                           repeatString);
    }
};

}

#endif // KIRAZ_AST_STATEMENT_H
