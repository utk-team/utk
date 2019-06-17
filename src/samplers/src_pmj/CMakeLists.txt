cmake_minimum_required(VERSION 2.8.4)

project(src_pmj)

set (CMAKE_CXX_STANDARD 11)

# add extra include directories
include_directories(/usr/local/include src/PJSampleSequenceGenerator)


# add extra lib directories
link_directories(/usr/local/lib /usr/local/opt/libomp/lib)


if( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )
	message("Compiling for Mac")
	SET(CMAKE_CXX_FLAGS "-lm ")
	add_compile_options(-std=c++11 -mtune=native -march=native -O3 -Xpreprocessor  )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
	message("Compiling for Ununtu")
	SET(CMAKE_CXX_FLAGS "-lm ")
	add_compile_options(-std=c++11 -mtune=native -march=native -O3 -Xpreprocessor )
endif()

set(source_dir ./src)

file(GLOB source_files ${source_dir}/*.cpp)

add_executable(ProgressiveJittered_2dd ${source_dir}/mains/ProgressiveJittered_2dd.cpp ${source_files})
add_executable(ProgressiveMultiJittered_2dd ${source_dir}/mains/ProgressiveMultiJittered_2dd.cpp ${source_files})
add_executable(ProgressiveMultiJittered02_2dd ${source_dir}/mains/ProgressiveMultiJittered02_2dd.cpp ${source_files})
add_executable(TestsGenerator ${source_dir}/mains/TestsGenerator.cpp ${source_files})

install(TARGETS ProgressiveJittered_2dd DESTINATION ~/bin/)
install(TARGETS ProgressiveMultiJittered_2dd DESTINATION ~/bin/)
install(TARGETS ProgressiveMultiJittered02_2dd DESTINATION ~/bin/)

