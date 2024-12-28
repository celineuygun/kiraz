#include "Statement.h"
#include "Literal.h"
#include <iostream>
#include <fstream>
#include <memory>

namespace ast {

    // Module
    Node::Ptr Module::compute_stmt_type(SymbolTable &st) {
        if(m_statements) {
            assert(m_statements->is_stmt_list());
            set_cur_symtab(st.get_cur_symtab());

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

    Node::Ptr Module::gen_wat(WasmContext &ctx) {
        ctx.body() << "(module\n";
        
        if (m_statements) {
            auto current = std::dynamic_pointer_cast<StatementList>(m_statements);
            while (current) {
                if (auto stmt = current->get_first()) {
                    stmt->gen_wat(ctx);
                }
                current = std::dynamic_pointer_cast<StatementList>(current->get_next());
            }
        }

        ctx.body() << ")\n";
        return nullptr;
    }


    // ImportStatement
    Node::Ptr ImportStatement::compute_stmt_type(SymbolTable &st) {
        if (m_identifier) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_identifier);
            if (!id) {
                return set_error("Import statement is missing an identifier");
            }
        }

        return nullptr;
    }

    Node::Ptr ImportStatement::add_to_symtab_forward(SymbolTable &st) {
        return nullptr;
    }

    void ImportStatement::integrate_module_symbols(Node::Ptr moduleAST, SymbolTable &st) {
        if (!moduleAST) {
            return;
        }
        auto module = std::dynamic_pointer_cast<Module>(moduleAST);
        auto stmts = module->get_statements();
        if(!stmts) {
            return;
        }
        auto stmtList = std::dynamic_pointer_cast<StatementList>(stmts);
        if(!stmtList) return;
        auto current = stmtList;
        while (current) {
            if (current->get_first()) {
                auto first = current->get_first();

                if (auto ret = first->add_to_symtab_ordered(st)) {
                    std::cerr << "Error in add_to_symtab_ordered: " << ret->as_string() << std::endl;
                    return;
                }
            }
            current = std::dynamic_pointer_cast<StatementList>(current->get_next());
        }
    }

    Node::Ptr ImportStatement::add_to_symtab_ordered(SymbolTable &st) {
        if(m_identifier) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_identifier);
            if (!id) {
                return set_error(FF("Import statement is missing an identifier"));
            }
            if (st.is_builtin(id->get_name())) {
                return set_error(FF("Identifier '{}' is a built-in type and cannot be used as an identifier", id->get_name()));
            }

            std::string moduleName = id->get_name();

