# pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <cstdint>
#include <stdexcept>

#include "utils.hpp"

#include "Variable_Environment.hpp"
#include "token.hpp"
#include "AST.hpp"

namespace sde::frontend {

template<class Num>
class Parser{
    public:
    Parser() = delete;
    Parser(const Environment<Num>& env, std::vector<Token>&& toks) noexcept
    : env(env), 
    tokens(toks),
    curr_pos(0) {}
    
    
    AST<Num> parse(){
        return AST<Num>(parse_expression(0));
    }

    private:
    const std::vector<Token> tokens;
    size_t curr_pos;
    const Environment<Num>& env;


    //-----utils-----
    inline const Token& peek() const{
        return tokens.at(curr_pos);
    }
    inline Token get(){
        if(curr_pos >= tokens.size()) throw std::runtime_error("attempt to get token out of range");
        return tokens.at(curr_pos++);
    }
    inline void expect(TokenType t) const{
        if(peek().type != t) throw std::runtime_error("unexpected token: expected " + std::to_string(static_cast<int>(t)) + " but got " + std::to_string(static_cast<int>(peek().type)));
    }

    //checks if num is a Numnode and returns the value of it if so, otherwise returns nullopt
    inline std::optional<Num> as_num(NodePtr<Num>& n) const {
        auto num = dynamic_cast<NumNode<Num>*>(n.get());
        if(num) return *num->value;
        return std::nullopt;
    }
    inline int get_prec(const TokenType& t) const{
        switch(t){
            case TokenType::PLUS:
            case TokenType::MINUS:
                return 10;
            case TokenType::MULTIPLY:
            case TokenType::DIVIDE:
                return 20;
            case TokenType::EXP:
                return 30;
            default:
                return -1; //not a binary operator
        }
    }
    inline bool is_right_associative(const TokenType& t) const{
        return t == TokenType::EXP;
    }
    std::optional<FuncIds> func_map(const std::string& name){
        if(name == "log") return FuncIds::LOG;
        if(name == "exp") return FuncIds::EXP;
        if(name == "sqrt") return FuncIds::SQRT;
        if(name == "sin") return FuncIds::SIN;
        if(name == "cos") return FuncIds::COS;
        if(name == "tan") return FuncIds::TAN;
        if(name == "abs") return FuncIds::ABS;
        if(name == "max") return FuncIds::MAX;
        if(name == "min") return FuncIds::MIN;
        if (name == "cosh") return FuncIds::COSH;
        if (name == "sinh") return FuncIds::SINH;
        if (name == "tanh") return FuncIds::TANH;
        return std::nullopt;
    }

    size_t get_expected_num_args(FuncIds id){
        switch(id){
            case FuncIds::LOG:
            case FuncIds::EXP:
            case FuncIds::SQRT:
            case FuncIds::SIN:
            case FuncIds::COS:
            case FuncIds::TAN:
            case FuncIds::ABS:
            case FuncIds::COSH:
            case FuncIds::SINH:
            case FuncIds::TANH:
                return 1;
            case FuncIds::MAX:
            case FuncIds::MIN:
                return 2;
            default:
                throw std::runtime_error("unknown function id: " + std::to_string(static_cast<int>(id)));
        }
    }
    //---------------


    //-----parsing functions-----
    NodePtr<Num> parse_expression(int min_prec);
    NodePtr<Num> parse_binop_or_fold(NodePtr<Num> left, NodePtr<Num> right, TokenType op);
    NodePtr<Num> parse_unary_or_fold(NodePtr<Num> child, TokenType op);
    NodePtr<Num> parse_prefix();
    NodePtr<Num> parse_ident();
    //---------------------------
};

}