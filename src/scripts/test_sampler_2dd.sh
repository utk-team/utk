DISCLIMIT=16
DISCEXEC="./bin/discrepancy/GeneralizedL2Discrepancy_fromfile_2dd"

SPECTRALEXEC="./bin/spectral/stkfourier_2dd"

ZONEPLATEEXEC="./bin/render/zoneplate_fromfile_2dd"

INTLIMIT=16
INTEGRATIONEXEC="./bin/integration/IntegrationNSphere_fromfile_2dd"

POINTSESTEXEC="./bin/utils/draw_fromfile_2dd"

################################################################
###########PARAMETERS###########################################
################################################################

SAMPLER=
DISCREPANCY=
ZONEPLATE=
SPECTRAL=
INTEGRATION=
SAMPLER_NAME=
RANDOMIZE=

ZONEPLATE_PTS=
FOURIER_PTS=

ID=$RANDOM

TEMP=`getopt -o dzsip -l sampler:,randomize:,sname:,z-file:,s-file: -n 'generate_discrepancy_graph_onfly' -- "$@"`
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

################################################################
###########FUNCTIONS############################################
################################################################

echo_parameters ()
{
	echo "[State]"
	echo "Sampler: " ${SAMPLER}
	echo "Discrepancy: " ${DISCREPANCY}
	echo "Zoneplate: " ${ZONEPLATE}
	echo "Spectral: " ${SPECTRAL}
	echo "Integration: " ${INTEGRATION}
	echo "Pointsets: " ${POINTSETS}
	echo "---"
	echo "Pointset Zoneplate: " ${ZONEPLATE_PTS}
	echo "Pointset Fourier: " ${FOURIER_PTS}
	echo "Randomize: " ${RANDOMIZE}
	echo "Name: " ${SAMPLER_NAME}
}

check_parameters ()
{
	if [ -z "${SAMPLER}" ]
	then
		echo
		echo "[ERROR] No sampler specified"
		echo
		display_help
	fi
}

get_parameters ()
{
	eval set -- "$TEMP"

	while true; do
	case "$1" in

		--sampler ) SAMPLER="$2"; shift 2 ;;
		
		-d) DISCREPANCY=true; shift ;;
		-s) SPECTRAL=true; shift ;;
		-i ) INTEGRATION=true; shift ;;
		-z ) ZONEPLATE=true; shift ;;
		-p ) POINTSETS=true; shift ;;

		--randomize ) RANDOMIZE="--$2"; shift 2 ;;
		--z-file ) ZONEPLATE_PTS=$2; shift 2 ;;
		--s-file ) FOURIER_PTS=$2; shift 2 ;;
		--sname ) SAMPLER_NAME="$2"; shift 2;;
		-- ) shift; break ;;
		* ) break ;;
		
	esac
	done
	
	check_parameters
	
	if [ -z ${SAMPLER_NAME} ]
	then
		SAMPLER_NAME=`basename ${SAMPLER// /}`
	fi
}

display_help ()
{
	echo "[Help]"
	echo "--sampler <sampler_exe> \t the sampler executable"
	echo "WILL BE RUN AS ./sampler_exe -o [ouput file] -n [nb_points] -[randomize] -m 100"
	echo "-d \t will compute the L2 discrepancy of the sampler from 2^4 to 2^${DISCLIMIT}"
	echo "-s \t will compute the fourier and radial spectrum for the sampler. If f-file is set, it will compute the spectrum for the specified set. Otherwise, it will use 10 realisations of 8192 samples"
	echo "-z \t will compute the zoneplate for the sampler. If z-file is set, it will compute the spectrum for the specified set. Otherwise, it will generate 262k samples (1spp on a 512x512 image)"
	echo "-i \t will compute the integration variance from 20 realisations on an analytical disk"
	echo "-p \t will draw the pointsets for 1024, 4096 and 8192 points"
	echo "---"
	echo "--z-file [pointset file] \t will use this pointste for the zoneplate computation"
	echo "--s-file [pointset file] \t will use this pointste for the spectral computation"
	echo "--randomize [cp | xor] \t If set, we will randomize the set using cranley-patterson (cp) or xor shifting when generating several realisations"
	echo "--sname [string] \t The name of the sample. If unset, it will use the name of the exec as a sampler name"
	exit
}

get_parameters
echo_parameters



