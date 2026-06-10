/**
 * @file DVector.hpp
 * @brief Header-only scientific library for numerical integration of ODE systems.
 * @author Editorial Científica IGO
 * @date 2026
 * * @details Provides a mathematical vector container (DVector) and a templated 
 * trajectory integrator supporting Euler and Runge-Kutta 4th Order (RK4) methods.
 * Optimized for high-performance computational physics under Linux environments.
 */

#pragma once

#include <cmath>
#include <cstddef>
#include <ostream>
#include <vector>

// Inclusión condicional de la librería de conceptos solo si se compila con C++20 o superior
#if __cplusplus >= 202002L
#include <concepts>
#endif

/**
 * @class DVector
 * @brief A dynamic vector container designed for algebraic operations in space phases.
 */
class DVector {
private:
    std::vector<double> coordinates;

public:
    /**
     * @brief Constructs a new DVector object.
     * @param coords Initializer list or vector of coordinates. Defaults to empty.
     */
    explicit DVector(const std::vector<double>& coords = {}) : coordinates(coords) {}

    /**
     * @brief Returns the dimensional size of the vector.
     * @return std::size_t Number of components.
     */
    [[nodiscard]] std::size_t size() const noexcept { return coordinates.size(); }

    /**
     * @brief Appends a new scalar component to the vector.
     * @param a Scalar value to append.
     */
    void push_back(double a) { coordinates.push_back(a); }

    /**
     * @brief Computes the Euclidean norm (L2 norm) of the vector.
     * @details Evaluates \f$ \|x\|_2 = \sqrt{\sum_{k=1}^N x_k^2} \f$ without trivial steps.
     * @return double The magnitude of the vector.
     */
    [[nodiscard]] double norm() const noexcept {
        double result = 0.0;
        const std::size_t N = size();
        for (std::size_t k = 0; k < N; ++k) { // Unificado a std::size_t para evitar warnings de signo
            result += coordinates[k] * coordinates[k];
        }
        return std::sqrt(result);
    }

    /**
     * @brief Reserves memory in the underlying vector container.
     * @param N Capacity allocation size.
     */
    void reserve(std::size_t N) {
        coordinates.reserve(N);
    }

    // --- Algebraic Operators ---

    /**
     * @brief Read-only subscript operator.
     * @param n Component index.
     * @return double Component value.
     */
    [[nodiscard]] double operator[](std::size_t n) const noexcept {
        return coordinates[n];
    }

    /**
     * @brief In-place vector addition operator.
     */
    friend void operator+=(DVector& vec1, const DVector& vec2) {
        vec1 = vec1 + vec2;
    }

    /**
     * @brief Left scalar multiplication.
     */
    [[nodiscard]] friend DVector operator*(double a, const DVector& vec) {
        std::vector<double> result;
        const std::size_t N = vec.size();
        result.reserve(N);
        for (std::size_t k = 0; k < N; ++k) {
            result.push_back(a * vec[k]);
        }
        return DVector(result);
    }

    /**
     * @brief Right scalar multiplication.
     */
    [[nodiscard]] friend DVector operator*(const DVector& vec, double a) {
        return a * vec; // Reuse left-multiplication logic
    }

    /**
     * @brief Scalar division operator. Prevents zero division using a strict numerical epsilon.
     */
    [[nodiscard]] friend DVector operator/(const DVector& vec, double a) {
        if (std::abs(a) < 1e-12) return DVector({}); // Returns a true empty DVector
        return vec * (1.0 / a);
    }

    /**
     * @brief Vector addition operator with support for mismatched dimensional sizes.
     * @details Non-overlapping dimensions are treated as padded with zeroes from the smaller vector.
     */
    [[nodiscard]] friend DVector operator+(const DVector& vec1, const DVector& vec2) {
        std::vector<double> result;
        const bool comp = vec2.size() < vec1.size();
        const DVector& minvec = comp ? vec2 : vec1;
        const DVector& maxvec = comp ? vec1 : vec2;
        
        const std::size_t minN = minvec.size();
        const std::size_t maxN = maxvec.size();
        result.reserve(maxN);

        for (std::size_t k = 0; k < maxN; ++k) {
            if (k < minN) {
                result.push_back(minvec[k] + maxvec[k]); 
            } else {
                result.push_back(maxvec[k]);
            } 
        }
        return DVector(result);
    }

