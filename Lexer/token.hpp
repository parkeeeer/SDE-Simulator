#pragma once

#include <string>


enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    VARIABLE,
    EXP,
    END
};


struct Token{
    TokenType type;
    std::string value;
};