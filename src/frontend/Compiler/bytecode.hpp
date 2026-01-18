#pragma once

#include <vector>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <utility>
#include "math.hpp"



namespace sde::frontend {

enum class operation: uint32_t {
    PUSH_CONST, LOAD_X, LOAD_T,


    ADD, SUB, MUL, DIV, POW,

    NEGATE, ABS, SIN, COS, TAN, SINH, COSH, TANH, EXP, LOG, SQRT,

    MAX, MIN, LSE_MAX, LSE_MIN, SOFTMAX,

    DONE
};


struct instruction{
    operation op;
    uint32_t arg;
};

template<concepts::fp_or_simd Num>
struct Program{
    std::vector<instruction> instrs;
    std::vector<Num> constants;
    uint32_t max_stack_size = 0;
    
    
    bool operator==(const Program& other) const {
        if (instrs.size() != other.instrs.size() || constants.size() != other.constants.size()) {
            return false;
        }
        for (size_t i = 0; i < instrs.size(); ++i) {
            if (instrs[i].op != other.instrs[i].op || instrs[i].arg != other.instrs[i].arg) {
                return false;
            }
        }
        for (size_t i = 0; i < constants.size(); ++i) {
            if (constants[i] != other.constants[i]) {
                return false;
            }
        }
        return true;
    }

