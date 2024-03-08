find_package(GSL)

if (GSL_FOUND)
    set(sot_patch git apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/externals/sot.patch)
    include(CPM)
    CPMAddPackage(
     NAME SOT
     GITHUB_REPOSITORY loispaulin/Sliced-Optimal-Transport-Sampling
     GIT_TAG e1a9aadabd0594291c0aa6131efc930c1f855102    
     UPDATE_DISCONNECTED 1 
     PATCH_COMMAND ${sot_patch}
    )
else()
    message("GSL not found, SOT will not be compiled.")
endif()