#!/bin/bash
set -e  # exit immediately if a command fails

# Remove old build directory
rm -rf build

# Create new build directory
mkdir build
cd build

# Run CMake
cmake ../src -DCMAKE_CXX_FLAGS="-fpermissive"

# Compile using all available cores
make -j$(nproc)
