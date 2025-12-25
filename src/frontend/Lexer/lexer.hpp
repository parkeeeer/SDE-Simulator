#pragma once

#include <vector>
#include <string_view>
#include <exception>
#include "token.hpp"


namespace sde::frontend {

class Lexer{
    std::string_view in;
    size_t curr_pos;

    public:

    Lexer(std::string_view input): in(input), curr_pos(0) {}

    std::vector<Token> lex();

    
    static std::vector<Token> insert_implicit_mul(const std::vector<Token>& in) {
        std::vector<Token> ret;
        ret.reserve(in.size() * 2);
        for (size_t i = 0; i < in.size(); ++i) {
            if (!ret.empty()) {
                const TokenType A = ret.back().type;
                const TokenType B = in[i].type;
                const bool looks_like_call = (A == TokenType::IDENTIFIER && B == TokenType::LPAREN);
                if ((A == TokenType::NUMBER || A == TokenType::IDENTIFIER || A == TokenType::RPAREN) && (B == TokenType::IDENTIFIER || B == TokenType::LPAREN) && !looks_like_call) {
                    ret.push_back(Token{TokenType::MULTIPLY, "*"});
                }
            }
            ret.push_back(in[i]);
        }
        return ret;
    }
};

} // namespace sde::frontend
