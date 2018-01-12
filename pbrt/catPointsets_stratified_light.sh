OUTPUT=$1
NBSPP=$2

if [[ -z "$OUTPUT" ]]
then
	echo "[ERROR] No output file specified"
	echo "Usage " $0 " <output> <nbpts>"
	exit
fi
if [[ -z "$NBSPP" ]]
then
	echo "[ERROR] No number of samples specified"
	echo "Usage " $0 " <output> <nbpts>"
	exit
fi

UTKPATH="~/Desktop/Recherche/UnicornTK2/src/build/bin/samplers/"
UTKDATAPATH="~/Desktop/Recherche/UnicornTK2/data/"


#"${UTKPATH}Whitenoise_2dd"
#"${UTKPATH}BNLDS_2dd -K 8 --lut-in ${UTKDATAPATH}BNLDS/factorK_8/fastoptim_lut_sobol1_2_k8_lvlmax3_sampler_BNLDS_optimizer_PCF_RandomOpti_2dd.dat"
#"${UTKPATH}SobolIndices_2dd -x 1 -y 2"
NBDIM=5
DIM[1]="${UTKPATH}SobolIndices_2dd -x 5 -y 6" #Image Plane (2D)
DIM[2]="${UTKPATH}SobolIndices_2dd -x 3 -y 4" #Depth of Field (2D)
#Bounce 1
DIM[3]="${UTKPATH}Stratified_2dd" #Light point (2D) 
DIM[4]="${UTKPATH}SobolIndices_2dd -x 7 -y 8"  #Light Scattering (2D)
DIM[5]="${UTKPATH}SobolIndices_2dd -x 9 -y 10" #BSDF (2D)
#.
#.
#.

function catPts()
{
	if [ -z $1 ] || [ -z $2 ] || [ -z $3 ]
	then
	echo "Usage ./cat_files.sh <input_file0> <input_file2> <output_file>"
	exit
	fi

	nblines1=`cat $1 | wc -l`
	nblines2=`cat $2 | wc -l`

	if [ "${nblines1}" != "${nblines2}" ]
	then
		echo "Joining $1 (${nblines1} lines) and $2 (${nblines2} lines)"
		echo "[ERROR] Cannot concatenate pointset of different sizes"
		exit
	fi

	paste $1 $2 >> $3
}

# if [ -e "${OUTPUT}" ]
# then
# 	echo "[ERROR] File ${OUTPUT} exists, Removing ?"
# 	read -p "(yes/no) " choice
# 	if [[ $choice == "yes" ]]
# 	then
# 		rm ${OUTPUT}
# 	elif [[ $choice == "no" ]]
# 	then
# 		exit	
# 	else
# 		echo "Unrecognized choice, exiting"
# 		exit
# 	fi
# fi
rm ${OUTPUT}

touch ${OUTPUT}
current_sobolid=0
for i in `seq 1 $NBDIM`
do
	echo "Filling Dimensions ${i} `echo ${i}+1 | bc` with :${DIM[$i]} -o pts_dim${i}.dat -n ${NBSPP}"
	eval "${DIM[$i]} -o pts_dim${i}.dat -n ${NBSPP}"
	if [ $i = 1 ]
	then
		mv pts_dim${i}.dat ${OUTPUT}
	else	
		catPts ${OUTPUT} "pts_dim${i}.dat" tmp.dat
		mv tmp.dat ${OUTPUT}
		rm pts_dim${i}.dat
	fi
done