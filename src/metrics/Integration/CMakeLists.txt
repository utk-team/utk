SET(Metrics
    
    GaussianIntegrationTest
    HeavisideIntegrationTest
    BlinnPhongIntegrationTest

    BuildGaussianIntegrandDatabase
    BuildHeavisideIntegrandDatabase
    BuildBlinnPhongIntegrandDatabase
)


FOREACH(FILE ${Metrics})
    add_executable(${FILE} ${FILE}.cpp)
    target_include_directories(${FILE} PRIVATE "${PROJECT_SOURCE_DIR}/include/")
    target_link_libraries(${FILE} PRIVATE spdlog::spdlog CLI11::CLI11)
    
    IF (OpenMP_FOUND)
        target_link_libraries(${FILE} PRIVATE OpenMP::OpenMP_CXX)
    ENDIF()

    install(TARGETS ${FILE}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)
