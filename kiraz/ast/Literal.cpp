
#include "Literal.h"

#include <cassert>
#include <kiraz/token/Literal.h>

namespace ast {
Integer::Integer(Token::Ptr t) : Node(L_INTEGER) {
    assert(t->get_id() == L_INTEGER);
    auto token_int = std::static_pointer_cast<const token::Integer>(t);
    auto base = token_int->get_base();
    try {
        m_value = std::stoll(token_int->get_value(), nullptr, base);
    }
    catch (std::out_of_range &e) {
        m_value = 0;
    }
}
}
