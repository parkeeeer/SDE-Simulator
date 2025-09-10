# pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <cstdint>
#include <stdexcept>

#include "Variable_Environment.hpp"
#include "token.hpp"
#include "AST.hpp"

template<class Num>
class Parser{
    public:
    Parser(const Environment<Num>& env, std::vector<Token>toks) noexcept : env(env), tokens(std::move(toks)), curr_pos(0) {}
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
        if(num) return num->value;
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
        return std::nullopt;
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




/*
template<class Num>
NodePtr<Num> Parser<Num>::parse_expression(int min_prec){
    NodePtr<Num> left = parse_prefix();

    while(true){
        const Token& next = peek();
        int prec = get_prec(next.type);
        if(prec < min_prec) break;

        get();

        int next_prec = is_right_associative(next.type) ? prec : prec + 1;
        NodePtr<Num> right = parse_expression(next_prec);
        
        left = parse_binop_or_fold(std::move(left), std::move(right), next.type);
    }
    return left;
}

template<class Num>
NodePtr<Num> Parser<Num>::parse_binop_or_fold(NodePtr<Num> left, NodePtr<Num> right, TokenType op){
    std::optional<Num> a = as_num(left);
    std::optional<Num> b = as_num(right);
    if(a && b){
        //can be folded
        switch(op){
            case TokenType::PLUS:
                return std::make_unique<NumNode<Num>>(*a + *b);
            case TokenType::MINUS:
                return std::make_unique<NumNode<Num>>(*a - *b);
            case TokenType::MULTIPLY:
                return std::make_unique<NumNode<Num>>(*a * *b);
            case TokenType::DIVIDE:
                if(*b == 0) throw std::runtime_error("division by zero");
                return std::make_unique<NumNode<Num>>(*a / *b);
            case TokenType::EXP:
                return std::make_unique<NumNode<Num>>(std::pow(*a, *b));
            default:
                throw std::runtime_error("unknown binary operator: " + std::to_string(static_cast<int>(op)));
        }
    }else{
        //cant be folded
        switch(op){
            case TokenType::PLUS:
                return std::make_unique<BinOpNode<Num>>(std::move(left), std::move(right), BinOps::ADD);
            case TokenType::MINUS:
                return std::make_unique<BinOpNode<Num>>(std::move(left), std::move(right), BinOps::SUBTRACT);
            case TokenType::MULTIPLY:
                return std::make_unique<BinOpNode<Num>>(std::move(left), std::move(right), BinOps::MULTIPLY);
            case TokenType::DIVIDE:
                return std::make_unique<BinOpNode<Num>>(std::move(left), std::move(right), BinOps::DIVIDE);
            case TokenType::EXP:
                return std::make_unique<BinOpNode<Num>>(std::move(left), std::move(right), BinOps::POWER);
            default:
                throw std::runtime_error("unknown binary operator: " + std::to_string(static_cast<int>(op)));
        }
    }
}

template<class Num>
NodePtr<Num> Parser<Num>::parse_unary_or_fold(NodePtr<Num> child, TokenType op){
    std::optional<Num> a = as_num(child);
    if(a){
        //can be folded
        switch(op){
            case TokenType::MINUS:
                return std::make_unique<NumNode<Num>>(-*a);
            default:
                throw std::runtime_error("unknown unary operator: " + std::to_string(static_cast<int>(op)));
        }
    }else{
        //cant be folded
        switch(op){
            case TokenType::MINUS:
                return std::make_unique<UnarOpNode<Num>>(std::move(child), UnarOps::NEGATE);
            default:
                throw std::runtime_error("unknown unary operator: " + std::to_string(static_cast<int>(op)));
        }
    }
}

template<class Num>
NodePtr<Num> Parser<Num>::parse_prefix(){
    const Token& next = peek();
    switch(next.type){
        case TokenType::PLUS: {
            get();
            return parse_expression(5);
        }
        case TokenType::MINUS: {
            get();
            NodePtr<Num> right = parse_expression(5);
            return parse_unary_or_fold(std::move(right), TokenType::MINUS);
        }
        case TokenType::NUMBER: {
            Num v = static_cast<Num>(std::stod(get().value));  //this line only designed to work for floating point types(float and double)
            return std::make_unique<NumNode<Num>>(v);
        }
        case TokenType::IDENTIFIER: {
            return parse_ident();
        }
        case TokenType::LPAREN: {
            get();
            NodePtr<Num> expr = parse_expression(0);
            expect(TokenType::RPAREN); get();
            return expr;
        }
        default:
            throw std::runtime_error("unexpected token at start of expression");
    }
}

template<class Num>
NodePtr<Num> Parser<Num>::parse_ident(){
    std::string name = get().value;
    if(peek().type == TokenType::LPAREN){
        //function call
        //TODO: add folding support for functions with constant arguments
        std::optional<FuncIds> id_opt = func_map(name);
        if (!id_opt) throw std::runtime_error("unknown function: " + name);
        FuncIds id = *id_opt;
        get();
        std::vector<NodePtr<Num>> args;
        if(peek().type != TokenType::RPAREN){
            while(true){
                args.push_back(parse_expression(0));
                if(peek().type == TokenType::COMMA){
                    get();
                }else{
                    break;
                }
            }
            expect(TokenType::RPAREN); get();
            return std::make_unique<FuncNode<Num>>(id, std::move(args));
        }

    }else if(env.is_param(name)){
        return std::make_unique<NumNode<Num>>(env.get_param(name));
    }
    return std::make_unique<VarNode<Num>>(name);
    
}
*/