            if (moduleName == "io") {
                auto ioModule = st.get_module_io();
                if (!ioModule) {
                    return set_error("Precompiled module 'io' is unavailable");
                }

                integrate_module_symbols(ioModule, st);
                
            } else {
                std::string filePath = resolve_module_path(moduleName);
                if (filePath.empty()) {
                    return set_error(FF("Module '{}' not found", moduleName));
                }

                std::ifstream file(filePath);
                if (!file.is_open()) {
                    return set_error(fmt::format("Unable to open file '{}'", filePath));
                }

                std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                file.close();

                auto moduleAST = Compiler::current()->compile_module(fileContent);
                if (!moduleAST) {
                    return set_error(fmt::format("Failed to parse module '{}'", moduleName));
                }

                integrate_module_symbols(moduleAST, st);
            }
        }
        return nullptr;
    }

    // CallStatement
    Node::Ptr CallStatement::compute_stmt_type(SymbolTable &st) {
        if(m_callee) {
            auto callee_id = std::dynamic_pointer_cast<Identifier>(m_callee);
            if (callee_id) {
                auto callee_symbol = st.lookup(callee_id->get_name());
                if (!callee_symbol) {
                    // return set_error(fmt::format("Variable '{}' is not defined.", callee_id->get_name()));
                    return nullptr;
                }

                auto class_stmt = std::dynamic_pointer_cast<ClassStatement>(callee_symbol);
                if (!class_stmt) {
                    return set_error(fmt::format("Variable '{}' is not a class instance.", callee_id->get_name()));
                }

                if (!class_stmt->get_symtab()) {
                    return set_error(fmt::format("Class '{}' has no symbol table.", callee_id->get_name()));
                }

                auto sub_id = std::dynamic_pointer_cast<Identifier>(m_arguments);
                if (sub_id) {
                    auto sub_symbol = class_stmt->get_symtab()->lookup(sub_id->get_name());
                    if (!sub_symbol) {
                        return set_error(fmt::format("Identifier '{}' is not found", sub_id->get_name()));
                    }
                }

            }
            return nullptr;
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
        
        std::shared_ptr<SymbolTable> parentSymTab = nullptr;
        auto parentClassId = std::dynamic_pointer_cast<Identifier>(m_parent_class);
        if (parentClassId) {
            auto parentSymbol = st.lookup(parentClassId->get_name());
            if (!parentSymbol) {
                return set_error(fmt::format("Parent class '{}' is not defined", parentClassId->get_name()));
            }

            auto parentClass = std::dynamic_pointer_cast<ClassStatement>(parentSymbol);
            if (!parentClass) {
                return set_error(fmt::format("'{}' is not a valid class", parentClassId->get_name()));
            }

            parentSymTab = std::shared_ptr<SymbolTable>(parentClass->get_symtab(), [](SymbolTable*) {});
        }
    
        if (m_stmts) {
            auto current = std::dynamic_pointer_cast<StatementList>(m_stmts);
            while (current) {
                if (current->get_first()) {
                    auto member = current->get_first();

                    // Check if it's a LetStatement (variable declaration)
                    auto letStmt = std::dynamic_pointer_cast<LetStatement>(member);
                    if (letStmt && letStmt->get_identifier()) {
                        auto id = std::dynamic_pointer_cast<Identifier>(letStmt->get_identifier());
                        if (id) {
                            if (parentSymTab && parentSymTab->lookup(id->get_name())) {
                                return set_error(fmt::format(
                                    "Identifier '{}' is already in symtab",
                                    id->get_name()
                                ));
                            }
                        }
                    }

                    // Check if it's a FunctionStatement (method declaration)
                    auto funcStmt = std::dynamic_pointer_cast<FunctionStatement>(member);
                    if (funcStmt && funcStmt->get_name()) {
                        auto id = std::dynamic_pointer_cast<Identifier>(funcStmt->get_name());
                        if (id) {
                            if (parentSymTab && parentSymTab->lookup(id->get_name())) {
                                return set_error(fmt::format(
                                    "Identifier '{}' is already in symtab",
                                    id->get_name()
                                ));
                            }
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

        if(m_parent_class) {
            auto id = std::dynamic_pointer_cast<Identifier>(m_parent_class);
            if(id) {
                if(!st.lookup(id->get_name())) {
                    return set_error(FF("Type '{}' is not found", id->get_name()));
                } 
                
            }
        }
        if (m_stmts) {
            auto current = std::dynamic_pointer_cast<StatementList>(m_stmts);
            while (current) {
                if (current->get_first()) {
                    auto ret = current->get_first()->add_to_symtab_ordered(*m_symtab);
                    if (ret) {
                        return ret;
                    }
                }
                current = std::dynamic_pointer_cast<StatementList>(current->get_next());
            }
        }
        // st.print();
        return nullptr;
    }

Node::Ptr ClassStatement::add_to_symtab_ordered(SymbolTable &st) {
    if (!m_symtab) {
        m_symtab = std::make_unique<SymbolTable>(ScopeType::Class);
    }

    if (m_stmts) {
        auto current = std::dynamic_pointer_cast<StatementList>(m_stmts);
        while (current) {
            if (current->get_first()) {
                auto member = current->get_first();
                
                // Check if it's a LetStatement (variable declaration)
                auto letStmt = std::dynamic_pointer_cast<LetStatement>(member);
                if (letStmt && letStmt->get_identifier()) {
                    auto id = std::dynamic_pointer_cast<Identifier>(letStmt->get_identifier());
                    if (id) {
                        // if(m_symtab->lookup(id->get_name())) {
                        //     return set_error(FF("Identifier '{}' is already in symtab", id->get_name()));
                        // }
                        m_symtab->add_symbol(id->get_name(), member);
                    }
                }

                // Check if it's a FunctionStatement (method declaration)
                auto funcStmt = std::dynamic_pointer_cast<FunctionStatement>(member);
                if (funcStmt && funcStmt->get_name()) {
                    auto id = std::dynamic_pointer_cast<Identifier>(funcStmt->get_name());
                    if (id) {
                        m_symtab->add_symbol(id->get_name(), member);
                    }
                }
            }
            current = std::dynamic_pointer_cast<StatementList>(current->get_next());
        }
    }
    // m_symtab->print();

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
            auto stmt_name = std::dynamic_pointer_cast<Identifier>(m_stmt->get_name());
            if (id && stmt_name) {
                if(m_type) {
                    auto type = std::dynamic_pointer_cast<Identifier>(m_type);
                    if(!BuiltinManager::get_builtin_type(type->get_name())){
                        return set_error(FF("Identifier '{}' in type of argument '{}' in function '{}' is not found", type->get_name(), id->get_name(), stmt_name->get_name()));
                    }
                    return m_type->compute_stmt_type(st);
                } 
                if(st.lookup(id->get_name())) {
                     return set_error(FF("Identifier '{}' in argument list of function '{}' is already in symtab", id->get_name(), stmt_name->get_name()));
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
            }
            m_name->add_to_symtab_forward(st);
        }
        return nullptr;
    }

Node::Ptr Parameter::add_to_symtab_ordered(SymbolTable &st) {
    if (m_name) {
        // Check if m_name is a String
        auto stringNode = std::dynamic_pointer_cast<String>(m_name);
        if (stringNode) {
            return set_error(fmt::format(
                "Parameter '{}' cannot be a String literal in argument list", 
                stringNode->as_string()
            ));
        }

        // Check if m_name is an Identifier
        auto id = std::dynamic_pointer_cast<Identifier>(m_name);
        if (id) {
            if (m_stmt) {
                auto stmt_name = std::dynamic_pointer_cast<Identifier>(m_stmt->get_name());
                if (id->get_name() == stmt_name->get_name()) {
                    return set_error(fmt::format(
                        "Identifier '{}' in argument list of function '{}' is already in symtab",
                        id->get_name(), stmt_name->get_name()
                    ));
                }
                if (st.is_builtin(id->get_name())) {
                    return set_error(FF("Identifier '{}' is a built-in type and cannot be used as an identifier", id->get_name()));
                }
                if (st.lookup(id->get_name())) {
                    return set_error(FF("Identifier '{}' in argument list of function '{}' is already in symtab", id->get_name(), stmt_name->get_name()));
                }
            }
            st.add_symbol(id->get_name(), shared_from_this());
        }
    }

    if (m_type) {
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
            error = m_first->add_to_symtab_ordered(st);
        }
        if (m_next) {
            error = m_next->add_to_symtab_ordered(st);
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
            error = m_parameters->compute_stmt_type(*m_symtab);
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
        if (!m_symtab) {
            m_symtab = std::make_unique<SymbolTable>(ScopeType::Func);
        }
        if (m_parameters) {
            auto error = m_parameters->add_to_symtab_ordered(*m_symtab);
            if (error) {
                return error; 
            }
        }
        if (m_body) {
            auto current = std::dynamic_pointer_cast<StatementList>(m_body);
            while (current) {
                if (current->get_first()) {
                    auto ret = current->get_first()->add_to_symtab_ordered(*m_symtab);
                    if (ret) return ret;
                }
                current = std::dynamic_pointer_cast<StatementList>(current->get_next());
            }
        }
        return nullptr;
    }

Node::Ptr FunctionStatement::gen_wat(WasmContext &ctx) {
    auto nameID = std::dynamic_pointer_cast<Identifier>(m_name);
    ctx.body() << "(func $" << nameID->get_name() << "\n";

    if (m_parameters) {
        auto current = std::dynamic_pointer_cast<ParameterList>(m_parameters);
        while (current) {
            if (auto param = current->get_first()) {
                auto p = std::dynamic_pointer_cast<Parameter>(param);
                auto id = std::dynamic_pointer_cast<Identifier>(p->get_name());
                if (id) {
                    ctx.body() << "  (param $" << id->get_name() << " i64)\n";
                }
            }
            current = std::dynamic_pointer_cast<ParameterList>(current->get_next());
        }
    }

    if (m_returnType) {
        ctx.body() << "  (result i64)\n";
    }


    if (m_body) {
        auto current = std::dynamic_pointer_cast<StatementList>(m_body);
        while (current) {
            if (auto stmt = current->get_first()) {
                stmt->gen_wat(ctx);
            }
            current = std::dynamic_pointer_cast<StatementList>(current->get_next());
        }
    }

    ctx.body() << ctx.locals().str();
    ctx.locals().str("");
    ctx.body() << ")\n";
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
            if(id) {
                if (st.is_builtin(id->get_name())) {
                    return set_error(fmt::format("Overriding builtin '{}' is not allowed", id->get_name()));
                }
                return m_left->compute_stmt_type(st);
            }
            auto dotExpr = std::dynamic_pointer_cast<OpDot>(m_left);
            if (dotExpr) {
                Node::Ptr dotError = dotExpr->compute_stmt_type(st);
                if (dotError) {
                    return dotError;
                }
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
        if(st.get_scope_type() != ScopeType::Func) {
            return  set_error(FF("Misplaced if statement"));
        }

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
        if(m_condition) {
            auto conditionType = m_condition->get_stmt_type();
            if (conditionType != BuiltinManager::Boolean) {
                    return set_error(FF("If only accepts tests of type 'Boolean'"));
            }
        }
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
        if(st.get_scope_type() != ScopeType::Func) {
            return  set_error(FF("Misplaced while statement"));
        }

        Node::Ptr error = m_condition->compute_stmt_type(st);
        if (error) return error;

        if (m_repeatBranch) {
            error = m_repeatBranch->compute_stmt_type(st);
            if (error) return error;
        }

        return nullptr;
    }

    Node::Ptr WhileStatement::add_to_symtab_forward(SymbolTable &st) {
        if(m_condition) {
            auto conditionType = m_condition->get_stmt_type();
            if (conditionType != BuiltinManager::Boolean) {
                    return set_error(FF("While only accepts tests of type 'Boolean'"));
            }
        }
        return nullptr;
    }

    Node::Ptr WhileStatement::add_to_symtab_ordered(SymbolTable &st) {
        if (m_repeatBranch) {
            m_repeatBranch->add_to_symtab_ordered(st);
        }
        return nullptr;
    }

    Node::Ptr OpDot::compute_stmt_type(SymbolTable &st) {
        Node::Ptr leftError = m_left->compute_stmt_type(st);
        if (leftError) {
            return leftError;
        }

        auto leftIdentifier = std::dynamic_pointer_cast<Identifier>(m_left);
        if (!leftIdentifier) {
            return set_error("Left-hand side of dot expression must be an identifier");
        }

        auto leftSymbol = st.lookup(leftIdentifier->get_name());
        if (!leftSymbol) {
            return set_error(fmt::format("Object '{}' is not defined", leftIdentifier->get_name()));
        }

        auto leftClass = std::dynamic_pointer_cast<ClassStatement>(leftSymbol);
        auto leftLet = std::dynamic_pointer_cast<LetStatement>(leftSymbol);
        if (leftLet) {
            auto classType = std::dynamic_pointer_cast<Identifier>(leftLet->get_type());
            if (classType) {
                auto classSymbol = st.lookup(classType->get_name());
                leftClass = std::dynamic_pointer_cast<ClassStatement>(classSymbol);
            }
        }

        if (!leftClass) {
            return set_error(fmt::format("'{}' is not a class instance", leftIdentifier->get_name()));
        }

        auto classSymTab = leftClass->get_symtab();
        if (!classSymTab) {
            return set_error(fmt::format("Class '{}' has no symbol table", leftClass->as_string()));
        }

        auto rightIdentifier = std::dynamic_pointer_cast<Identifier>(m_right);
        if (!rightIdentifier) {
            return set_error("Right-hand side of dot expression must be an identifier");
        }
        if (st.is_builtin(rightIdentifier->get_name())) {
            return set_error(FF("Identifier '{}' has no subsymbol '{}'", leftIdentifier->get_name(), rightIdentifier->get_name()));
        }

        auto memberSymbol = classSymTab->lookup(rightIdentifier->get_name());
        if (!memberSymbol) {
            auto parentClassIdentifier = std::dynamic_pointer_cast<Identifier>(leftClass->get_parent_class());
            
            if (parentClassIdentifier) {
                auto parentClassSymbol = st.lookup(parentClassIdentifier->get_name());
                auto parentClass = std::dynamic_pointer_cast<ClassStatement>(parentClassSymbol);
                if (parentClass && parentClass->get_symtab()) {
                    memberSymbol = parentClass->get_symtab()->lookup(rightIdentifier->get_name());
                }
            }
        }

        if (!memberSymbol) {
            std::string fullExpression = fmt::format("{}.{}", leftIdentifier->get_name(), rightIdentifier->get_name());
            return set_error(fmt::format("Identifier '{}' is not found", fullExpression));
        }
        return nullptr;
    }

    Node::Ptr OpDot::add_to_symtab_forward(SymbolTable &st) {
        return nullptr;
    }

    Node::Ptr OpDot::add_to_symtab_ordered(SymbolTable &st) {
        return nullptr;
    }

}