#pragma once
#include <iostream>
#include <variant>
#include <memory>
#include "concepts.hpp"


namespace sde::io {
    struct ExecFlags;
    struct StoreFlags;
    struct RunFlags;

    using command = std::variant<
        std::unique_ptr<ExecFlags>,
        std::unique_ptr<StoreFlags>,
        std::unique_ptr<RunFlags>>;

    command parse_cmdline(int argc, char** argv);

    template<sde::concepts::FloatingPoint Num>
    void output(const command& flags, const std::vector<Num>& results, std::ostream& out = std::cout);
}
