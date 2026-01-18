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
    Environment(){
        add_param("pi", M_PI);
        add_param("tau", M_PI * 2);
        add_param("e", M_E);
    }

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

    void print_params() const {
        for (const auto& param : map) {
            std::cout << param.first << "=" << param.second << ", ";
        }
    }

    std::string to_string() const {
        std::string result;
        for (const auto& param : map) {
            result += param.first + "=" + std::to_string(param.second)  + ", ";
        }
        result.pop_back();
        result.pop_back();
        return result;
    }
};



}