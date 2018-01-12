SET(SRCs

	testIO_DAT
	testIO_EDAT
	testIO_BIN
	testIO_EBIN
	
	testSampler_stratified
	testSampler_whitenoise
	testSampler_sobol
	)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} )
  
ENDFOREACH(FILE)
