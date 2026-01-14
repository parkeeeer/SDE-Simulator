#pragma once

#include "AST.hpp"
#include "ast-analysis.hpp"
#include "concepts.hpp"


namespace sde::engine::GPU {

    template<concepts::FloatingPoint Num>
    class MetalBuilder {
        frontend::AST<Num> *drift, *diffusion;
        std::string source;
        bool is_compiled = false;
        void append_device_expression(std::string_view name, const frontend::AST<Num>& ast);
        void append_rng();
    public:

        MetalBuilder(frontend::AST<Num>& drift, frontend::AST<Num>& diffusion) : source("#include<metal_stdlib>\nusing namespace metal;\n\n"), drift(&drift), diffusion(&diffusion) {
            append_rng();
            append_device_expression("drift", drift);
            append_device_expression("diffusion", diffusion);
        }
        void append_euler();
        void append_milstein();

        std::string& get_source(){return source;}

    };
}

namespace sde::detail {
    template<sde::concepts::FloatingPoint Num>
    void ast_to_metal(const sde::frontend::NodePtr<Num>& ast, std::string& source) {
        auto bin = dynamic_cast<frontend::BinOpNode<Num>*>(ast.get());
        if (bin) {
            source += "(";

            switch (bin->op) {
                case sde::frontend::BinOps::ADD:
                    ast_to_metal<Num>(bin->left, source);
                    source += " + ";
                    ast_to_metal<Num>(bin->right, source);
                    break;
                case sde::frontend::BinOps::SUBTRACT:
                    ast_to_metal<Num>(bin->left, source);
                    source += " - ";
                    ast_to_metal<Num>(bin->right, source);
                    break;
                case sde::frontend::BinOps::MULTIPLY:
                    ast_to_metal<Num>(bin->left, source);
                    source += " * ";
                    ast_to_metal<Num>(bin->right, source);
                    break;
                case sde::frontend::BinOps::DIVIDE:
                    ast_to_metal<Num>(bin->left, source);
                    source += " / ";
                    ast_to_metal<Num>(bin->right, source);
                    break;
                case sde::frontend::BinOps::POWER:
                    source += "pow(";
                    ast_to_metal<Num>(bin->left, source);
                    source += ",";
                    ast_to_metal<Num>(bin->right, source);
                    source += ")";
                    break;
            }
            source += ")";
        }
        auto unar = dynamic_cast<sde::frontend::UnarOpNode<Num>*>(ast.get());
        if (unar) {
            source += "(";
            switch (unar->op) {
                case sde::frontend::UnarOps::NEGATE:
                    source += "-";
                    ast_to_metal<Num>(unar->child, source);
                    break;
            }
            source += ")";
        }
        auto func = dynamic_cast<sde::frontend::FuncNode<Num>*>(ast.get());
        if (func) {
            source += "(";
            switch (func->func_id) {
                case sde::frontend::FuncIds::EXP: {
                    source += "exp(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::LOG: {
                    source += "log(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::SQRT: {
                    source += "sqrt(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::SIN: {
                    source += "sin(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                     break;
                }
                case sde::frontend::FuncIds::COS: {
                    source += "cos(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::TAN: {
                    source += "tan(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::SINH: {
                    source += "sinh(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::COSH: {
                    source += "cosh(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::TANH: {
                    source += "tanh(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::ABS: {
                    source += "abs(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::MAX: {
                    source += "max(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ",";
                    ast_to_metal<Num>(func->args[1], source);
                    source += ")";
                    break;
                }
                case sde::frontend::FuncIds::MIN: {
                    source += "min(";
                    ast_to_metal<Num>(func->args[0], source);
                    source += ",";
                    ast_to_metal<Num>(func->args[1], source);
                    source += ")";
                    break;
                }
            }
            source += ")";
        }
        auto state = dynamic_cast<sde::frontend::VarNode<Num>*>(ast.get());
        if (state) {
            if (state->index == sde::frontend::state_vars::state_X) {
                source += "X";
            }else if (state->index == sde::frontend::state_vars::state_t) {
                source += "t";
            }
        }
        auto num = dynamic_cast<sde::frontend::NumNode<Num>*>(ast.get());
        if (num) {
            source += std::to_string(*(num->value));
            if constexpr(std::is_same_v<Num, float>) {
                source += "f";
            }
        }
    }
}

