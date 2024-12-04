#include "BuiltinTypes.h"

namespace ast {

std::shared_ptr<BuiltinType> BuiltinManager::Boolean = std::make_shared<BuiltinType>("Boolean");
std::shared_ptr<BuiltinType> BuiltinManager::Integer64 = std::make_shared<BuiltinType>("Integer64");
std::shared_ptr<BuiltinType> BuiltinManager::Void = std::make_shared<BuiltinType>("Void");
std::shared_ptr<BuiltinType> BuiltinManager::String = std::make_shared<BuiltinType>("String");
std::shared_ptr<BuiltinType> BuiltinManager::Function = std::make_shared<BuiltinType>("Function");

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
