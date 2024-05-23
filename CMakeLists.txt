cmake_minimum_required(VERSION 3.15)

project(framework)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(frameworklib)

target_sources(frameworklib PRIVATE
    loop.hpp
    loop.cpp
    window.hpp
    window.cpp
    window_mode.hpp
)

target_compile_features(frameworklib PUBLIC cxx_std_23)
target_include_directories(frameworklib PUBLIC .)

add_executable(framework main.cpp)
target_link_libraries(framework PRIVATE frameworklib)
