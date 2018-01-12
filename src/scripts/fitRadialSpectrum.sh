#!/bin/bash

FITINF=-4
FITSUP=-1

if [ ! -z $3 ]
then
FITINF=$3
fi

if [ ! -z $4 ]
then
FITSUP=$4
fi

echo "Fitting between [" $FITINF "," $FITSUP "]"

echo "set xlabel \"Log Frequency\"" >> gp-tmp.txt
echo "set ylabel \"Log Radial Power Spectrum\"" >> gp-tmp.txt
echo "set border 3; set xtics nomirror; set ytics nomirror" >> gp-tmp.txt #Remove top and right borders

echo "f(x) = a*x+b" >> gp-tmp.txt
echo "fit [$FITINF:$FITSUP] f(x) \"${1}\" u 1:2 via a,b" >> gp-tmp.txt
echo "set label 1 sprintf(\"%.2f\", a) at -3,f(-3)-(f(-3)*0.1)" >> gp-tmp.txt
echo "show label" >> gp-tmp.txt

echo "set term pdf enhanced dashed size 6,3.5" >> gp-tmp.txt
echo "set output \"${2}\"" >> gp-tmp.txt
echo "set xrange[-4:1]" >> gp-tmp.txt
echo "plot \"${1}\" u 1:2 w l lw 4 lc rgb \"#3030F0\" title \"Radial Spectrum\", ( (x<-1)? f(x) : 1/0 ) lw 8 lt 7 lc rgb \"#FF0000\" notitle" >> gp-tmp.txt

gnuplot gp-tmp.txt

rm gp-tmp.txt