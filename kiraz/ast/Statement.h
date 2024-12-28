#ifndef KIRAZ_AST_STATEMENT_H
#define KIRAZ_AST_STATEMENT_H

#include <cassert>
#include <string>
#include <memory> 
#include <filesystem>
#include "kiraz/Node.h"
#include "Identifier.h"
#include "BuiltinTypes.h"

namespace fs = std::filesystem;

namespace ast {

class Parameter;
class ParameterList;

class Statement : public Node {
protected:
    explicit Statement(int type) {}
};

class Module : public Statement {
private:
    Node::Ptr m_statements;
    std::unique_ptr<SymbolTable> m_symtab;

public:
    Module(Node::Ptr statements) : Statement(IDENTIFIER), m_statements(statements) {}

    bool is_stmt_list() const override { return true; }

    auto get_statements() const { return m_statements; }

    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;

    Node::Ptr gen_wat(WasmContext &ctx) override;

    std::string as_string() const override {
    return fmt::format("Module([{}])", m_statements ? m_statements->as_string() : "");
    }

};

class StatementList : public Statement {
private:
    Node::Ptr m_first;
    Node::Ptr m_next;

public:
    StatementList(Node::Ptr first, Node::Ptr next)
        : Statement(IDENTIFIER), m_first(first), m_next(next) {}

    bool is_stmt_list() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;
    
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

class ImportStatement : public Statement {
private:
    Node::Ptr m_identifier;

public:
    ImportStatement(Node::Ptr identifier)
        : Statement(KW_IMPORT), m_identifier(identifier) {
        assert(identifier);
    }

    bool is_stmt_list() const override { return true; }

    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;

    auto get_identifier() const { return m_identifier; }

    std::string resolve_module_path(const std::string &moduleName) {
        std::vector<std::string> searchPaths = { "./", "/usr/include/", "/usr/local/include/" };

        for (const auto &path : searchPaths) {
            std::string fullPath = path + moduleName + ".ki";
            if (fs::exists(fullPath)) {
                std::cout << "Found module: " << fullPath << std::endl;
                return fullPath;
            }
        }
        std::cout << "Module not found: " << moduleName << std::endl;
        return "";
    }

    void integrate_module_symbols(Node::Ptr moduleAST, SymbolTable &st);

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

    bool is_func() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;

    auto get_callee() const { return m_callee; }
    auto get_arguments() const { return m_arguments; }

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
    Node::Ptr m_parent_class;
    Node::Ptr m_stmts; // Contains both methods and member variables
    std::unique_ptr<SymbolTable> m_symtab;

public:
    ClassStatement(Node::Ptr name, Node::Ptr stmts = nullptr, Node::Ptr parent_class = nullptr)
        : Statement(KW_CLASS), m_name(name), m_stmts(stmts), m_parent_class(parent_class) {}

    bool is_class() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;

    auto get_name() const { return m_name; }
    auto get_parent_class() const { return m_parent_class; }
    auto get_stmts() const { return m_stmts; }
    auto get_symtab() const { return m_symtab.get(); }

    std::string as_string() const override {
        std::string stmtString;
        if (m_parent_class) {
            stmtString = fmt::format(", p={}", m_parent_class->as_string());
        }
        return fmt::format("Class(n={}, s=[{}]{})", 
                           m_name->as_string(), 
                           m_stmts ? m_stmts->as_string() : "",
                           stmtString);
    }
};

class FunctionStatement : public Statement {
private:
    Node::Ptr m_name;
    Node::Ptr m_returnType;
    Node::Ptr m_parameters;
    Node::Ptr m_body;
    std::unique_ptr<SymbolTable> m_symtab;

public:
    FunctionStatement(Node::Ptr name, Node::Ptr parameters, Node::Ptr returnType, Node::Ptr body)
        : Statement(KW_FUNC),
          m_name(name), 
          m_returnType(returnType ? returnType : BuiltinManager::Void),
          m_parameters(parameters), 
          m_body(body) {}

    bool is_func() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;

    Node::Ptr gen_wat(WasmContext &ctx) override;
    
    auto get_name() const { return m_name; }
    auto get_return_type() const { return m_returnType; }
    auto get_parameters() const { return m_parameters; }
    auto get_body() const { return m_body; }

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

class ReturnStatement : public Statement {
private:
    Node::Ptr m_expression;

public:
    ReturnStatement(Node::Ptr expression)
        : Statement(KW_RETURN), m_expression(expression) {
        assert(expression);
    }

    bool is_stmt_list() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;
    
    auto get_expression() const { return m_expression; }

