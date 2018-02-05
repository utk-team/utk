SET(SRCs
		CranleyPatterson_2dd
		CranleyPatterson_2di
		
		DigitalXOR_2di
		
		OwenScrambling_2di
	)
FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} ${LINK_TBB} )
  install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)

