#include <doctest/doctest.h>

#include <iostream>
#include <string>
#include <vector>

#include "lexer.hpp"


using namespace sde::frontend;

TEST_CASE("basic binary operator"){
    Lexer l("4+2*7-1/9^5");
    std::vector<Token> tokens = l.lex();
    CHECK(tokens.size() == 12);
    CHECK(tokens[0] == Token{TokenType::NUMBER, "4"});
    CHECK(tokens[1] == Token{TokenType::PLUS, "+"});
    CHECK(tokens[2] == Token{TokenType::NUMBER, "2"});
    CHECK(tokens[3] == Token{TokenType::MULTIPLY, "*"});
    CHECK(tokens[4] == Token{TokenType::NUMBER, "7"});
    CHECK(tokens[5] == Token{TokenType::MINUS, "-"});
    CHECK(tokens[6] == Token{TokenType::NUMBER, "1"});
    CHECK(tokens[7] == Token{TokenType::DIVIDE, "/"});
    CHECK(tokens[8] == Token{TokenType::NUMBER, "9"});
    CHECK(tokens[9] == Token{TokenType::EXP, "^"});
    CHECK(tokens[10] == Token{TokenType::NUMBER, "5"});
    CHECK(tokens[11] == Token{TokenType::END, ""});
}


TEST_CASE("long number test"){
    Lexer l("3.14159265358979323846264338327950288419716939937510+2183670872304+8710347129387");
    std::vector<Token> tokens = l.lex();
    CHECK(tokens.size() == 6);
    CHECK(tokens[0] == Token{TokenType::NUMBER, "3.14159265358979323846264338327950288419716939937510"});
    CHECK(tokens[1] == Token{TokenType::PLUS, "+"});
    CHECK(tokens[2] == Token{TokenType::NUMBER, "2183670872304"});
    CHECK(tokens[3] == Token{TokenType::PLUS, "+"});
    CHECK(tokens[4] == Token{TokenType::NUMBER, "8710347129387"});
    CHECK(tokens[5] == Token{TokenType::END, ""});
}

TEST_CASE("whitespace handling test"){
    Lexer l("   42   +   23 \n \t  ");
    std::vector<Token> tokens = l.lex();
    CHECK(tokens.size() == 4);
    CHECK(tokens[0] == Token{TokenType::NUMBER, "42"});
    CHECK(tokens[1] == Token{TokenType::PLUS, "+"});
    CHECK(tokens[2] == Token{TokenType::NUMBER, "23"});
    CHECK(tokens[3] == Token{TokenType::END, ""});
}

TEST_CASE("identifier and function plus comma test"){
    Lexer l("exp(x_t) + pow(4,three) * x_test");
    std::vector<Token> tokens = l.lex();
    CHECK(tokens.size() == 14);
    CHECK(tokens[0] == Token{TokenType::IDENTIFIER, "exp"});
    CHECK(tokens[1] == Token{TokenType::LPAREN, "("});
    CHECK(tokens[2] == Token{TokenType::IDENTIFIER, "x_t"});
    CHECK(tokens[3] == Token{TokenType::RPAREN, ")"});
    CHECK(tokens[4] == Token{TokenType::PLUS, "+"});
    CHECK(tokens[5] == Token{TokenType::IDENTIFIER, "pow"});
    CHECK(tokens[6] == Token{TokenType::LPAREN, "("});
    CHECK(tokens[7] == Token{TokenType::NUMBER, "4"});
    CHECK(tokens[8] == Token{TokenType::COMMA, ","});
    CHECK(tokens[9] == Token{TokenType::IDENTIFIER, "three"});
    CHECK(tokens[10] == Token{TokenType::RPAREN, ")"});
    CHECK(tokens[11] == Token{TokenType::MULTIPLY, "*"});
    CHECK(tokens[12] == Token{TokenType::IDENTIFIER, "x_test"});
    CHECK(tokens[13] == Token{TokenType::END, ""});
}

TEST_CASE("invalid character test"){
    Lexer a("$");
    CHECK_THROWS_AS(a.lex(), std::runtime_error);

    Lexer b("&");
    CHECK_THROWS_AS(b.lex(), std::runtime_error);

    Lexer c("@");
    CHECK_THROWS_AS(c.lex(), std::runtime_error);

    Lexer d("#");
    CHECK_THROWS_AS(d.lex(), std::runtime_error);

    Lexer e("?");
    CHECK_THROWS_AS(e.lex(), std::runtime_error);

    Lexer f("~");
    CHECK_THROWS_AS(f.lex(), std::runtime_error);

    Lexer g("`");
    CHECK_THROWS_AS(g.lex(), std::runtime_error);

    Lexer h("=");
    CHECK_THROWS_AS(h.lex(), std::runtime_error);

    Lexer i("[");
    CHECK_THROWS_AS(i.lex(), std::runtime_error);

    Lexer j("]");
    CHECK_THROWS_AS(j.lex(), std::runtime_error);

    Lexer k("{");
    CHECK_THROWS_AS(k.lex(), std::runtime_error);

    Lexer l("}");
    CHECK_THROWS_AS(l.lex(), std::runtime_error);

    Lexer m(";");
    CHECK_THROWS_AS(m.lex(), std::runtime_error);

    Lexer n(":");
    CHECK_THROWS_AS(n.lex(), std::runtime_error);

    Lexer o("<");
    CHECK_THROWS_AS(o.lex(), std::runtime_error);

    Lexer p(">");
    CHECK_THROWS_AS(p.lex(), std::runtime_error);

    Lexer q("|");
    CHECK_THROWS_AS(q.lex(), std::runtime_error);

    Lexer r("\"");
    CHECK_THROWS_AS(r.lex(), std::runtime_error);

    Lexer s("'");
    CHECK_THROWS_AS(s.lex(), std::runtime_error);

    Lexer t("\\");
    CHECK_THROWS_AS(t.lex(), std::runtime_error);


    //might add '!', but honestly idk if ill add support for factorial or not because I really dont feel like
    //postfix support ugh
}


TEST_CASE("max one dot per number"){
    Lexer l("3.14.15.");
    CHECK_THROWS_AS(l.lex(), std::runtime_error);

    Lexer m("3..1415");
    CHECK_THROWS_AS(m.lex(), std::runtime_error);
}

TEST_CASE("Scientific notation support test"){
    Lexer l("1.23e10");
    std::vector<Token> tokens = l.lex();
    CHECK(tokens.size() == 2);
    CHECK(tokens[0] == Token{TokenType::NUMBER, "1.23e10"});
    CHECK(tokens[1] == Token{TokenType::END, ""});

    Lexer m("4.56E-20");
    std::vector<Token> tokens_m = m.lex();
    CHECK(tokens_m.size() == 2);
    CHECK(tokens_m[0] == Token{TokenType::NUMBER, "4.56E-20"});
    CHECK(tokens_m[1] == Token{TokenType::END, ""});

    Lexer n("7.89e+30");
    std::vector<Token> tokens_n = n.lex();
    CHECK(tokens_n.size() == 2);
    CHECK(tokens_n[0] == Token{TokenType::NUMBER, "7.89e+30"});
    CHECK(tokens_n[1] == Token{TokenType::END, ""});

    Lexer o("1.2e5e8");
    CHECK_THROWS_AS(o.lex(), std::runtime_error);
}
