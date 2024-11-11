
#include "Node.h"
#include "Compiler.h"

std::vector<Node::Ptr> Node::s_roots;
Token::Ptr curtoken;

Node::~Node() {}

Node::Ptr Node::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    return nullptr;
}

Node::Ptr Node::add_to_symtab_forward(SymbolTable &st) {
    return nullptr;
}

Node::Ptr Node::add_to_symtab_ordered(SymbolTable &st) {
    return nullptr;
}

Node::Ptr Node::pop_root() {
    assert(! s_roots.empty());
    auto retval = s_roots.back();
    s_roots.pop_back();
    return retval;
}

const Node::Ptr &Node::get_root_before() {
    assert(s_roots.size() > 1);
    return *std::next(s_roots.rbegin());
}