if [ $POINTSETS ]
then
eval ${SAMPLER} -o pointset_${SAMPLER_NAME}_${ID}.dat -n 1024
eval ${POINTSESTEXEC} -i pointset_${SAMPLER_NAME}_${ID}.dat -o pointset_${SAMPLER_NAME}_1024.ps --radius 0.006

eval ${SAMPLER} -o pointset_${SAMPLER_NAME}_${ID}.dat -n 4096
eval ${POINTSESTEXEC} -i pointset_${SAMPLER_NAME}_${ID}.dat -o pointset_${SAMPLER_NAME}_4096.ps --radius 0.004

eval ${SAMPLER} -o pointset_${SAMPLER_NAME}_${ID}.dat -n 8192
eval ${POINTSESTEXEC} -i pointset_${SAMPLER_NAME}_${ID}.dat -o pointset_${SAMPLER_NAME}_8192.ps --radius 0.002

rm pointset_${SAMPLER_NAME}_${ID}.dat
fi




if [ $SPECTRAL ]
then

PTS=
if [ ! -z $FOURIER_PTS ]
then
	PTS=${FOURIER_PTS}
else
	PTS=pointset_${SAMPLER_NAME}_${ID}.dat
	eval ${SAMPLER} -o pointset_${SAMPLER_NAME}_${ID}.dat -n 8192 -m 10 ${RANDOMIZE}
fi

eval $SPECTRALEXEC -i ${PTS} -P ${SAMPLER_NAME}_powerspectrum.png --limit-size
eval $SPECTRALEXEC -i ${PTS} --Rdat ${SAMPLER_NAME}_radial.dat --limit-size
eval $SPECTRALEXEC -i ${PTS} --Rdat ${SAMPLER_NAME}_logradial.dat --log-bins --limit-size --radial-nbbins 400

../scripts/plotRadialSpectrum.sh ${SAMPLER_NAME}_radial.dat ${SAMPLER_NAME}_radial.pdf
../scripts/fitRadialSpectrum.sh ${SAMPLER_NAME}_logradial.dat ${SAMPLER_NAME}_logradial_fit_4_3.pdf -4 -3
../scripts/fitRadialSpectrum.sh ${SAMPLER_NAME}_logradial.dat ${SAMPLER_NAME}_logradial_fit_4_2.pdf -4 -2
../scripts/fitRadialSpectrum.sh ${SAMPLER_NAME}_logradial.dat ${SAMPLER_NAME}_logradial_fit_4_1.pdf -4 -1

rm -f pointset_${SAMPLER_NAME}_${ID}.dat

fi




if [ $ZONEPLATE ]
then

PTS=
if [ ! -z $ZONEPLATE_PTS ]
then
	PTS=${ZONEPLATE_PTS}
else
	PTS=pointset_${SAMPLER_NAME}_${ID}.dat
	eval ${SAMPLER} -o pointset_${SAMPLER_NAME}_${ID}.dat -n 262144
fi

eval $ZONEPLATEEXEC -i ${PTS} -o ${SAMPLER_NAME}_zoneplate_1spp_zoom0-5.png -p 1 -z 0.5
eval $ZONEPLATEEXEC -i ${PTS} -o ${SAMPLER_NAME}_zoneplate_1spp_zoom1.png -p 1 -z 1
eval $ZONEPLATEEXEC -i ${PTS} -o ${SAMPLER_NAME}_zoneplate_1spp_zoom1-5.png -p 1 -z 1.5

eval $ZONEPLATEEXEC -i ${PTS} -o ${SAMPLER_NAME}_zoneplate_4spp_zoom0-5.png -p 4 -z 0.5
eval $ZONEPLATEEXEC -i ${PTS} -o ${SAMPLER_NAME}_zoneplate_4spp_zoom1.png -p 4 -z 1
eval $ZONEPLATEEXEC -i ${PTS} -o ${SAMPLER_NAME}_zoneplate_4spp_zoom1-5.png -p 4 -z 1.5

rm -f pointset_${SAMPLER_NAME}_${ID}.dat

fi





if [ $INTEGRATION ]
then
../scripts/generate_integration_graph_onfly.sh -i ${INTEGRATIONEXEC} --log --step 0.5 --min 4 --max ${INTLIMIT} --gnuplot -s "${SAMPLER} -m 20 ${RANDOMIZE}"
fi





if [ $DISCREPANCY ]
then
../scripts/generate_discrepancy_graph_onfly.sh -d ${DISCEXEC} --log --step 0.5 --min 4 --max ${DISCLIMIT} --gnuplot -s ${SAMPLER}
fi


