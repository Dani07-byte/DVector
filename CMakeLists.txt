# ============================================================================
# @file CMakeLists.txt
# @brief Root build configuration script for the IGO Numerical Integrator.
# @author Technical Editorial IGO
# @date 2026
# ============================================================================

cmake_minimum_required(VERSION 3.14)

# Define project metadata. 
# Explicitly specifying CXX guarantees CMake doesn't lookup for C compilers.
project(IGO_NumericalIntegrator 
        VERSION 1.0.0 
        LANGUAGES CXX
)

# ----------------------------------------------------------------------------
# 1. Compiler Configurations & Global Banners
# ----------------------------------------------------------------------------

# Set standard compliance. We enforce C++17 as a baseline to satisfy the fallback 
# architecture of DVector.hpp while allowing users to override it via CLI flags.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile_commands.json automatically to feed language servers (clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure strict compilation warnings to guarantee high performance and clean code
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -O3)
endif()

# ----------------------------------------------------------------------------
# 2. Target Definition: Header-Only Library (INTERFACE)
# ----------------------------------------------------------------------------

# Header-only libraries are modeled as INTERFACE targets because they do not 
# produce a compiled binary (.a / .so) on their own.
add_library(IGO_Integrator INTERFACE)

# Define public include paths for the interface.
# BUILD_INTERFACE ensures local examples resolve <IGO/DVector.hpp> seamlessly.
# INSTALL_INTERFACE bounds the layout for downstream distribution architectures.
target_include_directories(IGO_Integrator INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ----------------------------------------------------------------------------
# 3. Subdirectory Routing for Modular Build Hierarchy
# ----------------------------------------------------------------------------

# Scan and append the examples subdirectory if the folder context is present
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt)
    message(STATUS "[IGO BUILD] 'examples' context detected. Appending target tree.")
    add_subdirectory(examples)
else()
    message(WARNING "[IGO BUILD] 'examples/CMakeLists.txt' not found. Skipping sample builds.")
endif()