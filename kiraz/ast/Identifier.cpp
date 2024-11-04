#include "Identifier.h"
#include <cassert>
#include <iostream>

#include <kiraz/token/Identifier.h> 

namespace ast {

// FIXME try to use this constructor instead of the other
Identifier::Identifier(const Token::Ptr &token) : Node(IDENTIFIER) {
    std::cout << "Token : " << token->as_string() << std::endl;
    assert(token->get_id() == IDENTIFIER);
    auto token_id = std::static_pointer_cast<const token::Identifier>(token);
    if (token_id) {
        m_name = token_id->as_string();
    } else {
        m_name = "invalid";
    }
}

}