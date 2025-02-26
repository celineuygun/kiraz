
#include "Literal.h"

#include <cassert>
#include <kiraz/token/Literal.h>

namespace ast {
Integer::Integer(Token::Ptr t) {
    auto token_int = std::static_pointer_cast<const token::Integer>(t);
    m_base = token_int->get_base();
    try {
        m_value = std::stoll(token_int->get_value(), nullptr, m_base);
    }
    catch (std::out_of_range &e) {
        m_value = 0;
    }
    set_stmt_type(BuiltinManager::Integer64);
}

String::String(Token::Ptr t) {
    auto token_str = std::static_pointer_cast<const token::String>(t);
    m_value = token_str->get_value();
    set_stmt_type(BuiltinManager::String);
}

}
