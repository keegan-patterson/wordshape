cmake_minimum_required(VERSION 3.5)
project(WordShape LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#configure_file(src/wordshape_config.hpp.in src/wordshape_config.hpp)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(WordShape
    src/main.cpp 
    src/game_engine.cpp 
    src/phys_engine.cpp)
target_include_directories(WordShape PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(WordShape PRIVATE cxx_std_17)
target_link_libraries(WordShape PRIVATE SFML::Graphics)
