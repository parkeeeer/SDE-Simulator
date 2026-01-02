#pragma once

#include "concepts.hpp"
#include "AST.hpp"

namespace sde::frontend {
    template<concepts::fp_or_simd Num>
    AST<Num> differentiate(const AST<Num>& expr, std::string_view var, bool pedantic = false);

    template<concepts::fp_or_simd Num>
    NodePtr<Num> differentiate(const NodePtr<Num>& expr, const std::string_view var, bool pedantic = false);

    template<concepts::fp_or_simd Num>
    AST<Num> optimize(const AST<Num>& expr);

    template<concepts::fp_or_simd Num>
    AST<Num> optimize_one_pass(const AST<Num>& expr);

    template<concepts::fp_or_simd Num>
    void check_warning(NodePtr<Num>& expr);

    template<concepts::fp_or_simd Num>
    void ast_to_text(const NodePtr<Num>& expr);
}
