#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <vector>
#include "Variable_Environment.hpp"


template<class Num> struct ASTNode;

template<class Num>
using NodePtr = std::unique_ptr<ASTNode<Num>>;

enum class BinOps{
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    POWER
};

enum class FuncIds{
    LOG,
    EXP,
    SQRT,
    SIN,
    COS,
    TAN,
    ABS,
    MAX,
    MIN
};

enum class UnarOps{
    NEGATE
};

template<class Num>
struct ASTNode{
    virtual ~ASTNode() = default;
    virtual Num eval(const Env_view<Num>& env) const = 0;
};

template<class Num>
struct BinOpNode:public ASTNode<Num>{
    BinOpNode(NodePtr<Num> l, NodePtr<Num> r, BinOps o) noexcept : 
                                left(std::move(l)), right(std::move(r)), op(o) {}
    NodePtr<Num> left;
    NodePtr<Num> right;
    BinOps op;
    Num eval(const Env_view<Num>& env) const override;
};

template<class Num>
struct UnarOpNode: public ASTNode<Num>{
    UnarOpNode(NodePtr<Num> c, UnarOps o) noexcept : child(std::move(c)), op(o) {}
    NodePtr<Num> child;
    UnarOps op;
    Num eval(const Env_view<Num>& env) const override;
};

template<class Num>
struct FuncNode: public ASTNode<Num>{
    FuncNode(FuncIds id, std::vector<NodePtr<Num>> a) noexcept : func_id(id), args(std::move(a)) {}
    FuncIds func_id;
    std::vector<NodePtr<Num>> args;
    Num eval(const Env_view<Num>& env) const override;
};

template<class Num>
struct NumNode: public ASTNode<Num>{
    NumNode(Num v) : value(v) {}
    Num value;
    Num eval(const Env_view<Num>& env) const override;
};

template<class Num>
struct ParamNode: public ASTNode<Num>{
    ParamNode(std::string n,const Environment<Num>& env){
        if(env.is_param(n)){
            index = env.get_index(n);
        }else{
            throw std::runtime_error("parameter not found in environment: " + n);
        }
    }
    std::size_t index;
    Num eval(const Env_view<Num>& env) const override;
};

template<class Num>
struct VarNode: public ASTNode<Num>{
    VarNode(std::string& n) {
        if(n == "X" || n == "x"){
            index = state_X;
        }else if(n == "t" || n == "T"){
            index = state_t;
        }else if(n == "dW" || n == "dw "){
            index = state_dW;
        }else{
            throw std::runtime_error("state variable not found in environment: " + n);
        }
    }
    state_vars index;
    Num eval(const Env_view<Num>& env) const override;
};




//pretty thin wrapper around ASTNode, makes other things easier to handle with an obvious owner of the tree
//also prevents copying because you shouldnt need to, so dont try to do that

template<class Num>
class AST{

    NodePtr<Num> root;

    public:
    AST() = default; 
    AST(const AST& in) = delete; // dont use
    AST& operator=(const AST& in) = delete; // dont use
    AST(AST&&) noexcept = default;
    AST& operator=(AST&&) noexcept = default;
    AST(NodePtr<Num> root) noexcept : root(std::move(root)) {}
    
    
    Num eval(const Env_view<Num>& env) const{
        if(!root) throw std::runtime_error("root does not exist");

        return root->eval(env);
    }

};



/*
template<class Num>
Num BinOpNode<Num>::eval(const Env_view<Num>& env) const{
    Num l = left->eval(env);
    Num r = right->eval(env);
    switch(op){
        case BinOps::ADD:
            return l + r;
        case BinOps::SUBTRACT:
            return l - r;
        case BinOps::MULTIPLY:
            return l * r;
        case BinOps::DIVIDE:
            if(r == 0) throw std::runtime_error("division by zero");
            return l / r;
        case BinOps::POWER:
            return std::pow(l, r);
        //default:
            //throw std::runtime_error("unknown binary operator: " + op);
    }

}

template<class Num>
Num UnarOpNode<Num>::eval(const Env_view<Num>& env) const{
    Num operand = child->eval(env);

    switch(op){
        case UnarOps::NEGATE:
            return -operand;
        //default:
            //throw std::runtime_error("unknown unary operator: " + op);
    }
}

template<class Num>
inline Num NumNode<Num>::eval(const Env_view<Num>& env) const{
    return value;
}


template<class Num>
Num FuncNode<Num>::eval(const Env_view<Num>& env) const{
    std::array<Num, args.size()> evaluated_args;
    for(const NodePtr<Num>& arg : args){
        if(!arg){
            throw std::runtime_error("function argument is null");
        }
        evaluated_args.push_back(arg->eval(env));
    }
    switch(func_id){
        case FuncIds::LOG:
            if(evaluated_args.size() != 1) throw std::runtime_error("log function takes 1 argument");
            return std::log(evaluated_args[0]);
        case FuncIds::EXP:
            if(evaluated_args.size() != 1) throw std::runtime_error("exp function takes 1 argument");
            return exp(evaluated_args[0]);
        case FuncIds::SQRT:
            if(evaluated_args.size() != 1) throw std::runtime_error("sqrt function takes 1 argument");
            return std::sqrt(evaluated_args[0]);
        case FuncIds::SIN:
            if(evaluated_args.size() != 1) throw std::runtime_error("sin function takes 1 argument");
            return std::sin(evaluated_args[0]);
        case FuncIds::COS:
            if(evaluated_args.size() != 1) throw std::runtime_error("cos function takes 1 argument");
            return std::cos(evaluated_args[0]);
        case FuncIds::TAN:
            if(evaluated_args.size() != 1) throw std::runtime_error("tan function takes 1 argument");
            return std::tan(evaluated_args[0]);
        case FuncIds::ABS:
            if(evaluated_args.size() != 1) throw std::runtime_error("abs function takes 1 argument");
            return std::abs(evaluated_args[0]);
        case FuncIds::MAX:
            return *std::max_element(evaluated_args.begin(), evaluated_args.end());
        case FuncIds::MIN:
            return *std::min_element(evaluated_args.begin(), evaluated_args.end());
    }
}

template<class Num>
Num ParamNode<Num>::eval(const Env_view<Num>& env) const{
    return env.parameters[index];
}

template<class Num>
Num VarNode<Num>::eval(const Env_view<Num>& env) const{
    return env.state_vars[index];
}

*/