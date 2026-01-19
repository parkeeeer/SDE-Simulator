#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <random>

#include "frontend.hpp"
#include "allocator.hpp"
#include "CPUdispatch.hpp"
#if HAS_METAL
#include "MetalCodegen.hpp"
#include "MetalEngine.hpp"
#elif HAS_CUDA
#include "CudaCodegen.hpp"
#include "GPUengine.hpp"
#endif

namespace py = pybind11;

class SDESimulator {
    std::string diffusion;
    std::string drift;

    template<sde::concepts::FloatingPoint Num>
    pybind11::array_t<Num> simulate_metal(size_t num_paths, size_t num_steps, Num dt, Num initial_value, std::string_view method, uint64_t seed, const sde::frontend::Environment<double>& env);

    template<sde::concepts::FloatingPoint Num>
    pybind11::array_t<Num> simulate_cuda(size_t num_paths, size_t num_steps, Num dt, Num initial_value, std::string_view method, uint64_t seed, const sde::frontend::Environment<double>& env);

    template<sde::concepts::FloatingPoint Num>
    pybind11::array_t<Num> simulate_bytecode(size_t num_paths, size_t num_steps, Num dt, Num initial_value, std::string_view method, uint64_t seed, const sde::frontend::Environment<double>& env);

    template<sde::concepts::FloatingPoint Num>
    pybind11::array_t<Num> simulate_ast(size_t num_paths, size_t num_steps, Num dt, Num initial_value, std::string_view method, uint64_t seed, const sde::frontend::Environment<double>& env);

public:
    SDESimulator(std::string diffusion, std::string drift) : diffusion(diffusion), drift(drift) {}

    py::object simulate(
        size_t num_paths,
        size_t num_steps,
        double dt,
        const std::string& backend = "gpu",
        double initial_value = 1.0,
        const std::string& method = "euler",
        std::optional<uint64_t> seed = std::nullopt,
        std::unordered_map<std::string, double> params = {},
        const std::string& precision = "float"
    ) {
        uint64_t real_seed = seed.value_or(std::random_device{}());
        sde::frontend::Environment env(std::move(params));

        if (backend == "gpu") {
#if HAS_CUDA
            if (precision == "float" || precision == "float32") {
                return simulate_cuda<float>(num_paths, num_steps, dt, initial_value, method, real_seed, precision, env);
            } else if (precision == "double" || precision == "float64") {
                return simulate_cuda<double>(num_paths, num_steps, dt, initial_value, method, real_seed, precision, env);
            } else {
                throw std::runtime_error("Unknown precision: " + precision);
            }
#elif HAS_METAL
            if (precision == "float" || precision == "float32") {
                return simulate_metal<float>(num_paths, num_steps, dt, initial_value, method, real_seed, env);
            } else if (precision == "double" || precision == "float64") {
                throw std::runtime_error("metal does not support double precision");
            } else {
                throw std::runtime_error("Unknown precision: " + precision);
            }
#else
            throw std::runtime_error("gpu not supported, supported gpu frameworks: cuda, metal");
#endif
        } else if (backend == "cuda") {
#if HAS_CUDA
            if (precision == "float" || precision == "float32") {
                return simulate_cuda<float>(num_paths, num_steps, dt, initial_value, method, real_seed, precision, env);
            } else if (precision == "double" || precision == "float64") {
                return simulate_cuda<double>(num_paths, num_steps, dt, initial_value, method, real_seed, precision, env);
            } else {
                throw std::runtime_error("Unknown precision: " + precision);
            }
#else
            throw std::runtime_error("Cuda is not supported by your machine");
#endif
        }else if (backend == "metal") {
#if HAS_METAL
            if (precision == "float" || precision == "float32") {
                return simulate_metal<float>(num_paths, num_steps, dt, initial_value, method, real_seed, env);
            } else if (precision == "double" || precision == "float64") {
                throw std::runtime_error("metal does not support double precision");
            } else {
                throw std::runtime_error("Unknown precision: " + precision);
            }
#else
            throw std::runtime_error("Metal is not supported by your machine");
#endif
        }else if (backend == "bytecode" || backend == "cpu") {
            if (precision == "float" || precision == "float32") {
                return simulate_bytecode<float>(num_paths, num_steps, dt, initial_value, method, real_seed, env);
            } else if (precision == "double" || precision == "float64") {
                return simulate_bytecode<double>(num_paths, num_steps, dt, initial_value, method, real_seed, env);
            } else {
                throw std::runtime_error("Unknown precision: " + precision);
            }
        }else if (backend == "ast") {
            if (precision == "float" || precision == "float32") {
                return simulate_ast<float>(num_paths, num_steps, dt, initial_value, method, real_seed, env);
            } else if (precision == "double" || precision == "float64") {
                return simulate_ast<double>(num_paths, num_steps, dt, initial_value, method, real_seed, env);
            } else {
                throw std::runtime_error("Unknown precision: " + precision);
            }
        }else {
            throw std::runtime_error("Unknown backend: " + backend + ", make sure to use all lowercase.");
        }
    }
};

