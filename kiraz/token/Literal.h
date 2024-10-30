#ifndef KIRAZ_TOKEN_LITERAL_H
#define KIRAZ_TOKEN_LITERAL_H

#include <kiraz/Token.h>

namespace token {

class Integer : public Token {
public:
    Integer(int64_t base, std::string_view value) 
        : Token(L_INTEGER), m_base(base), m_value(value) {}
    virtual ~Integer();

    std::string as_string() const override {return fmt::format("Integer{}\n", m_value);}
    void print() {fmt::print("{}\n", as_string());}

    static int colno;

    auto get_base() const {return m_base;}
    auto get_value() const {return m_value;}

private:
    int m_id;
    int64_t m_base;
    std::string m_value;
};

}

#endif // KIRAZ_TOKEN_LITERAL_H
