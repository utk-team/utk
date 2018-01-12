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
	
	BNOT_2dd
	
	CVT_2dd
	
	CapCVT_2dd
	
	FPO_2dd
	
	#Experimental
	
	#GenerativeMatrix_2dd
	#BNOTSpp_2dd

# 	Nebraska_2dd
#
# 	DigitalSequence_2dd
#
# 	SamplerBNLDMicroJittSobol2Dx2D_2dd
# 	SamplerBNLDMicroJittSobol2Dx2D_2di
# 	SamplerBNLDMicroJittSobol2Dx2D_4dd
# 	SamplerBNLDMicroJittSobol2Dx2D_4di
#
# 	SamplerBNLDMicroJittSobolND_2dd
# 	SamplerBNLDMicroJittSobolND_4dd
# 	SamplerBNLDMicroJittSobolND_4di
#
# 	SamplerDartThrowing_2dd
#
# 	DTDyadic_2dd
#
# 	SobolPhiN_2dd
#
# 	TiledBasedLDSequence_2dd
# 	TiledBasedLDSequence_2di
#

	#Victor06022017_2dd
	#Victor06022017_2di
	#Victor06022017_4dd
	#Victor06022017_4di

	#Victor06022017_generator_2d
	
# 	VictorNDGenetic_06022017_2dd
# 	VictorNDGenetic_06022017_3dd
# 	VictorNDGenetic_06022017_3di
# 	VictorNDGenetic_06022017_4dd
# 	VictorNDGenetic_06022017_4di
# 
# 	Victor2D2D_06022017_2dd
# 	Victor2D2D_06022017_2di
# 	Victor2D2D_06022017_4dd
# 	
# 	Victor2D2DGenetic_06022017_2dd
# 	Victor2D2DGenetic_06022017_4dd
# 	Victor2D2DGenetic_06022017_10dd
# 	
# 	Victor2D2Dfullpcf_06022017_2dd
# 	
# 	Victor2D2DOwen_06022017_6dd
# 	Victor2D2DOwen_06022017_10dd
# 	Victor2DOwen_06022017_4dd
# 	Victor2DOwen_06022017_10dd

	)
FOREACH(FILE ${SRCs})
  add_executable(${FILE} ${FILE} ${UTK_FILES})
  target_link_libraries ( ${FILE} ${LINK} ${LINK_TBB} )
  install(TARGETS ${FILE}
					RUNTIME DESTINATION bin
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)


#FastPoisson
add_executable(FastPoisson_2dd 
			FastPoisson_2dd.cpp
			../../samplers/SamplerFastPoisson/PDSampling.cpp
			../../samplers/SamplerFastPoisson/RangeList.cpp 
			../../samplers/SamplerFastPoisson/RNG.cpp 
			../../samplers/SamplerFastPoisson/ScallopedSector.cpp 
			../../samplers/SamplerFastPoisson/WeightedDiscretePDF.h
			${UTK_FILES})
target_link_libraries ( FastPoisson_2dd ${LINK} ${LINK_TBB} )
install(TARGETS FastPoisson_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
			
			
#Fattal
add_executable(Fattal_2dd 
			../../samplers/SamplerFattal/bimage/bimage.cc
			../../samplers/SamplerFattal/npde/tensor2d.cc	
			Fattal_2dd.cpp
			${UTK_FILES})
target_link_libraries ( Fattal_2dd ${LINK} ${LINK_TBB})
install(TARGETS Fattal_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
			
#ForcedRandom
add_executable(ForcedRandom_2dd 
			../../samplers/SamplerForcedRandom/src/ForcedRandomSampling/main.hpp
			../../samplers/SamplerForcedRandom/src/Common/MathHelpers.cpp
			../../samplers/SamplerForcedRandom/src/Common/MathTypes.cpp
			../../samplers/SamplerForcedRandom/src/Common/Random.cpp
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

