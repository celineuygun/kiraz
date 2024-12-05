#include "Statement.h"
#include <iostream>
#include <memory>

namespace ast {

    // Module
    Node::Ptr Module::compute_stmt_type(SymbolTable &st) {
        if(m_statements) {
            assert(m_statements->is_stmt_list());
            set_cur_symtab(st.get_cur_symtab());
            // st.print();

            auto current = std::dynamic_pointer_cast<StatementList>(m_statements);
            while (current) {
                if (current->get_first()) {
                    if (auto ret = current->get_first()->add_to_symtab_forward(st)) {
                        return ret;
                    }
                    if (auto ret = current->get_first()->add_to_symtab_ordered(*m_symtab)) {
                        return ret;
                    }
                    if (auto ret = current->get_first()->compute_stmt_type(st)) {
                        return ret;
                    }
                }
                current = std::dynamic_pointer_cast<StatementList>(current->get_next());
                // st.print();
            }
        }

        return nullptr;
    }

    Node::Ptr Module::add_to_symtab_forward(SymbolTable &st) {
        if (m_statements) {
            auto ret = m_statements->add_to_symtab_forward(st);
            return ret;
        }
        return nullptr;
    }

    Node::Ptr Module::add_to_symtab_ordered(SymbolTable &st) {
        if (m_statements) {
            auto ret = m_statements->add_to_symtab_ordered(st);
            return ret;
        }
        return nullptr;
    }

    // ImportStatement
    Node::Ptr ImportStatement::compute_stmt_type(SymbolTable &st) {
        return nullptr;
    }

    Node::Ptr ImportStatement::add_to_symtab_forward(SymbolTable &st) {
        return nullptr;
    }

