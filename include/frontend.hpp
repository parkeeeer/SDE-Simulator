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


template<class Num>
Num run_bytecode(const Program<Num>& program, Num t, Num X, Num dW){
    return program.run(t, X, dW);
}

template<class Num>
Num run_AST(const AST<Num>& ast, Num t, Num X, Num dW){
    return ast.run(t, X, dW);
}

}