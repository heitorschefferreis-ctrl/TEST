cmake_minimum_required(VERSION 3.25)

# Troque "SeuModID" pelo ID do seu mod (ex: heitor.omni_pathfinder)
project(ai_designer.omnipathfinder VERSION 1.0.0 LANGUAGES CXX)

# CLI do Geode precisa dessas linhas exatas abaixo para baixar o SDK automaticamente no GitHub
include(FetchContent)
FetchContent_Declare(
    Geode
    GIT_REPOSITORY https://github.com
    GIT_TAG        main # Ou a tag da versão estável se souber
)
FetchContent_MakeAvailable(Geode)

setup_geode_mod(ai_designer.omnipathfinder)

# Adicione aqui os seus arquivos de código (.cpp) que estavam listados no arquivo antigo
target_sources(ai_designer.omnipathfinder PRIVATE src/main.cpp) 
