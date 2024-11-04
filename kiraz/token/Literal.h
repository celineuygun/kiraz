#ifndef KIRAZ_TOKEN_LITERAL_H
#define KIRAZ_TOKEN_LITERAL_H

#include <kiraz/Token.h>

namespace token {

class Integer : public Token {
public:
    Integer(int64_t base, std::string_view value) 
        : Token(L_INTEGER), m_base(base), m_value(value) {}
    virtual ~Integer() = default;

    std::string as_string() const override {return fmt::format("Integer({})\n", m_value);}
    void print() {fmt::print("{}\n", as_string());}

    static int colno;

    auto get_base() const {return m_base;}
    auto get_value() const {return m_value;}

private:
    int m_id;
    int64_t m_base;
    std::string m_value;
};

class String : public Token {
public:
    String(std::string_view value) 
        : Token(L_STRING), m_value(format_string(value)) {}
    virtual ~String() = default;

    std::string as_string() const override {
        std::string formatted_value = m_value;
        size_t pos = 0;
        while ((pos = formatted_value.find("\\n", pos)) != std::string::npos) {
            formatted_value.replace(pos, 2, "\n");
            pos += 1;
        }
        return fmt::format("String({})", formatted_value);
    }
    void print() {fmt::print("{}\n", as_string());}

    static int colno;

    auto get_value() const {return m_value;}

private:
    int m_id;
    std::string m_value;

    static std::string format_string(std::string_view input) {
        std::string result;

        if (!input.empty() && input.front() == '"' && input.back() == '"') {
            input = input.substr(1, input.size() - 2);
        }

        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] == '\\' && i + 1 < input.size()) {
                switch (input[i + 1]) {
                    case 'n': result += '\n'; ++i; break;
                    case 't': result += '\t'; ++i; break;
                    case '\\': result += '\\'; ++i; break;
                    case '"': result += '"'; ++i; break;
                    case 'r': result += '\r'; ++i; break;
                    case 'b': result += '\b'; ++i; break;
                    case 'f': result += '\f'; ++i; break;
                    case 'v': result += '\v'; ++i; break;
                    default: result += '\\'; break;
                }
            } else {
                result += input[i];
            }
        }
        return result;
    }
};

}

#endif // KIRAZ_TOKEN_LITERAL_H
