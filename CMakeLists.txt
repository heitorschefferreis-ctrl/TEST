cmake_minimum_required(VERSION 3.16)

project(omni-pathfinder)

find_package(geode PROFILE REQUIRED)

add_geode_executable(omni-pathfinder
    BINARY_NAME OmniPathfinder
    SOURCES main.cpp
)
