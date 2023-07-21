# Outer project to build UTK only once !

cmake_minimum_required(VERSION 3.12)
Project(UTKExamples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_BUILD_TYPE "Release")

find_package(UTK PATHS ../)

add_subdirectory(Evaluation)