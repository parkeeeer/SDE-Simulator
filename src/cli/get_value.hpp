#pragma once

#include <string_view>
#include <string>

namespace sde::cli {
    inline std::string_view get_value(std::string_view input, int& i, int argc, char** argv) {
        size_t equal_pos = input.find('=');
        if (equal_pos != std::string::npos) {
            return input.substr(equal_pos + 1);
        }
        if (i + 1 < argc) {
            return argv[++i];
        }
        throw std::runtime_error("Missing value for flag: " + std::string(input));
    }
}