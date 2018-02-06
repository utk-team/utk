Project(UnicornTK)
cmake_minimum_required (VERSION 2.8)


#### C++11
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS ON)
MESSAGE(STATUS "C++11 enabled by cmake. ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/modules")
set(UTK_DATA_PATH "${PROJECT_SOURCE_DIR}/data")
add_definitions(-DUTK_DATA_PATH=\"${UTK_DATA_PATH}\")

set(USE_TBB 0 CACHE BOOL "Using TBB library")
set(USE_DGTAL 0 CACHE BOOL "Using DGtal library")
set(USE_CIMG 0 CACHE BOOL "Using CIMG library")
set(USE_FFTW 0 CACHE BOOL "Using FFTW3 library")
set(USE_MPFR 0 CACHE BOOL "Using MPFR library")
set(USE_BOOST 0 CACHE BOOL "Using Boost Compute library")
set(USE_CGAL 0 CACHE BOOL "Using CGAL library version 3")
set(USE_CGALv2 0 CACHE BOOL "Using CGAL library version 2")
#set(USE_GLM 1 CACHE BOOL "Using GLM library")
#set(USE_FLANN 1 CACHE BOOL "Using FLANN library"

set(PROFILING_GPROF 0 CACHE BOOL "Profiling with gprof")

if(PROFILING_GPROF)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif(PROFILING_GPROF)

if(USE_TBB)
	message(STATUS "Including TBB")
	add_definitions(-DTBB_ON)
	find_package(TBB REQUIRED)
	include_directories(${TBB_INCLUDE_DIRS})
	SET(LINK_TBB ${TBB_LIBRARIES})
	message(STATUS "TBB OK")
endif(USE_TBB)

if(USE_DGTAL)
	message(STATUS "Including DGtal")
	add_definitions(-DDGTAL_ON)
	find_package(DGtal 0.9 REQUIRED)
	include_directories(${DGTAL_INCLUDE_DIRS})
	link_directories(${DGTAL_LIBRARY_DIRS})
	SET(LINK_DGTAL ${DGTAL_LIBRARIES})
	message(STATUS "DGtal OK")
endif(USE_DGTAL)

if(USE_CIMG)
	message(STATUS "Including CImg")
	add_definitions(-Dcimg_display=0)
	add_definitions(-DCIMG_ON)
	set(CIMG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/externals/CImg-1.6.9" CACHE FILEPATH "The directory containing CImg.h")
	include_directories(${CIMG_INCLUDE_DIR})
	SET(LINK_CIMG pthread)
	message(STATUS "CImg OK")
endif(USE_CIMG)

if(USE_FFTW)
	message(STATUS "Including fftw3")
	add_definitions(-DFFTW3_ON)
	find_package(FFTW REQUIRED)
	include_directories(${FFTW_INCLUDE_DIR})
	SET(LINK_FFTW ${FFTW_LIBRARIES})
	message(STATUS "fftw3 OK")
endif(USE_FFTW)

if(USE_MPFR)
	message(STATUS "Including MPFR")
	add_definitions(-DMPFR_ON)
	find_package(MPFR REQUIRED)
	find_package(GMP REQUIRED)
	set(MPFRC++_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/externals/mpfrc++-3.6.2" CACHE FILEPATH "The directory containing mpreal.h")
	include_directories(${MPFRC++_INCLUDE_DIR})
	include_directories(${MPFR_INCLUDES})
	include_directories(${GMP_INCLUDES})
	SET(LINK_MPFR ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
	message(STATUS "MPFR OK")
endif(USE_MPFR)

if(USE_BOOST)
	message(STATUS "Including Boost Compute")
	find_package(OpenCL REQUIRED)
	include_directories(SYSTEM ${OpenCL_INCLUDE_DIRS})

	find_package(Boost 1.48 REQUIRED COMPONENTS program_options)
	include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
	include_directories("${PROJECT_SOURCE_DIR}/externals/boost-compute/include")
	message(STATUS "Boost Compute OK")
endif(USE_BOOST)

if(USE_CGAL OR USE_CGALv2)
	message(STATUS "Including CGAL")
	message(STATUS "In case of issue, you can download the CGAL library, compile it, and then use it by changing the CGAL_DIR
	with the ccmake tool.")
	find_package(CGAL REQUIRED)
	find_package(GMP REQUIRED)

	include( ${CGAL_USE_FILE} )
	include_directories(
		${CGAL_INCLUDE_DIRS}
		${CGAL_3RD_PARTY_INCLUDE_DIRS})

	SET(LINK_CGAL ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} ${GMP_LIBRARY})
	message(STATUS "CGAL OK")
endif(USE_CGAL OR USE_CGALv2)


# if(USE_GLM)
# 	message(STATUS "Including GLM")
# 	find_package(GLM REQUIRED)
# 	include_directories(SYSTEM ${GLM_INCLUDE_DIRS})
# 	message(STATUS "GLM OK")
# endif(USE_GLM)

# if(USE_FLANN)
# 	message(STATUS "Including FLANN")
# 	add_definitions(-DFLANN_ON)
# 	message(STATUS "FLANN OK")
# endif(USE_FLANN)

set(UTK_FILES "")

add_subdirectory(src)

ENABLE_TESTING()
include(CTest)
message(STATUS "Build test files ENABLED")
add_subdirectory(unit-tests)
