SET(SRCs
)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} )
  install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)

if(USE_CIMG AND USE_FFTW)
SET(SRCs

	Fourier_2dd

	)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} ${LINK_FFTW} ${LINK_CIMG} )
  install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)
endif(USE_CIMG AND USE_FFTW)
