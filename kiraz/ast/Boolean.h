#ifndef KIRAZ_AST_BOOLEAN_H
#define KIRAZ_AST_BOOLEAN_H

#include <kiraz/Node.h>
#include "BuiltinTypes.h"
#include <string>
namespace ast {

class Boolean : public Node {
public:
    Boolean(bool value) 
        : m_value(value) {set_stmt_type(BuiltinManager::Boolean);}
    virtual ~Boolean() = default;

    auto get_value() const { return m_value; }

    std::string as_string() const override { 
        return fmt::format("{}", get_value()); 
    }
    void print() {fmt::print("{}\n", as_string());}

private:
    bool m_value;
};

}

#endif // KIRAZ_AST_BOOLEAN_H
