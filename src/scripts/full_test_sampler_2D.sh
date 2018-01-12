#!/bin/bash

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j 8

generatePointset=${1}
name=${2}
if [ -z "${name}" ]
then
	name="fulltest"
fi
drawPointset="../../../UnicornTK/src/build/utils/drawPointset2D"
powerspectrum="./bin/spectral/stkfourier_2dd"
zoneplate="../../../project_sampling/BlueLDS/src/build/stk/stk-zoneplate"
#zoneplate="../../../UnicornTK/src/build/zoneplates/zoneplate_stretch_pointset_to_domain"

#rm *${name}*

GENERATE=1
DRAW=1
FOURIER=1
STOCHASTIC=
ZONEPLATE=1
DISCREPANCY_POINTSET=1
DISCREPANCY_SEQUENCE=

star_discrepancy_limit=18
L2_discrepancy_limit=18


if [ ! -e $GENERATE ]
then

cmd="${generatePointset} -n 4 -o ${name}_4.dat"
echo ${cmd}
eval ${cmd}
cmd="${generatePointset} -n 16 -o ${name}_16.dat"
echo ${cmd}
eval ${cmd}
cmd="${generatePointset} -n 64 -o ${name}_64.dat"
echo ${cmd}
eval ${cmd}
cmd="${generatePointset} -n 256 -o ${name}_256.dat"
echo ${cmd}
eval ${cmd}
cmd="${generatePointset} -n 1024 -o ${name}_1024.dat"
echo ${cmd}
eval ${cmd}
cmd="${generatePointset} -n 4096 -o ${name}_4096.dat"
echo ${cmd}
eval ${cmd}

if [ ! -e $STOCHASTIC ]
then
cmd="${generatePointset} -n 4096 -o ${name}_4096_10.edat -m 10"
echo ${cmd}
eval ${cmd}
fi

cmd="${generatePointset} -n 8192 -o ${name}_8192.dat"
echo ${cmd}
eval ${cmd}
# cmd="${generatePointset} -n 16635 -o ${name}_16635.dat"
# echo ${cmd}
# eval ${cmd}
# cmd="${generatePointset} -n 65536 -o ${name}_65536.dat"
# echo ${cmd}
# eval ${cmd}

fi


if [ ! -e $DRAW ]
then

cmd="${drawPointset} -i ${name}_4.dat  -o ${name}_4_drawpointset.ps --numbered"
echo ${cmd}
eval ${cmd}
cmd="${drawPointset} -i ${name}_16.dat  -o ${name}_16_drawpointset.ps --numbered"
echo ${cmd}
eval ${cmd}
cmd="${drawPointset} -i ${name}_64.dat  -o ${name}_64_drawpointset.ps --numbered"
echo ${cmd}
eval ${cmd}
cmd="${drawPointset} -i ${name}_256.dat -o ${name}_256_drawpointset.ps --numbered"
echo ${cmd}
eval ${cmd}
cmd="${drawPointset} -i ${name}_1024.dat -o ${name}_1024_drawpointset.ps --radius 0.001"
echo ${cmd}
eval ${cmd}
cmd="${drawPointset} -i ${name}_4096.dat -o ${name}_4096_drawpointset.ps --radius 0.001"
echo ${cmd}
eval ${cmd}
cmd="${drawPointset} -i ${name}_8192.dat -o ${name}_8192_drawpointset.ps --radius 0.001"
echo ${cmd}
eval ${cmd}

for i in ${name}*.ps
do
	convert -density 100 ${i} ${i%%ps}png
done
	
fi


if [ ! -e $FOURIER ]
then

fourier_domain=4

cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_256.dat -P ${name}_256_powerspectrum_D${fourier_domain}.png --Rdat ${name}_256_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_256_radial_D${fourier_domain}.dat ${name}_256_radial_D${fourier_domain}.png

cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_1024.dat -P ${name}_1024_powerspectrum_D${fourier_domain}.png --Rdat ${name}_1024_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_1024_radial_D${fourier_domain}.dat ${name}_1024_radial_D${fourier_domain}.png

cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_4096.dat -P ${name}_4096_powerspectrum_D${fourier_domain}.png --Rdat ${name}_4096_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_4096_radial_D${fourier_domain}.dat ${name}_4096_radial_D${fourier_domain}.png

if [ ! -e $STOCHASTIC ]
then
cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_4096_10.edat -P ${name}_4096_10_powerspectrum_D${fourier_domain}.png --Rdat ${name}_4096_10_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_4096_10_radial_D${fourier_domain}.dat ${name}_4096_10_radial_D${fourier_domain}.png
fi

cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_8192.dat -P ${name}_8192_powerspectrum_D${fourier_domain}.png --Rdat ${name}_8192_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_8192_radial_D${fourier_domain}.dat ${name}_8192_radial_D${fourier_domain}.png

# cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_16635.dat -P ${name}_16635_powerspectrum_D${fourier_domain}.png --Rdat ${name}_16635_radial_D${fourier_domain}.dat"
# echo ${cmd}
# eval ${cmd}
# ../scripts/plotRadialSpectrum.sh ${name}_16635_radial_D${fourier_domain}.dat ${name}_16635_radial_D${fourier_domain}.png
# cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_65536.dat -P ${name}_65536_powerspectrum_D${fourier_domain}.bmp --Rdat ${name}_65536_radial_D${fourier_domain}.dat"
# echo ${cmd}
# eval ${cmd}
# ../scripts/plotRadialSpectrum.sh ${name}_65536_radial_D${fourier_domain}.dat ${name}_65536_radial_D${fourier_domain}.png
# convert -scale 50% ${name}_65536_powerspectrum_D${fourier_domain}.bmp ${name}_65536_powerspectrum_D${fourier_domain}_small.png

fourier_domain=2

cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_256.dat -P ${name}_256_powerspectrum_D${fourier_domain}.png --Rdat ${name}_256_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_256_radial_D${fourier_domain}.dat ${name}_256_radial_D${fourier_domain}.png

cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_1024.dat -P ${name}_1024_powerspectrum_D${fourier_domain}.png --Rdat ${name}_1024_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_1024_radial_D${fourier_domain}.dat ${name}_1024_radial_D${fourier_domain}.png

cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_4096.dat -P ${name}_4096_powerspectrum_D${fourier_domain}.png --Rdat ${name}_4096_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_4096_radial_D${fourier_domain}.dat ${name}_4096_radial_D${fourier_domain}.png

if [ ! -e $STOCHASTIC ]
then
cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_4096_10.edat -P ${name}_4096_10_powerspectrum_D${fourier_domain}.png --Rdat ${name}_4096_10_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_4096_10_radial_D${fourier_domain}.dat ${name}_4096_10_radial_D${fourier_domain}.png
fi

cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_8192.dat -P ${name}_8192_powerspectrum_D${fourier_domain}.png --Rdat ${name}_8192_radial_D${fourier_domain}.dat"
echo ${cmd}
eval ${cmd}
../scripts/plotRadialSpectrum.sh ${name}_8192_radial_D${fourier_domain}.dat ${name}_8192_radial_D${fourier_domain}.png

# cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_16635.dat -P ${name}_16635_powerspectrum_D${fourier_domain}.png --Rdat ${name}_16635_radial_D${fourier_domain}.dat"
# echo ${cmd}
# eval ${cmd}
# ../scripts/plotRadialSpectrum.sh ${name}_16635_radial_D${fourier_domain}.dat ${name}_16635_radial_D${fourier_domain}.png
# cmd="${powerspectrum} --domain ${fourier_domain} -i ${name}_65536.dat -P ${name}_65536_powerspectrum_D${fourier_domain}.png --Rdat ${name}_65536_radial_D${fourier_domain}.dat"
# echo ${cmd}
# eval ${cmd}
# ../scripts/plotRadialSpectrum.sh ${name}_65536_radial_D${fourier_domain}.dat ${name}_65536_radial_D${fourier_domain}.png
# convert -scale 50% ${name}_65536_powerspectrum_D${fourier_domain}.png ${name}_65536_powerspectrum_D${fourier_domain}_small.png


fi

if [ ! -e $ZONEPLATE ]
then

# cmd="${generatePointset} -n 1048576 -o ${name}_1048576.dat"
# echo ${cmd}
# eval ${cmd}
# cmd="${zoneplate} -i ${name}_1048576.dat -o ${name}_zoneplate4spp.png -p 4"
# echo ${cmd}
# eval ${cmd}
# cmd="${zoneplate} -i ${name}_1048576.dat -o ${name}_zoneplate4spp_large.png --zoom 0.5 -p 4"
# echo ${cmd}
# eval ${cmd}
# cmd="${zoneplate} -i ${name}_1048576.dat -o ${name}_zoneplate4spp_zoom.png --zoom 1.5 -p 4"
# echo ${cmd}
# eval ${cmd}
 
cmd="${generatePointset} -n 262144 -o ${name}_262144.dat"
echo ${cmd}
eval ${cmd}
cmd="${zoneplate} -i ${name}_262144.dat -o ${name}_zoneplate1spp.png -p 1"
echo ${cmd}
eval ${cmd}
cmd="${zoneplate} -i ${name}_262144.dat -o ${name}_zoneplate1spp_large.png --zoom 0.5 -p 1"
echo ${cmd}
eval ${cmd}
cmd="${zoneplate} -i ${name}_262144.dat -o ${name}_zoneplate1spp_zoom.png --zoom 1.5 -p 1"
echo ${cmd}
eval ${cmd}

fi


