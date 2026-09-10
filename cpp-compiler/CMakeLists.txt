cmake_minimum_required(VERSION 3.10.0)
project(language VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(language)

target_sources(language PRIVATE
    src/logging/log.cpp
    src/logging/log.tpp
    src/logging/log.hpp
    src/main.cpp
)

target_include_directories(language PRIVATE include)