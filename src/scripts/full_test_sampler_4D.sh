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
zoneplate="../../../sampling_old/BlueLDS/src/build/stk/stk-zoneplate"

#rm *${name}*

echo "TESTING ....";

GENERATE=1
DRAW=1
FOURIER=
STOCHASTIC=
ZONEPLATE=
DISCREPANCY_POINTSET=1
DISCREPANCY_SEQUENCE=

L2_discrepancy_limit=16

listpts="4 16 64 256 1024 4096 8192" # 16635" # 65535"


echo "Generate " $GENERATE

if [ -e $GENERATE ]
then

echo "Generating pointsets ...."

for n in $listpts
do
cmd="${generatePointset} -n ${n} -o ${name}_${n}.dat"
echo ${cmd}
eval ${cmd}
done

if [ ! -e $STOCHASTIC ]
then
cmd="${generatePointset} -n 4096 -o ${name}_4096_10.edat -m 10"
echo ${cmd}
eval ${cmd}

fi

fi


if [ -e $DRAW ]
then

echo "Drawing ..."
for n in $listpts
do

./bin/utils/full2DProjections_fromfile_4dd -i ${name}_${n}.dat -o ${name}_%d%d_2d_${n}.dat
for i in ${name}_01_2d_${n}.dat
do
../../../UnicornTK/src/build/utils/drawPointset2D -i ${i} -o ${name}_01_2d_${n}_drawpointset.ps --radius 0.002
convert -density 100 ${name}_01_2d_${n}_drawpointset.ps ${name}_01_2d_${n}_drawpointset.png
done

for i in ${name}_23_2d_${n}.dat
do
../../../UnicornTK/src/build/utils/drawPointset2D -i ${i} -o ${name}_23_2d_${n}_drawpointset.ps --radius 0.002
convert -density 100 ${name}_23_2d_${n}_drawpointset.ps ${name}_23_2d_${n}_drawpointset.png
done

done

fi


if [ -e $FOURIER ]
then

echo "Generating Fourier ...."

for n in $listpts
do

./bin/utils/full2DProjections_fromfile_4dd -i ${name}_${n}.dat -o ${name}_%d%d_2d_${n}.dat
for i in ${name}_01_2d_${n}.dat
do
./bin/spectral/stkfourier_2dd -i ${i} -P ${i%%.dat}_powerspectrum_D2.png --Rdat ${i%%.dat}_radial_D2.dat --domain 2
../scripts/plotRadialSpectrum.sh ${i%%.dat}_radial_D2.dat ${i%%.dat}_radial_D2.png

./bin/spectral/stkfourier_2dd -i ${i} -P ${i%%.dat}_powerspectrum_D4.png --Rdat ${i%%.dat}_radial_D4.dat --domain 4
../scripts/plotRadialSpectrum.sh ${i%%.dat}_radial_D4.dat ${i%%.dat}_radial_D4.png
done

for i in ${name}_23_2d_${n}.dat
do
./bin/spectral/stkfourier_2dd -i ${i} -P ${i%%.dat}_powerspectrum_D2.png --Rdat ${i%%.dat}_radial_D2.dat --domain 2
../scripts/plotRadialSpectrum.sh ${i%%.dat}_radial_D2.dat ${i%%.dat}_radial_D2.png

./bin/spectral/stkfourier_2dd -i ${i} -P ${i%%.dat}_powerspectrum_D4.png --Rdat ${i%%.dat}_radial_D4.dat --domain 4
../scripts/plotRadialSpectrum.sh ${i%%.dat}_radial_D4.dat ${i%%.dat}_radial_D4.png
done

done

fi

if [ ! -e $ZONEPLATE ]
then
echo "none yet"
fi


if [ -e $DISCREPANCY_POINTSET ]
then

echo "Generating Discrepancy ...."

rm *${name}_pointset*discrepancy*L2*.png
rm *${name}_pointset*discrepancy*L2*.txt
../scripts/generate_discrepancy_graph_onfly.sh --min 4 --max $L2_discrepancy_limit --log --gnuplot --step 0.25 -d "./bin/discrepancy/GeneralizedL2Discrepancy_fromfile_4dd" -s "${1}" --sname "${name}_pointset"

fi

if [ ! -e $DISCREPANCY_SEQUENCE ]
then

echo "Generating Discrepancy ...."