if [ ! -e $DISCREPANCY_POINTSET ]
then

rm *_${name}_pointset*discrepancy*Star*.png
../scripts/generate_discrepancy_graph_onfly.sh --min 4 --max $star_discrepancy_limit --log --gnuplot --step 0.25 -d "./bin/discrepancy/StarDiscrepancy_fromfile_2dd" -s "${1}" --sname "${name}_pointset"
../scripts/generate_discrepancy_graph_onfly.sh --min 4 --max $L2_discrepancy_limit --log --gnuplot --step 0.25 -d "./bin/discrepancy/GeneralizedL2Discrepancy_fromfile_2dd" -s "${1}" --sname "${name}_pointset"

fi

if [ ! -e $DISCREPANCY_SEQUENCE ]
then

rm *_${name}_sequence*discrepancy*Star*.png
../scripts/generate_discrepancy_graph_fromsequence.sh --min 4 --max $star_discrepancy_limit --log --gnuplot --step 0.25 -d "./bin/discrepancy/StarDiscrepancy_fromfile_2dd" -s "${1}" --sname "${name}_sequence"
../scripts/generate_discrepancy_graph_fromsequence.sh --min 4 --max $L2_discrepancy_limit --log --gnuplot --step 0.25 -d "./bin/discrepancy/GeneralizedL2Discrepancy_fromfile_2dd" -s "${1}" --sname "${name}_sequence"

fi

