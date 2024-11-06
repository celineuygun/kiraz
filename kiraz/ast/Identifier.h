#ifndef KIRAZ_AST_IDENTIFIER_H
#define KIRAZ_AST_IDENTIFIER_H

#include <kiraz/Node.h>
#include <string>
namespace ast {

class Identifier : public Node {
private:
    std::string m_name;

public:
    explicit Identifier(const Token::Ptr &token);

    auto get_name() const { return m_name; }

    std::string as_string() const override {
        return fmt::format("Id({})", m_name);
    }
};

}

#endif // KIRAZ_AST_IDENTIFIER_H
