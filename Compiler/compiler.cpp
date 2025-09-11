#include "compiler.hpp"


template<class Num>
Program<Num> Compiler<Num>::compile(const AST<Num>& ast){
    
}

template<class Num>
void Compiler<Num>::convert(ASTNode<Num>* node){
    if(auto a = dynamic_cast<NumNode<Num>*>(node)){
        program.instrs.push_back(instruction{operation::PUSH_CONST, add_const_and_return_index(a->value)});
        return;
    }
    if(auto b = dynamic_cast<VarNode<Num>*>(node)){
        program.instrs.push_back(instruction{operation::PUSH_VAR, static_cast<uint32_t>(b->index)});
        return;
    }
    if(auto c = dynamic_cast<BinOpNode<Num>*>(node)){
        convert(c->left.get());
        convert(c->right.get());
        switch(c->op){
            case BinOps::ADD:
                program.instrs.push_back(instruction{operation::ADD, 0});
                break;
            case BinOps::SUBTRACT:
                program.instrs.push_back(instruction{operation::SUB, 0});
                break;
            case BinOps::MULTIPLY:
                program.instrs.push_back(instruction{operation::MUL, 0});
                break;
            case BinOps::DIVIDE:
                program.instrs.push_back(instruction{operation::DIV, 0});
                break;
            case BinOps::POWER:
                program.instrs.push_back(instruction{operation::POW, 0});
                break;
        }
        return;
    }
    if(auto d = dynamic_cast<UnarOpNode<Num>*>(node)){
        convert(d->child.get());
        switch(d->op){
            case UnarOps::NEGATE:
                program.instrs.push_back(instruction{operation::NEGATE, 0});
                break;
        }
        return;
    }
    if(auto e = dynamic_cast<FuncNode<Num>*>(node)){
        for(auto& arg : e->args){
            convert(arg.get());
        }
        switch(e->func_id){
            case FuncIds::LOG:
                program.instrs.push_back(instruction{operation::LOG, 0});
                break;
            case FuncIds::EXP:
                program.instrs.push_back(instruction{operation::EXP, 0});
                break;
            case FuncIds::SQRT:
                program.instrs.push_back(instruction{operation::SQRT, 0});
                break;
            case FuncIds::SIN:
                program.instrs.push_back(instruction{operation::SIN, 0});
                break;
            case FuncIds::COS:
                program.instrs.push_back(instruction{operation::COS, 0});
                break;
            case FuncIds::TAN:
                program.instrs.push_back(instruction{operation::TAN, 0});
                break;
            case FuncIds::ABS:
                program.instrs.push_back(instruction{operation::ABS, 0});
                break;
            case FuncIds::MAX:
                program.instrs.push_back(instruction{operation::MAX, 0});
                break;
            case FuncIds::MIN:
                program.instrs.push_back(instruction{operation::MIN, 0});
                break;
        }
        return;
    }
}

template class Compiler<float>;
template class Compiler<double>;