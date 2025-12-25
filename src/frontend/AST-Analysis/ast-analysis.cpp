#include "ast-analysis.hpp"
#include "math.hpp"


using namespace sde;
using namespace std;

template<concepts::fp_or_simd Num>
frontend::NodePtr<Num> clone(const frontend::NodePtr<Num>& node) {
    auto bin = dynamic_cast<const frontend::BinOpNode<Num>*>(node.get());
    if (bin) {
        return make_unique<frontend::BinOpNode<Num>>(clone(bin->left), clone(bin->right), bin->op);
    }
    auto unar = dynamic_cast<const frontend::UnarOpNode<Num>*>(node.get());
    if (unar) {
        return make_unique<frontend::UnarOpNode<Num>>(clone(unar->child), unar->op);
    }
    auto vari = dynamic_cast<const frontend::VarNode<Num>*>(node.get());
    if (vari) {
        if (vari->index == frontend::state_vars::state_X) {
            return make_unique<frontend::VarNode<Num>>("X");
        }else {
            return make_unique<frontend::VarNode<Num>>("t");
        }
    }
    auto func = dynamic_cast<const frontend::FuncNode<Num>*>(node.get());
    if (func) {
        std::vector<frontend::NodePtr<Num>> args;
        for (auto& arg : func->args) {
            args.push_back(clone(arg));
        }
        return make_unique<frontend::FuncNode<Num>>(func->func_id, std::move(args));
    }
    auto num = dynamic_cast<const frontend::NumNode<Num>*>(node.get());
    if (num) {
        return make_unique<frontend::NumNode<Num>>(*(num->value));
    }
}

template<concepts::fp_or_simd Num>
vector<frontend::NodePtr<Num>> make_single_arg(frontend::NodePtr<Num> arg) {
    vector<frontend::NodePtr<Num>> args;
    args.push_back(std::move(arg));
    return args;
}

template<concepts::fp_or_simd Num>
vector<frontend::NodePtr<Num>> make_double_arg(frontend::NodePtr<Num> arg1, frontend::NodePtr<Num> arg2) {
    vector<frontend::NodePtr<Num>> args;
    args.push_back(std::move(arg1));
    args.push_back(std::move(arg2));
    return args;
}

template<concepts::fp_or_simd Num>
frontend::AST<Num> sde::frontend::differentiate (const frontend::AST<Num>& expr, string_view var, bool pedantic) {
    NodePtr<Num> diff = differentiate(expr.get_root(), var, pedantic);
    return AST<Num>(std::move(diff));
}

