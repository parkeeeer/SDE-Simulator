#pragma once

#include <vector>
#include <string_view>
#include "token.hpp"

class Lexer{
    std::string_view in;
    size_t curr_pos;

    public:

    Lexer(std::string_view input): in(input), curr_pos(0) {}

    std::vector<Token> lex();
};
