cmake_minimum_required(VERSION 3.16)

project(omni-pathfinder)

find_package(geode REQUIRED)

add_geode_mod(${PROJECT_NAME} VERSION 1.0.0 SOURCES main.cpp)

