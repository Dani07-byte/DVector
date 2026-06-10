/**
 * @file harmonic_oscillator.cpp
 * @brief Simulation of a Simple Harmonic Oscillator in phase space.
 * @author Technical Editorial IGO
 * @date 2026
 * @details Integrates the linear ODE system for an ideal spring-mass system 
 * using the RK4 engine. Maps the 2D phase space (position vs velocity) 
 * directly into a terminal-rendered ASCII geometric grid.
 */

#include "../include/IGO/DVector.hpp"
#include <iostream>
#include <vector>
#include <cmath>

// Terminal plot configuration for a clean geometric aspect ratio
const int WIDTH = 70;
const int HEIGHT = 30;

/**
 * @brief Evaluates the 2D Harmonic Oscillator vector field.
 * @details Computes the system equations derived from Hooke's Law (with m = 1, k = 1):
 * dx/dt = v
 * dv/dt = -x
 * @param state Current state vector where state[0] = position (x), state[1] = velocity (v).
 * @return DVector Tangent velocity vector in phase space.
 */
[[nodiscard]] DVector oscillator_field(const DVector& state) {
    DVector derivatives;
    derivatives.reserve(2);
    
    derivatives.push_back(state[1]);  // dx/dt = v
    derivatives.push_back(-state[0]); // dv/dt = -x

    return derivatives;
}

/**
 * @brief Renders the 2D phase space orbit (position vs velocity) using ASCII art.
 * @param points Vector of discrete states along the closed orbit.
 */
void render_phase_space(const std::vector<DVector>& points) {
    // Determine the dynamic boundaries of the numerical orbit to center the plot
    double min_x = 1e9, max_x = -1e9;
    double min_v = 1e9, max_v = -1e9;

    for (const auto& p : points) {
        if (p[0] < min_x) min_x = p[0];
        if (p[0] > max_x) max_x = p[0];
        if (p[1] < min_v) min_v = p[1];
        if (p[1] > max_v) max_v = p[1];
    }

    // Add a conservative structural padding to prevent boundary clipping
    double padding_x = (max_x - min_x) * 0.15;
    double padding_v = (max_v - min_v) * 0.15;
    min_x -= padding_x; max_x += padding_x;
    min_v -= padding_v; max_v += padding_v;

    // Initialize an empty character grid matrix
    std::vector<std::string> grid(HEIGHT, std::string(WIDTH, ' '));

    // Draw the structural axes (Origin intersection where x = 0 and v = 0)
    int center_col = static_cast<int>(((0.0 - min_x) / (max_x - min_x)) * (WIDTH - 1));
    int center_row = static_cast<int>(((0.0 - min_v) / (max_v - min_v)) * (HEIGHT - 1));
    center_row = (HEIGHT - 1) - center_row; // Invert for terminal lines

    for (int i = 0; i < HEIGHT; ++i) {
        if (center_col >= 0 && center_col < WIDTH) grid[i][center_col] = '|';
    }
    if (center_row >= 0 && center_row < HEIGHT) {
        for (int j = 0; j < WIDTH; ++j) {
            grid[center_row][j] = (grid[center_row][j] == '|') ? '+' : '-';
        }
    }

    // Project the continuous phase space orbit into discrete grid coordinates
    for (const auto& p : points) {
        int col = static_cast<int>(((p[0] - min_x) / (max_x - min_x)) * (WIDTH - 1));
        int row = static_cast<int>(((p[1] - min_v) / (max_v - min_v)) * (HEIGHT - 1));
        row = (HEIGHT - 1) - row; 

        if (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH) {
            grid[row][col] = 'o'; // 'o' denotes conservative bound states
        }
    }

    // Print the final geometric mapping to standard output
    std::cout << "\n>> Harmonic Oscillator Phase Space (Horizontal: Position [x], Vertical: Velocity [v]):\n";
    std::cout << "+" << std::string(WIDTH, '-') << "+\n";
    for (const auto& line : grid) {
        std::cout << "|" << line << "|\n";
    }
    std::cout << "+" << std::string(WIDTH, '-') << "+\n";
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "      IGO SCIENTIFIC ENGINE - HARMONIC OSCILLATOR       \n";
    std::cout << "========================================================\n\n";

    // Initial conditions: x0 = 2.0 (stretched spring), v0 = 0.0 (released from rest)
    const DVector initial_state({2.0, 0.0});
    
    const double t_final = 2.0 * M_PI; // Exactly one full geometric period (T = 2*pi)
    const double step_size = 0.01;

    std::cout << ">> Integrating equations of motion via 4th Order Runge-Kutta...\n";
    
    Trayectory oscillator(oscillator_field, initial_state);
    auto trajectory = oscillator.points_rk4(t_final, step_size);

    std::cout << ">> Numerical solution acquired. Launching grid projector...\n";

    // Render the elliptical closed loop orbit
    render_phase_space(trajectory);

    // Theoretical verification layer
    const std::size_t last_idx = trajectory.size() - 1;
    std::cout << "\n========================================================\n";
    std::cout << "                CONSERVATION DIAGNOSTICS                \n";
    std::cout << "========================================================\n";
    std::cout << "Initial State:  [ " << initial_state << " ]\n";
    std::cout << "Final State:    [ " << trajectory[last_idx] << " ]\n";
    
    // Total Energy: E = 0.5 * (v^2 + x^2)
    double E_init = 0.5 * (initial_state[0]*initial_state[0] + initial_state[1]*initial_state[1]);
    double E_final = 0.5 * (trajectory[last_idx][0]*trajectory[last_idx][0] + trajectory[last_idx][1]*trajectory[last_idx][1]);
    
    std::cout << "Energy Drift (E_final - E_init): " << (E_final - E_init) << "\n";
    std::cout << "========================================================\n";
    std::cout << "[SUCCESS] Mathematical simulation finished cleanly.\n";

    return 0;
}