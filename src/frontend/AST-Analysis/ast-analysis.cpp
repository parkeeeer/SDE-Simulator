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



template<sde::concepts::fp_or_simd Num>
frontend::NodePtr<Num> optimize_node(const frontend::NodePtr<Num>& expr, bool& changed) {
    auto bin = dynamic_cast<frontend::BinOpNode<Num>*>(expr.get());
    if (bin != nullptr) {
        switch (bin->op) {
            case frontend::BinOps::ADD: {
                auto num_left = dynamic_cast<frontend::NumNode<Num>*>(bin->left.get());
                auto num_right = dynamic_cast<frontend::NumNode<Num>*>(bin->right.get());
                if (num_left && num_right) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(*num_left->value + *num_right->value);
                }
                if (num_left && math::scalar_extract(*num_left->value) == 0.0) {
                    changed = true;
                    return optimize_node(bin->right, changed);
                }
                if (num_right && math::scalar_extract(*num_right->value) == 0.0) {
                    changed = true;
                    return optimize_node(bin->left, changed);
                }
                return make_unique<frontend::BinOpNode<Num>>(optimize_node(bin->left, changed), optimize_node(bin->right, changed), frontend::BinOps::ADD);
            }
            case frontend::BinOps::SUBTRACT:{
                auto num_left = dynamic_cast<frontend::NumNode<Num>*>(bin->left.get());
                auto num_right = dynamic_cast<frontend::NumNode<Num>*>(bin->right.get());
                if (num_left && num_right) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(*num_left->value - *num_right->value);
                }
                if (num_left && math::scalar_extract(*num_left->value) == 0.0) {
                    changed = true;
                    return make_unique<frontend::UnarOpNode<Num>>(optimize_node(bin->right, changed), frontend::UnarOps::NEGATE);
                }

                if (num_right && math::scalar_extract(*num_right->value) == 0.0) {
                    changed = true;
                    return optimize_node(bin->left, changed);
                }
                return make_unique<frontend::BinOpNode<Num>>(optimize_node(bin->left, changed), optimize_node(bin->right, changed), frontend::BinOps::SUBTRACT);
            }
            case frontend::BinOps::MULTIPLY: {
                auto num_left = dynamic_cast<frontend::NumNode<Num>*>(bin->left.get());
                auto num_right = dynamic_cast<frontend::NumNode<Num>*>(bin->right.get());
                if (num_left && num_right) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(*num_left->value * *num_right->value);
                }
                if (num_right && math::scalar_extract(*num_right->value) == 1.0) {
                    changed = true;
                    return optimize_node(bin->left, changed);
                }
                if (num_right && math::scalar_extract(*num_right->value) == -1.0) {
                    changed = true;
                    return make_unique<frontend::UnarOpNode<Num>>(optimize_node(bin->left, changed), frontend::UnarOps::NEGATE);
                }
                if(num_left && math::scalar_extract(*num_left->value) == 0.0) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::simd_cast<Num>(0.0));
                }
                if (num_right && math::scalar_extract(*num_right->value) == 0.0) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::simd_cast<Num>(0.0));
                }
                if (num_left && math::scalar_extract(*num_left->value) == 1.0) {
                    changed = true;
                    return optimize_node(bin->right, changed);
                }
                if (num_left && math::scalar_extract(*num_left->value) == -1.0) {
                    changed = true;
                    return make_unique<frontend::UnarOpNode<Num>>(optimize_node(bin->right, changed), frontend::UnarOps::NEGATE);
                }
                return make_unique<frontend::BinOpNode<Num>>(optimize_node(bin->left, changed), optimize_node(bin->right, changed), frontend::BinOps::MULTIPLY);
            }
            case frontend::BinOps::DIVIDE: {
                auto num_left = dynamic_cast<frontend::NumNode<Num>*>(bin->left.get());
                auto num_right = dynamic_cast<frontend::NumNode<Num>*>(bin->right.get());
                if (num_left && num_right) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(*num_left->value / *num_right->value);
                }
                if (num_right && math::scalar_extract(*num_right->value) == 1.0) {
                    changed = true;
                    return optimize_node(bin->left, changed);
                }
                if (num_left && math::scalar_extract(*num_left->value) == 0.0) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::simd_cast<Num>(0.0));
                }
                return make_unique<frontend::BinOpNode<Num>>(optimize_node(bin->left, changed), optimize_node(bin->right, changed), frontend::BinOps::DIVIDE);
            }
        }
    }
    auto func = dynamic_cast<frontend::FuncNode<Num>*>(expr.get());
    if (func != nullptr) {
        switch (func->func_id) {
            case frontend::FuncIds::ABS: {
                auto num = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                if (num) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::abs(*num->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::ABS, make_single_arg(optimize_node(func->args[0], changed)));
            }
            case frontend::FuncIds::SQRT: {
                auto num = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                if (num) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::sqrt(*num->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::SQRT, make_single_arg(optimize_node(func->args[0], changed)));
            }
            case frontend::FuncIds::EXP: {
                auto num = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                if (num) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::exp(*num->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::EXP, make_single_arg(optimize_node(func->args[0], changed)));
            }
            case frontend::FuncIds::LOG: {
                auto num = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                if (num) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::log(*num->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::LOG, make_single_arg(optimize_node(func->args[0], changed)));
            }
            case frontend::FuncIds::SIN: {
                auto num = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                if (num) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::sin(*num->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::SIN, make_single_arg(optimize_node(func->args[0], changed)));
            }
            case frontend::FuncIds::COS: {
                auto num = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                if (num) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::cos(*num->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::COS, make_single_arg(optimize_node(func->args[0], changed)));
            }
            case frontend::FuncIds::TAN: {
                auto num = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                if (num) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::tan(*num->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::TAN, make_single_arg(optimize_node(func->args[0], changed)));
            }
            case frontend::FuncIds::MAX: {
                auto num1 = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                auto num2 = dynamic_cast<frontend::NumNode<Num>*>(func->args[1].get());
                if (num1 && num2) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::max(*num1->value, *num2->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::MAX, make_double_arg(optimize_node(func->args[0], changed), optimize_node(func->args[1], changed)));
            }
            case frontend::FuncIds::MIN: {
                auto num1 = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                auto num2 = dynamic_cast<frontend::NumNode<Num>*>(func->args[1].get());
                if (num1 && num2) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::min(*num1->value, *num2->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::MIN, make_double_arg(optimize_node(func->args[0], changed), optimize_node(func->args[1], changed)));
            }
            case frontend::FuncIds::SOFTMAX: {
                auto num1 = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                auto num2 = dynamic_cast<frontend::NumNode<Num>*>(func->args[1].get());
                if (num1 && num2) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::softmax_weight(*num1->value, *num2->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::SOFTMAX, make_double_arg(optimize_node(func->args[0], changed), optimize_node(func->args[1], changed)));
            }
            case frontend::FuncIds::LSE_MAX: {
                auto num1 = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                auto num2 = dynamic_cast<frontend::NumNode<Num>*>(func->args[1].get());
                if (num1 && num2) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::lse_max(*num1->value, *num2->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::LSE_MAX, make_double_arg(optimize_node(func->args[0], changed), optimize_node(func->args[1], changed)));
            }
            case frontend::FuncIds::LSE_MIN: {
                auto num1 = dynamic_cast<frontend::NumNode<Num>*>(func->args[0].get());
                auto num2 = dynamic_cast<frontend::NumNode<Num>*>(func->args[1].get());
                if (num1 && num2) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(math::lse_min(*num1->value, *num2->value));
                }
                return make_unique<frontend::FuncNode<Num>>(frontend::FuncIds::LSE_MIN, make_double_arg(optimize_node(func->args[0], changed), optimize_node(func->args[1], changed)));
            }
        }
    }
    auto unar = dynamic_cast<frontend::UnarOpNode<Num>*>(expr.get());
    if (unar != nullptr) {
        switch (unar->op) {
            case frontend::UnarOps::NEGATE: {
                auto num = dynamic_cast<frontend::NumNode<Num>*>(unar->child.get());
                if (num) {
                    changed = true;
                    return make_unique<frontend::NumNode<Num>>(-*num->value);
                }
                return make_unique<frontend::UnarOpNode<Num>>(optimize_node(unar->child, changed), frontend::UnarOps::NEGATE);
            }
        }
    }
    //if num node or var node: nothing to do
    return clone(expr);
}

