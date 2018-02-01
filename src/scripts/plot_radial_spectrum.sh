#!/bin/bash

echo "set xlabel \"Frequency\"" >> gp-tmp.txt
echo "set ylabel \"Radial Power Spectrum\"" >> gp-tmp.txt
echo "set border 3; set xtics nomirror; set ytics nomirror" >> gp-tmp.txt #Remove top and right borders
echo "set yrange [0:3]" >> gp-tmp.txt
echo "set style fill transparent solid 0.5 noborder" >> gp-tmp.txt

# echo "set term pngcairo dashed size 1024,384" >> gp-tmp.txt
# echo "set output \"$2\"" >> gp-tmp.txt
# echo "plot \"$1\" u 1:(\$2 < 3 ? \$2 : 3) w filledcurves y1=0 lc rgb \"#A0A0FF\" notitle" >> gp-tmp.txt
# echo "set output \"$2\"" >> gp-tmp.txt
# echo "replot \"$1\" u 1:2 w l lt 1 lc rgb \"#0505A0\" notitle" >> gp-tmp.txt
# echo "set output \"$2\"" >> gp-tmp.txt
# echo "replot 1 w l lt 2 lc rgb \"#505050\" notitle" >> gp-tmp.txt
# echo "1 2" >> gp-tmp2.txt
# echo "set output \"$2\"" >> gp-tmp.txt
# echo "replot \"gp-tmp2.txt\" with impulses lt 2 lc rgb \"#505050\" notitle" >> gp-tmp.txt

echo "1 2" >> gp-tmp2.txt
echo "set xlabel font \",15\"" >> gp-tmp.txt
echo "set ylabel font \",15\"" >> gp-tmp.txt
echo "set term pdf enhanced dashed rounded size 5,2.25" >> gp-tmp.txt
echo "set output \"${2}\"" >> gp-tmp.txt
echo "plot \"$1\" u 1:(\$2 < 3 ? \$2 : 3) w filledcurves y1=0 lc rgb \"#A0A0FF\" notitle, \
\"$1\" u 1:2 w l lt 1 lc rgb \"#0505A0\" notitle, \
1 w l lt 2 lc rgb \"#505050\" notitle, \
\"gp-tmp2.txt\" with impulses lt 2 lc rgb \"#505050\" notitle" >> gp-tmp.txt


gnuplot gp-tmp.txt

rm gp-tmp.txt
rm gp-tmp2.txt