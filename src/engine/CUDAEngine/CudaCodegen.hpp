#pragma once


#include <string>


#include "ast-analysis.hpp"
#include "AST.hpp"
#include "concepts.hpp"




namespace sde::engine::GPU {

    template<concepts::FloatingPoint Num>
    class CudaBuilder {
        std::string source;
        bool is_compiled = false;
        frontend::AST<Num>* drift, *diffusion;
        void append_device_expression(std::string_view name, const frontend::AST<Num>& ast);
        void append_extra_functions();
    public:

        CudaBuilder(frontend::AST<Num>& drift, frontend::AST<Num>& diffusion) : source(""), drift(&drift), diffusion(&diffusion) {
            append_extra_functions();
            append_device_expression("drift", drift);
            append_device_expression("diffusion", diffusion);
        }
        std::string& get_source(){return source;}

        void append_euler();
        void append_milstein();
    };
}

namespace sde::detail{
    template<sde::concepts::FloatingPoint Num>
    void ast_to_cuda(const sde::frontend::NodePtr<Num>& ast, std::string& source) {
        auto bin = dynamic_cast<sde::frontend::BinOpNode<Num>*>(ast.get());
        if (bin) {
            source += "(";
            ast_to_cuda(bin->left, source);
            switch (bin->op) {
                case sde::frontend::BinOps::ADD:{
                    source += "+";
                    break;
                }
                case sde::frontend::BinOps::SUBTRACT: {
                    source += "-";
                    break;
                }
                case sde::frontend::BinOps::MULTIPLY: {
                    source += "*";
                    break;
                }
                case sde::frontend::BinOps::DIVIDE: {
                    source += "/";
                    break;
                }
            }
            ast_to_cuda(bin->right, source);
            source += ")";
        }
        auto un = dynamic_cast<sde::frontend::UnarOpNode<Num>*>(ast.get());
        if (un) {
            source += "(";

            switch (un->op) {
                case sde::frontend::UnarOps::NEGATE: {
                    source += "-";
                    break;
                }
            }
            ast_to_cuda(un->child, source);
            source += ")";
        }
        auto func = dynamic_cast<sde::frontend::FuncNode<Num>*>(ast.get());
        if (func) {
            switch (func->func_id) {
                case sde::frontend::FuncIds::LOG: {
                    if constexpr (std::is_same_v<Num, float>) {
                        source += "logf(";
                    } else {
                        source += "log(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::EXP: {
                    if constexpr (std::is_same_v<Num, float>) {
                        source += "expf(";
                    } else{
                    source += "exp(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::SQRT: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "sqrtf(";
                    } else {
                        source += "sqrt(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::SIN: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "sinf(";
                    } else {
                        source += "sin(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::COS: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "cosf(";
                    } else {
                        source += "cos(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::TAN: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "tanf(";
                    } else {
                        source += "tan(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::SINH: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "sinhf(";
                    } else {
                        source += "sinh(";
                    }
                }
                case sde::frontend::FuncIds::COSH: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "coshf(";
                    } else {
                        source += "cosh(";
                    }
                }
                case sde::frontend::FuncIds::TANH: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "tanhf(";
                    } else {
                        source += "tanh(";
                    }
                }
                case sde::frontend::FuncIds::ABS: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "fabsf(";
                    }else {
                        source += "fabs(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::MAX: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "fmaxf(";
                    }else {
                        source += "fmax(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::MIN: {
                    if constexpr(std::is_same_v<Num, float>) {
                        source += "fminf(";
                    }else {
                        source += "fmin(";
                    }
                    break;
                }
                case sde::frontend::FuncIds::LSE_MAX: {
                    source += "lse_max(";
                }
                case sde::frontend::FuncIds::LSE_MIN: {
                    source += "lse_min(";
                }
                case sde::frontend::FuncIds::SOFTMAX: {
                    source += "softmax_weight(";
                }
            }
            for (auto& arg : func->args) {
                    ast_to_cuda(arg, source);
                source += ", ";
            }
            source.pop_back();
            source.pop_back();
            source += ")";
        }
        auto num = dynamic_cast<sde::frontend::NumNode<Num>*>(ast.get());
        if (num) {
            source += std::to_string(*(num->value));
            if constexpr (std::is_same_v<Num, float>) {
                source += "f";
            }
        }
        auto var = dynamic_cast<sde::frontend::VarNode<Num>*>(ast.get());
        if (var) {
            switch (var->index) {
                case sde::frontend::state_vars::state_X: {
                    source += "X";
                    break;
                }
                case sde::frontend::state_vars::state_t: {
                    source += "t";
                    break;
                }
            }
        }
    }
}



template<sde::concepts::FloatingPoint Num>
void sde::engine::GPU::CudaBuilder<Num>::append_device_expression(std::string_view name, const frontend::AST<Num>& ast) {
    source += "__device__ __forceinline__ ";
    std::string var_name;
    if constexpr (std::is_same_v<Num, float>) {
        var_name = "float";
    } else {
        var_name = "double";
    }
    source += var_name;
    source += " ";
    source += std::string(name);
    source += "(";
    source += var_name;
    source += " X";
    source += ", ";
    source += var_name;
    source += " t) { \n";
    source += "    return ";
    detail::ast_to_cuda(ast.get_root(), source);
    source += ";\n}\n\n";
}


template<sde::concepts::FloatingPoint Num>
void sde::engine::GPU::CudaBuilder<Num>::append_euler() {
    if (is_compiled) throw std::runtime_error("already compiled");
    is_compiled = true;
    std::string var_name;
    if constexpr (std::is_same_v<Num, float>) {
        var_name = "float";
    }else {
        var_name = "double";
    }
    source += "__global__ void kernel(\n";
    source += var_name;
    source += "* __restrict__ paths,\n";
    source += "const unsigned long long seed,\n";
    source += var_name;
    source += " dt,\n";
    source += var_name;
    source += " t0,\n";
    source += var_name;
    source += " initial_value,\n";
    source += "const unsigned int num_paths,\n";
    source += "const unsigned int num_steps) {\n";
    source += "    unsigned int path_idx = blockIdx.x * blockDim.x + threadIdx.x;\n";
    source += "    if(path_idx >= num_paths) return;\n";
    source += "    curandState_t state;\n";
    source += "    curand_init(seed, path_idx, 0, &state);\n";
    source += "    paths[path_idx] = initial_value;\n";
    source += "    ";
    source += var_name;
    source += " X = paths[path_idx];\n";
    source += "    for(unsigned int step = 1; step < num_steps; ++step) {\n";
    source += "        ";
    source += var_name;
    source += " t = t0 + dt * step;\n";
    source += "        ";
    source += var_name;
    source += " Dw = ";
    if constexpr (std::is_same_v<Num, float>) {
        source += "sqrtf(dt) * curand_normal(&state);\n";
    }else {
        source += "sqrt(dt) * curand_normal(&state);\n";
    }
    source += "        X = X + drift(X,t) * dt + diffusion(X,t) * dW;\n";
    source += "        paths[path_idx + step * num_paths] = X;\n";
    source += "    }\n}\n";
}

template <sde::concepts::FloatingPoint Num>
void sde::engine::GPU::CudaBuilder<Num>::append_milstein() {
    if (is_compiled) throw std::runtime_error("already compiled");
    is_compiled = true;
    frontend::AST<Num> diffusion_prime = frontend::differentiate(*diffusion, "X", false);
    frontend::AST<Num> diffusion_prime_optimized = frontend::optimize_one_pass(diffusion_prime);
    frontend::AST<Num> diffusion_prime_optimized1 = frontend::optimize(diffusion_prime_optimized);
    append_device_expression("diffusion_prime", diffusion_prime_optimized1);
    std::string var_name;
    if constexpr (std::is_same_v<Num, float>) {
        var_name = "float";
    }else {
        var_name = "double";
    }
    source += "__global__ void kernel(\n";
    source += var_name;
    source += "* __restrict__ paths,\n";
    source += "const unsigned long long seed,\n";
    source += var_name;
    source += " dt,\n";
    source += var_name;
    source += " t0,\n";
    source += var_name;
    source += " initial_value,\n";
    source += "const unsigned int num_paths,\n";
    source += "const unsigned int num_steps) {\n";
    source += "    unsigned int path_idx = blockIdx.x * blockDim.x + threadIdx.x;\n";
    source += "    if(path_idx >= num_paths) return;\n";
    source += "    curandState_t state;\n";
    source += "    curand_init(seed, path_idx, 0, &state);\n";
    source += "    paths[path_idx] = initial_value;\n";
    source += "    ";
    source += var_name;
    source += " X = paths[path_idx];\n";
    source += "    for(unsigned int step = 1; step < num_steps; ++step) {\n";
    source += "        ";
    source += var_name;
    source += " t = t0 + dt * step;\n";
    source += "        ";
    source += var_name;
    source += " dw = ";
    if constexpr (std::is_same_v<Num, float>) {
        source += "sqrtf(dt) * curand_normal(&state);\n";
    }else {
        source += "sqrt(dt) * curand_normal(&state);\n";
    }
    source += "        X = X + drift(X,t) * dt + diffusion(X,t) * dw + .5";
    if constexpr (std::is_same_v<Num, float>) {
        source += "f";
    }
    source += " * diffusion(X,t) * diffusion_prime(X,t) * (dw * dw - dt);\n";
    source += "        paths[path_idx + step * num_paths] = X;\n";
    source += "    }\n}\n";
}

template<sde::concepts::FloatingPoint Num>
void sde::engine::GPU::CudaBuilder<Num>::append_extra_functions() {
    std::string var;
    std::string exp;
    std::string log;
    std::string max;
    std::string min;
    if constexpr (std::is_same_v<Num, float>) {
        var = "float";
        exp = "expf";
        log = "logf";
        max = "fmaxf";
        min = "fminf";
    } else {
        var = "double";
        exp = "exp";
        log = "log";
        max = "fmax";
        min = "fmin";
    }
    source += "__device__ " + var + " lse_max(" + var + " a, " + var + " b, " + var + " k = 10.0) {\n";
    source += "    " + var + " m = " + max + "(a,b);\n";
    source += "    return m + " + log + "(" + exp + "(k * (a - m)) + " + exp + "(k * (b - m))) / k;\n";
    source += "}\n\n";

    source += "__device__ " + var + " lse_min(" + var + " a, " + var + " b, " + var + " k = 10.0) {\n";
    source += "    " + var + " m = " + min + "(a,b);\n";
    source += "    return m + " + log + "(" + exp + "(k * (m - a)) + " + exp + "(k * (m - b))) / k;\n";
    source += "}\n\n";

    source += "__device__ " + var + " softmax_weight(" + var + " a, " + var + " b, " + var + " k = 10.0) {\n";
    source += "    return 1.0 / (10 + " + exp + "(k * (b - a)));";
    source += "}\n\n";
}



