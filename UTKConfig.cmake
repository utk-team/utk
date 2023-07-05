# Set all apropriates files
include(${CMAKE_CURRENT_LIST_DIR}/cmake/openmp.cmake)

if (UTK_LOG)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/spdlog.cmake)
    add_definitions(-DUTK_LOG)
endif()

set(UTK_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/")
set(UTK_DATA_PATH "${CMAKE_CURRENT_LIST_DIR}/data")
set(UTK_EXTERNALS_FOLDER "${CMAKE_CURRENT_LIST_DIR}/externals/")
file(GLOB_RECURSE UTK_EXTERNALS_SRC
    "${UTK_EXTERNALS_FOLDER}/*.cpp"
)

add_definitions(-DUTK_USE_OPENMP)
add_definitions(-DUTK_DATA_PATH=\"${UTK_DATA_PATH}\")
add_definitions(-D_USE_MATH_DEFINES) # Windows... Oh dear Windows...

add_library(UTK_LIBRARY ${UTK_EXTERNALS_SRC})
# target_include_directories(UTK_LIBRARY PRIVATE ${UTK_INCLUDE_DIR} ${UTK_EXTERNALS_FOLDER})
include_directories(${UTK_INCLUDE_DIR})
include_directories(${UTK_EXTERNALS_FOLDER})

target_link_libraries(UTK_LIBRARY OpenMP::OpenMP_CXX)