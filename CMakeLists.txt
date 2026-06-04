cmake_minimum_required(VERSION 3.25)

# Troque "heitor.omni_pathfinder" pelo ID real do seu mod
project(ai_designer.omnipathfinder VERSION 1.0.0 LANGUAGES CXX)

# IMPORTANTE: No ambiente do GitHub, use Geode com a primeira letra maiúscula
find_package(Geode REQUIRED)

setup_geode_mod(ai_designer.omnipathfinder)

# Informe os arquivos de código .cpp reais do seu projeto
target_sources(ai_designer.omnipathfinder PRIVATE src/main.cpp)
