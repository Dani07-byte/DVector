# ============================================================================
# @file examples/CMakeLists.txt
# @brief Subdirectory build definitions for autonomous IGO engine simulations.
# ============================================================================

# Define autonomous executable targets
add_executable(lorenz_simulation lorenz_simulation.cpp)
add_executable(harmonic_oscillator harmonic_oscillator.cpp)

# Link the executables to the interface target.
# Using 'PRIVATE' handles internal dependency tracking while 'IGO_Integrator' 
# automatically injects the include header directories ('-Iinclude') into g++.
target_link_libraries(lorenz_simulation PRIVATE IGO_Integrator)
target_link_libraries(harmonic_oscillator PRIVATE IGO_Integrator)