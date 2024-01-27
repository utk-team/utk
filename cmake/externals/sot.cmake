include(FetchContent)

find_package(GSL)

if (GSL_FOUND)
    set(sot_patch git apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/externals/sot.patch)
    FetchContent_Declare(
        SOT
        GIT_REPOSITORY https://github.com/loispaulin/Sliced-Optimal-Transport-Sampling
        GIT_TAG e1a9aadabd0594291c0aa6131efc930c1f855102
        PATCH_COMMAND ${sot_patch}
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SOT)
else()
    message("GSL not found, SOT will not be compiled.")
endif()