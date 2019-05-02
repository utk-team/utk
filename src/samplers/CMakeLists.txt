SET(SRCs

	#test_allowedpairs

	Whitenoise_2dd
	Whitenoise_3dd
	Whitenoise_4dd
  Whitenoise_6dd

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

	Halton_2dd
	Halton_3dd
	Halton_4dd
	Halton_10dd

	Hammersley_2dd

	RegularGrid_2dd
	RegularGrid_3dd

	HexagonalGrid_2dd

	Stratified_2dd
	Stratified_3dd
  Stratified_4dd
  Stratified_6dd

	NRooks_2dd
	NRooks_3dd

	LDBN_2dd
	LDBN_2di

	ProjectiveBlueNoise_2dd
	ProjectiveBlueNoise_3dd
	ProjectiveBlueNoise_4dd

	DartThrowing_2dd
	DartThrowing_6dd

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

	Rank1_2dd

	AAPatterns_2dd

	Penrose_2dd

	Korobov_2dd

	ART_2dd

	BNLDS_2dd
	BNLDS_optimizer_Mindist_Zero_2dd
	BNLDS_optimizer_Mindist_Random_2dd
	BNLDS_optimizer_Mindist_RandomOpti_2dd
	BNLDS_optimizer_PCF_Zero_2dd
	BNLDS_optimizer_PCF_RandomOpti_2dd
	BNLDS_optimizer_PCF_Random_2dd

	VanDerCorput_1dd

	#Experimental

 	GeneticOwen_2dd
 	GeneticOwen_2di

	ExhaustiveOwen_2dd

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
add_executable(GradientDescent_2dd
			GradientDescent_2dd.cpp
			../../externals/dCpp/dCpp.cpp
			../../externals/dCpp/operators.cpp
			../../externals/dCpp/tau.cpp
			../../externals/dCpp/var.cpp
			${UTK_FILES})
target_link_libraries ( GradientDescent_2dd ${LINK} ${LINK_TBB} )
install(TARGETS GradientDescent_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)


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

if(USE_CGALv2)
INCLUDE_DIRECTORIES(SamplerCapCVT/)
file(GLOB SOURCES SamplerCapCVT/Geex/basics/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/mathematics/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/numerics/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/symbolics/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/properties/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/combinatorics/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/graphics/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/CVT/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/image/*.c*  ${SOURCES} )
INCLUDE_DIRECTORIES(SamplerCapCVT/Geex/third_party/)
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/zlib/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/gzstream/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/png/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/GLsdk/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/pbuffer/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/glut_viewer/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/glu/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/misc/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/LBFGSB/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/HLBFGS/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/METIS/*.c*  ${SOURCES} )
INCLUDE_DIRECTORIES(SamplerCapCVT/Geex/third_party/tsANN/include)
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/tsANN/src/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/tetgen/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/AntTweakBar/*.c*  ${SOURCES} )
INCLUDE_DIRECTORIES(SamplerCapCVT/Geex/third_party/AntTweakBar/include)
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/LMBM/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/f2c/src/libF77/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/f2c/src/libI77/*.c*  ${SOURCES} )
INCLUDE_DIRECTORIES(SamplerCapCVT/Geex/third_party/f2c/include)
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/bluemath/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/rand48/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party/tinyxml/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party2/geodesic/*.c*  ${SOURCES} )
file(GLOB SOURCES SamplerCapCVT/Geex/third_party2/lodepng/*.c*  ${SOURCES} )
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/samplers/SamplerCapCVT/Geex/third_party/AntTweakBar/lib)
ADD_LIBRARY(geex ${SOURCES} ${HeaderFiles})

file(GLOB CapCVT_SOURCES SamplerCapCVT/gx_capcvt2d/generated/*.c*  ${CapCVT_SOURCES} )
add_executable(CapCVT_2dd
			SamplerCapCVT/gx_capcvt2d/cvt.cpp
			SamplerCapCVT/gx_capcvt2d/delaunay.cpp
			SamplerCapCVT/gx_capcvt2d/delaunay_cvt.cpp
			SamplerCapCVT/gx_capcvt2d/polygons.cpp
			${CapCVT_SOURCES}
			CapCVT_2dd.cpp
			${UTK_FILES})
target_link_libraries ( CapCVT_2dd ${LINK} geex glut GL GLU X11 Xext Xi CGAL AntTweakBar)
install(TARGETS CapCVT_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

add_executable(CVT_2dd
			SamplerCapCVT/gx_capcvt2d/cvt.cpp
			SamplerCapCVT/gx_capcvt2d/delaunay.cpp
			SamplerCapCVT/gx_capcvt2d/delaunay_cvt.cpp
			SamplerCapCVT/gx_capcvt2d/polygons.cpp
			${CapCVT_SOURCES}
			CVT_2dd.cpp
			${UTK_FILES})
target_link_libraries ( CVT_2dd ${LINK} geex glut GL GLU X11 Xext Xi CGAL AntTweakBar)
install(TARGETS CVT_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

add_executable(FPO_2dd
			SamplerFPO/DT.cpp
			FPO_2dd.cpp
			${UTK_FILES})
target_link_libraries ( FPO_2dd ${LINK})
install(TARGETS FPO_2dd
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
endif(USE_CGALv2)



# if(USE_CIMG)
# SET(SRCs
#
# 	Victor2D2D_06021027_adaptive
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
# endif(USE_CIMG)


# if(USE_BOOST)
# SET(SRCs
#
# 	BNLDS_optimize_pattern_2dd
# 	#BNLDS_optimize_pattern_local
# 	BNLDS64_optimizer_PCF_RandomOpti_GPU_2dd
#
# 	)
# FOREACH(FILE ${SRCs})
#   add_executable(${FILE} ${FILE} ${UTK_FILES})
#   target_link_libraries ( ${FILE} ${LINK} ${Boost_LIBRARIES} ${OpenCL_LIBRARIES})
# 	install(TARGETS ${FILE}
# 					RUNTIME DESTINATION bin
# 					LIBRARY DESTINATION lib
# 					ARCHIVE DESTINATION lib)
# ENDFOREACH(FILE)
# endif(USE_BOOST)


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
