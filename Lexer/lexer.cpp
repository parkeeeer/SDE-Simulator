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
        if(isdigit(curr)){
            std::string num_str;
            while(curr_pos < in.size() && isdigit(in[curr_pos])){
                num_str += in[curr_pos];
                curr_pos++;
            }
            tokens.emplace_back(TokenType::NUMBER,num_str);
            continue;
        }
        if(isalpha(curr)){
            tokens.emplace_back(TokenType::VARIABLE,curr);
            curr_pos++;
            continue;
        }
        switch(curr){
            case '+':
                tokens.emplace_back(TokenType::PLUS,'+');
                curr_pos++;
                break;
            case '-':
                tokens.emplace_back(TokenType::MINUS,'-');
                curr_pos++;
                break;
            case '*':
                tokens.emplace_back(TokenType::MULTIPLY,'*');
                curr_pos++;
                break;
            case '/':
                tokens.emplace_back(TokenType::DIVIDE,'/');
                curr_pos++;
                break;
            case '(':
                tokens.emplace_back(TokenType::LPAREN,'(');
                curr_pos++;
                break;
            case ')':
                tokens.emplace_back(TokenType::RPAREN,')');
                curr_pos++;
                break;
            case '^':
                tokens.emplace_back(TokenType::EXP,'^');
                curr_pos++;
                break;
        }
    }
}