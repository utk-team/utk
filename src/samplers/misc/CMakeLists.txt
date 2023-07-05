SET(SRCs
    Nebraska
)

FOREACH(FILE ${SRCs})
    add_executable(${FILE} ${FILE}.cpp)

    target_link_libraries(${FILE} PRIVATE CLI11::CLI11 spdlog::spdlog)

    install(TARGETS ${FILE}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)
