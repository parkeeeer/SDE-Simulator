#pragma once

#include "sql_cache.hpp"



namespace sde::cli {
    struct StoreFlags{
        std::string diffusion;
        std::string drift;
        std::string name;
    };

    StoreFlags parse_store(int argc, char** argv);


    inline void output_store(const StoreFlags& flags) {
        std::cout << "storage complete!!\nname: " << flags.name << "\ndiffusion: " << flags.diffusion << "\ndrift: " << flags.drift;
    }

    struct ViewFlags {
        std::string name;
    };

    ViewFlags parse_view(int argc, char** argv);

    inline void output_view(const ViewFlags& flags) {
        Cache cache;
        cache.view(flags.name);
    }

    struct ListFlags {}; //basically a tag lol

    inline ListFlags parse_list(int argc, char** argv) {return ListFlags{};}

    inline void output_list(const ListFlags& flags) {
        Cache cache;
        cache.list();
    }

    struct RemoveFlags {
        std::string name;
    };

    RemoveFlags parse_remove(int argc, char** argv);

    inline void output_remove(const RemoveFlags& flags) {
        std::cout << "Removed expression " << flags.name << std::endl;
    }
}

