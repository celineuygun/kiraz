#ifndef KIRAZ_AST_IDENTIFIER_H
#define KIRAZ_AST_IDENTIFIER_H

#include <kiraz/Node.h>
#include <string>
#include <iostream>

namespace ast {

class Identifier : public Node {
private:
    std::string m_name;

public:
    explicit Identifier(const std::string &name)
        : Node(IDENTIFIER), m_name(name) {}
    explicit Identifier(const Token::Ptr &token)
        : Node(IDENTIFIER), m_name(token->as_string()) {}

    auto get_name() const { return m_name; }

    std::string as_string() const override {
        return fmt::format("Identifier({})", m_name);
    }
};

}

#endif // KIRAZ_AST_IDENTIFIER_H