    std::string as_string() const override {
        return fmt::format("Return({})", m_expression->as_string());
    }
};

class ParameterList : public Statement {
private:
    Node::Ptr m_first;
    Node::Ptr m_next;
    std::shared_ptr<FunctionStatement> m_stmt;

public:
    ParameterList(Node::Ptr first, Node::Ptr next)
        : Statement(IDENTIFIER), m_first(first), m_next(next) {}
    
    bool is_funcarg_list() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;
    
    auto get_first() const { return m_first; }
    auto get_next() const { return m_next; }
    auto get_stmt() const { return m_stmt; }
    void set_stmt(std::shared_ptr<FunctionStatement> stmt) { m_stmt = stmt; }

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
    std::shared_ptr<FunctionStatement> m_stmt;

public:
    Parameter(const Node::Ptr &name, const Node::Ptr &type)
        : Statement(IDENTIFIER), 
          m_name(name), 
          m_type(type)
    {
        assert(name);
    }

    bool is_funcarg_list() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;
    
    auto get_type() const { return m_type; }
    auto get_name() const { return m_name; }
    auto get_stmt() const { return m_stmt; }
    void set_stmt(std::shared_ptr<FunctionStatement> stmt) { m_stmt = stmt; }

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

    bool is_stmt_list() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;

    Node::Ptr gen_wat(WasmContext &ctx) override {
        if (m_value) {
            m_value->gen_wat(ctx);
        }
        auto id = std::dynamic_pointer_cast<Identifier>(m_identifier);
        ctx.locals() << "(local $" << id->get_name() << " i64)\n";
        return nullptr;
    }
    
    LetStatement(const Node::Ptr &identifier, const Node::Ptr &type, const Node::Ptr &value)
        : Statement(KW_LET), m_identifier(identifier), m_type(type), m_value(value) {
            assert(identifier);
            assert(type);
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
            assert(type);
        }

    bool is_stmt_list() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;
    
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

    bool is_stmt_list() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;

    Node::Ptr gen_wat(WasmContext &ctx) override {
        ctx.body() << "(if \n";

        ctx.body() << "  (condition\n";
        if (m_condition) {
            m_condition->gen_wat(ctx);
        }
        ctx.body() << "  )\n";

        ctx.body() << "  (then\n";
        if (m_thenBranch) {
            m_thenBranch->gen_wat(ctx);
        }
        ctx.body() << "  )\n";

        if (m_elseBranch) {
            ctx.body() << "  (else\n";
            m_elseBranch->gen_wat(ctx);
            ctx.body() << "  )\n";
        }

        ctx.body() << ")\n";
        return nullptr;
    }

    auto get_condition() const { return m_condition; }
    auto get_then_branch() const { return m_thenBranch; }
    auto get_else_branch() const { return m_elseBranch; }

    std::string as_string() const override {
        std::string thenString;
        if (m_thenBranch) {
            thenString = fmt::format("[{}]", m_thenBranch->as_string());
        } else {
            thenString = "[]";
        }

        std::string elseString;
        if (m_elseBranch) {
            if (auto elseIfStatement = dynamic_cast<IfStatement*>(m_elseBranch.get())) {
                elseString = fmt::format("If(?={}, then=[{}], else=[{}])",
                                        elseIfStatement->get_condition()->as_string(),
                                        elseIfStatement->get_then_branch() ? elseIfStatement->get_then_branch()->as_string() : "[]",
                                        elseIfStatement->get_else_branch() ? elseIfStatement->get_else_branch()->as_string() : "[]");
            } else {
                elseString = fmt::format("[{}]", m_elseBranch->as_string());
            }
        } else {
            elseString = "[]";
        }

        return fmt::format("If(?={}, then={}, else={})",
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

    bool is_stmt_list() const override { return true; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;
    
    auto get_condition() const { return m_condition; }
    auto get_repeat_branch() const { return m_repeatBranch; }

    std::string as_string() const override {
        std::string repeatString = m_repeatBranch ? m_repeatBranch->as_string() : "";

        return fmt::format("While(?={}, repeat=[{}])", 
                           m_condition->as_string(), 
                           repeatString);
    }
};

class OpDot : public Statement {
private:
    Node::Ptr m_left;
    Node::Ptr m_right;

public:
    OpDot(Node::Ptr &left, Node::Ptr &right)
        : Statement(OP_DOT), m_left(left), m_right(right) {
        }

    auto get_left() const { return m_left; }
    auto get_right() const { return m_right; }

    std::string as_string() const override {
        return fmt::format("Dot(l={}, r={})", m_left->as_string(), m_right->as_string());
    }

    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;
};


}

#endif // KIRAZ_AST_STATEMENT_H