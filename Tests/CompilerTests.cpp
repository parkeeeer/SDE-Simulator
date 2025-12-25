#include <doctest/doctest.h>

#include <frontend.hpp>


using namespace sde::frontend;

TEST_CASE("basic binary operator") {
    std::string input = "1+2";
    Lexer l(input);
    Parser<double> p(Environment<double>(),l.lex());
    AST<double> ast = p.parse();
    Program<double> prog = Compiler<double>().compile(ast);
    double stack[5];
    CHECK(prog.run(0,0, stack) == doctest::Approx(3.0));
}

TEST_CASE("LOL IDK") {
    std::string input = "";
}