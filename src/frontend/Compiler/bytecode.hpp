#pragma once

#include <vector>
#include <cstdint>
#include <cmath>

enum class operation{
    PUSH_CONST, LOAD_DW, LOAD_X, LOAD_T,


    ADD, SUB, MUL, DIV, POW,


    NEGATE, ABS, SIN, COS, TAN, EXP, LOG, SQRT,

    MAX, MIN
};


struct instruction{
    operation op;
    uint32_t arg;
};

template<class Num>
struct Program{
    std::vector<instruction> instrs;
    std::vector<Num> constants;
    uint32_t max_stack_size;
    
    
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

    Num run(Num t, Num X, Num dW){
        std::vector<Num> stack;
        stack.reserve(max_stack_size);
        for(const auto& inst: instrs){
            switch(inst.op){
                case operation::PUSH_CONST:
                    stack.push_back(constants[inst.arg]);
                    break;
                case operation::LOAD_DW:
                    stack.push_back(dW);
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
                }
                case operation::POW:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(std::pow(b, a));
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
                    stack.push_back(std::abs(a));
                    break;
                }
                case operation::SIN:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(std::sin(a));
                    break;
                }
                case operation::COS:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(std::cos(a));
                    break;
                }
                case operation::TAN:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(std::tan(a));
                    break;
                }
                case operation::EXP:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(std::exp(a));
                    break;
                }
                case operation::LOG:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(std::log(a));
                    break;
                }
                case operation::SQRT:{
                    Num a = stack.back();
                    stack.pop_back();
                    stack.push_back(std::sqrt(a));
                    break;
                }
                case operation::MAX:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(std::max(a, b));
                    break;
                }
                case operation::MIN:{
                    Num a = stack.back();
                    stack.pop_back();
                    Num b = stack.back();
                    stack.pop_back();
                    stack.push_back(std::min(a, b));
                    break;
                }
            }
        }
        return stack.back();
    }


    void compute_max_stack_size(){
        std::uint32_t max_size = 0;
        std::uint32_t current_size = 0;
        for(const auto& inst: instrs){
            switch(inst.op){
                case operation::PUSH_CONST:
                case operation::LOAD_DW:
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
                    break;
            }
            if(current_size > max_size) max_size = current_size;
        }
        max_stack_size = max_size;
    }
};