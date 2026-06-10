# API Reference

## `DVector`

### `DVector(const std::vector<double>& coords = {})`
- Constructs a dynamic vector from an existing `std::vector<double>`.
- Default constructs an empty vector.

### `std::size_t size() const noexcept`
- Returns the number of elements in the vector.

### `void push_back(double a)`
- Appends one scalar value to the end of the vector.

### `void reserve(std::size_t N)`
- Reserves capacity for `N` coordinates in the underlying storage.

### `double norm() const noexcept`
- Returns the Euclidean L2 norm of the vector.
- Computes `sqrt(sum(coordinate_i^2))`.

### `double operator[](std::size_t n) const noexcept`
- Returns the value at index `n`.
- Read-only access only.

### `friend void operator+=(DVector& vec1, const DVector& vec2)`
- Adds `vec2` into `vec1` in place.
- Handles vectors of different sizes by padding the shorter vector with zeros.

### `friend DVector operator*(double a, const DVector& vec)`
- Returns a new vector equal to `a * vec`.

### `friend DVector operator*(const DVector& vec, double a)`
- Returns a new vector equal to `vec * a`.

### `friend DVector operator/(const DVector& vec, double a)`
- Returns a new vector equal to `vec / a`.
- If `a` is extremely close to zero, returns an empty `DVector`.

### `friend DVector operator+(const DVector& vec1, const DVector& vec2)`
- Returns the vector sum of `vec1` and `vec2`.
- If sizes differ, the smaller vector is treated as if padded with zeros.

### `friend DVector operator-(const DVector& vec1, const DVector& vec2)`
- Returns `vec1 - vec2`.
- Implemented using scalar multiplication and addition.

### `friend std::ostream& operator<<(std::ostream& file, const DVector& vec)`
- Prints the vector values as space-separated components.
- Useful for logging or terminal output.

---

## `Trayectory`

### `Trayectory(Func df, DVector initState, double initTime = 0.0)`
- Constructs an integrator instance.
- `df` is a callable that accepts `const DVector&` and returns `DVector`.
- `initState` is the initial state vector.
- `initTime` sets the starting time, default `0.0`.

> Note: With C++20, `Func` is constrained by the `VectorField` concept and must satisfy `std::invocable<F, const DVector&>` returning `DVector`. In C++17, the template is unrestricted.

### `std::vector<DVector> points(double tf, double h = 0.01) const`
- Integrates the ODE system from the current start time to `tf` using explicit Euler.
- `tf` is the final time.
- `h` is the step size.
- Returns a vector of `DVector` states along the trajectory.

### `std::vector<DVector> points_rk4(double tf, double h = 0.01) const`
- Integrates the ODE system from the current start time to `tf` using classical 4th-order Runge-Kutta (RK4).
- `tf` is the final time.
- `h` is the step size.
- Returns a vector of `DVector` states along the trajectory.

---

## `VectorField` (C++20 only)

### `template <typename F> concept VectorField`
- Valid only when compiling with C++20 or later.
- Requires that `F` can be invoked with `const DVector&`.
- Requires the result type to be `DVector`.

---

## Example usage

```cpp
#include <IGO/DVector.hpp>

DVector field(const DVector& state) {
    DVector d;
    d.reserve(2);
    d.push_back(state[1]);
    d.push_back(-state[0]);
    return d;
}

int main() {
    DVector initial({1.0, 0.0});
    Trayectory solver(field, initial);
    auto trajectory = solver.points_rk4(6.28, 0.01);
    return 0;
}
```

---

This reference covers the public symbols available in DVector.hpp.