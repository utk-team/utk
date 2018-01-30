SET(SRCs

	CranleyPatterson_fromfile_2dd
	CranleyPatterson_fromfile_3dd
	CranleyPatterson_fromfile_2di
	CranleyPatterson_fromfile_3di

	XOR_fromfile_2di
	XOR_fromfile_3di

	OwenScrambling_fromfile_2di

	)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} )
	install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)
