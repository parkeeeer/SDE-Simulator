#pragma once

#include <memory>
#include <string>


//#define NodePtr std::unique_ptr<ASTNode>

using NodePtr = std::unique_ptr<ASTNode>;



struct ASTNode{
    virtual ~ASTNode() = default;
    virtual double eval() const = 0;
};


struct BinOpNode:public ASTNode{
    BinOpNode(NodePtr l, NodePtr r, char o) noexcept : 
                                left(std::move(l)), right(std::move(r)), op(o) {}
    NodePtr left;
    NodePtr right;
    char op;
    double eval() const override;
};

struct UnarOpNode: public ASTNode{
    UnarOpNode(NodePtr c, char o) noexcept : child(std::move(c)), op(o) {}
    NodePtr child;
    char op;
    double eval() const override;
};

struct NumNode: public ASTNode{
    NumNode(double v) : value(v) {}
    double value;
    double eval() const override;
};

struct VarNode: public ASTNode{
    VarNode(std::string n) noexcept : name(std::move(n)) {}
    std::string name;
    double eval() const override;
};




class AST{

    NodePtr root;

    public:
    AST() = default; 
    AST(const AST& in) = delete; // dont use
    AST& operator=(const AST& in) = delete; // dont use
    AST(AST&&) noexcept = default;
    AST& operator=(AST&&) noexcept = default;
    AST(NodePtr root) : root(std::move(root)) {}
    double eval() const{
        if(!root) throw std::runtime_error("root does not exist");

        return root->eval();
    }

};