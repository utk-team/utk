SET(SRCs

	LookForMatrices_2dd
	drawPointset
	test_recursivity
	lookForSobolIndices_ortho2D_3D
	LookForMatrices_base3_2dd

	)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} )
	install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
  
ENDFOREACH(FILE)
