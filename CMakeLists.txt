cmake_minimum_required(VERSION 3.25)

# Insira o ID do seu mod aqui
project(heitor.omni_pathfinder VERSION 1.0.0 LANGUAGES CXX)

# IMPORTANTE: Use "geode" tudo em letras minúsculas
find_package(geode REQUIRED)

setup_geode_mod(ai_designer.omnipathfinder)

# Altere "src/main.cpp" pelos arquivos de código reais do seu projeto
target_sources(ai_designer.omnipathfinder PRIVATE src/main.cpp)
