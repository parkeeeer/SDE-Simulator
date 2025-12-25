#pragma once

#include "lexer.hpp"
#include "Parser.hpp"
#include "../src/frontend/Compiler/compiler.hpp"

#include "concepts.hpp"

namespace sde::frontend{

    template<concepts::fp_or_simd Num>
    Environment<Num> env_cast(const Environment<double>& old_env) {
        Environment<Num> new_env;
        for (const auto& [name, value] : old_env) {
            Num value_casted = static_cast<Num>(static_cast<concepts::lane_t<Num>>(value));
            new_env.add_param(name, value_casted);
        }
        return new_env;
    }
    

//Takes an input string and environment to creat a compiled bytecode program
//bytecode can be run with run_bytecode
//the input should be in the form "drift(x,t) dt + diffusion(x,t) dW (this is required due to how partitioning works.
template<sde::concepts::fp_or_simd Num>
std::pair<Program<Num>, Program<Num>> compile_to_bytecode(const std::string& input, const Environment<Num>& env){
    size_t idx_dt = input.find("dt");
    size_t idx_dw = input.find("dW");
    if(idx_dw == std::string::npos) idx_dw = input.find("dw");
    const std::string_view expr(input);
    const std::string_view first_expr = expr.substr(0, idx_dt);
    const std::string_view second_expr = expr.substr(idx_dt + 2, idx_dw - idx_dt + 2);

    Lexer lexer(first_expr);
    std::vector<Token> tokens = lexer.lex();
    Parser<Num> parser(env, std::move(tokens));
    AST<Num> ast = parser.parse();
    Compiler<Num> compiler;
    
    Lexer lexer2(second_expr);
    std::vector<Token> tokens2 = lexer2.lex();
    Parser<Num> parser2(env, std::move(tokens2));
    AST<Num> ast2 = parser2.parse();
    Compiler<Num> compiler2;

    return {compiler.compile(ast), compiler2.compile(ast2)};
}

template<sde::concepts::fp_or_simd Num>
std::pair<AST<Num>, AST<Num>> parse_to_ast(const std::string& input, const Environment<Num>& env){
    size_t idx_dt = input.find("dt");
    size_t idx_dw = input.find("dW");
    if(idx_dw == std::string::npos) idx_dw = input.find("dw");
    std::string_view expr(input);
    std::string_view first_expr = expr.substr(0, idx_dt);
    std::string_view second_expr = expr.substr(idx_dt + 2, idx_dw - idx_dt + 2);

    Lexer lexer(first_expr);
    std::vector<Token> tokens = lexer.lex();
    Parser<Num> parser(env, std::move(tokens));
    
    Lexer lexer2(second_expr);
    std::vector<Token> tokens2 = lexer2.lex();
    Parser<Num> parser2(env, std::move(tokens2));

    return {parser.parse(), parser2.parse()};
}

template<sde::concepts::fp_or_simd Num>
Program<Num> compile(std::string_view input, const Environment<double>& env) {
    Environment<Num> new_env = env_cast<Num>(env);
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.lex();
    Parser<Num> parser(new_env, std::move(tokens));
    AST<Num> ast = parser.parse();
    Compiler<Num> compiler;
    return compiler.compile(ast);
}

template<sde::concepts::fp_or_simd Num>
Program<Num> compile(const AST<Num>& ast) {
    Compiler<Num> compiler;
    return compiler.compile(ast);
}

template<sde::concepts::fp_or_simd Num>
AST<Num> parse(std::string_view input, const Environment<double>& env) {
    Environment<Num> new_env = env_cast<Num>(env);
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.lex();
    Parser<Num> parser(new_env, std::move(tokens));
    return parser.parse();
}




template<sde::concepts::FloatingPoint Num>
inline Num run_bytecode(const Program<Num>& program, Num t, Num X, Num dW){
    return program.run(t, X, dW);
}

template<sde::concepts::FloatingPoint Num>
inline Num run_AST(const AST<Num>& ast, Num t, Num X, Num dW){
    return ast.eval(t, X, dW);
}

}