template<concepts::fp_or_simd Num>
frontend::AST<Num> frontend::optimize(const AST<Num> &expr) {
    bool changed = false;
    size_t i = 0;
    NodePtr<Num> root = optimize_node(expr.get_root(), changed);
    do {
        changed = false;
        root = optimize_node(root, changed);
    }while(changed);
    return AST<Num>(std::move(root));
}

template<concepts::fp_or_simd Num>
sde::frontend::AST<Num> sde::frontend::optimize_one_pass(const sde::frontend::AST<Num>& expr) {
    bool dummy = true;
    frontend::NodePtr<Num> root = optimize_node(expr.get_root(), dummy);
    return AST<Num>(std::move(root));
}


    template<sde::concepts::fp_or_simd Num>
    void sde::frontend::ast_to_text(const sde::frontend::NodePtr<Num>& expr) {
        auto bin = dynamic_cast<sde::frontend::BinOpNode<Num>*>(expr.get());
        if (bin) {
            cout << "(";
            ast_to_text(bin->left);
            switch (bin->op) {
                case sde::frontend::BinOps::ADD:{
                    cout << "+";
                    break;
                }
                case sde::frontend::BinOps::SUBTRACT: {
                    cout << "-";
                    break;
                }
                case sde::frontend::BinOps::MULTIPLY: {
                    cout << "*";
                    break;
                }
                case sde::frontend::BinOps::DIVIDE: {
                    cout << "/";
                    break;
                }
            }
            ast_to_text(bin->right);
            cout << ")";
        }
        auto un = dynamic_cast<sde::frontend::UnarOpNode<Num>*>(expr.get());
        if (un) {
            cout << "(";

            switch (un->op) {
                case sde::frontend::UnarOps::NEGATE: {
                    cout << "-";
                    break;
                }
            }
            ast_to_text(un->child);
            cout << ")";
        }
        auto func = dynamic_cast<sde::frontend::FuncNode<Num>*>(expr.get());
        if (func) {
            switch (func->func_id) {
                case sde::frontend::FuncIds::LOG: {
                    if constexpr (std::is_same_v<Num, float>) {
                        cout << "logf(";
                    } else {
                        cout << "log(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::EXP: {
                    if constexpr (std::is_same_v<Num, float>) {
                        cout << "expf(";
                    } else{
                    cout << "exp(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::SQRT: {
                    if constexpr(std::is_same_v<Num, float>) {
                        cout << "sqrtf(";
                    } else {
                        cout << "sqrt(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::SIN: {
                    if constexpr(std::is_same_v<Num, float>) {
                        cout << "sinf(";
                    } else {
                        cout << "sin(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::COS: {
                    if constexpr(std::is_same_v<Num, float>) {
                        cout << "cosf(";
                    } else {
                        cout << "cos(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::TAN: {
                    if constexpr(std::is_same_v<Num, float>) {
                        cout << "tanf(";
                    }else {
                        cout << "tan(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::ABS: {
                    if constexpr(std::is_same_v<Num, float>) {
                        cout << "fabsf(";
                    }else {
                        cout << "fabs(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::MAX: {
                    if constexpr(std::is_same_v<Num, float>) {
                        cout << "fmaxf(";
                    }else {
                        cout << "fmax(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::MIN: {
                    if constexpr(std::is_same_v<Num, float>) {
                        cout << "fminf(";
                    }else {
                        cout << "fmin(";
                    }
                    break;
                }
            }
            for (auto& arg : func->args) {
                ast_to_text(arg);
                cout << ", ";
            }
            cout << ")";
        }
        auto num = dynamic_cast<sde::frontend::NumNode<Num>*>(expr.get());
        if (num) {
            cout << std::to_string(math::scalar_extract(*(num->value)));
            if constexpr (std::is_same_v<Num, float>) {
                cout << "f";
            }
        }
        auto var = dynamic_cast<sde::frontend::VarNode<Num>*>(expr.get());
        if (var) {
            switch (var->index) {
                case sde::frontend::state_vars::state_X: {
                    cout << "X";
                    break;
                }
                case sde::frontend::state_vars::state_t: {
                    cout << "t";
                    break;
                }
            }
        }
    }



template frontend::NodePtr<float> sde::frontend::differentiate(const NodePtr<float>&, std::string_view, bool);
template frontend::NodePtr<double> sde::frontend::differentiate(const NodePtr<double>&, std::string_view, bool);
template frontend::NodePtr<sde::simd::floatv> sde::frontend::differentiate(const NodePtr<sde::simd::floatv>&, std::string_view, bool);
template frontend::NodePtr<sde::simd::doublev> sde::frontend::differentiate(const NodePtr<sde::simd::doublev>&, std::string_view, bool);

template frontend::AST<float> sde::frontend::differentiate(const frontend::AST<float>&, std::string_view, bool);
template frontend::AST<double> sde::frontend::differentiate(const frontend::AST<double>&, std::string_view, bool);
template frontend::AST<sde::simd::floatv> sde::frontend::differentiate(const frontend::AST<sde::simd::floatv>&, std::string_view, bool);
template frontend::AST<sde::simd::doublev> sde::frontend::differentiate(const frontend::AST<sde::simd::doublev>&, std::string_view, bool);

template frontend::AST<float> sde::frontend::optimize(const frontend::AST<float>&);
template frontend::AST<double> sde::frontend::optimize(const frontend::AST<double>&);
template frontend::AST<sde::simd::floatv> sde::frontend::optimize(const frontend::AST<sde::simd::floatv>&);
template frontend::AST<sde::simd::doublev> sde::frontend::optimize(const frontend::AST<sde::simd::doublev>&);

template frontend::AST<float> sde::frontend::optimize_one_pass(const frontend::AST<float>&);
template frontend::AST<double> sde::frontend::optimize_one_pass(const frontend::AST<double>&);
template frontend::AST<sde::simd::floatv> sde::frontend::optimize_one_pass(const frontend::AST<sde::simd::floatv>&);
template frontend::AST<sde::simd::doublev> sde::frontend::optimize_one_pass(const frontend::AST<sde::simd::doublev>&);

template void sde::frontend::ast_to_text(const sde::frontend::NodePtr<float>&);
template void sde::frontend::ast_to_text(const sde::frontend::NodePtr<double>&);
template void sde::frontend::ast_to_text(const sde::frontend::NodePtr<sde::simd::floatv>&);
template void sde::frontend::ast_to_text(const sde::frontend::NodePtr<sde::simd::doublev>&);