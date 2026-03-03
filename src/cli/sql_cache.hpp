#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <bytecode.hpp>
#include "exec_cli.hpp"

namespace sde::cli {
    class Cache {
        sqlite3* db;

    public:
        explicit Cache(std::string path);
        Cache();
        ~Cache();

        Cache(const Cache& other) = delete;
        Cache& operator=(const Cache& other) = delete;
        Cache(const Cache&& other) = delete;
        Cache& operator=(const Cache&& other) = delete;

        void create_table();

        void init_default_expressions();

        void store(const std::string& name, const std::string& drift, const std::string& diffusion);
        void view(const std::string& name);
        void remove(const std::string& name);
        void list();
        size_t count();

        struct CachedExpression {
            std::string diffusion;
            std::string drift;
            std::vector<std::string> params;
        };

        CachedExpression load(const std::string& name);
    };
}
