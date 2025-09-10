#include "lexer.hpp"



std::vector<Token> Lexer::lex(){
    std::vector<Token> tokens{};
    char curr;
    while(curr_pos < in.size()){
        curr = in[curr_pos];
        if(isspace(curr)){
            curr_pos++;
            continue;
        }
        if(isdigit(curr) || curr == '.'){
            std::string num_str;
            bool seen_dot = false;
            bool seen_e = false;
            while(curr_pos < in.size() && (isdigit(in[curr_pos]) || in[curr_pos] == '.' || in[curr_pos] == 'e' || in[curr_pos] == 'E')){
                if(in[curr_pos] == '.'){
                    if(seen_dot){
                        throw std::runtime_error("invalid number: max one decimal point allowed per number");
                    }
                    seen_dot = true;
                }else if(in[curr_pos] == 'e' || in[curr_pos] == 'E'){
                    if(seen_e){
                        throw std::runtime_error("invalid number: max one exponent allowed per number");
                    }
                    if(in[curr_pos + 1] == '+' || in[curr_pos + 1] == '-'){
                        num_str += in[curr_pos];
                        curr_pos++;
                    }
                    seen_e = true;
                }
                num_str += in[curr_pos];
                curr_pos++;
            }
            tokens.emplace_back(TokenType::NUMBER,num_str);
            continue;
        }
        if(isalpha(curr)){
            std::string id_str;
            while(curr_pos < in.size() && (isalpha(in[curr_pos]) || in[curr_pos] == '_')){
                id_str += in[curr_pos];
                curr_pos++;
            }
            tokens.emplace_back(TokenType::IDENTIFIER,id_str);
            continue;
        }
        switch(curr){
            case '+':
                tokens.emplace_back(TokenType::PLUS,"+");
                curr_pos++;
                break;
            case '-':
                tokens.emplace_back(TokenType::MINUS,"-");
                curr_pos++;
                break;
            case '*':
                tokens.emplace_back(TokenType::MULTIPLY,"*");
                curr_pos++;
                break;
            case '/':
                tokens.emplace_back(TokenType::DIVIDE,"/");
                curr_pos++;
                break;
            case '(':
                tokens.emplace_back(TokenType::LPAREN,"(");
                curr_pos++;
                break;
            case ')':
                tokens.emplace_back(TokenType::RPAREN,")");
                curr_pos++;
                break;
            case '^':
                tokens.emplace_back(TokenType::EXP,"^");
                curr_pos++;
                break;
            case ',':
                tokens.emplace_back(TokenType::COMMA,",");
                curr_pos++;
                break;
            default:
                throw std::runtime_error(std::string("unexpected character at position ") + std::to_string(curr_pos) + ": '" + curr + "'");
        }
    }
    tokens.emplace_back(TokenType::END,"");
    return insert_implicit_mul(tokens);
}