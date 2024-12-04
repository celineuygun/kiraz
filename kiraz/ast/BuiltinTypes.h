#ifndef KIRAZ_AST_BUILTINS_H
#define KIRAZ_AST_BUILTINS_H

#include "kiraz/Node.h"
#include "Operator.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace ast {

// Base class for type nodes
class TypeNode : public Node {
protected:
    explicit TypeNode(int type) : Node(type) {}
};

class BuiltinType : public TypeNode {
private:
    std::string m_name;

public:
    explicit BuiltinType(const std::string& name)
        : TypeNode(IDENTIFIER), m_name(name) {}

    auto get_name() const { return m_name; }

    std::string as_string() const override {
        return fmt::format("BuiltinType({})", m_name);
    }

    Node::Ptr compute_stmt_type(SymbolTable&) override {
        return shared_from_this();
    }
};

class BuiltinManager {
public:
    static std::shared_ptr<BuiltinType> Boolean;
    static std::shared_ptr<BuiltinType> Integer64;
    static std::shared_ptr<BuiltinType> Void;
    static std::shared_ptr<BuiltinType> String;
    static std::shared_ptr<BuiltinType> Function;

    static Node::Ptr AndFunction(Node::Ptr left, Node::Ptr right);
    static Node::Ptr OrFunction(Node::Ptr left, Node::Ptr right);
    static Node::Ptr NotFunction(Node::Ptr operand);


    static Node::Ptr get_builtin_type(const std::string& name) {
        static const std::unordered_map<std::string, std::shared_ptr<BuiltinType>> builtins = {
            {"Boolean", Boolean},
            {"Integer64", Integer64},
            {"Void", Void},
            {"String", String},
        };

        auto it = builtins.find(name);
        return it != builtins.end() ? it->second : nullptr;
    }
};

} // namespace ast

#endif // KIRAZ_AST_BUILTINS_H
