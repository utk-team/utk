# CMake Project file for bounded BN-OT
# Fernando de Goes (fdegoes@caltech.edu)
# Copyright @ Aug 2012
project (pbnot)

cmake_minimum_required(VERSION 2.4.5)
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
if( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )
endif()
set( CMAKE_SHARED_LIBRARY_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS} -framework Accelerate" )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set( QT_USE_QTXML    TRUE )
set( QT_USE_QTMAIN   TRUE )
set( QT_USE_QTSCRIPT  TRUE )
set( QT_USE_QTOPENGL  TRUE )



SET(Boost_USE_STATIC_LIBS   ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
SET(Boost_FOUND FALSE)
FIND_PACKAGE(Boost 1.46.0 REQUIRED COMPONENTS program_options)
if ( Boost_FOUND )
  message(STATUS "Boost and boost_program_options found.")
  include_directories( ${Boost_INCLUDE_DIRS} )
endif( Boost_FOUND )
message(STATUS "booooooo ${Boost_LIBRAIRIES}")


    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
    
set(INCLUDESUITESPARSE 0 CACHE BOOL "Include /usr/include/suitesparse")
if(INCLUDESUITESPARSE)
	include_directories(/usr/include/suitesparse/)
endif(INCLUDESUITESPARSE)

find_package(CGAL )
find_package(Qt4)
find_package(OpenGL)

set( SUITESPARSE_LIBRARIES
     -lm -lamd -lcamd -lcolamd -lccolamd -lcholmod -lspqr -ltbb -lmetis -lblas -llapack )

set( SUITESPARSE_LIBRARIES
     -lm -lamd -lcamd -lcolamd -lccolamd -lcholmod -lspqr -ltbb -lmetis -lblas -llapack -lsuitesparseconfig)

set( HDRS
     matrix/sparse_array.h matrix/sparse_matrix.h matrix/suite_sparse_qr.h
     console_color.h convex_polygon.h dialog.h domain.h
     line_search.h primitives.h pw_line_search.h ramp.h random.h rt2.h
     scene.h timer.h types.h util.h window.h )

set( SRCS
     matrix/sparse_array.cpp matrix/sparse_matrix.cpp
     energy.cpp  histogram.cpp init.cpp io.cpp main.cpp
     optimizer.cpp regularity.cpp render.cpp sites.cpp  main.cpp)

set( MOCS moc_dialog.cxx moc_glviewer.cxx moc_window.cxx )

if( CGAL_FOUND AND QT4_FOUND AND OPENGL_FOUND  )
    include(${QT_USE_FILE})
    include(${CGAL_USE_FILE})


    # The executable itself.
    add_executable( pbnot ${SRCS} ${DT_UI_FILES} ${DT_RESOURCE_FILES} )

    # Link with Qt libraries
    target_link_libraries( pbnot ${QT_LIBRARIES}      ${Boost_PROGRAM_OPTIONS_LIBRARY})

    # Link with Glut and OpenGL
    target_link_libraries( pbnot ${GLUT_LIBRARY}     ${OPENGL_LIBRARY} )

    # Link with CGAL
    target_link_libraries( pbnot ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )

    # Link with SuiteSparse
    target_link_libraries( pbnot ${SUITESPARSE_LIBRARIES} )

    include_directories(.)
else()
    set( OT_MISSING_DEPS "")

    if(NOT OPENGL_FOUND)
        set(OT_MISSING_DEPS "OpenGL, ${OT_MISSING_DEPS}")
    endif()

    if(NOT QT4_FOUND)
        set(OT_MISSING_DEPS "Qt4, ${OT_MISSING_DEPS}")
    endif()

    if(NOT CGAL_FOUND)
        set(OT_MISSING_DEPS "the CGAL library, ${OT_MISSING_DEPS}")
    endif()

    if(NOT CGAL_Qt4_FOUND)
        set(OT_MISSING_DEPS "the CGAL Qt4 library, ${OT_MISSING_DEPS}")
    endif()

    message(STATUS "NOTICE: This project requires ${OT_MISSING_DEPS} and will not be compiled.")
endif()