    /**
     * @brief Vector subtraction operator via linear combination.
     */
    [[nodiscard]] friend DVector operator-(const DVector& vec1, const DVector& vec2) {
        return vec1 + (-1.0) * vec2;
    }

    /**
     * @brief Stream output operator for clean space-separated data formatting.
     */
    friend std::ostream& operator<<(std::ostream& file, const DVector& vec) {
        const std::size_t N = vec.size();
        if (N == 0) return file;
        
        std::size_t k = 0;
        while (k < N - 1) {
            file << vec[k] << " ";
            k++;
        }
        file << vec[k];
        return file;
    }
};

// ============================================================================
// |-----[ C++20 Concepts / C++17 Fallback Compilation Trigger ]--------------|
// ============================================================================

#if __cplusplus >= 202002L

/**
 * @brief Concept that restricts a type to be an explicit vector field function.
 * @details Mandates that the callable entity accepts a DVector and returns a DVector.
 * This is vastly superior to raw function pointers as it natively supports Lambdas, 
 * std::function, and Functors under zero-overhead abstractions.
 */
template <typename F>
concept VectorField = std::invocable<F, const DVector&> && 
                      std::same_as<std::invoke_result_t<F, const DVector&>, DVector>;

/**
 * @class Trayectory
 * @brief Computational engine for solving ordinary differential equations (ODEs).
 * @tparam Func A callable type restricted by the VectorField concept (C++20).
 */
template <VectorField Func> 
class Trayectory {

#else

/**
 * @class Trayectory
 * @brief Computational engine for solving ordinary differential equations (ODEs).
 * @tparam Func Unrestricted template fallback parameter for backward compatibility (C++17).
 */
template <typename Func>
class Trayectory {

#endif

// ============================================================================
// |-----[ Trayectory Class Implementation ]----------------------------------|
// ============================================================================
private:
    Func DF; // Dynamical Field Function: f(x)
    DVector initialState;
    double t0;

public:
    /**
     * @brief Constructs an integrator instance.
     * @param initState Initial state vector in phase space.
     * @param df Mathematical vector field.
     * @param initTime Initial time coordinate. Defaults to 0.
     */
    Trayectory(Func df, DVector initState, double initTime = 0.0) 
        : DF(df), initialState(std::move(initState)), t0(initTime) {}

    /**
     * @brief Solves the ODE system using the first-order Euler Method.
     * @details Accumulates error globally at \f$ \mathcal{O}(h) \f$.
     * @param tf Final time of simulation.
     * @param h Step size. Defaults to 0.01.
     * @return std::vector<DVector> Discrete trajectory points in phase space.
     */
    [[nodiscard]] std::vector<DVector> points(double tf, double h = 0.01) const {
        std::vector<DVector> result({initialState});

        const auto N = static_cast<std::size_t>(std::ceil((tf - t0) / h)) + 1;
        result.reserve(N);

        double t = t0;
        DVector actualState = initialState;
        actualState.reserve(initialState.size());

        while (t < tf) {
            actualState += h * DF(actualState);
            result.push_back(actualState);
            t += h;
        }

        return result;
    }

    /**
     * @brief Solves the ODE system using the classical 4th Order Runge-Kutta Method (RK4).
     * @details samples 4 intermediate slopes to bound the global truncation error to \f$ \mathcal{O}(h^4) \f$.
     * @param tf Final time of simulation.
     * @param h Step size. Defaults to 0.01.
     * @return std::vector<DVector> High-precision discrete trajectory points.
     */
    [[nodiscard]] std::vector<DVector> points_rk4(double tf, double h = 0.01) const {
        std::vector<DVector> result({initialState});
        
        const auto N = static_cast<std::size_t>(std::ceil((tf - t0) / h)) + 1;
        result.reserve(N);

        double t = t0;
        DVector actualState = initialState;
        
        while (t < tf) {
            DVector k1 = DF(actualState);
            DVector k2 = DF(actualState + (h * 0.5) * k1);
            DVector k3 = DF(actualState + (h * 0.5) * k2);
            DVector k4 = DF(actualState + h * k3);

            actualState += (h / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        
            result.push_back(actualState);
            t += h;
        }

        return result;
    }
};