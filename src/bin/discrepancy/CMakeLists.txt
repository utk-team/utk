SET(SRCs

	StarDiscrepancy_fromfile_2dd
	StarDiscrepancy_fromfile_2di

	L2Discrepancy_fromfile_2dd
	L2Discrepancy_fromfile_3dd
	L2Discrepancy_fromfile_4dd

	GeneralizedL2Discrepancy_fromfile_2dd
	GeneralizedL2Discrepancy_fromfile_3dd
	GeneralizedL2Discrepancy_fromfile_4dd
	GeneralizedL2Discrepancy_fromfile_10dd

	UnanchoredL2Discrepancy_fromfile_2dd
	UnanchoredL2Discrepancy_fromfile_3dd

	SymmetricL2Discrepancy_fromfile_2dd
	SymmetricL2Discrepancy_fromfile_3dd

	CenteredL2Discrepancy_fromfile_2dd
	CenteredL2Discrepancy_fromfile_3dd

	BoundariesStarDiscrepancy_fromfile_2dd
	BoundariesStarDiscrepancy_fromfile_3dd
	BoundariesStarDiscrepancy_fromfile_4dd

	)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} )
  install(TARGETS ${FILE}
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)

if(USE_TBB)

SET(SRCs
	Parallel_GeneralizedL2Discrepancy_fromfile_4dd
	Parallel_GeneralizedL2Discrepancy_fromfile_2dd
	)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK_TBB} )
	install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)

endif(USE_TBB)


# if(USE_MPFR)
# 
# SET(SRCs
# 	GeneralizedL2Discrepancy_HighPrec_fromfile_2dd
# 	GeneralizedL2Discrepancy_HighPrec_fromfile_3dd
# 	GeneralizedL2Discrepancy_HighPrec_fromfile_4dd
# 	)
# 
# FOREACH(FILE ${SRCs})
#   add_executable(${FILE} ${FILE} ${UTK_FILES})
#   target_link_libraries ( ${FILE} ${LINK_MPFR} )
# 	install(TARGETS ${FILE}
# 					RUNTIME DESTINATION bin
# 					LIBRARY DESTINATION lib
# 					ARCHIVE DESTINATION lib)
# 
# ENDFOREACH(FILE)
# 
# endif(USE_MPFR)
