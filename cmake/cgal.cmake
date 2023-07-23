if(TARGET CGAL::CGAL)
    return()
endif()

# Try to find CGAL first
find_package(CGAL REQUIRED)

IF(CGAL_FOUND)
    message(STATUS "Cgal found at: ${CGAL_DATA_DIR}")
ENDIF()