#include "BuiltinTypes.h"

namespace ast {

std::shared_ptr<BuiltinType> BuiltinManager::Boolean = []() {
    auto type = std::make_shared<BuiltinType>("Boolean");
    type->set_stmt_type(type);
    return type;
}();

std::shared_ptr<BuiltinType> BuiltinManager::Integer64 = []() {
    auto type = std::make_shared<BuiltinType>("Integer64");
    type->set_stmt_type(type);
    return type;
}();

std::shared_ptr<BuiltinType> BuiltinManager::Void = []() {
    auto type = std::make_shared<BuiltinType>("Void");
    type->set_stmt_type(type);
    return type;
}();

std::shared_ptr<BuiltinType> BuiltinManager::String = []() {
    auto type = std::make_shared<BuiltinType>("String");
    type->set_stmt_type(type);
    return type;
}();

std::shared_ptr<BuiltinType> BuiltinManager::Function = []() {
    auto type = std::make_shared<BuiltinType>("Function");
    type->set_stmt_type(type);
    return type;
}();

// Implementations for logical functions
Node::Ptr BuiltinManager::AndFunction(Node::Ptr left, Node::Ptr right) {
    return std::make_shared<OpAnd>(left, right);
}

Node::Ptr BuiltinManager::OrFunction(Node::Ptr left, Node::Ptr right) {
    return std::make_shared<OpOr>(left, right);
}

Node::Ptr BuiltinManager::NotFunction(Node::Ptr operand) {
    return std::make_shared<OpNot>(operand);
}

} // namespace ast
