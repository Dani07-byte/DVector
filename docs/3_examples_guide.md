# Examples Guide

This guide explains how the example programs turn a system of differential equations into a `DVector` function and then solve it with `Trayectory`.

## How an equation-based system maps into `DVector`

The library uses:

- `DVector` to represent a state vector
- `Trayectory` to integrate the state over time
- a callable function that takes `const DVector& state` and returns `DVector` derivatives

A system like:

```
dx/dt = f(x, y, z)
dy/dt = g(x, y, z)
dz/dt = h(x, y, z)
```

is represented as:

- `state[0] = x`
- `state[1] = y`
- `state[2] = z`

Then the derivative function builds a new `DVector`:

```cpp
DVector d;
d.reserve(3);
d.push_back(f(state[0], state[1], state[2]));
d.push_back(g(state[0], state[1], state[2]));
d.push_back(h(state[0], state[1], state[2]));
return d;
```

`Trayectory` then calls this function repeatedly to compute the next state.

---

## Example 1: Lorenz attractor

The Lorenz system has three variables `x`, `y`, `z` and these derivatives:

```
dx/dt = sigma * (y - x)
dy/dt = x * (rho - z) - y
dz/dt = x * y - beta * z
```

The example converts that into a `DVector` function:

```cpp
[[nodiscard]] DVector lorenz_field(const DVector& pos) {
    const double beta  = 8.0 / 3.0;
    const double rho   = 28.0;
    const double sigma = 10.0;
    
    DVector derivatives;
    derivatives.reserve(3);
    derivatives.push_back(sigma * (pos[1] - pos[0]));
    derivatives.push_back(pos[0] * (rho - pos[2]) - pos[1]);
    derivatives.push_back(pos[0] * pos[1] - beta * pos[2]);

    return derivatives;
}
```

- `pos[0]` is `x`
- `pos[1]` is `y`
- `pos[2]` is `z`

The returned `DVector` is the velocity vector in phase space.

Then the example creates a `Trayectory`:

```cpp
Trayectory system1(lorenz_field, init_state1);
auto points1 = system1.points_rk4(t_final, step_size);
```

This produces a sequence of `DVector` states representing the Lorenz trajectory.

---

## Example 2: Harmonic oscillator

The harmonic oscillator uses two variables `x` and `v`:

```
dx/dt = v
dv/dt = -x
```

The example converts that into:

```cpp
[[nodiscard]] DVector oscillator_field(const DVector& state) {
    DVector derivatives;
    derivatives.reserve(2);
    
    derivatives.push_back(state[1]);  // dx/dt = v
    derivatives.push_back(-state[0]); // dv/dt = -x

    return derivatives;
}
```

- `state[0]` is position `x`
- `state[1]` is velocity `v`

The solver is created the same way:

```cpp
Trayectory oscillator(oscillator_field, initial_state);
auto trajectory = oscillator.points_rk4(t_final, step_size);
```

This yields the phase-space orbit of the oscillator as a list of `DVector` states.

---

## Key patterns in both examples

1. **State vector initialization**
   - Use `DVector({ ... })` or push values into a `DVector`.
   - Example: `DVector initial_state({2.0, 0.0});`

2. **Derivative function**
   - Accept `const DVector& state`
   - Build a new `DVector` for derivatives
   - Use `push_back(...)` in the correct order

3. **Indexing**
   - `state[0]` is the first variable
   - `state[1]` is the second
   - `state[2]` is the third, if present

4. **Solver construction**
   - `Trayectory solver(field_function, initial_state);`

5. **Integration call**
   - `points_rk4(final_time, step_size)`
   - or `points(final_time, step_size)` for Euler

---

## Why this pattern works

The examples are built around the idea that a system of first-order equations is just a vector-valued function:

```
F(x, y, z) = [dx/dt, dy/dt, dz/dt]
```

`DVector` is the container for both:

- the current state
- the derivative vector returned by the system function

`Trayectory` takes care of applying the integrator to that function.

---

## Practical tip

If you have a system with variables named differently, choose a consistent ordering and use that ordering for both:

- `state[0]` = first variable
- `state[1]` = second variable
- `state[2]` = third variable

Then return the derivative vector in the same order.

---

## Summary

- The examples show how to transform a system of equations into a `DVector`-based derivative function.
- `push_back(...)` is how you add each derivative component.
- `state[n]` is how you read each variable.
- `Trayectory` solves the system by repeatedly evaluating that function and advancing the state.