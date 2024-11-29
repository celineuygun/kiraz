#include "Statement.h"
#include <iostream>

namespace ast {

    // Module
    Node::Ptr Module::compute_stmt_type(SymbolTable &st) {
        std::cout << "Module::compute_stmt_type" << std::endl;
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
        std::cout << "Module::add_to_symtab_forward" << std::endl;
        if (m_statements) {
            return m_statements->add_to_symtab_forward(st);
        }
        return nullptr;
    }

    Node::Ptr Module::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "Module::add_to_symtab_ordered" << std::endl;
        if (m_statements) {
            return m_statements->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    // ImportStatement
    Node::Ptr ImportStatement::compute_stmt_type(SymbolTable &st) {
        std::cout << "ImportStatement::compute_stmt_type" << std::endl;
        return nullptr;
    }

    Node::Ptr ImportStatement::add_to_symtab_forward(SymbolTable &st) {
        std::cout << "ImportStatement::add_to_symtab_forward" << std::endl;
        return nullptr;
    }

    Node::Ptr ImportStatement::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "ImportStatement::add_to_symtab_ordered: " << m_identifier->as_string() << std::endl;
        st.add_symbol(m_identifier->as_string(), shared_from_this());
        return nullptr;
    }

    // CallStatement
    Node::Ptr CallStatement::compute_stmt_type(SymbolTable &st) {
        std::cout << "CallStatement::compute_stmt_type" << std::endl;
        Node::Ptr calleeError = m_callee->compute_stmt_type(st);
        if (calleeError) return calleeError;
        Node::Ptr argumentsError = m_arguments->compute_stmt_type(st);
        if (argumentsError) return argumentsError;
        return nullptr;
    }

    Node::Ptr CallStatement::add_to_symtab_forward(SymbolTable &st) {
        std::cout << "CallStatement::add_to_symtab_forward" << std::endl;
        m_callee->add_to_symtab_forward(st);
        return nullptr;
    }

    Node::Ptr CallStatement::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "CallStatement::add_to_symtab_ordered" << std::endl;
        m_callee->add_to_symtab_ordered(st);
        m_arguments->add_to_symtab_ordered(st);
        return nullptr;
    }

    // ClassStatement
    Node::Ptr ClassStatement::compute_stmt_type(SymbolTable &st) {
        std::cout << "ClassStatement::compute_stmt_type" << std::endl;

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
        std::cout << "ClassStatement::add_to_symtab_forward: " << m_name->as_string() << std::endl;
        if(m_name) {
            st.add_symbol(m_name->as_string(), shared_from_this());
        }
        return nullptr;
    }

    Node::Ptr ClassStatement::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "ClassStatement::add_to_symtab_ordered" << std::endl;
        if (m_stmts) {
            m_stmts->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    // ReturnStatement
    Node::Ptr ReturnStatement::compute_stmt_type(SymbolTable &st) {
        std::cout << "ReturnStatement::compute_stmt_type" << std::endl;
        return m_expression->compute_stmt_type(st);
    }

    Node::Ptr ReturnStatement::add_to_symtab_forward(SymbolTable &st) {
        std::cout << "ReturnStatement::add_to_symtab_forward" << std::endl;
        return nullptr;
    }

    Node::Ptr ReturnStatement::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "ReturnStatement::add_to_symtab_ordered" << std::endl;
        return nullptr;
    }

    // Parameter
    Node::Ptr Parameter::compute_stmt_type(SymbolTable &st) {
        std::cout << "Parameter::compute_stmt_type" << std::endl;
        if(m_type) {
            return m_type->compute_stmt_type(st);
        } return nullptr;
    }

    Node::Ptr Parameter::add_to_symtab_forward(SymbolTable &st) {
        std::cout << "Parameter::add_to_symtab_forward: " << m_name->as_string() << std::endl;
        if(m_name) {
            st.add_symbol(m_name->as_string(), shared_from_this());
        }
        return nullptr;
    }

    Node::Ptr Parameter::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "Parameter::add_to_symtab_ordered" << std::endl;
        if(m_name) {
            m_name->add_to_symtab_ordered(st);

        }
        if(m_type) {
            m_type->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    // ParameterList
    Node::Ptr ParameterList::compute_stmt_type(SymbolTable &st) {
        std::cout << "ParameterList::compute_stmt_type" << std::endl;
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
        std::cout << "ParameterList::add_to_symtab_forward" << std::endl;
        if (m_first) {
            m_first->add_to_symtab_forward(st);
        }
        if (m_next) {
            m_next->add_to_symtab_forward(st);
        }
        return nullptr;
    }

    Node::Ptr ParameterList::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "ParameterList::add_to_symtab_ordered" << std::endl;
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
        std::cout << "StatementList::compute_stmt_type" << std::endl;
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
        std::cout << "StatementList::add_to_symtab_forward" << std::endl;
        if (m_first) {
            m_first->add_to_symtab_forward(st);
        }
        if (m_next) {
            m_next->add_to_symtab_forward(st);
        }
        return nullptr;
    }

    Node::Ptr StatementList::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "StatementList::add_to_symtab_ordered" << std::endl;
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
        std::cout << "FunctionStatement::compute_stmt_type" << std::endl;

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
        std::cout << "FunctionStatement::add_to_symtab_forward: " << m_name->as_string() << std::endl;
        if(m_name) {
            st.add_symbol(m_name->as_string(), shared_from_this());
        }
        return nullptr;
    }

    Node::Ptr FunctionStatement::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "FunctionStatement::add_to_symtab_ordered: " << m_name->as_string() << std::endl;
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
            std::cout << "LetStatement::compute_stmt_type" << std::endl;
            return m_value->compute_stmt_type(st);
        }
        return nullptr;
    }

    Node::Ptr LetStatement::add_to_symtab_forward(SymbolTable &st) {
        std::cout << "LetStatement::add_to_symtab_forward: " << m_identifier->as_string() << std::endl;
        if(m_identifier)
            st.add_symbol(m_identifier->as_string(), shared_from_this());
        return nullptr;
    }

    Node::Ptr LetStatement::add_to_symtab_ordered(SymbolTable &st) {
        std::cout << "LetStatement::add_to_symtab_ordered: " << m_identifier->as_string() << std::endl;
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
            std::cout << "AssignmentStatement::add_to_symtab_forward (left)" << std::endl;
            return m_left->add_to_symtab_forward(st);
        }
        return nullptr;
    }

    Node::Ptr AssignmentStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_left) {
            std::cout << "AssignmentStatement::add_to_symtab_ordered (left)" << std::endl;
            m_left->add_to_symtab_ordered(st);
        }
        if (m_right) {
            std::cout << "AssignmentStatement::add_to_symtab_ordered (right)" << std::endl;
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
            std::cout << "IfStatement::add_to_symtab_ordered (thenBranch)" << std::endl;
            m_thenBranch->add_to_symtab_ordered(st);
        }
        if (m_elseBranch) {
            std::cout << "IfStatement::add_to_symtab_ordered (elseBranch)" << std::endl;
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
            std::cout << "WhileStatement::add_to_symtab_ordered (repeatBranch)" << std::endl;
            m_repeatBranch->add_to_symtab_ordered(st);
        }
        return nullptr;
    }
}
