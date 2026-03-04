#include "sql_cache.hpp"

#include <unordered_set>
#include <sstream>

#include "Variable_Environment.hpp"

namespace sde::cli {
    const char* CREATE_TABLE = R"(
    CREATE TABLE IF NOT EXISTS expressions (
        name TEXT PRIMARY KEY,
        drift TEXT NOT NULL,
        diffusion TEXT NOT NULL,
        params TEXT,

        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
)";

    Cache::Cache(std::string path) {
        if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
        }
        create_table();

        if (count() == 0) {
            init_default_expressions();
        }
    }

    Cache::Cache() {
#ifdef _WIN32
        std::string home = std::getenv("USERPROFILE");
#else
        std::string home = std::getenv("HOME");
#endif
        std::string path = home + "/.sde_cache.db";
        if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
        }
        create_table();
        if (count() == 0) {
            init_default_expressions();
        }
    }

    Cache::~Cache() {
        sqlite3_close(db);
    }

    void Cache::create_table() {
        char* err = nullptr;
        if (sqlite3_exec(db, CREATE_TABLE, nullptr, nullptr, &err) != SQLITE_OK) {
            std::string error = err;
            sqlite3_free(err);
            throw std::runtime_error("failed to create table: " + error);
        }
    }

    std::string serialize_params(std::string drift, std::string diffusion) {
        sde::frontend::Lexer drift_lex(drift);
        sde::frontend::Lexer diffusion_lex(diffusion);
        auto drift_tokens = drift_lex.lex();
        auto diffusion_tokens = diffusion_lex.lex();

        std::unordered_set<std::string> params;

        for (size_t i = 0;i < drift_tokens.size()-1; ++i) {
            if (drift_tokens[i].type == frontend::TokenType::IDENTIFIER && drift_tokens[i+1].type != frontend::TokenType::LPAREN) {
                if (drift_tokens[i].value != "x" && drift_tokens[i].value != "X" && drift_tokens[i].value != "t") {
                    params.insert(drift_tokens[i].value);
                }
            }
        }
        for (size_t i = 0;i < diffusion_tokens.size()-1; ++i) {
            if (diffusion_tokens[i].type == frontend::TokenType::IDENTIFIER && diffusion_tokens[i+1].type != frontend::TokenType::LPAREN) {
                if (diffusion_tokens[i].value != "x" && diffusion_tokens[i].value != "X" && diffusion_tokens[i].value != "t") {
                    params.insert(diffusion_tokens[i].value);
                }
            }
        }


        std::ostringstream json;
        json << '[';
        bool first = true;
        for (const auto& param : params) {
            if (!first) json << ", ";
            json << "\"" << param << "\"";
            first = false;
        }
        json << "]";

        return json.str();

    }

    void Cache::store(const std::string& name, const std::string& drift, const std::string& diffusion) {


        const char* sql = R"(
            INSERT OR REPLACE INTO expressions
            (name, drift, diffusion, params)
            VALUES (?, ?, ?, ?)
        )";

        std::string params_json = serialize_params(drift, diffusion);

        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, name.data(), name.size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, drift.data(), drift.size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, diffusion.data(), diffusion.size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, params_json.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    void Cache::list() {
        const char* sql = "SELECT name FROM expressions ORDER BY name";
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

        std::cout << "Stored Expressions:\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* name = (const char*)sqlite3_column_text(stmt, 0);
            std::cout << "  " << name << std::endl;
        }
        sqlite3_finalize(stmt);
    }

    void Cache::view(const std::string& name) {
        const char* sql = "SELECT drift, diffusion, params FROM expressions WHERE name = ?";
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, name.data(), name.size(), SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::cout << "Name: " << name << std::endl;
            std::cout << "Drift: " << sqlite3_column_text(stmt, 0) << std::endl;
            std::cout << "Diffusion: " << sqlite3_column_text(stmt, 1) << std::endl;
            std::cout << "Params: " << sqlite3_column_text(stmt, 2) << std::endl;
        } else {
            std::cout << "Expression " << name << " not found" << std::endl;
        }
        sqlite3_finalize(stmt);
    }

    void Cache::remove(const std::string& name) {
        const char* sql = "DELETE FROM expressions WHERE name = ?";
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, name.data(), name.size(), SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
        int rows_changed = sqlite3_changes(db);
        sqlite3_finalize(stmt);
        if (rows_changed == 0) throw std::runtime_error("Expression " + name + " not found");
    }

    std::vector<std::string> parse_params_json(const char* json) {

        std::vector<std::string> params;
        std::string str(json);

        size_t start = str.find('[');
        size_t end = str.find(']');
        if (start == std::string::npos || end == std::string::npos) return params;

        std::string content = str.substr(start + 1, end - start - 1);
        std::istringstream ss(content);
        std::string param;

        while (std::getline(ss, param, ',')) {

            param.erase(0, param.find_first_not_of(" \t\""));
            param.erase(param.find_last_not_of(" \t\"") + 1);
            if (!param.empty()) {
                params.push_back(param);
            }
        }

        return params;
    }

    Cache::CachedExpression Cache::load(const std::string& name) {
        const char* sql = "SELECT drift, diffusion, params FROM expressions WHERE name = ?";
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, name.data(), name.size(), SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            Cache::CachedExpression expr;
            expr.drift = (const char*)sqlite3_column_text(stmt, 0);
            expr.diffusion = (const char*)sqlite3_column_text(stmt, 1);

            expr.params = parse_params_json((const char*)sqlite3_column_text(stmt, 2));
            sqlite3_finalize(stmt);
            return expr;
        } else {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Expression " + name + " not found");
        }
    }

    size_t Cache::count() {
        const char* sql = "SELECT COUNT(*) FROM expressions";
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_step(stmt);
        size_t result = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return result;
    }

    void Cache::init_default_expressions() {
        std::cout << "initializing default expressions..." << std::endl;
        store("gbm", "mu * x", "sigma * x");
        store("ou", "theta * (mu - x)",  "sigma");
        store("cir", "kappa * (theta - x)", "sigma * sqrt(x)");
        store("vasicek", "alpha * (beta - x)", "sigma");
        store("cev", "mu * x", "sigma * pow(x, beta)");
        std::cout << "Default expressions initialized" << std::endl;

    }
}