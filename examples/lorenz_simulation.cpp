/**
 * @file lorenz_simulation.cpp
 * @brief Numerical simulation of the chaotic Lorenz Attractor.
 * @author Technical Editorial IGO
 * @date 2026
 * @details Integrates the Lorenz equations using the high-precision RK4 engine
 * and renders a real-time mathematical ASCII plot of the phase space trajectory 
 * directly in the Linux terminal. No external dependencies required.
 */

#include "../include/IGO/DVector.hpp"
#include <iostream>
#include <vector>
#include <cmath>

// Terminal plot dimensions
const int WIDTH = 70;
const int HEIGHT = 30;

/**
 * @brief Evaluates the 3D Lorenz Attractor vector field.
 * @param pos Current state vector in phase space (x, y, z).
 * @return DVector Tangent velocity vector.
 */
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

/**
 * @brief Renders a 2D projection (x-z plane) of the trajectory using ASCII art.
 * @param points Vector of discrete states in phase space.
 */
void render_ascii_plot(const std::vector<DVector>& points) {
    // Dynamically look for bounding boxes to scale the phase space coordinates
    double min_x = 1e9, max_x = -1e9;
    double min_z = 1e9, max_z = -1e9;

    for (const auto& p : points) {
        if (p[0] < min_x) min_x = p[0];
        if (p[0] > max_x) max_x = p[0];
        if (p[2] < min_z) min_z = p[2];
        if (p[2] > max_z) max_z = p[2];
    }

    // Initialize an empty text grid matrix
    std::vector<std::string> grid(HEIGHT, std::string(WIDTH, ' '));

    // Map and project x-z coordinates into grid integer indices
    for (const auto& p : points) {
        int col = static_cast<int>(((p[0] - min_x) / (max_x - min_x)) * (WIDTH - 1));
        int row = static_cast<int>(((p[2] - min_z) / (max_z - min_z)) * (HEIGHT - 1));
        
        // Invert row index because terminal lines print from top to bottom
        row = (HEIGHT - 1) - row;

        if (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH) {
            grid[row][col] = '#'; // Mark the orbit path
        }
    }

    // Print the scaled bounding box and geometric grid to standard output
    std::cout << "\n>> Phase Space Projection (X-Z Plane Map):\n";
    std::cout << "+" << std::string(WIDTH, '-') << "+\n";
    for (const auto& line : grid) {
        std::cout << "|" << line << "|\n";
    }
    std::cout << "+" << std::string(WIDTH, '-') << "+\n";
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "      IGO SCIENTIFIC ENGINE - LORENZ ATTRACTOR SIM      \n";
    std::cout << "========================================================\n\n";

    // Defining three highly adjacent initial states to verify deterministic chaos
    const DVector init_state1({10.0, 10.0, 10.0});
    const DVector init_state2({10.0, 10.0, 10.01});
    const DVector init_state3({10.0, 10.0, 10.02});

    const double t_final = 50.0; // Reduced time for optimization and terminal rendering
    const double step_size = 0.005;

    std::cout << ">> Initializing high-precision RK4 numerical integration kernels...\n";
    
    Trayectory system1(lorenz_field, init_state1);
    Trayectory system2(lorenz_field, init_state2);
    Trayectory system3(lorenz_field, init_state3);

    auto points1 = system1.points_rk4(t_final, step_size);
    auto points2 = system2.points_rk4(t_final, step_size);
    auto points3 = system3.points_rk4(t_final, step_size);

    std::cout << ">> Computation completed.\n";

    // Render the phase space butterfly diagram of the first orbit
    render_ascii_plot(points1);

    const std::size_t last_idx = points1.size() - 1;
    
    // Evaluate Euclidean separation to show sensitive dependence on initial conditions
    double err1 = (points1[last_idx] - points2[last_idx]).norm();
    double err2 = (points1[last_idx] - points3[last_idx]).norm();
    double err3 = (points2[last_idx] - points3[last_idx]).norm();

    std::cout << "\n========================================================\n";
    std::cout << "                METRIC SPACE DIAGNOSTICS                \n";
    std::cout << "========================================================\n";
    std::cout << "TRAJECTORY 1:\n  Final Point: [ " << points1[last_idx] << " ]\n\n";
    std::cout << "TRAJECTORY 2:\n  Final Point: [ " << points2[last_idx] << " ]\n\n";
    std::cout << "TRAJECTORY 3:\n  Final Point: [ " << points3[last_idx] << " ]\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "EXPONENTIAL DIVERGENCE (Lyapunov Phase Separation):\n";
    std::cout << "  |x1 - x2| = " << err1 << "\n";
    std::cout << "  |x1 - x3| = " << err2 << "\n";
    std::cout << "  |x2 - x3| = " << err3 << "\n";
    std::cout << "========================================================\n";
    std::cout << "[SUCCESS] Simulation finished successfully.\n";

    return 0;
}