#pragma once

#include "lexer.hpp"
#include "Parser.hpp"
#include "compiler.hpp"

namespace sde::frontend{
    
template<class Num>
Program<Num> compile_to_bytecode(const std::string& input, const Environment<Num>& env){
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.lex();
    Parser<Num> parser(env, std::move(tokens));
    AST<Num> ast = parser.parse();
    Compiler<Num> compiler;
    return compiler.compile(ast);
}

template<class Num>
AST<Num> parse_to_ast(const std::string& input, const Environment<Num>& env){
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.lex();
    Parser<Num> parser(env, std::move(tokens));
    return parser.parse();
}

}