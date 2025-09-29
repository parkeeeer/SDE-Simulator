#include "AST.hpp"

using namespace sde::frontend;

template<class Num>
Num BinOpNode<Num>::eval(Num dW, Num X, Num t) const{
    Num l = left->eval(dW, X, t);
    Num r = right->eval(dW, X, t);
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
Num UnarOpNode<Num>::eval(Num dW, Num X, Num t) const{
    Num operand = child->eval(dW, X, t);

    switch(op){
        case UnarOps::NEGATE:
            return -operand;
        //default:
            //throw std::runtime_error("unknown unary operator: " + op);
    }
}

template<class Num>
inline Num NumNode<Num>::eval(Num dW, Num X, Num t) const{
    return value;
}


template<class Num>
Num FuncNode<Num>::eval(Num dW, Num X, Num t) const{
    switch(func_id){
        case FuncIds::LOG:
            if(args.size() != 1) throw std::runtime_error("log function takes 1 argument");
            return std::log(args[0]->eval(dW, X, t));
        case FuncIds::EXP:
            if(args.size() != 1) throw std::runtime_error("exp function takes 1 argument");
            return exp(args[0]->eval(dW, X, t));
        case FuncIds::SQRT:
            if(args.size() != 1) throw std::runtime_error("sqrt function takes 1 argument");
            return std::sqrt(args[0]->eval(dW, X, t));
        case FuncIds::SIN:
            if(args.size() != 1) throw std::runtime_error("sin function takes 1 argument");
            return std::sin(args[0]->eval(dW, X, t));
        case FuncIds::COS:
            if(args.size() != 1) throw std::runtime_error("cos function takes 1 argument");
            return std::cos(args[0]->eval(dW, X, t));
        case FuncIds::TAN:
            if(args.size() != 1) throw std::runtime_error("tan function takes 1 argument");
            return std::tan(args[0]->eval(dW, X, t));
        case FuncIds::ABS:
            if(args.size() != 1) throw std::runtime_error("abs function takes 1 argument");
            return std::abs(args[0]->eval(dW, X, t));
        case FuncIds::MAX:
            if(args.size() != 2) throw std::runtime_error("max function takes 2 arguments");
            return std::max(args[0]->eval(dW, X, t), args[1]->eval(dW, X, t));
        case FuncIds::MIN:
            if(args.size() != 2) throw std::runtime_error("min function takes 2 arguments");
            return std::min(args[0]->eval(dW, X, t), args[1]->eval(dW, X, t));
    }
}

/*
template<class Num>
Num ParamNode<Num>::eval(const Env_view<Num>& env) const{
    return env.parameters[index];
}
*/

template<class Num>
Num VarNode<Num>::eval(Num dW, Num X, Num t) const{
    switch(index){
        case state_X:
            return X;
        case state_t:
            return t;
        case state_dW:
            return dW;
    };
}

template class BinOpNode<double>;
template class UnarOpNode<double>;
template class FuncNode<double>;
template class NumNode<double>;
//template class ParamNode<double>;
template class VarNode<double>;

template class BinOpNode<float>;
template class UnarOpNode<float>;
template class FuncNode<float>;
template class NumNode<float>;
//template class ParamNode<float>;
template class VarNode<float>;