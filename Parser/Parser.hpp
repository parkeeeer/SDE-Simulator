#pragma once

#include <vector>
#include <string>
#include "AST.hpp"
//#include "../Lexer/token.hpp"

#include "token.hpp"





template<class Num>
class Parser{


    public:
    AST<Num> parse(const std::vector<Token>& tokens);
};