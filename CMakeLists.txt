cmake_minimum_required(VERSION 3.15)

project(framework)

add_library(frameworklib)

target_sources(frameworklib PRIVATE
    window.hpp
    window.cpp
)

add_executable(framework main.cpp)
target_link_libraries(framework PRIVATE frameworklib)