template<sde::concepts::FloatingPoint Num>
pybind11::array_t<Num> SDESimulator::simulate_metal(size_t num_paths, size_t num_steps, Num dt, Num initial_value, std::string_view method, uint64_t seed, const sde::frontend::Environment<double>& env) {
#if HAS_METAL
    py::array_t<Num> arr({num_steps, num_paths});
    auto drift_ast = sde::frontend::parse<Num>(drift, env);
    auto diff_ast = sde::frontend::parse<Num>(diffusion, env);
    sde::engine::GPU::MetalBuilder<Num> builder(drift_ast, diff_ast);
    if (method == "euler" || method == "em" || method == "euler-maruyama") {
        builder.append_euler();
    }else if (method == "milstein") {
        builder.append_milstein();
    }
    sde::engine::GPU::MetalProgram<Num> prog(builder.get_source());

    auto buf = arr.request();
    auto ptr = static_cast<Num*>(buf.ptr);
    prog.launch(ptr, seed, dt, 0.0, initial_value, num_paths, num_steps);
    return arr;
#else
    throw std::runtime_error("Metal is not supported by your machine");
#endif
}

template<sde::concepts::FloatingPoint Num>
pybind11::array_t<Num> SDESimulator::simulate_cuda(size_t num_paths, size_t num_steps, Num dt, Num initial_value, std::string_view method, uint64_t seed, const sde::frontend::Environment<double>& env) {
#if HAS_CUDA
    auto drift_ast = sde::frontend::parse<Num>(drift, env);
    auto diff_ast = sde::frontend::parse<Num>(diffusion, env);
    sde::engine::GPU::CudaBuilder<Num> builder(drift_ast, diff_ast);
    if (method == "euler" || method == "em" || method == "euler-maruyama") {
        builder.append_euler();
    }else if (method == "milstein") {
        builder.append_milstein();
    }

    Num* paths;
    size_t size = config.num_paths * config.num_steps * sizeof(Num);
    cudaMalloc(&paths, size);

    sde::engine::GPU::CudaProgram<Num> prog(builder.get_source());
    prog.launch(paths, seed, dt, 0.0, initial_value, num_paths, num_steps);

    py::array_t<Num> arr({num_steps, num_paths});
    auto buf = arr.request();
    Num* ptr = static_cast<Num*>(buf.ptr);

    cudaMemcpy(ptr, paths, size, cudaMemcpyDeviceToHost);
    cudaFree(paths);

    return arr;
#else
throw std::runtime_error("Cuda is not supported by your machine");
#endif
}

