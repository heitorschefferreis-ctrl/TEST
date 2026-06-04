cmake_minimum_required(VERSION 3.25)

# Troque pelo ID do seu mod
project(ai_designer.omnipathfinder VERSION 1.0.0 LANGUAGES CXX)

# Encontra o Geode injetado automaticamente pela GitHub Action v3
find_package(geode REQUIRED)

# Cria o mod
setup_geode_mod(ai_designer.omnipathfinder)

# Adicione aqui os arquivos .cpp do seu projeto original
target_sources(ai_designer.omnipathfinder PRIVATE src/main.cpp)
