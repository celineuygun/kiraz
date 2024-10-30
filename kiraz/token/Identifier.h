#ifndef KIRAZ_TOKEN_IDENTIFIER_H
#define KIRAZ_TOKEN_IDENTIFIER_H

#include <kiraz/Token.h>

namespace token {

class Identifier : public Token {
public:
    Identifier(std::string_view value) 
        : Token(IDENTIFIER), m_value(value) {}
    virtual ~Identifier() = default;

    std::string as_string() const override {
        return fmt::format("Identifier({})", m_value);
    }
    
    void print() {fmt::print("{}\n", as_string());}

private:
    std::string m_value;
};

}

#endif // KIRAZ_TOKEN_IDENTIFIER_H
