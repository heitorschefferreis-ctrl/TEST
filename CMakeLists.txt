cmake_minimum_required(VERSION 3.25)

# Project setup
project(ai_designer.omnipathfinder VERSION 1.0.0 LANGUAGES CXX)

# FIX: Use lowercase "geode" to match GitHub Actions runner variables
find_package(geode REQUIRED)

# Initialize the Geode mod
setup_geode_mod(ai_designer.omnipathfinder)

# Your source files
target_sources(ai_designer.omnipathfinder PRIVATE src/main.cpp)