

#include "../src/minisimd/minisimd.hpp"
#include <iostream>

int main() {
    std::cout << "==================Begin Float Tests==================\n\n";
    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::sin(sde::simd::simd<float>(x));
        auto expected = std::sin(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "sin(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::cos(sde::simd::simd<float>(x));
        auto expected = std::cos(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "cos(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::tan(sde::simd::simd<float>(x));
        auto expected = std::tan(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "tan(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::log(sde::simd::simd<float>(x));
        auto expected = std::log(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "log(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::exp(sde::simd::simd<float>(x));
        auto expected = std::exp(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "exp(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::cosh(sde::simd::simd<float>(x));
        auto expected = std::cosh(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "cosh(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::sinh(sde::simd::simd<float>(x));
        auto expected = std::sinh(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "sinh(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::tanh(sde::simd::simd<float>(x));
        auto expected = std::tanh(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "tanh(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    std::cout << "\n\n==================Begin Double Tests==================\n\n";

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::sin(sde::simd::simd<double>(x));
        auto expected = std::sin(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "sin(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::cos(sde::simd::simd<double>(x));
        auto expected = std::cos(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "cos(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::tan(sde::simd::simd<double>(x));
        auto expected = std::tan(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "tan(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::log(sde::simd::simd<double>(x));
        auto expected = std::log(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "log(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::exp(sde::simd::simd<double>(x));
        auto expected = std::exp(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "exp(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::cosh(sde::simd::simd<double>(x));
        auto expected = std::cosh(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "cosh(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::sinh(sde::simd::simd<double>(x));
        auto expected = std::sinh(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "sinh(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }

    for (float x : {1.5, 2.0, 2.718, 3.0, 5.0, 10.0, 100.0}) {
        auto result = sde::simd::tanh(sde::simd::simd<double>(x));
        auto expected = std::tanh(x);
        double error = std::abs(result[0] - expected) / expected;
        std::cout << "tanh(" << x << ") = " << result[0]
                  << ", expected = " << expected
                  << ", error = " << (error * 100) << "%" << std::endl;
    }
    return 0;
}