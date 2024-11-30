#include "BuiltinTypes.h"

namespace ast {

std::shared_ptr<BuiltinType> BuiltinManager::Boolean = std::make_shared<BuiltinType>("Boolean");
std::shared_ptr<BuiltinType> BuiltinManager::Integer64 = std::make_shared<BuiltinType>("Integer64");
std::shared_ptr<BuiltinType> BuiltinManager::Void = std::make_shared<BuiltinType>("Void");
std::shared_ptr<BuiltinType> BuiltinManager::String = std::make_shared<BuiltinType>("String");

} // namespace ast
