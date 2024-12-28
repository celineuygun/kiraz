
#include "Operator.h"
#include "BuiltinTypes.h"
#include <iostream>

namespace ast {
OpNot::OpNot(Node::Ptr &operand): OpUnary(OP_NOT, operand) { set_stmt_type(BuiltinManager::Boolean); }

OpGT::OpGT(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_GT, left, right) { set_stmt_type(BuiltinManager::Boolean); }
OpLT::OpLT(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_LT, left, right) { set_stmt_type(BuiltinManager::Boolean); }
OpGE::OpGE(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_GE, left, right) { set_stmt_type(BuiltinManager::Boolean); }
OpLE::OpLE(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_LE, left, right) { set_stmt_type(BuiltinManager::Boolean); }

OpAnd::OpAnd(Node::Ptr &left, Node::Ptr &right): m_left(left), m_right(right) { set_stmt_type(BuiltinManager::Boolean); }
OpOr::OpOr(Node::Ptr &left, Node::Ptr &right): m_left(left), m_right(right) { set_stmt_type(BuiltinManager::Boolean); }

Node::Ptr OpAdd::compute_stmt_type(SymbolTable &st) {
    // std::cout << "left: " << get_left()->as_string() << std::endl;
    // std::cout << "right: " << get_right()->as_string() << std::endl;

    //     auto leftType = get_left()->get_stmt_type();
    //     auto rightType = get_right()->get_stmt_type();

    //     if (!leftType || !rightType) {
    //         return set_error("Operand type cannot be determined");
    //     }

    //     if (leftType != rightType) {
    //         return set_error(fmt::format(
    //             "Operator '+' not defined for types '{}' and '{}'",
    //             leftType->as_string(), rightType->as_string()));
    //     }

    //     if (leftType != BuiltinManager::Integer64 && leftType != BuiltinManager::String) {
    //         return set_error(fmt::format(
    //             "Operator '+' not supported for type '{}'", leftType->as_string()));
    //     }

    //     return std::const_pointer_cast<Node>(leftType);
        return nullptr;
    }
}