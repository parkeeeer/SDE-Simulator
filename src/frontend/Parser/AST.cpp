#include "AST.hpp"
#include "math.hpp"

#include <experimental/bits/simd.h>

using namespace sde::frontend;

template<class Num>
Num BinOpNode<Num>::eval(Num X, Num t) const{
    Num l = left->eval(X, t);
    Num r = right->eval(X, t);
    switch(op){
        case BinOps::ADD:
            return l + r;
        case BinOps::SUBTRACT:
            return l - r;
        case BinOps::MULTIPLY:
            return l * r;
        case BinOps::DIVIDE:
            return l / r;
        case BinOps::POWER:
            return math::pow(l, r);
        //default:
            //throw std::runtime_error("unknown binary operator: " + op);
    }

}

template<class Num>
Num UnarOpNode<Num>::eval(Num X, Num t) const{
    Num operand = child->eval(X, t);

    switch(op){
        case UnarOps::NEGATE:
            return -operand;
        //default:
            //throw std::runtime_error("unknown unary operator: " + op);
    }
}

template<class Num>
inline Num NumNode<Num>::eval(Num X, Num t) const{
    return *value;
}


template<class Num>
Num FuncNode<Num>::eval(Num X, Num t) const{
    switch(func_id){
        case FuncIds::LOG:
            return math::log(args[0]->eval(X, t));
        case FuncIds::EXP:
            return math::exp(args[0]->eval(X, t));
        case FuncIds::SQRT:
            return math::sqrt(args[0]->eval(X, t));
        case FuncIds::SIN:
            return math::sin(args[0]->eval(X, t));
        case FuncIds::COS:
            return math::cos(args[0]->eval(X, t));
        case FuncIds::TAN:
            return math::tan(args[0]->eval(X, t));
        case FuncIds::ABS:
            return math::abs(args[0]->eval(X, t));
        case FuncIds::MAX:
            return math::max(args[0]->eval(X, t), args[1]->eval(X, t));
        case FuncIds::MIN:
            return math::min(args[0]->eval(X, t), args[1]->eval(X, t));
        case FuncIds::LSE_MAX:
            return math::lse_max(args[0]->eval(X, t), args[1]->eval(X, t));
        case FuncIds::LSE_MIN:
            return math::lse_min(args[0]->eval(X, t), args[1]->eval(X, t));
        case FuncIds::SOFTMAX:
            return math::softmax_weight(args[0]->eval(X, t), args[1]->eval(X, t));
    }
}

/*
template<class Num>
Num ParamNode<Num>::eval(const Env_view<Num>& env) const{
    return env.parameters[index];
}
*/

template<class Num>
Num VarNode<Num>::eval(Num X, Num t) const{
    switch(index){
        case state_X:
            return X;
        case state_t:
            return t;
    };
}

template<class Num>
Num BinOpNode<Num>::safe_eval(Num X, Num t) const {
    Num l = left->safe_eval(X, t);
    Num r = right->safe_eval(X, t);
    switch (op) {
        case BinOps::ADD:
            return l + r;
            break;
        case BinOps::SUBTRACT:
            return l - r;
            break;
        case BinOps::MULTIPLY:
            return l * r;
            break;
        case BinOps::DIVIDE:
            return math::safe_div(l, r);
        case BinOps::POWER:
            return math::pow(l, r);

    }
}

template<class Num>
Num UnarOpNode<Num>::safe_eval(Num X, Num t) const {
    Num operand = child->safe_eval(X, t);
    switch (op) {
        case UnarOps::NEGATE:
            return -operand;
    }
}

template<class Num>
Num NumNode<Num>::safe_eval(Num X, Num t) const {
    return *value;
}

template<class Num>
Num FuncNode<Num>::safe_eval(Num X, Num t) const {
    switch (func_id) {
        case FuncIds::LOG:
            return math::safe_log(args[0]->safe_eval(X, t));
        case FuncIds::EXP:
            return math::exp(args[0]->safe_eval(X, t));
        case FuncIds::SQRT:
            return math::safe_sqrt(args[0]->safe_eval(X,t));
        case FuncIds::SIN:
            return math::sin(args[0]->safe_eval(X, t));
        case FuncIds::COS:
            return math::cos(args[0]->safe_eval(X, t));
        case FuncIds::TAN:
            return math::tan(args[0]->safe_eval(X, t));
        case FuncIds::ABS:
            return math::abs(args[0]->safe_eval(X,t));
        case FuncIds::MAX:
            return math::max(args[0]->safe_eval(X, t), args[1]->safe_eval(X, t));
        case FuncIds::MIN:
            return math::min(args[0]->safe_eval(X, t), args[1]->safe_eval(X, t));
        case FuncIds::LSE_MAX:
            return math::lse_max(args[0]->safe_eval(X, t), args[1]->safe_eval(X, t));
        case FuncIds::LSE_MIN:
            return math::lse_min(args[0]->safe_eval(X, t), args[1]->safe_eval(X, t));
        case FuncIds::SOFTMAX:
            return math::softmax_weight(args[0]->safe_eval(X,t), args[1]->safe_eval(X,t));
    }
}

template<class Num>
Num VarNode<Num>::safe_eval(Num X, Num t) const {
    switch (index) {
        case state_X:
            return X;
        case state_t:
            return t;
    }
}

namespace sde::frontend {
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

    template class BinOpNode<std::experimental::native_simd<float>>;
    template class UnarOpNode<std::experimental::native_simd<float>>;
    template class FuncNode<std::experimental::native_simd<float>>;
    template class NumNode<std::experimental::native_simd<float>>;
    template class VarNode<std::experimental::native_simd<float>>;

    template class BinOpNode<std::experimental::native_simd<double>>;
    template class UnarOpNode<std::experimental::native_simd<double>>;
    template class FuncNode<std::experimental::native_simd<double>>;
    template class NumNode<std::experimental::native_simd<double>>;
    template class VarNode<std::experimental::native_simd<double>>;
}