    Node::Ptr ImportStatement::add_to_symtab_ordered(SymbolTable &st) {
        if(m_identifier) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_identifier);
            if (st.is_builtin(id->get_name())) {
                return set_error(FF("Identifier '{}' is a built-in type and cannot be used as an identifier", id->get_name()));
            }
            st.add_symbol(id->get_name(), shared_from_this());
        }
        return nullptr;
    }

    // CallStatement
    Node::Ptr CallStatement::compute_stmt_type(SymbolTable &st) {
        if(m_callee) {
            return m_callee->compute_stmt_type(st);
        }
        if(m_arguments) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_arguments);
            if(id) {
                if(!st.lookup(id->get_name())) {
                    return set_error(FF("Identifier '{}' is not found", id->get_name()));
                }
            }
            return m_arguments->compute_stmt_type(st);
        }
        return nullptr;
    }

    Node::Ptr CallStatement::add_to_symtab_forward(SymbolTable &st) {
        if(m_callee)
            m_callee->add_to_symtab_forward(st);
        return nullptr;
    }

    Node::Ptr CallStatement::add_to_symtab_ordered(SymbolTable &st) {
        if(m_callee)
            m_callee->add_to_symtab_ordered(st);
        if(m_arguments)
            m_arguments->add_to_symtab_ordered(st);
        return nullptr;
    }

    // ClassStatement
    Node::Ptr ClassStatement::compute_stmt_type(SymbolTable &st) {
        set_cur_symtab(st.get_cur_symtab());
        auto scope = st.enter_scope(ScopeType::Class, shared_from_this());
        if(m_stmts) {
            assert(m_stmts->is_stmt_list());

            auto current = std::dynamic_pointer_cast<StatementList>(m_stmts);
            while (current) {
                if (current->get_first()) {
                    if (auto ret = current->get_first()->add_to_symtab_forward(st)) {
                        return ret;
                    }
                    if (auto ret = current->get_first()->add_to_symtab_ordered(*m_symtab)) {
                        return ret;
                    }
                    if (auto ret = current->get_first()->compute_stmt_type(st)) {
                        return ret;
                    }
                }
                current = std::dynamic_pointer_cast<StatementList>(current->get_next());
            }
            // st.print();
        }
        return nullptr;
    }

    Node::Ptr ClassStatement::add_to_symtab_forward(SymbolTable &st) {
        if(m_name) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_name);
            if (st.is_builtin(id->get_name())) {
                return set_error(FF("Identifier '{}' is a built-in type and cannot be used as an identifier", id->get_name()));
            }
            if(st.lookup(id->get_name())) {
                return set_error(FF("Identifier '{}' is already in symtab", id->get_name()));
            }
            if(st.get_symbol(id->get_name()).first_letter_lowercase()) {
                return set_error(FF("Class name '{}' can not start with an lowercase letter", id->get_name()));
            }
            st.add_symbol(id->get_name(), shared_from_this());
        }

        if(m_parent) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_parent);
            if(id) {
                if(!st.lookup(id->get_name())) {
                    return set_error(FF("Type '{}' is not found", id->get_name()));
                } 
            }
        }

        return nullptr;
    }

    Node::Ptr ClassStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_stmts) {
            m_stmts->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    // ReturnStatement
    Node::Ptr ReturnStatement::compute_stmt_type(SymbolTable &st) {
        if(st.get_scope_type() != ScopeType::Func) {
            return  set_error(FF("Misplaced return statement"));
        }
        return m_expression->compute_stmt_type(st);
    }

    Node::Ptr ReturnStatement::add_to_symtab_forward(SymbolTable &st) {
        return nullptr;
    }

    Node::Ptr ReturnStatement::add_to_symtab_ordered(SymbolTable &st) {
        return nullptr;
    }

    // Parameter
    Node::Ptr Parameter::compute_stmt_type(SymbolTable &st) {
        if(m_name) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_name);
            auto parent_name = std::dynamic_pointer_cast<Identifier>(m_parent->get_name());
            if (id && parent_name) {
                if(st.lookup(id->get_name())) {
                     return set_error(FF("Identifier '{}' in argument list of function '{}' is already in symtab", id->get_name(), parent_name->get_name()));
                }
                if(m_type) {
                    auto type = std::dynamic_pointer_cast<Identifier>(m_type);
                    if(!BuiltinManager::get_builtin_type(type->get_name())){
                        return set_error(FF("Identifier '{}' in type of argument '{}' in function '{}' is not found", type->get_name(), id->get_name(), parent_name->get_name()));
                    }
                    return m_type->compute_stmt_type(st);
                } 
            }
        }
        return nullptr;
    }

    Node::Ptr Parameter::add_to_symtab_forward(SymbolTable &st) {
        if(m_name) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_name);
            if(id) {
                if (st.is_builtin(id->get_name())) {
                    return set_error(FF("Identifier '{}' is a built-in type and cannot be used as an identifier", id->get_name()));
                }
                // if(st.lookup(id->get_name())) {
                //     std::cout << "!!!" << std::endl;
                //     return set_error(FF("Identifier '{}' is already in symtab", id->get_name()));
                // }
            }
            m_name->add_to_symtab_forward(st);
        }
        return nullptr;
    }

    Node::Ptr Parameter::add_to_symtab_ordered(SymbolTable &st) {
        if(m_name) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_name);
            if(id) {
                if(m_parent) {
                    auto parent_name = std::dynamic_pointer_cast<Identifier>(m_parent->get_name());
                    if (st.is_builtin(id->get_name())) {
                        return set_error(FF("Identifier '{}' is a built-in type and cannot be used as an identifier", id->get_name()));
                    }
                }
                // if(st.lookup(id->get_name())) {
                //         std::cout << "!!!" << std::endl;
                //     return set_error(FF("Identifier '{}' is already in symtab", id->get_name()));
                // }
            }
            m_name->add_to_symtab_ordered(st);
        }
        if(m_type) {
            m_type->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    // ParameterList
    Node::Ptr ParameterList::compute_stmt_type(SymbolTable &st) {
        if (m_first) {
            Node::Ptr error = m_first->compute_stmt_type(st);
            if (error) return error;
            if (m_next) {
                return m_next->compute_stmt_type(st);
            }
        }
        return nullptr;
    }

    Node::Ptr ParameterList::add_to_symtab_forward(SymbolTable &st) {
        Node::Ptr error = nullptr;
        if (m_first) {
            error = m_first->add_to_symtab_forward(st);
        }
        if (m_next) {
            error = m_next->add_to_symtab_forward(st);
        }
        return error;
    }

    Node::Ptr ParameterList::add_to_symtab_ordered(SymbolTable &st) {
        Node::Ptr error = nullptr;
        if (m_first) {
            error = m_first->add_to_symtab_forward(st);
        }
        if (m_next) {
            error = m_next->add_to_symtab_forward(st);
        }
        return error;
    }

    // StatementList
    Node::Ptr StatementList::compute_stmt_type(SymbolTable &st) {
        if (m_first) {
            Node::Ptr error = m_first->compute_stmt_type(st);
            if (error) return error;
            if (m_next) {
                return m_next->compute_stmt_type(st);
            }
        }
        return nullptr;
    }

    Node::Ptr StatementList::add_to_symtab_forward(SymbolTable &st) {
        if (m_first) {
            m_first->add_to_symtab_forward(st);
        }
        if (m_next) {
            m_next->add_to_symtab_forward(st);
        }
        return nullptr;
    }

    Node::Ptr StatementList::add_to_symtab_ordered(SymbolTable &st) {
        if (m_first) {
            m_first->add_to_symtab_ordered(st);
        }
        if (m_next) {
            m_next->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    // FunctionStatement
    Node::Ptr FunctionStatement::compute_stmt_type(SymbolTable &st) {
        set_cur_symtab(st.get_cur_symtab());
        auto scope = st.enter_scope(ScopeType::Func, shared_from_this());

        Node::Ptr error;
        if (m_returnType) {
            auto type = std::dynamic_pointer_cast<Identifier>(m_returnType);
            auto name = std::dynamic_pointer_cast<Identifier>(m_name);
            if (!BuiltinManager::get_builtin_type(type->get_name())) {
                return set_error(FF("Return type '{}' of function '{}' is not found", type->get_name(), name->get_name()));
            }
            error = m_returnType->compute_stmt_type(st);
            if (error) return error;
        }

        if (m_parameters) {
            error = m_parameters->compute_stmt_type(st);
            if (error) return error;
        }
        if (m_body) {
            assert(m_body->is_stmt_list());

            auto current = std::dynamic_pointer_cast<StatementList>(m_body);
            while (current) {
                if (current->get_first()) {
                    auto id = std::dynamic_pointer_cast<Identifier>(current->get_first());
                    if(id) {
                        if(!st.lookup(id->get_name())) {
                            return set_error(FF("Identifier '{}' is not found", id->get_name()));
                        }
                    }
                    if (auto ret = current->get_first()->add_to_symtab_forward(st)) {
                        return ret;
                    }
                    if (auto ret = current->get_first()->add_to_symtab_ordered(*m_symtab)) {
                        return ret;
                    }
                    if (auto ret = current->get_first()->compute_stmt_type(st)) {
                        return ret;
                    }
                }
                current = std::dynamic_pointer_cast<StatementList>(current->get_next());
            }
        }
        // st.print();

        return nullptr;
    }


    Node::Ptr FunctionStatement::add_to_symtab_forward(SymbolTable &st) {
        if(m_name) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_name);
            if (st.is_builtin(id->get_name())) {
                return set_error(FF("Identifier '{}' is a built-in type and cannot be used as an identifier", id->get_name()));
            }
            if(st.lookup(id->get_name())) {
                return set_error(FF("Identifier '{}' is already in symtab", id->get_name()));
            }
            st.add_symbol(id->get_name(), shared_from_this());
        }
        return nullptr;
    }

    Node::Ptr FunctionStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_parameters) {
            auto error = m_parameters->add_to_symtab_ordered(st);
            if (error) {
                return error; 
            }
        }
        if (m_body) {
            m_body->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    Node::Ptr LetStatement::compute_stmt_type(SymbolTable &st) {
        if (m_value) {
            return m_value->compute_stmt_type(st);
        }
        return nullptr;
    }

    Node::Ptr LetStatement::add_to_symtab_forward(SymbolTable &st) {
        if(m_identifier) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_identifier);
            if (st.is_builtin(id->get_name())) {
                return set_error(FF("Identifier '{}' is a built-in type and cannot be used as an identifier", id->get_name()));
            }
            if(st.lookup(id->get_name())) {
                return set_error(FF("Identifier '{}' is already in symtab", id->get_name()));
            }
            if(st.get_symbol(id->get_name()).first_letter_uppercase()) {
                return set_error(FF("Variable name '{}' can not start with an uppercase letter", id->get_name()));
            }
            st.add_symbol(id->get_name(), shared_from_this());
        }
        return nullptr;
    }

    Node::Ptr LetStatement::add_to_symtab_ordered(SymbolTable &st) {
        if(m_value) {
            m_value->add_to_symtab_ordered(st);
        }
        if(m_type) {
            m_type->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    Node::Ptr AssignmentStatement::compute_stmt_type(SymbolTable &st) {
        if (m_left) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_left);
            if (st.is_builtin(id->get_name())) {
                return set_error(fmt::format("Overriding builtin '{}' is not allowed", id->get_name()));
            }
            return m_left->compute_stmt_type(st);
        }
        if (m_right) {
            Node::Ptr rightError = m_right->compute_stmt_type(st);
            if (rightError) {
                return rightError;
            }
        }
        return nullptr;
    }

    Node::Ptr AssignmentStatement::add_to_symtab_forward(SymbolTable &st) {
        if (m_left) {
            return m_left->add_to_symtab_forward(st);
        }
        return nullptr;
    }

    Node::Ptr AssignmentStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_left) {
            m_left->add_to_symtab_ordered(st);
        }
        if (m_right) {
            m_right->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    Node::Ptr IfStatement::compute_stmt_type(SymbolTable &st) {
        Node::Ptr error = m_condition->compute_stmt_type(st);
        if (error) return error;

        if (m_thenBranch) {
            error = m_thenBranch->compute_stmt_type(st);
            if (error) return error;
        }

        if (m_elseBranch) {
            error = m_elseBranch->compute_stmt_type(st);
            if (error) return error;
        }

        return nullptr;
    }

    Node::Ptr IfStatement::add_to_symtab_forward(SymbolTable &st) {
        return nullptr;
    }

    Node::Ptr IfStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_thenBranch) {
            m_thenBranch->add_to_symtab_ordered(st);
        }
        if (m_elseBranch) {
            m_elseBranch->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    Node::Ptr WhileStatement::compute_stmt_type(SymbolTable &st) {
        Node::Ptr error = m_condition->compute_stmt_type(st);
        if (error) return error;

        if (m_repeatBranch) {
            error = m_repeatBranch->compute_stmt_type(st);
            if (error) return error;
        }

        return nullptr;
    }

    Node::Ptr WhileStatement::add_to_symtab_forward(SymbolTable &st) {
        std::cout << m_condition->as_string() << std::endl;
         auto conditionType = m_condition->get_stmt_type();
        if (conditionType != BuiltinManager::Boolean) {
                return set_error(FF("While only accepts tests of type 'Boolean'"));
            }
        return nullptr;
    }

    Node::Ptr WhileStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_repeatBranch) {
            m_repeatBranch->add_to_symtab_ordered(st);
        }
        return nullptr;
    }
}