template<sde::concepts::FloatingPoint Num>
void sde::engine::GPU::MetalBuilder<Num>::append_device_expression(std::string_view name, const frontend::AST<Num> &ast) {
    std::string var_name;
    if constexpr(std::is_same_v<Num, float>) {
        var_name = "float";
    }else {
        var_name = "double";
    }
    source += "inline ";
    source += var_name;
    source += " ";
    source += name;
    source += "(";
    source += var_name;
    source += " X, ";
    source += var_name;
    source += " t) {\n\treturn ";
    sde::detail::ast_to_metal<Num>(ast.get_root(), source);
    source += ";\n}\n\n";
}

template<sde::concepts::FloatingPoint Num>
void sde::engine::GPU::MetalBuilder<Num>::append_rng() {
    source += "struct SplitMix64 {\n"
              "    uint64_t x;\n"
              "    explicit SplitMix64(uint64_t seed): x(seed) {}\n"
              "    uint64_t next() {\n"
              "        uint64_t z = (x += 0x9E3779B97F4A7C15ull);\n"
              "        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;\n"
              "        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;\n"
              "        return z ^ (z >> 31);\n"
              "  }\n"
              "};\n\n";

    source += "struct xoshiro256Plus {\n"
              "    uint64_t s[4];\n"
              "    \n"
              "    static xoshiro256Plus seed(uint64_t seed) {\n"
              "        SplitMix64 a(seed);\n"
              "        xoshiro256Plus x;\n"
              "        for(int i = 0;i < 4;++i) {\n"
              "            x.s[i] = a.next();\n"
              "        }\n"
              "        return x;\n"
              "    }\n\n"
              "    static inline uint64_t rot_left(uint64_t v, int k){ return (v<<k)|(v>>(64-k)); }\n\n"
              "    uint64_t next_u64() {\n"
              "         uint64_t const result = s[0] + s[3];\n"
              "         uint64_t const t = s[1] << 17;\n"
              "         s[2] ^= s[0];\n"
              "         s[3] ^= s[1];\n"
              "         s[1] ^= s[2];\n"
              "         s[0] ^= s[3];\n"
              "         s[2] ^= t;\n"
              "         s[3] = rot_left(s[3], 45);\n"
              "         return result;\n"
              "    }\n\n";
    if constexpr(std::is_same_v<Num, float>) {
        source += "    inline float next() {\n";
        source += "        uint64_t a = next_u64();\n";
        source += "        constexpr float scale = 1.0f / 16777216.0f;\n";
        source += "        return static_cast<float>(a >> 40) * scale;\n";
        source += "    }\n\n";
        source += "    inline float normal() {\n";
        source += "        float a = next();\n";
        source += "        float b = next();\n";
        source += "        return sqrt(-2.0f * log(a)) * cos(2.0f * M_PI_F * b);\n";
        source += "    }\n\n";
    } else {
        source += "    inline double next() {\n";
        source += "        uint64_t a = next_u64();\n";
        source += "        constexpr double scale = 1.0 / 9007199254740992.0;\n";
        source += "        return static_cast<double>(a >> 11) * scale;\n";
        source += "    }\n\n";
        source += "    inline double normal() {\n";
        source += "        double a = next();\n";
        source += "        double b = next();\n";
        source += "        return sqrt(-2.0 * log(a)) & cos(2.0 * M_PI * b);\n";
        source += "    }\n\n";
    }
    source += "};";
}

