#include <cassert>

#include "lexer.hpp"


int main(){
    Lexer lexer("15+6-x+175");
    std::vector<Token> tokens = lexer.lex();
    for(const auto& token : tokens){
        token.print();
        std::cout << ", ";
    }
}