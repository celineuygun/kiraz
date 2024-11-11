
#include <regex>

#include <gtest/gtest.h>

#include <lexer.hpp>
#include <main.h>

#include <kiraz/Compiler.h>
#include <kiraz/Node.h>

extern int yydebug;

namespace kiraz {

struct CompilerFixture : public ::testing::Test {
    void SetUp() override {}
    void TearDown() override {}

    /**
     * @brief verify_first: Verifies the AST of the given module.
     * @param code: Kiraz source code, as a string.
     * @param ast:  Expected AST for the given code.
     */
    void verify_root(const std::string &code, const std::string &ast) {
        Compiler compiler;

        std::stringstream ostr;

        /* perform */
        compiler.compile_string(code, ostr);

        /* verify */
        if (! Node::get_root_before()) {
            fmt::print("{}\n", compiler.get_error());
        }

        ASSERT_TRUE(Node::get_root_before());
        auto root = Node::get_root_before();
        ASSERT_EQ(fmt::format("{}", *root), ast);
    }

    void verify_ok(const std::string &code) {
        Compiler compiler;

        std::stringstream ostr;

        /* perform */
        compiler.compile_string(code, ostr);

        /* verify */
        if (! Node::get_root_before()) {
            fmt::print("{}\n", compiler.get_error());
        }

        ASSERT_TRUE(Node::get_root_before());
    }

    /**
     * @brief verify_first: Verifies the AST of the first statement in the given module.
     * @param code: Kiraz source code, as a string.
     * @param ast:  Expected AST for the given code.
     */
    void verify_first(const std::string &code, const std::string &ast) {
        Compiler compiler;

        std::stringstream ostr;

        /* perform */
        compiler.compile_string(code, ostr);

        /* verify */
        if (! Node::get_root_before()) {
            fmt::print("{}\n", compiler.get_error());
        }

        ASSERT_TRUE(Node::get_root_before());
        ASSERT_TRUE(Node::get_first_before());
        auto first = Node::get_first_before();
        ASSERT_EQ(fmt::format("{}", *first), ast);
    }

