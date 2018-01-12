K=$3
lvlmax=$4
sampler=$5

strid="sobol${1}_${2}_k${K}_lvlmax${lvlmax}_sampler_${5}"

test_sampler()
{

for i in `seq 1 ${lvlmax}`
do
	eval "./bin/samplers/${sampler} --level $i -K ${K} -o pts${i}_${strid}.dat --lut-in filled2_${strid}.dat --lut-out filled2_${strid}.dat --sobolD0 ${1} --sobolD1 ${2};"
	./bin/spectral/stkfourier_2dd -i pts${i}_${strid}.dat -P fourier${i}_${strid}.png --Rdat radial${i}_${strid}.dat --domain 1.5;
	../scripts/plotRadialSpectrum.sh radial${i}_${strid}.dat radial${i}_${strid}.pdf;
done

./bin/statistics/computePCF_2dd -i pts1_${strid}.dat -o pcf1_${strid}.dat
./bin/statistics/computePCF_2dd -i pts2_${strid}.dat -o pcf2_${strid}.dat
if [ "$K" == "4" ]
then
./bin/statistics/computePCF_2dd -i pts3_${strid}.dat -o pcf3_${strid}.dat
fi

for i in `ls pcf*_${strid}.dat`
do
touch gp_tmp.txt;
echo "set term pdf size 8,5; set output \"${i%%dat}pdf\"" >> gp_tmp.txt;
echo "plot \"${i}\" w l lw 2 notitle" >> gp_tmp.txt;
gnuplot gp_tmp.txt;
rm gp_tmp.txt;
done

touch gp_tmp.txt;
echo "set term pdf size 8,8; set output \"pts_${strid}.pdf\"" >> gp_tmp.txt;
echo -n "plot \"pts1_${strid}.dat\" w p pt 7 ps 1 notitle" >> gp_tmp.txt;
echo -n ",\"pts2_${strid}.dat\" w p pt 7 lc rgb \"#0000FF\" ps 0.5 notitle" >> gp_tmp.txt;
echo ",\"pts3_${strid}.dat\" w p pt 7 lc rgb \"#00FF00\" ps 0.25 notitle" >> gp_tmp.txt;
gnuplot gp_tmp.txt;
rm gp_tmp.txt;

touch gp_tmp.txt;
echo "set xrange[0:0.5]; set yrange[0:0.5]; set term pdf size 8,8; set output \"pts_${strid}_zoom.pdf\"" >> gp_tmp.txt;
echo -n "plot \"pts1_${strid}.dat\" w p pt 7 ps 1 notitle" >> gp_tmp.txt;
echo -n ",\"pts2_${strid}.dat\" w p pt 7 lc rgb \"#0000FF\" ps 0.5 notitle" >> gp_tmp.txt;
echo ",\"pts3_${strid}.dat\" w p pt 7 lc rgb \"#00FF00\" ps 0.25 notitle" >> gp_tmp.txt;
gnuplot gp_tmp.txt;
rm gp_tmp.txt;

touch gp_tmp.txt;
echo "set xrange[0:0.25]; set yrange[0:0.25]; set term pdf size 8,8; set output \"pts_${strid}_zzoom.pdf\"" >> gp_tmp.txt;
echo -n "plot \"pts1_${strid}.dat\" w p pt 7 ps 1 notitle" >> gp_tmp.txt;
echo -n ",\"pts2_${strid}.dat\" w p pt 7 lc rgb \"#0000FF\" ps 0.5 notitle" >> gp_tmp.txt;
echo ",\"pts3_${strid}.dat\" w p pt 7 lc rgb \"#00FF00\" ps 0.25 notitle" >> gp_tmp.txt;
gnuplot gp_tmp.txt;
rm gp_tmp.txt;

}

#make
test_sampler $1 $2
mkdir test_${strid}
mv filled2_${strid}.dat test_${strid}/fastoptim_lut_${strid}.dat
mv *_${strid}*.dat test_${strid}/
mv *_${strid}*.pdf test_${strid}/
mv *_${strid}*.png test_${strid}/