function generateTexFile
{
	rm $1.tex
	touch $1.tex
	
	echo "\documentclass[11pt,a4paper]{article}" >> $1.tex
	echo "\usepackage{graphicx}" >> $1.tex
	echo "\usepackage{float}" >> $1.tex
	echo "\usepackage[margin=0.5in]{geometry}" >> $1.tex

	echo "\begin{document}" >> $1.tex
	echo "\title{Results}" >> $1.tex
	echo "\maketitle" >> $1.tex
	
	
	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Pointsets" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex

	echo "\#pts & Spectrum & \#pts & Spectrum \\\\" >> $1.tex
	echo "16 & \includegraphics[width=0.33\textwidth]{${name}_16_drawpointset.png} &" >> $1.tex
	echo "64 & \includegraphics[width=0.33\textwidth]{${name}_64_drawpointset.png} \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_256_drawpointset.png} &" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_1024_drawpointset.png} \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_4096_drawpointset.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_8192_drawpointset.png} \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex
	
	echo "\caption{Pointsets}" >> $1.tex
	echo "\end{figure}" >> $1.tex

	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Fourier" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex

	fourier_domain=2
	echo "\#pts & Spectrum & \#pts & Spectrum \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_256_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_8192_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_1024_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "16635 & \includegraphics[width=0.33\textwidth]{${name}_16635_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_4096_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "65536 & \includegraphics[width=0.33\textwidth]{${name}_65536_powerspectrum_D${fourier_domain}_small.png} \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex

	echo "\caption{Fourier power spectra with frequency domain [-${fourier_domain}, ${fourier_domain}]}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	for i in `ls ${name}_4096_10_radial_D${fourier_domain}.png`
	do
		echo "" >> $1.tex
		echo "" >> $1.tex
		echo "%%Avg Fourier" >> $1.tex
		echo "\begin{figure}[H]" >> $1.tex
		echo "\centering" >> $1.tex
		echo "\includegraphics[width=0.5\textwidth]{${name}_4096_10_powerspectrum_D${fourier_domain}.png}" >> $1.tex
		echo "\includegraphics[width=0.5\textwidth]{${name}_4096_10_radial_D${fourier_domain}.png}" >> $1.tex
		echo "\caption{Fourier Spectrum averaged over 10 realisations}" >> $1.tex
		echo "\end{figure}" >> $1.tex
		break;
	done
	

	fourier_domain=4
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex

	echo "\#pts & Spectrum & \#pts & Spectrum \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_256_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_8192_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_1024_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "16635 & \includegraphics[width=0.33\textwidth]{${name}_16635_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_4096_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "65536 & \includegraphics[width=0.33\textwidth]{${name}_65536_powerspectrum_D${fourier_domain}_small.png} \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex

	echo "\caption{Fourier power spectra with frequency domain [-${fourier_domain}, ${fourier_domain}]}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	
	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Fourier" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c }" >> $1.tex

	echo "\#pts & Radial \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.5\textwidth]{${name}_256_radial_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "1024 & \includegraphics[width=0.5\textwidth]{${name}_1024_radial_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.5\textwidth]{${name}_4096_radial_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "8192 & \includegraphics[width=0.5\textwidth]{${name}_8192_radial_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "16635 & \includegraphics[width=0.5\textwidth]{${name}_16635_radial_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "65536 & \includegraphics[width=0.5\textwidth]{${name}_65536_radial_D${fourier_domain}.png} \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex

	echo "\caption{Fourier radial average spectra}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	for i in `ls ${name}_4096_10_radial_D${fourier_domain}.png`
	do
		echo "" >> $1.tex
		echo "" >> $1.tex
		echo "%%Avg Fourier" >> $1.tex
		echo "\begin{figure}[H]" >> $1.tex
		echo "\centering" >> $1.tex
		echo "\includegraphics[width=0.5\textwidth]{${name}_4096_10_powerspectrum_D${fourier_domain}.png}" >> $1.tex
		echo "\includegraphics[width=0.5\textwidth]{${name}_4096_10_radial_D${fourier_domain}.png}" >> $1.tex
		echo "\caption{Fourier Spectrum averaged over 10 realisations}" >> $1.tex
		echo "\end{figure}" >> $1.tex
		break;
	done
	

	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Zoneplates" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex
	
	echo "\#Spp & Zoneplate & \#Spp & Zoneplate \\\\" >> $1.tex
	echo "1 & \includegraphics[width=0.33\textwidth]{${name}_zoneplate1spp_zoom.png} &" >> $1.tex
	echo "4 & \includegraphics[width=0.33\textwidth]{${name}_zoneplate4spp_zoom.png} \\\\" >> $1.tex
	echo "1 & \includegraphics[width=0.33\textwidth]{${name}_zoneplate1spp.png} &" >> $1.tex
	echo "4 & \includegraphics[width=0.33\textwidth]{${name}_zoneplate4spp.png} \\\\" >> $1.tex
	echo "1 & \includegraphics[width=0.33\textwidth]{${name}_zoneplate1spp_large.png} &" >> $1.tex
	echo "4 & \includegraphics[width=0.33\textwidth]{${name}_zoneplate4spp_large.png} \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex

	echo "\caption{Zoneplates}" >> $1.tex
	echo "\end{figure}" >> $1.tex

	discrepancy_type="pointsets"
	
	for i in `ls ${name}_pointset*discrepancy*Star*.png`
	do
		echo "" >> $1.tex
		echo "" >> $1.tex
		echo "%%Discrepancy" >> $1.tex
		echo "\begin{figure}[H]" >> $1.tex
		echo "\centering" >> $1.tex
		echo "\includegraphics[width=\textwidth]{$i}" >> $1.tex
		echo "\caption{Star discrepancy (from ${discrepancy_type} }" >> $1.tex
		echo "\end{figure}" >> $1.tex
		
		break;
	done
	
	discrepancy_type="a single sequence"
	
	for i in `ls ${name}_sequence*discrepancy*Star*.png`
	do
		echo "" >> $1.tex
		echo "" >> $1.tex
		echo "%%Discrepancy" >> $1.tex
		echo "\begin{figure}[H]" >> $1.tex
		echo "\centering" >> $1.tex
		echo "\includegraphics[width=\textwidth]{$i}" >> $1.tex
		echo "\caption{Star discrepancy (from ${discrepancy_type}) }" >> $1.tex
		echo "\end{figure}" >> $1.tex
		
		break;
	done

# 	if [ ! -z "${discrepancy_file}" ]
# 	then
# 	
# 	echo "" >> $1.tex
# 	echo "" >> $1.tex
# 	echo "%%Discrepancy" >> $1.tex
# 	echo "\begin{figure}[H]" >> $1.tex
# 	echo "\centering" >> $1.tex
# 	echo "\includegraphics[width=\textwidth]{${discrepancy_file}}" >> $1.tex
# 	echo "\caption{Star discrepancy (from ${discrepancy_type} }" >> $1.tex
# 	echo "\end{figure}" >> $1.tex
# 
# 	fi
# 	
# 	discrepancy_type="a single sequence"
# 	discrepancy_file=`ls *_${name}_sequence*discrepancy*Star*.png`
# 
# 	if [ ! -z "${discrepancy_file}" ]
# 	then
# 	
# 	echo "" >> $1.tex
# 	echo "" >> $1.tex
# 	echo "%%Discrepancy" >> $1.tex
# 	echo "\begin{figure}[H]" >> $1.tex
# 	echo "\centering" >> $1.tex
# 	echo "\includegraphics[width=\textwidth]{${discrepancy_file}}" >> $1.tex
# 	echo "\caption{Star discrepancy (from ${discrepancy_type} }" >> $1.tex
# 	echo "\end{figure}" >> $1.tex
# 	
# 	fi
	
	echo "\end{document}" >> $1.tex
	
	pdflatex $1.tex -interaction nonstopmode
	rm *.aux
	rm *.log
}

generateTexFile $name

gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.5 -dPDFSETTINGS=/default -dNOPAUSE -dQUIET -dBATCH -dUseCIEColor -sOutputFile=${name}_compressed.pdf ${name}.pdf
