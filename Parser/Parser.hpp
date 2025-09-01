#pragma once

#include <vector>
#include <string>
#include "AST.hpp"
//#include "../Lexer/token.hpp"

#include "token.hpp"






class Parser{


    public:
    AST parse(const std::vector<Token>& tokens);
};