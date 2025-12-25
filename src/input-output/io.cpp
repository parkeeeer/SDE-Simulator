
#include <io.hpp>
#include "exec_io.hpp"
#include "store_io.hpp"
#include "run_io.hpp"

using namespace sde::io;

command sde::io::parse_cmdline(int argc, char** argv) {
    if (argc < 2) {
        throw std::runtime_error("need a command");
    }
    std::string cmd = argv[1];
    if (cmd == "exec") {
        return std::make_unique<ExecFlags>(parse_exec(argc, argv));
    }else if (cmd == "store") {
        return std::make_unique<StoreFlags>();
    }else if (cmd == "load" || cmd == "run") {
        return std::make_unique<RunFlags>();
    }else {
        throw std::runtime_error("unknown command");
    }
}



template<sde::concepts::FloatingPoint Num>
void sde::io::output(const command& flags, const std::vector<Num>& results, std::ostream& out) {
    std::visit([&results, &out](const auto& ptr) {
        using T = std::decay_t<decltype(*ptr)>;
        if constexpr (std::is_same_v<T, ExecFlags>) {
            output_exec(*ptr, results, out);
        }else if constexpr (std::is_same_v<T, StoreFlags>) {
            out << "not yet implemented\n";
            //output_store(*ptr, results);
        }else if constexpr (std::is_same_v<T, RunFlags>) {
            out << "not yet implemented\n";
            //output_run(*ptr, results);
        }else {
            throw std::runtime_error("unknown command");
        }
    }, flags);
}



template void sde::io::output<float>(const command&, const std::vector<float>&, std::ostream&);
template void sde::io::output<double>(const command&, const std::vector<double>&, std::ostream&);