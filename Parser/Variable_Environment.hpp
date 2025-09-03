#pragma once


#include <vector>
#include <unordered_map>
#include <string>

enum state_vars:std::size_t{
    state_X = 0,
    state_t = 1,
    state_dW = 2
};

//lightweight view into the environment for read-only access
template<class Num>
struct Env_view{
    const Num* parameters;
    const Num* state_vars;
};

//environment for variables/parameters in the expression
//binds variables to an index in a vector rather than using a map
//so that we can access them more efficiently(there will be many indexes so we want to avoid the overhead of a map)

template<class Num>
class Environment{
    std::unordered_map<std::string, std::size_t> index_map;
    std::vector<Num> params;
    std::vector<Num> state_vars;
    std::size_t next_index;
    public:
    Environment() : next_index(0) {
        
    }

    std::size_t get_index(const std::string& name) const{
        return index_map.at(name);
    }


    std::size_t add_param(const std::string& name, Num value){
        index_map[name] = next_index;
        params.push_back(value);
        return next_index++;
    }

    bool is_param(const std::string& name) const {
        return index_map.find(name) != index_map.end();
    }

    Env_view<Num> get_view() const {
        return Env_view<Num>{params.data(), state_vars.data()};
    }
};



