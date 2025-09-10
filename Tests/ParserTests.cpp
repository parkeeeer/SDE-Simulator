#include <doctest/doctest.h>

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "lexer.hpp"
#include "Parser.hpp"


TEST_CASE("basic expression parsing test"){
    Lexer l("4 + 2 * 7 - 1 / 9 ^ 5");
    std::vector<Token> tokens = l.lex();
    Environment<double> env;
    Parser<double> p(env,tokens);
    AST<double> ast = p.parse();
    CHECK(ast.eval(env.get_view()) == doctest::Approx(4.0 + 2.0 * 7.0 - 1.0 / std::pow(9.0, 5.0)));
}

TEST_CASE("test exp associativity(right)"){
    Lexer l("2 ^ 3 ^ 2");
    std::vector<Token> tokens = l.lex();
    Environment<double> env;
    Parser<double> p(env,tokens);
    AST<double> ast = p.parse();
    CHECK(ast.eval(env.get_view()) == doctest::Approx(std::pow(2.0, std::pow(3.0, 2.0))));
}

TEST_CASE("functions test"){
    Lexer l("exp(1) + log(10) + sqrt(4) + sin(0) + cos(0) + tan(0) + abs(-5) + max(1,2) + min(1,2)");
    std::vector<Token> tokens = l.lex();
    Environment<double> env;
    Parser<double> p(env,tokens);
    AST<double> ast = p.parse();
    CHECK(ast.eval(env.get_view()) == 
    doctest::Approx(std::exp(1.0) + std::log(10.0) + std::sqrt(4.0) + std::sin(0.0) + std::cos(0.0) + std::tan(0.0) + std::abs(-5.0) + std::max(1.0,2.0) + std::min(1.0,2.0)));
}

TEST_CASE("unary start test"){
    Lexer l("-3 + -(-2)");
    std::vector<Token> tokens = l.lex();
    Environment<double> env;
    Parser<double> p(env,tokens);
    AST<double> ast = p.parse();
    CHECK(ast.eval(env.get_view()) == doctest::Approx(-3.0 + -(-2.0)));
}

TEST_CASE("parameter test"){
    Lexer l("param1 + paramb * paramc");
    std::vector<Token> tokens = l.lex();
    Environment<double> env;
    env.add_param("param1", 4.0);
    env.add_param("paramb", 2.0);
    env.add_param("paramc", 7.0);
    Parser<double> p(env,tokens);
    AST<double> ast = p.parse();
    CHECK(ast.eval(env.get_view()) == doctest::Approx(4.0 + 2.0 * 7.0));
}

TEST_CASE("test state variables"){
    Lexer l("X + 2 * t * dt - X *8 * dW");
    auto tokens = l.lex();
    Environment<double> env;
    env.update_state(3.0, 4.0, 0.5);
    env.set_dt(.05);
    Parser<double> p(env,tokens);
    AST<double> ast = p.parse();
    CHECK(ast.eval(env.get_view()) == doctest::Approx(3.0 + 2.0 * 4.0 * 0.05 - 3.0 * 8.0 * 0.5));
}

TEST_CASE("test implicit multiplication"){
    Lexer l("2x + 3(4 + x) + sin(x)cos(x)");
    std::vector<Token> tokens = l.lex();
    Environment<double> env;
    env.add_param("x", 1.0);
    Parser<double> p(env,tokens);
    AST<double> ast = p.parse();
    CHECK(ast.eval(env.get_view()) == doctest::Approx(2.0 * 1.0 + 3.0 * (4.0 + 1.0) + std::sin(1.0) * std::cos(1.0)));
}

TEST_CASE("test nested function calls"){
    Lexer l("sin(cos(0)) + log(exp(1)) + sqrt(4 + sqrt(16))");
    std::vector<Token> tokens = l.lex();
    Environment<double> env;
    Parser<double> p(env,tokens);
    AST<double> ast = p.parse();
    CHECK(ast.eval(env.get_view()) == doctest::Approx(std::sin(std::cos(0.0)) + std::log(std::exp(1.0)) + std::sqrt(4.0 + std::sqrt(16.0))));
}
