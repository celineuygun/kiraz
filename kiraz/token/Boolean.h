#ifndef KIRAZ_TOKEN_BOOLEAN_H
#define KIRAZ_TOKEN_BOOLEAN_H

#include <kiraz/Token.h>

namespace token {
class Boolean : public Token {
public:
    Boolean(bool value) 
        : Token(value ? L_TRUE : L_FALSE), m_value(value) {}
    virtual ~Boolean() = default;

    auto get_value() const { return m_value; }

    // std::string as_string() const override {
    //     return m_value ? "true" : "false";
    // }
    std::string as_string() const override { 
        return fmt::format("{}", get_value()); 
    }
    void print() {fmt::print("{}\n", as_string());}

private:
    bool m_value;
};

}

#endif // KIRAZ_TOKEN_BOOLEAN_H
