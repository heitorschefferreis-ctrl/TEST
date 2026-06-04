cmake_minimum_required(VERSION 3.25)

# Troque pelo ID do seu mod
project(ai_designer.omnipathfinder VERSION 1.0.0 LANGUAGES CXX)

# O @main do GitHub Actions vai injetar o pacote correto aqui
find_package(geode REQUIRED)

setup_geode_mod(ai_designer.omnipathfinder)

# Lembre-se de listar os arquivos .cpp corretos do seu mod aqui!
target_sources(ai_designer.omnipathfinder PRIVATE src/main.cpp)
