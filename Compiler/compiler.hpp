#pragma once

#include "bytecode.hpp"
#include "Parser.hpp"
#include "Variable_Environment.hpp"
#include "AST.hpp"



template<class Num>
class Compiler{
    Program<Num> program;
    uint32_t stack_size;

    void convert(ASTNode<Num>* node);

    uint32_t add_const_and_return_index(Num value){
        program.constants.push_back(value);
        return static_cast<uint32_t>(program.constants.size() - 1);
    }






    public:
    Compiler(const Environment<Num>& e) noexcept : env(e), stack_size(0) {}

    Program<Num> compile(const AST<Num>& ast){
        convert(ast.get_root().get());
        return std::move(program);
    }

    static Num run(const Program<Num>& program, const Env_view<Num>& env);
};




//#include "compiler.tpp"