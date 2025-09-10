#pragma once

#include <vector>
#include <string_view>
#include <exception>
#include "token.hpp"

class Lexer{
    std::string_view in;
    size_t curr_pos;

    public:

    Lexer(std::string_view input): in(input), curr_pos(0) {}

    std::vector<Token> lex();

    static bool is_primary_tail(TokenType t) {
        return t == TokenType::NUMBER || t == TokenType::IDENTIFIER || t == TokenType::RPAREN;
    }
    static bool is_primary_head(TokenType t) {
        return t == TokenType::IDENTIFIER || t == TokenType::LPAREN;
    }

    static std::vector<Token> insert_implicit_mul(const std::vector<Token>& in) {
        std::vector<Token> out;
        out.reserve(in.size() * 2);
        for (size_t i = 0; i < in.size(); ++i) {
            if (!out.empty()) {
                const TokenType A = out.back().type;
                const TokenType B = in[i].type;
                const bool looks_like_call = (A == TokenType::IDENTIFIER && B == TokenType::LPAREN);
                if (is_primary_tail(A) && is_primary_head(B) && !looks_like_call) {
                    out.push_back(Token{TokenType::MULTIPLY, "*"});
                }
            }
            out.push_back(in[i]);
        }
        return out;
    }
};
