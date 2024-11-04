
#include "Literal.h"

#include <cassert>
#include <kiraz/token/Literal.h>

namespace ast {
Integer::Integer(Token::Ptr t) : Node(L_INTEGER) {
    assert(t->get_id() == L_INTEGER);
    auto token_int = std::static_pointer_cast<const token::Integer>(t);
    m_base = token_int->get_base();
    try {
        m_value = std::stoll(token_int->get_value(), nullptr, m_base);
    }
    catch (std::out_of_range &e) {
        m_value = 0;
    }
}

String::String(Token::Ptr t) : Node(L_STRING) {
    assert(t->get_id() == L_STRING);
    auto token_str = std::static_pointer_cast<const token::String>(t);
    m_value = token_str->get_value();
}

}
