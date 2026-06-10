# Quickstart

This quickstart shows how to use `DVector` and `Trayectory` from the IGO header-only library to define a simple system, integrate it, and print results.

## 1. Install the library

The library is header-only. From the repository root, the public header path is:

```cpp
#include <IGO/DVector.hpp>
```

If you compile manually, add `-Iinclude` to the compiler command.

## 2. Build the examples quickly

From the repository root:

```bash
cmake -B build
cmake --build build
```

This builds example executables under `build/examples/`.

## 3. Your first program

Create a file named `hello_igo.cpp` with this content:

```cpp
#include <iostream>
#include <IGO/DVector.hpp>

DVector linear_field(const DVector& state) {
    DVector derivative;
    derivative.reserve(2);
    derivative.push_back(state[1]);
    derivative.push_back(-state[0]);
    return derivative;
}

int main() {
    DVector initial_state({1.0, 0.0}); // position = 1, velocity = 0

    Trayectory integrator(linear_field, initial_state);

    auto trajectory = integrator.points_rk4(6.28, 0.01);

    std::cout << "Trajectory length: " << trajectory.size() << "\n";
    std::cout << "Last state: " << trajectory.back() << "\n";
    std::cout << "Norm of last state: " << trajectory.back().norm() << "\n";

    return 0;
}
```

This example uses:

- `DVector` to hold a vector of `double`
- `Trayectory` to integrate the system
- `points_rk4()` for classical Runge-Kutta integration

## 4. Compile the program

Run:

```bash
g++ -std=c++17 hello_igo.cpp -Iinclude -O3 -o hello_igo
```

Then execute:

```bash
./hello_igo
```

## 5. How it works

- `DVector({1.0, 0.0})` creates a 2D state vector.
- `linear_field()` returns the derivative vector `dx/dt = v`, `dv/dt = -x`.
- `Trayectory integrator(linear_field, initial_state)` sets up the solver.
- `points_rk4(6.28, 0.01)` computes the trajectory from `t = 0` to `t = 6.28` with step size `0.01`.

## 6. Notes

- `DVector` supports basic algebra: addition, subtraction, scalar multiplication, and norm.
- `Trayectory` also provides `points()` for the simpler Euler method.
- The header-only design means no library linking is required — only the include directory.