rm *${name}_sequence*discrepancy*L2*.png
rm *${name}_sequence*discrepancy*L2*.txt
../scripts/generate_discrepancy_graph_fromsequence.sh --min 4 --max $L2_discrepancy_limit --log --gnuplot --step 0.25 -d "./bin/discrepancy/GeneralizedL2Discrepancy_fromfile_4dd" -s "${1}" --sname "${name}_sequence"

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
	echo "16 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_16_drawpointset.png} &" >> $1.tex
	echo "64 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_64_drawpointset.png} \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_256_drawpointset.png} &" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_1024_drawpointset.png} \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_4096_drawpointset.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_8192_drawpointset.png} \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	
	
	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Pointsets" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex

	echo "\#pts & Spectrum & \#pts & Spectrum \\\\" >> $1.tex
	echo "16 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_16_drawpointset.png} &" >> $1.tex
	echo "64 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_64_drawpointset.png} \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_256_drawpointset.png} &" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_1024_drawpointset.png} \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_4096_drawpointset.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_8192_drawpointset.png} \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	discrepancy_type="pointsets"
	
	for i in `ls ${name}_pointset*discrepancy*L2*.png`
	do
		echo "" >> $1.tex
		echo "" >> $1.tex
		echo "%%Discrepancy" >> $1.tex
		echo "\begin{figure}[H]" >> $1.tex
		echo "\centering" >> $1.tex
		echo "\includegraphics[width=\textwidth]{$i}" >> $1.tex
		echo "\caption{L2 discrepancy (from ${discrepancy_type} }" >> $1.tex
		echo "\end{figure}" >> $1.tex
		
		break;
	done
	
	discrepancy_type="a single sequence"
	
	for i in `ls ${name}_sequence*discrepancy*L2*.png`
	do
		echo "" >> $1.tex
		echo "" >> $1.tex
		echo "%%Discrepancy" >> $1.tex
		echo "\begin{figure}[H]" >> $1.tex
		echo "\centering" >> $1.tex
		echo "\includegraphics[width=\textwidth]{$i}" >> $1.tex
		echo "\caption{L2 discrepancy (from ${discrepancy_type}) }" >> $1.tex
		echo "\end{figure}" >> $1.tex
		
		break;
	done
	
	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Fourier" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex

	fourier_domain=2
	echo "\#pts & Spectrum & \#pts & Spectrum \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_256_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_8192_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_1024_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	#echo "16635 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_16635_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo " &  \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_4096_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	#echo "65536 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_65536_powerspectrum_D${fourier_domain}_small.png} \\\\" >> $1.tex
	echo " &  \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex

	echo "\caption{01 Projection, Fourier power spectra with frequency domain [-${fourier_domain}, ${fourier_domain}]}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	
	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Fourier" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex

	fourier_domain=4
	echo "\#pts & Spectrum & \#pts & Spectrum \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_256_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_8192_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_1024_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	#echo "16635 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_16635_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo " &  \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_4096_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	#echo "65536 & \includegraphics[width=0.33\textwidth]{${name}_01_2d_65536_powerspectrum_D${fourier_domain}_small.png} \\\\" >> $1.tex
	echo " &  \\\\" >> $1.tex
	
	echo "\end{tabular}" >> $1.tex

	echo "\caption{01 Projection, Fourier power spectra with frequency domain [-${fourier_domain}, ${fourier_domain}]}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Fourier" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex

	fourier_domain=2
	echo "\#pts & Spectrum & \#pts & Spectrum \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_256_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_8192_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_1024_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	#echo "16635 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_16635_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo " &  \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_4096_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	#echo "65536 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_65536_powerspectrum_D${fourier_domain}_small.png} \\\\" >> $1.tex
	echo " &  \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex

	echo "\caption{23 Projection, Fourier power spectra with frequency domain [-${fourier_domain}, ${fourier_domain}]}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	
	echo "" >> $1.tex
	echo "" >> $1.tex
	echo "%%Fourier" >> $1.tex
	echo "\begin{figure}[H]" >> $1.tex
	echo "\centering" >> $1.tex
	
	echo "\begin{tabular}{ r c r c }" >> $1.tex

	fourier_domain=4
	echo "\#pts & Spectrum & \#pts & Spectrum \\\\" >> $1.tex
	echo "256 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_256_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	echo "8192 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_8192_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo "1024 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_1024_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	#echo "16635 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_16635_powerspectrum_D${fourier_domain}.png} \\\\" >> $1.tex
	echo " &  \\\\" >> $1.tex
	echo "4096 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_4096_powerspectrum_D${fourier_domain}.png} &" >> $1.tex
	#echo "65536 & \includegraphics[width=0.33\textwidth]{${name}_23_2d_65536_powerspectrum_D${fourier_domain}_small.png} \\\\" >> $1.tex
	echo " &  \\\\" >> $1.tex

	echo "\end{tabular}" >> $1.tex

	echo "\caption{23 Projection, Fourier power spectra with frequency domain [-${fourier_domain}, ${fourier_domain}]}" >> $1.tex
	echo "\end{figure}" >> $1.tex
	
	
	echo "\end{document}" >> $1.tex
	
	pdflatex $1.tex -interaction nonstopmode
	rm *.aux
	rm *.log
}

generateTexFile $name

gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.5 -dPDFSETTINGS=/default -dNOPAUSE -dQUIET -dBATCH -dUseCIEColor -sOutputFile=${name}_compressed.pdf ${name}.pdf
