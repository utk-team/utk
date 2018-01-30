SET(SRCs

	#test_allowedpairs

	Whitenoise_2dd
	Whitenoise_3dd
	Whitenoise_4dd

	Sobol_2dd
	Sobol_3dd
	Sobol_2di
	Sobol_3di
	Sobol_4dd
	Sobol_4di
	Sobol_10dd

	SobolIndices_2dd
	SobolIndices_2di
	SobolIndices_3dd
	SobolIndices_4dd
	SobolIndices_10dd

	SobolIndices_GrayCode_2dd

	SobolIndices_Tiled_2dd

	Halton_2dd
	Halton_3dd
	Halton_4dd
	Halton_10dd
	HaltonSpp_2dd
	HaltonSpp_10dd

	Hammersley_2dd

	RegularGrid_2dd
	RegularGrid_3dd

	HexagonalGrid_2dd

	Stratified_2dd
	Stratified_3dd
	Stratified_4dd

	StratifiedNRooks_2dd
	StratifiedNRooks_3dd

	NRooks_2dd
	NRooks_3dd

	LDBN_2dd
	LDBN_2di
	LDBN_4dd
	LDBN_Random_2dd

	ProjectiveBlueNoise_2dd
	ProjectiveBlueNoise_3dd
	ProjectiveBlueNoise_4dd

	DartThrowing_2dd

	RelaxedDartThrowing_2dd

	Step_2dd

	SinglePeak_2dd

	Niederreiter_2dd
	Niederreiter_2di
	Niederreiter_3dd

	Faure_2dd
	Faure_2di

	MultiJitter_2dd
	CMJ_2dd

	Owen_2dd
	Owen_4dd
	OwenPrime_2dd
 	OwenPrime_4dd
 	GeneticOwen_2dd
 	GeneticOwen_2di
	ExhautiveOwen_2dd

	Rank1_2dd
	Rank1Fibo_2dd

	AAPatterns_2dd

	Penrose_2dd

	Korobov_2dd

	BNLDS_2dd
	BNLDS_2dd_subset
	Sobol_BNLDS_4dd
	BNLDS_optimizer_Mindist_Zero_2dd
	BNLDS_optimizer_Mindist_Random_2dd
	BNLDS_optimizer_Mindist_RandomOpti_2dd
	BNLDS_optimizer_PCF_Zero_2dd
	BNLDS_optimizer_PCF_RandomOpti_2dd
	BNLDS_optimizer_PCF_Random_2dd

	TKSNetsMMD_2dd

	ART_2dd

	)
FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} ${LINK_TBB} )
  install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)


#Gradient Descent
#add_executable(GradientDescent_2dd
#			GradientDescent_2dd.cpp
#			../../externals/dCpp/dCpp.cpp
#			../../externals/dCpp/operators.cpp
##			../../externals/dCpp/tau.cpp
#			../../externals/dCpp/var.cpp
#			${UTK_FILES})
#target_link_libraries ( GradientDescent_2dd ${LINK} ${LINK_TBB} )
#install(TARGETS GradientDescent_2dd
#			RUNTIME DESTINATION bin
#			LIBRARY DESTINATION lib
#			ARCHIVE DESTINATION lib)


#FastPoisson
add_executable(FastPoisson_2dd
			FastPoisson_2dd.cpp
			SamplerFastPoisson/PDSampling.cpp
			SamplerFastPoisson/RangeList.cpp
			SamplerFastPoisson/RNG.cpp
			SamplerFastPoisson/ScallopedSector.cpp
			SamplerFastPoisson/WeightedDiscretePDF.h
			${UTK_FILES})
target_link_libraries ( FastPoisson_2dd ${LINK} ${LINK_TBB} )
install(TARGETS FastPoisson_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)


#Fattal
add_executable(Fattal_2dd
			SamplerFattal/bimage/bimage.cc
			SamplerFattal/npde/tensor2d.cc
			Fattal_2dd.cpp
			${UTK_FILES})
target_link_libraries ( Fattal_2dd ${LINK} ${LINK_TBB})
install(TARGETS Fattal_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

#ForcedRandom
add_executable(ForcedRandom_2dd
			SamplerForcedRandom/src/ForcedRandomSampling/main.hpp
			SamplerForcedRandom/src/Common/MathHelpers.cpp
			SamplerForcedRandom/src/Common/MathTypes.cpp
			SamplerForcedRandom/src/Common/Random.cpp
			ForcedRandom_2dd.cpp
			${UTK_FILES})
target_link_libraries ( ForcedRandom_2dd ${LINK} ${LINK_TBB})
install(TARGETS ForcedRandom_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)


if(USE_CIMG)
SET(SRCs

	Victor2D2D_06021027_adaptive

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


if(USE_BOOST)
SET(SRCs

	BNLDS_optimize_pattern_2dd
	#BNLDS_optimize_pattern_local
	BNLDS64_optimizer_PCF_RandomOpti_GPU_2dd

	)
FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} ${Boost_LIBRARIES} ${OpenCL_LIBRARIES})
	install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)
endif(USE_BOOST)


# if(USE_FLANN)
# SET(SRCs
#
# 	ProjectiveBlueNoise_Flann_2dd
#
# 	)
# FOREACH(FILE ${SRCs})
#   add_executable(${FILE} ${FILE} ${UTK_FILES})
#   target_link_libraries ( ${FILE} ${LINK} ${LINK_CIMG} )
# 	install(TARGETS ${FILE}
# 					RUNTIME DESTINATION bin
# 					LIBRARY DESTINATION lib
# 					ARCHIVE DESTINATION lib)
# ENDFOREACH(FILE)
# endif(USE_FLANN)
