SET(SRCs

# 	full2DProjections_fromfile_3dd
# 	full2DProjections_fromfile_4dd
# 	full2DProjections_fromfile_10dd
#
# 	full3DProjections_fromfile_4dd

	draw_fromfile_2dd
#	draw_subset_fromfile_2dd
	histogram_compare_2dd
	2ddto2di
	2dito2dd
  2dito2dd-bis

  #generate_movement_gif
	normalize_pointset_2di_2dd

	)

FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} )
	install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)

ENDFOREACH(FILE)

#if(USE_CIMG)
#SET(SRCs
#	owen_scrambling_image
#	histogram_from_image
#	fourier_image
#	radial_image
#	)
#FOREACH(FILE ${SRCs})
#  add_executable(${FILE} ${FILE} ${UTK_FILES})
#  target_link_libraries ( ${FILE} ${LINK} ${LINK_CIMG} )
#	install(TARGETS ${FILE}
#					RUNTIME DESTINATION bin
#					LIBRARY DESTINATION lib
#					ARCHIVE DESTINATION lib)
#ENDFOREACH(FILE)
#endif(USE_CIMG)
