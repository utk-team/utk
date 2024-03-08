IF (UTK_USE_OPENMP)
    include(CPM)
    
    set(semidiscrete_ot_2d_patch git apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/externals/semidiscrete_ot_2d.patch)
    CPMAddPackage(
        NAME SEMIDISCRETE_OT_2D
        GITHUB_REPOSITORY nbonneel/semi_discrete_ot_2d
        GIT_TAG af331f8f1058291e359e51830fc3fe5c7009b2f4
        UPDATE_DISCONNECTED 1 
        PATCH_COMMAND ${semidiscrete_ot_2d_patch}
    )

    SET(UTK_EXTERNALS_SEMIDISCRETE_OT semidiscrete_ot_2d)
    add_definitions(-DUTK_EXTERNALS_SEMIDISCRETE_OT)
ENDIF()