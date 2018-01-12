image=$1

#convert ${image} -resize 1600x1600 ${image}
./bin/utils/fourier_image -i ${image} -o ${image%%.exr}_fourier.png -r ${image%%.exr}_radial.dat
#./bin/utils/radial_image -i ${image%%.exr}_fourier.png -o ${image%%.exr}_radial.dat

#echo "set term pdf; set output \"${image%%.exr}_radial.pdf\"; plot \"${image%%.exr}_radial.dat\" w l;" >> tmp.txt;
echo "set logscale xy; set term pdf; set output \"${image%%.exr}_radial.pdf\"; f(x)=b*(x**a); b= 10000; a=-0.5; fit [0:4] f(x) \"${image%%.exr}_radial.dat\" via a,b; set label 1 sprintf(\"O(p^(%f))\", a) at 0.03,f(0.3)*0.5; plot f(x) w l t \"Fitted O(p^x) curve\", \"${image%%.exr}_radial.dat\" w l t \"Spectrum HDR Image\";" >> tmptutu.txt;
gnuplot tmptutu.txt;
rm tmptutu.txt;
