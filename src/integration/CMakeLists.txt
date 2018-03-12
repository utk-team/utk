if(!USE_CIMG)
SET(SRCs

	IntegrationNSphere_fromfile_2dd
	IntegrationNSphere_fromfile_3dd
	IntegrationNSphere_fromfile_2di
	IntegrationNSphere_fromfile_3di
	IntegrationNSphere_fromfile_4dd
	
	IntegrationGaussian_fromfile_2dd
	IntegrationCenteredGaussian_fromfile_2dd
	IntegrationCorneredGaussian_fromfile_2dd

	)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} )
	install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
					
ENDFOREACH(FILE)
endif(!USE_CIMG)

if(USE_CIMG)
SET(SRCs

	IntegrationNSphere_fromfile_2dd
	IntegrationNSphere_fromfile_3dd
	IntegrationNSphere_fromfile_2di
	IntegrationNSphere_fromfile_3di
	IntegrationNSphere_fromfile_4dd
	
	IntegrationGaussian_fromfile_2dd
	IntegrationCenteredGaussian_fromfile_2dd
	IntegrationCorneredGaussian_fromfile_2dd

	IntegrationImage2D_fromfile_2dd
	IntegrationImage2D_fromfile_2di

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


if(USE_CGAL)
SET(SRCs

	IntegrationBoxArrangement_fromfile_2dd
	IntegrationTriangleArrangement_fromfile_2dd

	)
FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} ${LINK_CGAL} )
	install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)
endif(USE_CGAL)

	
