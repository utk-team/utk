
if(USE_CIMG)
SET(SRCs
	ConstantThreshold
	RandomDithering
	BayerDithering
	BlueNoiseDithering
	)
FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} ${LINK_CIMG} )
	install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)
endif(USE_CIMG)