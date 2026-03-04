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
        void append_rng();
    public:

        CudaBuilder(frontend::AST<Num>& drift, frontend::AST<Num>& diffusion) : source(""), drift(&drift), diffusion(&diffusion) {
            append_rng();
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
void sde::engine::GPU::CudaBuilder<Num>::append_rng() {
    source += "struct SplitMix64 {\n"
              "    unsigned long long x;\n"
              "    __device__ explicit SplitMix64(unsigned long long seed) : x(seed) {}\n"
              "    __device__ unsigned long long next() {\n"
              "        unsigned long long z = (x += 0x9E3779B97F4A7C15ull);\n"
              "        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;\n"
              "        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;\n"
              "        return z ^ (z >> 31);\n"
              "    }\n"
              "};\n\n";

    source += "struct xoshiro256Plus {\n"
              "    unsigned long long s[4];\n"
              "    __device__ static xoshiro256Plus seed(unsigned long long seed) {\n"
              "        SplitMix64 a(seed);\n"
              "        xoshiro256Plus x;\n"
              "        for(int i = 0; i < 4; ++i) x.s[i] = a.next();\n"
              "        return x;\n"
              "    }\n"
              "    __device__ static inline unsigned long long rot_left(unsigned long long v, int k) {\n"
              "        return (v << k) | (v >> (64 - k));\n"
              "    }\n"
              "    __device__ unsigned long long next_u64() {\n"
              "        unsigned long long const result = s[0] + s[3];\n"
              "        unsigned long long const t = s[1] << 17;\n"
              "        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3];\n"
              "        s[2] ^= t; s[3] = rot_left(s[3], 45);\n"
              "        return result;\n"
              "    }\n";

    if constexpr(std::is_same_v<Num, float>) {
        source += "    __device__ inline float next() {\n"
                  "        unsigned long long a = next_u64();\n"
                  "        constexpr float scale = 1.0f / 16777216.0f;\n"
                  "        return (float)(a >> 40) * scale;\n"
                  "    }\n"
                  "    __device__ inline float normal() {\n"
                  "        float a = next(); float b = next();\n"
                  "        return sqrtf(-2.0f * logf(a)) * cosf(2.0f * 3.14159265358979323846f * b);\n"
                  "    }\n";
    } else {
        source += "    __device__ inline double next() {\n"
                  "        unsigned long long a = next_u64();\n"
                  "        constexpr double scale = 1.0 / 9007199254740992.0;\n"
                  "        return (double)(a >> 11) * scale;\n"
                  "    }\n"
                  "    __device__ inline double normal() {\n"
                  "        double a = next(); double b = next();\n"
                  "        return sqrt(-2.0 * log(a)) * cos(2.0 * 3.14159265358979323846 * b);\n"
                  "    }\n";
    }

    source += "};\n\n";
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
    source += "extern \"C\" __global__ void kernel(\n";
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
    source += "    xoshiro256Plus rng = xoshiro256Plus::seed(seed + path_idx);\n";
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
    source += " dW = ";
    if constexpr (std::is_same_v<Num, float>) {
        source += "sqrtf(dt) * rng.normal();\n";
    }else {
        source += "sqrt(dt) * rng.normal();\n";
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
    source += "extern \"C\"__global__ void kernel(\n";
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
    source += "    xoshiro256Plus rng = xoshiro256Plus::seed(seed + path_idx);\n";
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
    source += " dW = ";
    if constexpr (std::is_same_v<Num, float>) {
        source += "sqrtf(dt) * rng.normal();\n";
    }else {
        source += "sqrt(dt) * rng.normal();\n";
    }
    source += "        X = X + drift(X,t) * dt + diffusion(X,t) * dW + .5";
    if constexpr (std::is_same_v<Num, float>) {
        source += "f";
    }
    source += " * diffusion(X,t) * diffusion_prime(X,t) * (dW * dW - dt);\n";
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



