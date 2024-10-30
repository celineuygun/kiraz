#ifndef KIRAZ_TOKEN_KEYWORD_H
#define KIRAZ_TOKEN_KEYWORD_H

#include <kiraz/Token.h>

namespace token {
class Keyword : public Token {
public:
    Keyword(int id) : Token(id), m_id(id) {}
    virtual ~Keyword() = default;

    std::string as_string() const override { return fmt::format("Keyword{}", m_id); }
private:
    int m_id;
};

class KeywordImport : public Keyword {
public:
    KeywordImport() : Keyword(KW_IMPORT) {}
    std::string as_string() const override { return "KW_IMPORT"; }
};

class KeywordFunc : public Keyword {
public:
    KeywordFunc() : Keyword(KW_FUNC) {}
    std::string as_string() const override { return "KW_FUNC"; }
};

class KeywordIf : public Keyword {
public:
    KeywordIf() : Keyword(KW_IF) {}
    std::string as_string() const override { return "KW_IF"; }
};

class KeywordWhile : public Keyword {
public:
    KeywordWhile() : Keyword(KW_WHILE) {}
    std::string as_string() const override { return "KW_WHILE"; }
};

class KeywordClass : public Keyword {
public:
    KeywordClass() : Keyword(KW_CLASS) {}
    std::string as_string() const override { return "KW_CLASS"; }
};

}

#endif // KIRAZ_TOKEN_KEYWORD_H 