    void verify_error(const std::string &code, const std::string &err) {
        Compiler compiler;

        std::stringstream ostr;

        /* perform */
        compiler.compile_string(code, ostr);

        /* verify */
        if (Node::get_root_before()) {
            fmt::print("{}\n", *Node::get_root_before());
        }

        ASSERT_FALSE(Node::get_root_before());
        ASSERT_EQ(std::regex_replace(compiler.get_error(),
                          std::regex("^Error at [0-9]+:[0-9]+: (.*)\n$"), "$1"),
                err);
    }
};

TEST_F(CompilerFixture, class_subsymbol) {
    verify_ok("class C { let i = 0; }; func f() : Void { let c: C; c.i;};");
}

TEST_F(CompilerFixture, class_subsymbol_missing) {
    verify_error("class C { let i = 0; }; func f():Void{ let c: C; c.j;};",
            "Identifier 'c.j' is not found");
}

TEST_F(CompilerFixture, class_parent) {
    verify_ok("class P {}; class C:P {};");
}

TEST_F(CompilerFixture, class_parent_missing) {
    verify_error("class C:P {};", "Type 'P' is not found");
}

TEST_F(CompilerFixture, class_parent_subsymbol) {
    verify_ok("class C { let i = 0; }; class C:P {}; func f() : Void { let c: P; c.i;};");
}

TEST_F(CompilerFixture, import_missing) {
    verify_error("func f() : Void { io.foo.bar();\n };", "Identifier 'io' is not found");
}

TEST_F(CompilerFixture, func_rettype_missing) {
    verify_error("func f() : R { let a = 5; return a + b; };",
            "Return type 'R' of function 'f' is not found");
}

TEST_F(CompilerFixture, func_argtype_missing) {
    verify_error("func f(a: A) : Void { };",
            "Identifier 'A' in type of argument 'a' in function 'f' is not found");
}

TEST_F(CompilerFixture, func_arg_dup) {
    verify_error("func f(a: Integer64, a: Integer64) : Void { };",
            "Identifier 'a' in argument list of function 'f' is already in symtab");
}

TEST_F(CompilerFixture, func_arg_name_dup) {
    verify_error("func f(f: Integer64) : Void { };",
            "Identifier 'f' in argument list of function 'f' is already in symtab");
}

TEST_F(CompilerFixture, func_adjacent) {
    verify_ok("func f(a: Integer64) : Void { };"
              "func g(a: Integer64) : Void { };");
}

TEST_F(CompilerFixture, var_missing) {
    verify_error("func f() : Void { a; };", "Identifier 'a' is not found");
}

TEST_F(CompilerFixture, var_found) {
    verify_ok("func f() : Void { let a = 5; a; };");
}

TEST_F(CompilerFixture, var_scoped_conflict) {
    verify_error(
            "let a = 5; func f() : Void { let a = 5; };", "Identifier 'a' is already in symtab");
}

TEST_F(CompilerFixture, var_scoped) {
    verify_ok("func f() : Void { let a = 5; }; let a = 5;");
}

TEST_F(CompilerFixture, class_scope) {
    verify_ok("class A { let a = 5; };");
}

TEST_F(CompilerFixture, class_scope_var_var_conflict) {
    verify_error("class A { let a = 5; let a = 5; };", "Identifier 'a' is already in symtab");
}

TEST_F(CompilerFixture, class_scope_var_func_conflict) {
    verify_error(
            "class A { let a = 5; func a() : Void {}; };", "Identifier 'a' is already in symtab");
}

TEST_F(CompilerFixture, func_name_conflict) {
    verify_error("class A { }; func A() :Void {};", "Identifier 'A' is already in symtab");
}

TEST_F(CompilerFixture, op_add_int) {
    verify_ok("func f() : Void { let a = 5; let b = 10; let c = a + b; };");
}

TEST_F(CompilerFixture, op_add_str) {
    verify_ok(R"(func f() : Void { let a = "5"; let b = "10"; let c = a + b; };)");
}

TEST_F(CompilerFixture, op_add_type_mismatch) {
    verify_error(R"(func f() : Void { let a = 5; let b = "10"; let c = a + b; };)",
            "Operator '+' not defined for types 'Integer64' and 'String'");
}

TEST_F(CompilerFixture, class_method_conflict) {
    verify_error("class A { func A() :Void {}; };", "Identifier 'A' is already in symtab");
}

TEST_F(CompilerFixture, class_member_conflict) {
    verify_error("class A { let A: Integer64; };", "Identifier 'A' is already in symtab");
}

TEST_F(CompilerFixture, class_name_conflict) {
    verify_error("func C() :Void {}; class C { };", "Identifier 'C' is already in symtab");
}

TEST_F(CompilerFixture, class_scope_var_func_parent_scope) {
    verify_ok("class A { let a = 5; func f() : Void { a; }; };");
}

TEST_F(CompilerFixture, class_scoped_func_conflict) {
    verify_error("class A { let a = 5; func f() : Void { let a = 5; }; };",
            "Identifier 'a' is already in symtab");
}

TEST_F(CompilerFixture, class_use_before_definition) {
    verify_ok("let a : A; class A { };");
}

TEST_F(CompilerFixture, class_method_self) {
    verify_ok("class A { func m() : Void { self; }; };");
}

TEST_F(CompilerFixture, func_no_self) {
    verify_error("func m() : Void { self; };", "Identifier 'self' is not found");
}

TEST_F(CompilerFixture, func_hello_world) {
    verify_ok(R"(import io; func main() : Void { io.print("Hello world!\n"); };)");
}

TEST_F(CompilerFixture, func_return) {
    verify_ok(R"(func f() : Integer64 { return 0; };)");
}

TEST_F(CompilerFixture, func_return_mismatch) {
    verify_error(R"(func f() : Integer64 { return "0"; };)",
            "Return statement type 'String' does not match function return type 'Integer64'");
}

TEST_F(CompilerFixture, func_use_before_definition) {
    verify_ok(R"(import io;
            func say_hello() : Void {
            let h = get_hello();
            io.print(h);
            }
            func get_hello() : String {
            return "Hello, World!\n";
            }
            )");
}

TEST_F(CompilerFixture, assignment_type_mismatch_func) {
    verify_error(R"(
            func f() : Void {
            let h : Integer64;
            h = g();
            }
            func g() : String { }
            )",
            "Left type 'Integer64' of assignment does not match the right type 'String'");
}

TEST_F(CompilerFixture, let_type_mismatch_func) {
    verify_error(R"(
            func f() : Void {
            let h : Integer64 = g();
            h = g();
            }
            func g() : String { }
            )",
            "Initializer type 'String' doesn't match explicit type 'Integer64'");
}

TEST_F(CompilerFixture, let_type_mismatch_var) {
    verify_error(R"(func f() : Void { let h : Integer64 = "string"; };)",
            "Initializer type 'String' doesn't match explicit type 'Integer64'");
}

TEST_F(CompilerFixture, assignment_type_mismatch_var) {
    verify_error(R"(func f() : Void { let h : Integer64; h = "string"; };)",
            "Left type 'Integer64' of assignment does not match the right type 'String'");
}

TEST_F(CompilerFixture, func_missing) {
    verify_error("func say_hello() : Void { let h = get_hello(); };",
            "Identifier 'get_hello' is not found");
}

TEST_F(CompilerFixture, import_subsymbol_missing) {
    verify_error("import io; func f() : Void { io.missing(); };",
            "Identifier 'io.missing' is not found");
}

TEST_F(CompilerFixture, func_call_argnum_mismatch) {
    verify_error("import io; func f() : Void { io.print(); };",
            "Call to function 'io.print' has wrong number of arguments");
}

TEST_F(CompilerFixture, func_call_type_mismatch) {
    verify_error("import io; func f() : Void { io.print(42); };",
            "Argument 1 in call to function 'io.print' has type 'Integer64'"
            " which does not match definition type 'String'");
}

} // namespace kiraz