template<sde::concepts::FloatingPoint Num>
pybind11::array_t<Num> SDESimulator::simulate_bytecode(size_t num_paths, size_t num_steps, Num dt, Num initial_value, std::string_view method, uint64_t seed, const sde::frontend::Environment<double>& env) {
    size_t simd_width = sde::simd::simd<Num>::size;
    size_t padded_paths = ((num_paths + simd_width - 1) / simd_width) * simd_width;
    size_t size = padded_paths * num_steps;
    Num* paths = sde::memory::aligned_allocator<Num>{}.allocate(size);

    py::capsule free_when_done(paths, [](void* p) {
        sde::memory::aligned_allocator<Num>{}.deallocate(static_cast<Num*>(p), 1);
    });

    auto arr = py::array_t<Num>({num_steps, num_paths},{num_paths * sizeof(Num), sizeof(Num)} ,paths, free_when_done);
    auto buf = arr.request();
    Num* ptr = static_cast<Num*>(buf.ptr);
    sde::Config config;
    config.num_paths = num_paths;
    config.num_steps = num_steps;
    config.dt = dt;
    config.initial_value = initial_value;
    config.seed = seed;
    auto drift_ast = sde::frontend::parse<Num>(drift, env);
    auto diff_ast = sde::frontend::parse<Num>(drift, env);
    auto drift_bytecode = sde::frontend::compile(drift_ast);
    auto diff_bytecode = sde::frontend::compile(diff_ast);
    sde::engine::CPU::BytecodeSafeEval<Num> drift_eval(drift_bytecode);
    sde::engine::CPU::BytecodeSafeEval<Num> diff_eval(diff_bytecode);
    if (method == "euler" || method == "em" || method == "euler-maruyama") {
        sde::engine::CPU::euler_functor em;
        sde::engine::CPU::dispatch_simulation<Num>(config, ptr, drift_eval, diff_eval, diff_eval, em, padded_paths);
    }else if (method == "milstein") {
        sde::engine::CPU::milstein_functor milstein;
        auto b_prime = sde::frontend::differentiate(diff_ast, "x");
        auto b_prime_opt = sde::frontend::optimize(b_prime);
        auto b_prime_bytecode = sde::frontend::compile(b_prime_opt);
        sde::engine::CPU::BytecodeSafeEval<Num> b_prime_eval(b_prime_bytecode);
        sde::engine::CPU::dispatch_simulation<Num>(config, ptr, drift_eval, diff_eval, b_prime_eval, milstein, padded_paths);
    }
    return arr;
}

template<sde::concepts::FloatingPoint Num>
pybind11::array_t<Num> SDESimulator::simulate_ast(size_t num_paths, size_t num_steps, Num dt, Num initial_value, std::string_view method, uint64_t seed, const sde::frontend::Environment<double>& env) {
    size_t simd_width = sde::simd::simd<Num>::size;
    size_t padded_paths = ((num_paths + simd_width - 1) / simd_width) * simd_width;
    size_t size = padded_paths * num_steps;
    Num* paths = sde::memory::aligned_allocator<Num>{}.allocate(size);

    py::capsule free_when_done(paths, [](void* p) {
        sde::memory::aligned_allocator<Num>{}.deallocate(static_cast<Num*>(p), 1);
    });

    auto arr = py::array_t<Num>({num_steps, num_paths},{num_paths * sizeof(Num), sizeof(Num)} ,paths, free_when_done);
    auto buf = arr.request();
    Num* ptr = static_cast<Num*>(buf.ptr);
    sde::Config config;
    config.num_paths = num_paths;
    config.num_steps = num_steps;
    config.dt = dt;
    config.initial_value = initial_value;
    config.seed = seed;
    auto drift_ast = sde::frontend::parse<Num>(drift, env);
    auto diff_ast = sde::frontend::parse<Num>(drift, env);
    sde::engine::CPU::ASTSafeEval<Num> drift_eval(drift_ast);
    sde::engine::CPU::ASTSafeEval<Num> diff_eval(diff_ast);
    if (method == "euler" || method == "em" || method == "euler-maruyama") {
        sde::engine::CPU::euler_functor em;
        sde::engine::CPU::dispatch_simulation<Num>(config, ptr, drift_eval, diff_eval, diff_eval, em, padded_paths);
    }else if (method == "milstein") {
        sde::engine::CPU::milstein_functor milstein;
        auto b_prime = sde::frontend::differentiate(diff_ast, "x");
        auto b_prime_opt = sde::frontend::optimize(b_prime);
        sde::engine::CPU::ASTSafeEval<Num> b_prime_eval(b_prime_opt);
        sde::engine::CPU::dispatch_simulation<Num>(config, ptr, drift_eval, diff_eval, b_prime_eval, milstein, padded_paths);
    }
    return arr;
}

PYBIND11_MODULE(sde_simulator, m) {
    m.doc() = "High-performance SDE simulator";

    py::class_<SDESimulator>(m, "SDESimulator")
        .def(py::init<std::string, std::string>(),
             py::arg("diffusion"),
             py::arg("drift"))
        .def("simulate", &SDESimulator::simulate,
             py::arg("num_paths"),
             py::arg("num_steps"),
             py::arg("dt"),
             py::arg("backend") = "gpu",
             py::arg("initial_value") = 1.0,
             py::arg("method") = "euler",
             py::arg("seed") = py::none(),
             py::arg("params") = py::dict(),
             py::arg("precision") = "float");
}