    Num run(Num X, Num t, Num* stack) const noexcept{
        #ifdef __GNUC__
        static void* labels[] = {
            &&op_PUSH_CONST, &&op_LOAD_X, &&op_LOAD_T,
            &&op_ADD, &&op_SUB, &&op_MUL, &&op_DIV, &&op_POW,
            &&op_NEGATE, &&op_ABS, &&op_SIN, &&op_COS, &&op_TAN, &&op_SINH, &&op_COSH, &&op_TANH,
            &&op_EXP, &&op_LOG, &&op_SQRT,
            &&op_MAX, &&op_MIN, &&op_LSE_MAX, &&op_LSE_MIN, &&op_SOFTMAX,
            &&op_DONE
        };
        
        const instruction* instructs = instrs.data();

        #define DISPATCH() goto *labels[std::to_underlying(instructs++->op)]

        size_t sp = 0;

        auto PUSH = [&](Num v){stack[sp++] = v;};
        auto POP = [&]() -> Num {return stack[--sp];};

        DISPATCH();

        op_PUSH_CONST:
            PUSH(constants[(instructs - 1)->arg]);
            DISPATCH();

        op_LOAD_X:
            PUSH(X);
            DISPATCH();

        op_LOAD_T:
            PUSH(t);
            DISPATCH();
        
        op_ADD:{
            Num a = POP();
            Num b = POP();
            PUSH(b + a);
            DISPATCH();
        }

        op_SUB:{
            Num a = POP();
            Num b = POP();
            PUSH(b - a);
            DISPATCH();
        }

        op_MUL:{
            Num a = POP();
            Num b = POP();
            PUSH(b * a);
            DISPATCH();
        }

        op_DIV:{
            Num a = POP();
            Num b = POP();
            PUSH(b / a);
            DISPATCH();
        }

        op_POW:{
            Num a = POP();
            Num b = POP();
            PUSH(math::pow(b, a));
            DISPATCH();
        }

        op_NEGATE:{
            Num a = POP();
            PUSH(-a);
            DISPATCH();
        }

        op_ABS:{
            Num a = POP();
            PUSH(math::abs(a));
            DISPATCH();
        }

        op_SIN:{
            Num a = POP();
            PUSH(math::sin(a));
            DISPATCH();
        }

        op_COS:{
            Num a = POP();
            PUSH(math::cos(a));
            DISPATCH();
        }

        op_TAN:{
            Num a = POP();
            PUSH(math::tan(a));
            DISPATCH();
        }

        op_SINH:{
            Num a = POP();
            PUSH(math::sinh(a));
            DISPATCH();
        }

        op_COSH:{
            Num a = POP();
            PUSH(math::cosh(a));
            DISPATCH();
        }

        op_TANH:{
            Num a = POP();
            PUSH(math::tanh(a));
            DISPATCH();
        }

        op_EXP:{
            Num a = POP();
            PUSH(math::exp(a));
            DISPATCH();
        }

        op_LOG:{
            Num a = POP();
            PUSH(math::log(a));
            DISPATCH();
        }

        op_SQRT:{
            Num a = POP();
            PUSH(math::sqrt(a));
            DISPATCH();
        }

        op_MAX:{
            Num a = POP();
            Num b = POP();
            PUSH(math::max(b, a));
            DISPATCH();
        }

        op_MIN:{
            Num a = POP();
            Num b = POP();
            PUSH(math::min(b, a));
            DISPATCH();
        }

        op_LSE_MAX:{
            Num a = POP();
            Num b = POP();
            PUSH(math::lse_max(b,a));
            DISPATCH();
        }

        op_LSE_MIN:{
            Num a = POP();
            Num b = POP();
            PUSH(math::lse_min(b,a));
            DISPATCH();
        }

        op_SOFTMAX:{
            Num a = POP();
            Num b = POP();
            PUSH(math::softmax_weight(b,a));
            DISPATCH();
        }

        op_DONE:{
            Num ret = POP();
            return ret;
        }
        #undef DISPATCH
        #else
        std::vector<Num> stack;
        stack.reserve(max_stack_size);
        for(const auto& inst: instrs){
            switch(inst.op){
                case operation::PUSH_CONST:
                    stack.push_back(constants[inst.arg]);
                    break;
                case operation::LOAD_X:
                    stack.push_back(X);
                    break;
                case operation::LOAD_T:
                    stack.push_back(t);
                    break;
                case operation::ADD:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(b + a);
                    break;
                }
                case operation::SUB:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(b - a);
                    break;
                }
                case operation::MUL:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(b * a);
                    break;
                }
                case operation::DIV:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(b / a);
                    break;
                }
                case operation::POW:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::pow(b, a));
                    break;
                }
                case operation::NEGATE:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(-a);
                    break;
                }
                case operation::ABS:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::abs(a));
                    break;
                }
                case operation::SIN:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::sin(a));
                    break;
                }
                case operation::COS:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::cos(a));
                    break;
                }
                case operation::TAN:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::tan(a));
                    break;
                }
                case operation::SINH: {
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::sinh(a));
                    break;
                }
                case operation::COSH: {
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::cosh(a));
                    break;
                }
                case operation::TANH: {
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::tanh(a));
                    break;
                }
                case operation::EXP:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::exp(a));
                    break;
                }
                case operation::LOG:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::log(a));
                    break;
                }
                case operation::SQRT:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::sqrt(a));
                    break;
                }
                case operation::MAX:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::max(a, b));
                    break;
                }
                case operation::MIN: {
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::min(a, b));
                    break;
                }
                case operation::LSE_MAX: {
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::lse_max(b, a));
                }
                case operation::LSE_MIN: {
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::lse_min(b, a));
                }
                case operation::SOFTMAX: {
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::softmax_weight(b, a));
                }
                case operation::DONE:
                    break;
            }
        }
        Num ret = stack.back();
        stack.pop_back();
        return ret;
        #endif
    }

    Num safe_run(Num X, Num t, Num* stack) const noexcept{
        #ifdef __GNUC__
        static void* labels[] = {
            &&op_PUSH_CONST, &&op_LOAD_X, &&op_LOAD_T,
            &&op_ADD, &&op_SUB, &&op_MUL, &&op_DIV, &&op_POW,
            &&op_NEGATE, &&op_ABS, &&op_SIN, &&op_COS, &&op_TAN, &&op_SINH, &&op_COSH, &&op_TANH,
            &&op_EXP, &&op_LOG, &&op_SQRT,
            &&op_MAX, &&op_MIN, &&op_LSE_MAX, &&op_LSE_MIN, &&op_SOFTMAX,
            &&op_DONE
        };
        
        const instruction* instructs = instrs.data();

        #define DISPATCH() goto *labels[std::to_underlying(instructs++->op)]

        size_t sp = 0;

        auto PUSH = [&](Num v){stack[sp++] = v;};
        auto POP = [&]() -> Num {return stack[--sp];};

        DISPATCH();

        op_PUSH_CONST:
            PUSH(constants[(instructs - 1)->arg]);
            DISPATCH();

        op_LOAD_X:
            PUSH(X);
            DISPATCH();

        op_LOAD_T:
            PUSH(t);
            DISPATCH();
        
        op_ADD:{
            Num a = POP();
            Num b = POP();
            PUSH(b + a);
            DISPATCH();
        }

        op_SUB:{
            Num a = POP();
            Num b = POP();
            PUSH(b - a);
            DISPATCH();
        }

        op_MUL:{
            Num a = POP();
            Num b = POP();
            PUSH(b * a);
            DISPATCH();
        }

        op_DIV:{
            Num a = POP();
            Num b = POP();
            PUSH(math::safe_div(b, a));
            DISPATCH();
        }

        op_POW:{
            Num a = POP();
            Num b = POP();
            PUSH(math::pow(b, a));
            DISPATCH();
        }

        op_NEGATE:{
            Num a = POP();
            PUSH(-a);
            DISPATCH();
        }

        op_ABS:{
            Num a = POP();
            PUSH(math::abs(a));
            DISPATCH();
        }

        op_SIN:{
            Num a = POP();
            PUSH(math::sin(a));
            DISPATCH();
        }

        op_COS:{
            Num a = POP();
            PUSH(math::cos(a));
            DISPATCH();
        }

        op_TAN:{
            Num a = POP();
            PUSH(math::tan(a));
            DISPATCH();
        }

        op_SINH:{
            Num a = POP();
            PUSH(math::sinh(a));
            DISPATCH();
        }

        op_COSH:{
            Num a = POP();
            PUSH(math::cosh(a));
            DISPATCH();
        }

        op_TANH:{
            Num a = POP();
            PUSH(math::tanh(a));
            DISPATCH();
        }

        op_EXP:{
            Num a = POP();
            PUSH(math::exp(a));
            DISPATCH();
        }

        op_LOG:{
            Num a = POP();
            PUSH(math::safe_log(a));
            DISPATCH();
        }

        op_SQRT:{
            Num a = POP();
            PUSH(math::safe_sqrt(a));
            DISPATCH();
        }

        op_MAX:{
            Num a = POP();
            Num b = POP();
            PUSH(math::max(b, a));
            DISPATCH();
        }

        op_MIN:{
            Num a = POP();
            Num b = POP();
            PUSH(math::min(b, a));
            DISPATCH();
        }

        op_LSE_MAX:{
            Num a = POP();
            Num b = POP();
            PUSH(math::lse_max(b,a));
            DISPATCH();
        }

        op_LSE_MIN:{
            Num a = POP();
            Num b = POP();
            PUSH(math::lse_min(b,a));
            DISPATCH();;
        }

        op_SOFTMAX:{
            Num a = POP();
            Num b = POP();
            PUSH(math::softmax_weight(b,a));
            DISPATCH();
        }

        op_DONE:{
            Num ret = POP();
            return ret;
        }
        #undef DISPATCH
        #else
        std::vector<Num> stack;
        stack.reserve(max_stack_size);
        for(const auto& inst: instrs){
            switch(inst.op){
                case operation::PUSH_CONST:
                    stack.push_back(constants[inst.arg]);
                    break;
                case operation::LOAD_X:
                    stack.push_back(X);
                    break;
                case operation::LOAD_T:
                    stack.push_back(t);
                    break;
                case operation::ADD:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(b + a);
                    break;
                }
                case operation::SUB:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(b - a);
                    break;
                }
                case operation::MUL:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(b * a);
                    break;
                }
                case operation::DIV:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::safe_div(b, a));
                    break;
                }
                case operation::POW:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::pow(b, a));
                    break;
                }
                case operation::NEGATE:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(-a);
                    break;
                }
                case operation::ABS:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::abs(a));
                    break;
                }
                case operation::SIN:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::sin(a));
                    break;
                }
                case operation::COS:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::cos(a));
                    break;
                }
                case operation::TAN:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::tan(a));
                    break;
                }
                case operation::SINH: {
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::sinh(a));
                    break;
                }
                case operation::COSH: {
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::cosh(a));
                    break;
                }
                case operation::TANH: {
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::tanh(a));
                    break;
                }
                case operation::EXP:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::exp(a));
                    break;
                }
                case operation::LOG:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::safe_log(a));
                    break;
                }
                case operation::SQRT:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(math::safe_sqrt(a));
                    break;
                }
                case operation::MAX:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::max(a, b));
                    break;
                }
                case operation::MIN:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::min(a, b));
                    break;
                }
                case operation::LSE_MAX: {
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::lse_max(b, a));
                }
                case operation::LSE_MIN: {
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::lse_min(b, a));
                }
                case operation::SOFTMAX: {
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(math::softmax_weight(b, a));
                }
                case operation::DONE:
                    break;
            }
        }
        Num ret = stack.back();
        stack.pop_back();
        return ret;
        #endif
    }




    void compute_max_stack_size(){
        std::uint32_t max_size = 0;
        std::uint32_t current_size = 0;
        for(const auto& inst: instrs){
            switch(inst.op){
                case operation::PUSH_CONST:
                case operation::LOAD_X:
                case operation::LOAD_T:
                    current_size += 1;
                    break;
                case operation::ADD:
                case operation::SUB:
                case operation::MUL:
                case operation::DIV:
                case operation::POW:
                case operation::MAX:
                case operation::MIN:
                case operation::LSE_MAX:
                case operation::LSE_MIN:
                case operation::SOFTMAX:
                    current_size -= 1;
                    break;
                case operation::NEGATE:
                case operation::ABS:
                case operation::SIN:
                case operation::COS:
                case operation::TAN:
                case operation::EXP:
                case operation::LOG:
                case operation::SQRT:
                case operation::DONE:
                    break;
            }
            if(current_size > max_size) max_size = current_size;
        }
        max_stack_size = max_size;
    }

    inline size_t get_max_stack_size() const {return max_stack_size;}

    void print() const {
        for(const auto& inst: instrs){
            std::cout << std::to_underlying(inst.op) << " " << inst.arg << ", ";
        }
    }
};


} // namespace sde::frontend