#include "Identifier.h"
#include <cassert>

#include <kiraz/token/Identifier.h> 

namespace ast {

Identifier::Identifier(const Token::Ptr &token) : Node(IDENTIFIER) {
    assert(token->get_id() == IDENTIFIER);
    auto token_id = std::static_pointer_cast<const token::Identifier>(token);
    if (token_id) {
        m_name = token_id->get_value();
    } else {
        m_name = "invalid";
    }
}

}