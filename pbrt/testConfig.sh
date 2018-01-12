scene=$1

sampler=$2
#sampler="utkpixel"
#sampler="sobol"
#sampler="halton"
#sampler="stratified"
#sampler="random"

spp=$3
resolution=$4
bounces=$5
filepts=$6
fullsamples=$7
global=$8

if [[ -z "$scene" ]]
then
	echo "[ERROR] No scene specified"
	echo "Usage " $0 " <scene> <sampler> <spp> <imgsize> <bounces>"
	exit
fi
if [[ -z "$sampler" ]]
then
	echo "[ERROR] No sampler specified"
	echo "Usage " $0 " <scene> <sampler> <spp> <imgsize> <bounces>"
	exit
fi
if [[ -z "$spp" ]]
then
	echo "[ERROR] No number of samples per pixel specified"
	echo "Usage " $0 " <scene> <sampler> <spp> <imgsize> <bounces>"
	exit
fi
if [[ -z "$resolution" ]]
then
	echo "[ERROR] No image size specified"
	echo "Usage " $0 " <scene> <sampler> <spp> <imgsize> <bounces>"
	exit
fi
if [[ -z "$bounces" ]]
then
	echo "[ERROR] No number of bounces specified"
	echo "Usage " $0 " <scene> <sampler> <spp> <imgsize> <bounces>"
	exit
fi

python adaptPBRTScene.py ${scene} ${sampler} ${spp} ${resolution} ${bounces}

if [[ "$sampler" = "utkpixel" ]]
then
	if [[ -z "$filepts" ]]
	then
		echo "[ERROR] No file for samples specified (mandatory only for utkpixel sample)"
		echo "Usage " $0 " <scene> <sampler> <spp> <imgsize> <bounces> [samplesfile] [nbsamplestile] [globality]"
		exit
	fi
	if [[ -z "$fullsamples" ]]
	then
		echo "[ERROR] No number of samples for $filepts specified (mandatory only for utkpixel sample)"
		echo "Usage " $0 " <scene> <sampler> <spp> <imgsize> <bounces> [samplesfile] [nbsamplestile] [globality]"
		exit
	fi
	if [[ -z "$global" ]]
	then
		echo "[ERROR] No globality specified (mandatory only for utkpixel sample). Allowed values are Global, Local and LocalCP"
		echo "Usage " $0 " <scene> <sampler> <spp> <imgsize> <bounces> [samplesfile] [nbsamplestile] [globality]"
		exit
	fi
	
	#./catPointsets.sh $filepts $fullsamples
	
	totaldim=`echo "2*(2+3*${bounces})" | bc`
	echo "Requires "${totaldim}"D samples in "${filepts}
	nbcol=`head -n 1 ${filepts} | awk '{print NF}'`
	echo "File contains "${nbcol}"D samples"
	if [[ "${totaldim}" != "${nbcol}" ]]
	then
		echo "[ERROR] samples are not of the proper dimension"
		exit
	fi
	python adaptUTKSampler.py ../src/samplers/utksampler_pixel.h ${resolution} ${bounces} ${filepts} ${global}
	python adaptUTKSampler.py ../src/samplers/utksampler_pixel.cpp ${resolution} ${bounces} ${filepts} ${global}
fi

make && ./pbrt ${scene} --nthreads 1
