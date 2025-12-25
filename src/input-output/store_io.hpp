#pragma once

#include <io.hpp>


namespace sde::io {
    struct StoreFlags{
        std::string filename;
    };


    void output_store(const StoreFlags& flags, const std::vector<double>& results);
}