template<sde::concepts::FloatingPoint Num>
void sde::engine::GPU::MetalBuilder<Num>::append_euler() {
    if (is_compiled) {
        throw std::runtime_error("compiled to metal multiple times!");
    }
    is_compiled = true;
    std::string var_name;
    if constexpr(std::is_same_v<Num, float>) {
        var_name = "float";
    } else {
        var_name = "double";
    }
    source +=
        "kernel void sde_kernel(\n"
        "    device ";
    source += var_name;
    source +=
        "* paths [[buffer(0)]],\n"
        "    constant uint64_t& seed [[buffer(1)]],\n"
        "    constant ";
    source += var_name;
    source +=
        "& dt [[buffer(2)]],\n"
        "    constant ";
    source += var_name;
    source +=
        "& t0 [[buffer(3)]],\n"
        "    constant ";
    source += var_name;
    source +=
        "& initial_value [[buffer(4)]],\n"
        "    constant uint& num_paths [[buffer(5)]],\n"
        "    constant uint& num_steps [[buffer(6)]],\n"
        "    uint path_idx [[thread_position_in_grid]])\n"
        "    {\n"
        "    if(path_idx >= num_paths) return;\n"
        "    xoshiro256Plus rng = xoshiro256Plus::seed(seed + path_idx);\n"
        "    paths[path_idx] = initial_value;\n"
        "    ";
    source += var_name;
    source +=
        " X = initial_value;\n"
        "    for(uint64_t step = 1;step < num_steps; ++step) {\n"
        "        ";
    source += var_name;
    source +=
        " t = t0 + dt * step;\n"
        "        ";
    source += var_name;
    source +=
        " dW = sqrt(dt) * rng.normal();\n"
        "        X = X + drift(X,t) * dt + diffusion(X,t) * dW;\n"
        "        paths[path_idx + step * num_paths] = X;\n"
        "    }\n"
        "}\n";
}

template<sde::concepts::FloatingPoint Num>
void sde::engine::GPU::MetalBuilder<Num>::append_milstein() {
    if (is_compiled) {
        throw std::runtime_error("compiled to metal multiple times!");
    }
    is_compiled = true;
    std::string var_name;
    if constexpr(std::is_same_v<Num, float>) {
        var_name = "float";
    } else {
        var_name = "double";
    }
    frontend::AST<Num> diffusion_prime = frontend::differentiate(*diffusion, "x");
    frontend::AST<Num> diffusion_prime_optimized = frontend::optimize(diffusion_prime);
    append_device_expression("diffusion_prime", diffusion_prime_optimized);

    source +=
        "kernel void sde_kernel(\n"
        "    device ";
    source += var_name;
    source +=
        "* paths [[buffer(0)]],\n"
        "    constant uint64_t& seed [[buffer(1)]],\n"
        "    constant ";
    source += var_name;
    source +=
        "& dt [[buffer(2)]],\n"
        "    constant ";
    source += var_name;
    source +=
        "& t0 [[buffer(3)]],\n"
        "    constant ";
    source += var_name;
    source +=
        "& initial_value [[buffer(4)]],\n"
        "    constant uint& num_paths [[buffer(5)]],\n"
        "    constant uint& num_steps [[buffer(6)]],\n"
        "    uint path_idx [[thread_position_in_grid]])\n"
        "    {\n"
        "    if(path_idx >= num_paths) return;\n"
        "    xoshiro256Plus rng = xoshiro256Plus::seed(seed + path_idx);\n"
        "    paths[path_idx] = initial_value;\n"
        "    ";
    source += var_name;
    source +=
        " X = initial_value;\n"
        "    for(uint64_t step = 1;step < num_steps; ++step) {\n"
        "        ";
    source += var_name;
    source +=
        " t = t0 + dt * step;\n"
        "        ";
    source += var_name;
    source +=
        " dW = sqrt(dt) * rng.normal();\n"
        "        X = X + drift(X,t) * dt + diffusion(X,t) * dW + 0.5";
    if constexpr(std::is_same_v<Num, float>) {
        source += "f";
    }
    source +=
        " * diffusion(X,t) * diffusion_prime(X,t) * (dW * dW - dt);\n"
        "        paths[path_idx + step * num_paths] = X;\n"
        "    }\n"
        "}\n";
}



