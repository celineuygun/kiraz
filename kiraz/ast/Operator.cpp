
#include "Operator.h"
#include "BuiltinTypes.h"

namespace ast {
OpNot::OpNot(Node::Ptr &operand): OpUnary(OP_NOT, operand) { set_stmt_type(BuiltinManager::Boolean); }

OpGT::OpGT(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_GT, left, right) { set_stmt_type(BuiltinManager::Boolean); }
OpLT::OpLT(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_LT, left, right) { set_stmt_type(BuiltinManager::Boolean); }
OpGE::OpGE(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_GE, left, right) { set_stmt_type(BuiltinManager::Boolean); }
OpLE::OpLE(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_LE, left, right) { set_stmt_type(BuiltinManager::Boolean); }

OpAnd::OpAnd(Node::Ptr &left, Node::Ptr &right): m_left(left), m_right(right) { set_stmt_type(BuiltinManager::Boolean); }
OpOr::OpOr(Node::Ptr &left, Node::Ptr &right): m_left(left), m_right(right) { set_stmt_type(BuiltinManager::Boolean); }
    
}