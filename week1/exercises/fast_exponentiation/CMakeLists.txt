cmake_minimum_required(VERSION 3.22)

project(FastExponention
  VERSION 0.1
  DESCRIPTION "Fast Exponentiation Algorithm"
  HOMEPAGE_URL "https://cryptocamp.website"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(fast_exp
  fast_exp.cpp
)
