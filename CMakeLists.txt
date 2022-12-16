Project(UnicornTK)
cmake_minimum_required (VERSION 3.12)


if(APPLE)
    message("-- Setting OpenMP flags on MacOs. Assuming `brew install libomp`")
    if(CMAKE_C_COMPILER_ID MATCHES "Clang\$")
      set(OpenMP_C_LIB_NAMES "omp" CACHE INTERNAL "OpenMP lib name for Xcode toolchain.")
      IF(EXISTS "/opt/homebrew/") #for homebrew on Apple Silicon
				message(STATUS "using M1 path for openmp")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp  -I/opt/homebrew/include" CACHE INTERNAL "OpenMP flags for Xcode toolchain.")
        set(OpenMP_omp_LIBRARY "/opt/homebrew/lib/libomp.dylib" CACHE INTERNAL "OpenMP lib name for Xcode toolchain.")
      else()
				message(STATUS "using classical path for openmp")
				set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include" CACHE INTERNAL "OpenMP flags for Xcode toolchain.")
        set(OpenMP_omp_LIBRARY "/usr/local/opt/libomp/lib/libomp.dylib" CACHE INTERNAL "OpenMP lib name for Xcode toolchain.")
      endif()
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang\$")
      set(OpenMP_CXX_LIB_NAMES "omp" CACHE INTERNAL "OpenMP lib name for Xcode toolchain.")
      IF(EXISTS "/opt/homebrew/")
			  message(STATUS "using M1 path for openmp")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp  -I/opt/homebrew/include"  CACHE INTERNAL "OpenMP flags for Xcode toolchain.")
        set(OpenMP_omp_LIBRARY "/opt/homebrew/lib/libomp.dylib" CACHE INTERNAL "OpenMP lib name for Xcode toolchain.")
      else()
				message(STATUS "using classical path for openmp")
				set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include" CACHE INTERNAL "OpenMP flags for Xcode toolchain.")
        set(OpenMP_omp_LIBRARY "/usr/local/opt/libomp/lib/libomp.dylib" CACHE INTERNAL "OpenMP lib name for Xcode toolchain.")
      endif()
    endif()
endif()
find_package(OpenMP REQUIRED)

#### C++11
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS ON)
MESSAGE(STATUS "C++11 enabled by cmake. ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/modules")
set(UTK_DATA_PATH "${PROJECT_SOURCE_DIR}/data")
add_definitions(-DUTK_DATA_PATH=\"${UTK_DATA_PATH}\")


option(USE_TBB "Using TBB library" false)
option(USE_DGTAL "Using DGtal library" false)
option(USE_CIMG "Using CIMG library" true)
option(USE_FFTW "Using FFTW3 library" true)
option(USE_MPFR "Using MPFR library" false)
option(USE_BOOST "Using Boost Compute library" false)
option(USE_CGAL "Using CGAL library version 3" false)
option(USE_CGALv2 "Using CGAL library version 2" false)

option(PROFILING_GPROF "Profiling with gprof" false)

if(PROFILING_GPROF)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif(PROFILING_GPROF)

message(STATUS "=========== Optional dependencies ===========")
if(USE_TBB)
	message(STATUS "==== USE_TBB      true")
	add_definitions(-DTBB_ON)
	find_package(TBB REQUIRED)
	include_directories(${TBB_INCLUDE_DIRS})
	SET(LINK_TBB ${TBB_LIBRARIES})
	message(STATUS "     TBB OK")
else()
  message(STATUS "==== USE_TBB      false")
endif(USE_TBB)

if(USE_DGTAL)
	message(STATUS "==== USE_DGTAL     true")
	add_definitions(-DDGTAL_ON)
	find_package(DGtal 0.9 REQUIRED)
	include_directories(${DGTAL_INCLUDE_DIRS})
	link_directories(${DGTAL_LIBRARY_DIRS})
	SET(LINK_DGTAL ${DGTAL_LIBRARIES})
	message(STATUS "     DGtal OK")
else()
  message(STATUS "==== USE_DGTAL    false")
endif(USE_DGTAL)

if(USE_CIMG)
  message(STATUS "==== USE_CIMG     true")
  add_definitions(-Dcimg_display=0)
	add_definitions(-DCIMG_ON)
	set(CIMG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/externals/CImg-1.6.9" CACHE FILEPATH "The directory containing CImg.h")
	include_directories(${CIMG_INCLUDE_DIR})
	SET(LINK_CIMG pthread)
	message(STATUS "     CImg OK")
else()
  message(STATUS "==== USE_CIMG     false")
endif(USE_CIMG)

if(USE_FFTW)
  message(STATUS "==== USE_FFTW     true")
  add_definitions(-DFFTW3_ON)
	find_package(FFTW REQUIRED)
	include_directories(${FFTW_INCLUDES})
	SET(LINK_FFTW ${FFTW_LIBRARIES})
	message(STATUS "     fftw3 OK")
else()
  message(STATUS "==== USE_FFTW     false")
endif(USE_FFTW)

if(USE_MPFR)
  message(STATUS "==== USE_MPFR     true")
	add_definitions(-DMPFR_ON)
	find_package(MPFR REQUIRED)
	find_package(GMP REQUIRED)
	set(MPFRC++_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/externals/mpfrc++-3.6.2" CACHE FILEPATH "The directory containing mpreal.h")
	include_directories(${MPFRC++_INCLUDE_DIR})
	include_directories(${MPFR_INCLUDES})
	include_directories(${GMP_INCLUDES})
	SET(LINK_MPFR ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
	message(STATUS "     MPFR OK")
else()
  message(STATUS "==== USE_MPFR     false")
endif(USE_MPFR)

if(USE_BOOST)
  message(STATUS "==== USE_BOOST    true")
	message(STATUS "     Including Boost Compute")
	find_package(OpenCL REQUIRED)
	include_directories(SYSTEM ${OpenCL_INCLUDE_DIRS})

	find_package(Boost 1.48 REQUIRED COMPONENTS program_options)
	include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
	include_directories("${PROJECT_SOURCE_DIR}/externals/boost-compute/include")
	message(STATUS "     Boost Compute OK")
	else()
	message(STATUS "==== USE_BOOST    false")

endif(USE_BOOST)

if(USE_CGAL OR USE_CGALv2)
message(STATUS "==== USE_CGAL     true")
message(STATUS "     Including CGAL")
	message(STATUS "     In case of issue, you can download the CGAL library, compile it, and then use it by changing the CGAL_DIR
	with the ccmake tool.")
	find_package(CGAL REQUIRED)
	find_package(GMP REQUIRED)

	include( ${CGAL_USE_FILE} )
	include_directories(
		${CGAL_INCLUDE_DIRS}
		${CGAL_3RD_PARTY_INCLUDE_DIRS})

	SET(LINK_CGAL ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} ${GMP_LIBRARY})
	message(STATUS "     CGAL OK")
else()
	message(STATUS "==== USE_CGAL     false")
endif(USE_CGAL OR USE_CGALv2)

message(STATUS "=============================================")


set(UTK_FILES "")


ENABLE_TESTING()
include(CTest)
message(STATUS "Build test files ENABLED")
add_subdirectory(unit-tests)


add_subdirectory(src)


SET(CMAKE_INSTALL_PREFIX $ENV{HOME})
