SET(Scramblers
    CranleyPatterson
    DigitalShift
    Owen
    MicroJittering
)

SET(pyutk_scramblers)
SET(pyutk_scramblers_libs pybind11::module spdlog::spdlog)

FOREACH(FILE ${Scramblers})
    IF(UTK_BUILD_EXE)
        add_executable(${UTK_EXE_PREFIX}${FILE} ${FILE}.cpp)
        target_include_directories(${UTK_EXE_PREFIX}${FILE} PRIVATE "${UTK_INCLUDE_DIR}")
    ENDIF()

    IF (EXISTS ${UTK_EXTERNALS_FOLDER}/${FILE})
        FILE(GLOB_RECURSE ADDITIONNAL_CODE
                    "${UTK_EXTERNALS_FOLDER}/${FILE}/*.cpp"
            )
        
        LIST(APPEND pyutk_scramblers ${ADDITIONNAL_CODE})
        
        IF (UTK_BUILD_EXE)
            target_sources(${UTK_EXE_PREFIX}${FILE} PRIVATE ${ADDITIONNAL_CODE})
            target_include_directories(${UTK_EXE_PREFIX}${FILE} PRIVATE ${UTK_EXTERNALS_FOLDER})
        ENDIF()
    ENDIF()

    IF(UTK_BUILD_EXE)
        target_link_libraries(${UTK_EXE_PREFIX}${FILE} PRIVATE spdlog::spdlog CLI11::CLI11)

        install(TARGETS ${UTK_EXE_PREFIX}${FILE}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
    ENDIF()
ENDFOREACH(FILE)


IF (UTK_PYTHON)
    SET(pyutk_scramblers_bindings 
        "python/BaseScrambling.cpp"
        "python/Scrambling.cpp"
    )
    
    add_library(pyutkScramblers ${pyutk_scramblers} ${pyutk_scramblers_bindings})

    # Required by pybind
    set_target_properties(pyutkScramblers PROPERTIES POSITION_INDEPENDENT_CODE ON) 
    
    target_include_directories(pyutkScramblers PRIVATE "${UTK_INCLUDE_DIR}")
    target_include_directories(pyutkScramblers PRIVATE ${UTK_EXTERNALS_FOLDER})
    target_link_libraries(pyutkScramblers PRIVATE ${pyutk_scramblers_libs})
ENDIF()
