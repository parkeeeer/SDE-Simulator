#pragma once


#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

namespace sde::frontend {

enum state_vars:std::size_t{
    state_X = 0,
    state_t = 1,
};

//lightweight view into the environment for read-only
template<class Num>
struct Env_view{
    const Num* state;
};

//environment for variables/parameters in the expression
//binds variables to an index in a vector rather than using a map
//so that we can access them more efficiently(there will be many indexes so we want to avoid the overhead of a map)

template<class Num>
class Environment{
    std::unordered_map<std::string, Num> map;
    public:
    Environment() = default;

    Num get_param(const std::string& name) const{
        if(is_param(name)){
            return map.at(name);
        }else{
            throw std::runtime_error("parameter not found in environment: " + name);
        }
    }


    void add_param(const std::string& name, Num value) {
        map.emplace(name, value);
    }

    bool is_param(const std::string& name) const {
        return map.find(name) != map.end();
    }

    void set_dt(Num dt){
        add_param("dt", dt);
    }

    auto begin() const {return map.begin();}
    auto end() const {return map.end();}
};



}