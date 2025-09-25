#pragma once

#include "bytecode.hpp"
#include "Parser.hpp"
#include "Variable_Environment.hpp"
#include "AST.hpp"



template<class Num>
class Compiler{
    Program<Num> program;
    

    void convert(ASTNode<Num>* node);

    uint32_t add_const_and_return_index(Num value){
        program.constants.push_back(value);
        return static_cast<uint32_t>(program.constants.size() - 1);
    }






    public:
    Compiler() = default;

    Program<Num> compile(const AST<Num>& ast){
        convert(ast.get_root().get());
        program.compute_max_stack_size();
        return std::move(program);
    }

    
};




//#include "compiler.tpp"