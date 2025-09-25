#pragma once

#include <string>
#include <iostream>


enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    IDENTIFIER,
    EXP,
    COMMA,
    END
};


struct Token{
    TokenType type;
    std::string value;

    void print() const{
        switch(type){
            case TokenType::NUMBER:
                std::cout << "NUMBER(" << value << ")";
                break;
            case TokenType::PLUS:
                std::cout << "PLUS(" << value << ")";
                break;
            case TokenType::MINUS:
                std::cout << "MINUS(" << value << ")";
                break;
            case TokenType::MULTIPLY:
                std::cout << "MULTIPLY(" << value << ")";
                break;
            case TokenType::DIVIDE:
                std::cout << "DIVIDE(" << value << ")";
                break;
            case TokenType::LPAREN:
                std::cout << "LPAREN(" << value << ")";
                break;
            case TokenType::RPAREN:
                std::cout << "RPAREN(" << value << ")";
                break;
            case TokenType::IDENTIFIER:
                std::cout << "IDENTIFIER(" << value << ")";
                break;
            case TokenType::EXP:
                std::cout << "EXP(" << value << ")";
                break;
            case TokenType::END:
                std::cout << "END(" << value << ")";
                break;
            case TokenType::COMMA:
                std::cout << "COMMA(" << value << ")";
                break;
        }
    }

    bool operator==(const Token& other) const{
        return type == other.type && value == other.value;
    }
};