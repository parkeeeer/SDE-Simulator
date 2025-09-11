#pragma once

#include <vector>
#include <cmath>

#include "bytecode.hpp"
#include "../Parser/Variable_Environment.hpp"

template<class Num>
class Interpreter{
    public:
    static Num run(const Program<Num>& program, const Env_view<Num>& env){
        std::vector<Num> stack;
        stack.reserve(128);
        for(const auto& inst: program.instrs){
            switch(inst.op){
                case operation::PUSH_CONST:
                    stack.push_back(program.constants[inst.arg]);
                    break;
                case operation::LOAD_STATE:
                    stack.push_back(env.state[inst.arg]);
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
};