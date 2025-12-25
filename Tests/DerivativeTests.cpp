#include <doctest/doctest.h>

#include <string>
#include <vector>


#include <frontend.hpp>

#include <ast-analysis.hpp>

using namespace sde::frontend;

TEST_CASE("simple derivative") {
    std::string input = "2x";
    Lexer l(input);
    Parser<double> p(Environment<double>(),l.lex());
    AST<double> ast = p.parse();
    CHECK(ast.eval(1,1) == doctest::Approx(2.0));
    CHECK(ast.eval(0,0) == doctest::Approx(0.0));
    AST<double> deriv = differentiate(ast, "x");
    CHECK(deriv.eval(0,0) == doctest::Approx(2.0));
    CHECK(deriv.eval(1,0) == doctest::Approx(2.0));
    CHECK(deriv.eval(2,0) == doctest::Approx(2.0));
    CHECK(deriv.eval(3,0) == doctest::Approx(2.0));
}

TEST_CASE("test pedantic mode"){
    std::string input1 = "min(x,3)";
    std::string input2 = "max(x,3)";
    std::string input3 = "abs(-x)";
    Lexer l1(input1);
    Lexer l2(input2);
    Lexer l3(input3);
    Parser<double> p1(Environment<double>(),l1.lex());
    Parser<double> p2(Environment<double>(),l2.lex());
    Parser<double> p3(Environment<double>(),l3.lex());
    AST<double> ast1 = p1.parse();
    AST<double> ast2 = p2.parse();
    AST<double> ast3 = p3.parse();
    CHECK_THROWS_AS(differentiate(ast1, "x", true), std::runtime_error);
    CHECK_THROWS_AS(differentiate(ast2, "x", true), std::runtime_error);
    CHECK_THROWS_AS(differentiate(ast3, "x", true), std::runtime_error);
}

TEST_CASE("test power rule") {
    std::string input = "2x^2";
    std::string input2 = "3^(4x)";
    std::string input3 = "X^4x";
    Lexer l(input);
    Lexer l2(input2);
    Lexer l3(input3);
    Parser<double> p(Environment<double>(),l.lex());
    Parser<double> p2(Environment<double>(),l2.lex());
    Parser<double> p3(Environment<double>(),l3.lex());
    AST<double> ast = p.parse();
    AST<double> ast2 = p2.parse();
    AST<double> ast3 = p3.parse();
    AST<double> deriv = differentiate(ast, "x");
    AST<double> deriv2 = differentiate(ast2, "x");
    AST<double> deriv3 = differentiate(ast3, "x");

    CHECK(deriv.safe_eval(0,0) == doctest::Approx(0.0));
    CHECK(deriv.eval(1,0) == doctest::Approx(4.0));

    CHECK(deriv2.eval(0,0) == doctest::Approx(4.0 * std::log(3)));
    CHECK(deriv2.eval(1,0) == doctest::Approx(4.0 * std::log(3) * 81.0));
}