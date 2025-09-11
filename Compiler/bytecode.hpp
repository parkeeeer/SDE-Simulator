#pragma once

#include <vector>
#include <cstdint>

enum class operation{
    PUSH_CONST, LOAD_STATE,


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
};