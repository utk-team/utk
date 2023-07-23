cmake_minimum_required(VERSION 3.12)
Project(UnicornTK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_BUILD_TYPE "Release")

option(UTK_LOG "Enable utk logging" ON)

# Disabled by default, better to install it with 
# pybind provided extension
option(UTK_PYTHON "Enable build of pyutk (see setup.py)" OFF)
option(UTK_BUILD_EXE "Enable building of utk's executables" ON)

option(UTK_USE_OPENMP "Enable USE of OpenMp" ON)
# FFTW options
option(UTK_USE_FFTW "Enable use of FFTW" ON)
# CGal options
option(UTK_USE_CGAL "Enable projects depending on CGAL. " OFF)
 
# Finding libraries
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(spdlog)
include(cli11)

if (UTK_LOG)
    add_definitions(-DUTK_LOG)
endif()

if (UTK_USE_OPENMP)
    include(openmp)
    add_definitions(-DUTK_USE_OPENMP)
endif()

if (UTK_USE_FFTW)
    include(fftw)
    if (FFTW_FOUND)
        add_definitions(-DUTK_USE_FFTW)
        
        include_directories(${FFTW_INCLUDES})
    else()
        message("FFTW not found. UTK compiled without.")
    endif()
endif()

if (UTK_USE_CGAL)
    include(cgal)
    find_package(SuiteSparse)
endif()

set(UTK_DATA_PATH "${PROJECT_SOURCE_DIR}/data")
set(UTK_EXTERNALS_FOLDER "${PROJECT_SOURCE_DIR}/externals/")
set(UTK_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/")

add_definitions(-DUTK_DATA_PATH=\"${UTK_DATA_PATH}\")
add_definitions(-D_USE_MATH_DEFINES) # Windows... Oh dear Windows...

### UTK COMPILATION ###

IF (UTK_PYTHON)
    include(pybind11)
ENDIF()

add_subdirectory(src)