template<concepts::fp_or_simd Num>
frontend::NodePtr<Num> frontend::differentiate(const NodePtr<Num>& expr, const std::string_view var, bool pedantic) {
    auto bin = dynamic_cast<const BinOpNode<Num>*>(expr.get());
    if (bin) {
        switch (bin->op) {
            case BinOps::ADD:
                return make_unique<BinOpNode<Num>>(differentiate(bin->left, var), differentiate(bin->right, var), BinOps::ADD);
            case BinOps::SUBTRACT:
                return make_unique<BinOpNode<Num>>(differentiate(bin->left, var), differentiate(bin->right, var), BinOps::SUBTRACT);
            case BinOps::MULTIPLY:
                return make_unique<BinOpNode<Num>>(
                    make_unique<BinOpNode<Num>>(differentiate(bin->left, var), clone(bin->right), BinOps::MULTIPLY),
                    make_unique<BinOpNode<Num>>(clone(bin->left), differentiate(bin->right, var), BinOps::MULTIPLY),
                    BinOps::ADD);
            case BinOps::DIVIDE:
                return make_unique<BinOpNode<Num>>( //main divide
                    make_unique<BinOpNode<Num>>(//numeratror subtract
                        make_unique<BinOpNode<Num>>(differentiate(bin->left, var), clone(bin->right), BinOps::MULTIPLY),
                        make_unique<BinOpNode<Num>>(clone(bin->left), differentiate(bin->right, var), BinOps::MULTIPLY),
                        BinOps::SUBTRACT),
                        make_unique<BinOpNode<Num>>(clone(bin->right), clone(bin->right), BinOps::MULTIPLY),BinOps::DIVIDE);
            case BinOps::POWER:
                return make_unique<BinOpNode<Num>>(
                    clone(expr), make_unique<BinOpNode<Num>>(
                        make_unique<BinOpNode<Num>>(differentiate(bin->right, var), make_unique<FuncNode<Num>>(FuncIds::LOG, make_single_arg(clone(bin->left))), BinOps::MULTIPLY),
                        make_unique<BinOpNode<Num>>(clone(bin->right), make_unique<BinOpNode<Num>>(differentiate(bin->left, var), clone(bin->left), BinOps::DIVIDE), BinOps::MULTIPLY),
                        BinOps::ADD), BinOps::MULTIPLY);
        }
    }
    auto unar = dynamic_cast<const UnarOpNode<Num>*>(expr.get());
    if (unar) {
        switch (unar->op) {
            case UnarOps::NEGATE:
                return make_unique<UnarOpNode<Num>>(differentiate(unar->child, var), UnarOps::NEGATE);
        }
    }
    auto vari = dynamic_cast<const VarNode<Num>*>(expr.get());
    if (vari) {
        if (vari->index == state_vars::state_X && (var == "X" || var == "x")) {
            return make_unique<NumNode<Num>>(math::simd_cast<Num>(1.0));
        }
        if (vari->index == state_vars::state_t && (var == "t" || var == "T")) {
            return make_unique<NumNode<Num>>(math::simd_cast<Num>(1.0));
        }
        return make_unique<NumNode<Num>>(math::simd_cast<Num>(0.0));
    }
    auto func = dynamic_cast<const FuncNode<Num>*>(expr.get());
    if (func) {
        switch (func->func_id) {
            case FuncIds::LOG:
                return make_unique<BinOpNode<Num>>(differentiate(func->args[0], var), clone(func->args[0]), BinOps::DIVIDE);
            case FuncIds::EXP:
                return make_unique<BinOpNode<Num>>(clone(expr), differentiate(func->args[0], var), BinOps::MULTIPLY);
            case FuncIds::SQRT:
                return make_unique<BinOpNode<Num>>(differentiate(func->args[0], var),
                    make_unique<BinOpNode<Num>>(make_unique<NumNode<Num>>(math::simd_cast<Num>(2.0)), make_unique<FuncNode<Num>>(FuncIds::SQRT, make_single_arg(clone(func->args[0]))), BinOps::MULTIPLY), BinOps::DIVIDE);
            case FuncIds::SIN:
                return make_unique<BinOpNode<Num>>(differentiate(func->args[0], var), make_unique<FuncNode<Num>>(FuncIds::COS, make_single_arg(clone(func->args[0]))), BinOps::MULTIPLY);
            case FuncIds::COS:
                return make_unique<UnarOpNode<Num>>(make_unique<BinOpNode<Num>>(differentiate(func->args[0], var), make_unique<FuncNode<Num>>(FuncIds::SIN, make_single_arg(clone(func->args[0]))), BinOps::MULTIPLY), UnarOps::NEGATE);
            case FuncIds::TAN:
                return make_unique<BinOpNode<Num>>(differentiate(func->args[0], var), make_unique<BinOpNode<Num>>(make_unique<FuncNode<Num>>(FuncIds::COS, make_single_arg(clone(func->args[0]))), make_unique<FuncNode<Num>>(FuncIds::COS, make_single_arg(clone(func->args[0]))), BinOps::MULTIPLY), BinOps::DIVIDE);
            case FuncIds::ABS:
                if (pedantic) throw std::runtime_error("abs function is not differentiable everywhere");
                return make_unique<BinOpNode<Num>>(
                    make_unique<BinOpNode<Num>>(clone(func->args[0]), make_unique<FuncNode<Num>>(FuncIds::ABS, make_single_arg(clone(func->args[0]))), BinOps::DIVIDE),
                    differentiate(func->args[0], var), BinOps::MULTIPLY);
            case FuncIds::MAX:
                if (pedantic) throw std::runtime_error("max function is not differentiable everywhere");
                return make_unique<BinOpNode<Num>>(
                    make_unique<BinOpNode<Num>>(make_unique<FuncNode<Num>>(FuncIds::SOFTMAX, make_double_arg(clone(func->args[0]), clone(func->args[1]))), differentiate(func->args[0], var), BinOps::MULTIPLY),
                    make_unique<BinOpNode<Num>>(make_unique<FuncNode<Num>>(FuncIds::SOFTMAX, make_double_arg(clone(func->args[1]), clone(func->args[0]))), differentiate(func->args[1], var), BinOps::MULTIPLY),
                    BinOps::ADD);
            case FuncIds::MIN:
                if (pedantic) throw std::runtime_error("min function is not differentiable everywhere");
                return make_unique<BinOpNode<Num>>(
                    make_unique<BinOpNode<Num>>(make_unique<FuncNode<Num>>(FuncIds::SOFTMAX, make_double_arg(clone(func->args[0]), clone(func->args[1]))), differentiate(func->args[1], var), BinOps::MULTIPLY),
                    make_unique<BinOpNode<Num>>(make_unique<FuncNode<Num>>(FuncIds::SOFTMAX, make_double_arg(clone(func->args[1]), clone(func->args[0]))), differentiate(func->args[0], var), BinOps::MULTIPLY),
                    BinOps::ADD);
        }
    }
    auto num = dynamic_cast<const NumNode<Num>*>(expr.get());
    if (num) return make_unique<NumNode<Num>>(math::simd_cast<Num>(0.0));
    throw std::runtime_error("unknown node type, how did you get here?");
}


template frontend::NodePtr<float> sde::frontend::differentiate(const NodePtr<float>&, std::string_view, bool);
template frontend::NodePtr<double> sde::frontend::differentiate(const NodePtr<double>&, std::string_view, bool);
template frontend::NodePtr<stdx::native_simd<float>> sde::frontend::differentiate(const NodePtr<stdx::native_simd<float>>&, std::string_view, bool);
template frontend::NodePtr<stdx::native_simd<double>> sde::frontend::differentiate(const NodePtr<stdx::native_simd<double>>&, std::string_view, bool);

template frontend::AST<float> sde::frontend::differentiate(const frontend::AST<float>&, std::string_view, bool);
template frontend::AST<double> sde::frontend::differentiate(const frontend::AST<double>&, std::string_view, bool);
template frontend::AST<stdx::native_simd<float>> sde::frontend::differentiate(const frontend::AST<stdx::native_simd<float>>&, std::string_view, bool);
template frontend::AST<stdx::native_simd<double>> sde::frontend::differentiate(const frontend::AST<stdx::native_simd<double>>&, std::string_view, bool);