#include "AST.hpp"


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


template<class Num, std::size_t N>
Num FuncNode<Num, N>::eval(const Env_view<Num>& env) const{
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