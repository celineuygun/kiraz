#include "Statement.h"
#include <iostream>
#include <memory>

namespace ast {

    // Module
    Node::Ptr Module::compute_stmt_type(SymbolTable &st) {
        std::cout << "stmts: " << m_statements->as_string() << std::endl;
        
        if(m_statements) {
            assert(m_statements->is_stmt_list());
            set_cur_symtab(st.get_cur_symtab());
            st.print();

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
                st.print();
            }
        }

        return nullptr;
    }


    Node::Ptr Module::add_to_symtab_forward(SymbolTable &st) {
        if (m_statements) {
            return m_statements->add_to_symtab_forward(st);
        }
        return nullptr;
    }

    Node::Ptr Module::add_to_symtab_ordered(SymbolTable &st) {
        if (m_statements) {
            return m_statements->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    // ImportStatement
    Node::Ptr ImportStatement::compute_stmt_type(SymbolTable &st) {
        return nullptr;
    }

    Node::Ptr ImportStatement::add_to_symtab_ordered(SymbolTable &st) {
        if(m_identifier) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_identifier);
            if (BuiltinManager::get_builtin_type(id->get_name())) {
                std::cerr << "** Error: Identifier " << id->get_name() << " is a built-in type and cannot be used as an identifier!" << std::endl;
                return nullptr;
            }
            st.add_symbol(id->get_name(), shared_from_this());
        }
        return nullptr;
    }

    // CallStatement
    Node::Ptr CallStatement::compute_stmt_type(SymbolTable &st) {
        if(m_callee) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_callee);
            if (id) {
                const std::string &func_name = id->get_name();
                if (!st.lookup(func_name)) {
                    Compiler::current()->set_error("Identifier '" + func_name + "' is not found");
                    return nullptr;
                }
            } else {
                throw std::runtime_error("CallStatement callee is not an Identifier.");
            }
        }
        if(m_arguments) {
            Node::Ptr argumentsError = m_arguments->compute_stmt_type(st);
            if (argumentsError) return argumentsError;
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
        if(m_stmts) {
            assert(m_stmts->is_stmt_list());
            set_cur_symtab(st.get_cur_symtab());
            auto scope = st.enter_scope(ScopeType::Class, shared_from_this());

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
            st.print();
        }
        return nullptr;
    }


    Node::Ptr ClassStatement::add_to_symtab_forward(SymbolTable &st) {
        if(m_name) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_name);
            if (BuiltinManager::get_builtin_type(id->get_name())) {
                std::cerr << "** Error: Identifier " << id->get_name() << " is a built-in type and cannot be used as an identifier!" << std::endl;
                return nullptr;
            }
            if(st.lookup(id->get_name())) {
                throw std::runtime_error(fmt::format("Identifier '{}' is already in symtab", id->get_name()));
                return nullptr;
            }
            st.add_symbol(id->get_name(), shared_from_this());
        }


        m_symtab = std::make_unique<SymbolTable>(ScopeType::Class);

        if (m_parent) {
            // TODO
            // auto parentSymbol = st.lookup(m_parent->as_string());
            // if (!parentSymbol || !parentSymbol->is_class()) {
            //     throw std::runtime_error(fmt::format("Parent class '{}' is not defined.", m_parent->as_string()));
            // }
            // auto parentClass = dynamic_cast<ClassStatement *>(parentSymbol.get());
            // assert(parentClass);

            // auto parentSymTab = parentClass->get_cur_symtab();
            // if (parentSymTab) {
            //     for (const auto &symbol : parentSymTab->symbols) {
            //         if (m_symtab->lookup(symbol.first)) {
            //             std::cerr << "** Warning: Member '" << symbol.first 
            //                     << "' in class '" << m_name->as_string() 
            //                     << "' overrides parent class member." << std::endl;
            //         }
            //         m_symtab->add_symbol(symbol.first, symbol.second);
            //     }
            // }
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
        if (m_expression)
            return m_expression->compute_stmt_type(st);
        return nullptr;
    }

    // Parameter
    Node::Ptr Parameter::compute_stmt_type(SymbolTable &st) {
        if(m_type) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_type);
            if(id) {
                if(m_parent) {
                    auto parent_name = std::dynamic_pointer_cast<Identifier>(m_parent->get_name());
                    if(!BuiltinManager::get_builtin_type(id->get_name()) && !st.lookup(id->get_name())) {
                        throw std::runtime_error(fmt::format("Identifier '{}' in function '{}' is not found", id->get_name(), parent_name->get_name()));
                        return nullptr;
                    }
                }
            }
            return m_type->compute_stmt_type(st);
        } return nullptr;
    }

    Node::Ptr Parameter::add_to_symtab_ordered(SymbolTable &st) {
        if(m_name) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_name);
            if(id) {
                if(m_parent) {
                    auto parent_name = std::dynamic_pointer_cast<Identifier>(m_parent->get_name());
                    std::cout << parent_name->as_string() << std::endl;
                    if (BuiltinManager::get_builtin_type(id->get_name())) {
                        std::cerr << "** Error: Identifier " << id->get_name() << " is a built-in type and cannot be used as an identifier!" << std::endl;
                        return nullptr;
                    }
                    if(st.lookup(id->get_name())) {
                        throw std::runtime_error(fmt::format("Identifier '{}' in argument list of function '{}' is already in symtab", id->get_name(), parent_name->get_name()));
                        return nullptr;
                    }
                }
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

    Node::Ptr ParameterList::add_to_symtab_ordered(SymbolTable &st) {
        if (m_first) {
            m_first->add_to_symtab_ordered(st);
        }
        if (m_next) {
            m_next->add_to_symtab_ordered(st);
        }
        return nullptr;
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

        Node::Ptr error;
        if (m_returnType) {
            error = m_returnType->compute_stmt_type(st);
            if (error) return error;
        }

        if (m_parameters) {
            error = m_parameters->compute_stmt_type(st);
            if (error) return error;
        }
        if (m_body) {
            assert(m_body->is_stmt_list());
            set_cur_symtab(st.get_cur_symtab());
            auto scope = st.enter_scope(ScopeType::Func, shared_from_this());

            auto current = std::dynamic_pointer_cast<StatementList>(m_body);
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
            st.print();
        }

        return nullptr;
    }


    Node::Ptr FunctionStatement::add_to_symtab_forward(SymbolTable &st) {
        if(m_name) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_name);
            if (BuiltinManager::get_builtin_type(id->get_name())) {
                std::cerr << "** Error: Identifier " << id->get_name() << " is a built-in type and cannot be used as an identifier!" << std::endl;
                return nullptr;
            }
            if(st.lookup(id->get_name())) {
                throw std::runtime_error(fmt::format("Identifier '{}' is already in symtab", id->get_name()));
                return nullptr;
            }
            st.add_symbol(id->get_name(), shared_from_this());
        }
        if (m_parameters) {
            auto func_name = std::dynamic_pointer_cast<Identifier>(m_name)->get_name();
            auto param_list = std::dynamic_pointer_cast<ParameterList>(m_parameters);

            if (param_list) { // FIXME
                // auto all_params = param_list->get_all_parameters();
                // for (const auto &param : all_params) {
                //     std::cout << "*" << std::endl;
                //     auto param_id = std::dynamic_pointer_cast<Identifier>(param->get_name());
                //     if (param_id) {
                //         if(st.lookup(param_id->get_name()))
                //             throw std::runtime_error(fmt::format("Parameter name '{}' conflicts with the function name", func_name));
                //     }
                // }
            }
        }
        if (m_returnType) {
            auto return_type_id = std::dynamic_pointer_cast<Identifier>(m_returnType);
            if (return_type_id) {
                auto return_type_name = return_type_id->get_name();

                if (!BuiltinManager::get_builtin_type(return_type_name) && !st.lookup(return_type_name)) {
                    throw std::runtime_error(fmt::format(
                        "Return type '{}' of function '{}' is not found",
                        return_type_name, 
                        std::dynamic_pointer_cast<Identifier>(m_name)->get_name()
                    ));
                }
            }
        }
        return nullptr;
    }

    Node::Ptr FunctionStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_parameters) {
            m_parameters->add_to_symtab_ordered(st);
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
            if (BuiltinManager::get_builtin_type(id->get_name())) {
                std::cerr << "** Error: Identifier " << id->get_name() << " is a built-in type and cannot be used as an identifier!" << std::endl;
                return nullptr;
            }
            if(st.lookup(id->get_name())) {
                throw std::runtime_error(fmt::format("Identifier '{}' is already in symtab", id->get_name()));
                return nullptr;
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
            Node::Ptr leftError = m_left->compute_stmt_type(st);
            if (leftError) {
                return leftError;
            }
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
        return nullptr;
    }

    Node::Ptr WhileStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_repeatBranch) {
            m_repeatBranch->add_to_symtab_ordered(st);
        }
        return nullptr;
    }
}
