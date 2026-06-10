# IGO Numerical Integrator

A high-performance, header-only scientific C++ library designed for the numerical integration of ordinary differential equations (ODEs) and dynamical systems in computational physics. Developed under a strict academic, minimalist aesthetic by Editorial Científica IGO (Daniel Cárdenas Salazar), this framework provides a robust algebraic vector container and native integration engines optimized for Linux production environments.

---

## Key Features

* **Header-Only Architecture:** Seamless integration by including a single header file without linking compiled binary artifacts.
* **Dual-Standard Compatibility:** Uses C++17 as a baseline while supporting C++20 concepts for stricter compile-time vector-field validation.
* **Zero External Dependencies:** Built strictly on top of the C++ Standard Template Library (STL).
* **Performance-Driven Design:** Leverages move semantics, pre-allocated memory reserves, and avoids virtual polymorphism to reduce runtime overhead.

---

## Mathematical Foundations

The core engine maps and simulates the discrete time-evolution of a continuous dynamical system defined by a system of coupled first-order autonomous differential equations:

$$\frac{d\mathbf{x}}{dt} = \mathbf{f}(\mathbf{x})$$

Where $\mathbf{x} \in \mathbb{R}^N$ represents the state vector in the system's phase space, and $\mathbf{f}: \mathbb{R}^N \rightarrow \mathbb{R}^N$ denotes the continuous tangent vector field.

### 1. Algebraic Vector Space ($\mathbb{R}^N$)
The `DVector` class models the topological vector space. Beyond standard linear combinations, it implements a robust dimension-mismatch fallback wrapper. If two vectors have unequal dimensions ($n \neq m$), the smaller vector is padded with structural zeroes:

$$\mathbf{x} + \mathbf{y} = \mathbf{z} \in \mathbb{R}^{\max(n,m)}$$

$$
z_k =
\begin{cases}
x_k + y_k & \text{if } k \le \min(n,m) \\
x_k & \text{if } n > m \text{ and } k > m \\
y_k & \text{if } m > n \text{ and } k > n
\end{cases}
$$

The geometric magnitude of the state trajectory is tracked via the Euclidean $L_2$ norm:

$$\|\mathbf{x}\|_2 = \sqrt{\sum_{k=1}^N x_k^2}$$

### 2. Numerical Integration Mechanics

#### First-Order Euler Method
An explicit scheme that advances the phase space state using the localized initial derivative. It accumulates a global truncation error bounded at $\mathcal{O}(h)$:

$$\mathbf{x}_{n+1} = \mathbf{x}_n + h \mathbf{f}(\mathbf{x}_n)$$

#### Classical Fourth-Order Runge-Kutta (RK4)
To preserve geometric structures and minimize numerical energy drift in highly sensitive chaotic regimes, the engine evaluates four intermediate convex-combination slopes across the step interval $h$. This bounds the global truncation error strictly to $\mathcal{O}(h^4)$:

$$\mathbf{k}_1 = \mathbf{f}(\mathbf{x}_n)$$

$$\mathbf{k}_2 = \mathbf{f}\left(\mathbf{x}_n + \frac{h}{2}\mathbf{k}_1\right)$$

$$\mathbf{k}_3 = \mathbf{f}\left(\mathbf{x}_n + \frac{h}{2}\mathbf{k}_2\right)$$

$$\mathbf{k}_4 = \mathbf{f}(\mathbf{x}_n + h\mathbf{k}_3)$$

$$\mathbf{x}_{n+1} = \mathbf{x}_n + \frac{h}{6}\left(\mathbf{k}_1 + 2\mathbf{k}_2 + 2\mathbf{k}_3 + \mathbf{k}_4\right)$$

---

## Repository Architecture

```bash
.
├── .clangd
├── .gitignore
├── LICENSE
├── CMakeLists.txt
├── README.md
├── include/
│   └── IGO/
│       └── DVector.hpp
└── examples/
    ├── CMakeLists.txt
    ├── harmonic_oscillator.cpp
    └── lorenz_simulation.cpp
```

---

## Installation & Compilation Workflow

### Professional Build via CMake

This project defines the header-only library as an `INTERFACE` target. To compile the example simulations under Linux:

```bash
cmake -B build
cmake --build build
```

The compiled executables appear under `./build/examples/`. Run them directly:

```bash
./build/examples/lorenz_simulation
./build/examples/harmonic_oscillator
```

### C++ Standard Notes

The project uses C++17 by default in CMakeLists.txt. If your toolchain supports C++20 and you want the stricter concept-based validation, enable it explicitly:

```bash
cmake -B build20 -S . -DCMAKE_CXX_STANDARD=20
cmake --build build20
```

---

## Manual CLI Compilation (Without CMake)

The library is header-only and can be compiled directly from source:

```bash
g++ -std=c++17 examples/lorenz_simulation.cpp -Iinclude -O3 -o lorenz_sim
./lorenz_sim
```

For C++20 support:

```bash
g++ -std=c++20 examples/lorenz_simulation.cpp -Iinclude -O3 -o lorenz_sim
./lorenz_sim
```

If your system does not define `M_PI`, use `std::numbers::pi` with C++20 or define `M_PI` manually.

---

## Public API Example

```cpp
#include <IGO/DVector.hpp>

DVector lorenz_field(const DVector& state) {
    DVector derivative;
    derivative.reserve(3);
    derivative.push_back(10.0 * (state[1] - state[0]));
    derivative.push_back(state[0] * (28.0 - state[2]) - state[1]);
    derivative.push_back(state[0] * state[1] - (8.0 / 3.0) * state[2]);
    return derivative;
}

int main() {
    DVector initial_state({10.0, 10.0, 10.0});
    Trayectory lorenz(lorenz_field, initial_state);
    auto points = lorenz.points_rk4(50.0, 0.01);
    return 0;
}
```

---

## License

This project is licensed under the MIT License. You are free to copy, modify, distribute, and use this software for academic, research, or commercial workflows, provided the original copyright notice and credit to Daniel Cárdenas Salazar / Editorial Científica IGO are maintained.