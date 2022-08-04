cmake_minimum_required(VERSION 3.14)

project(wordstats LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP)

add_executable(wordstats
  main.cpp
)
if(OpenMP_CXX_FOUND)
    target_link_libraries(wordstats PUBLIC OpenMP::OpenMP_CXX)
endif()

