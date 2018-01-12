#!/bin/bash

echo "set term pngcairo dashed size 1500,1500 crop" >> gp-tmp.txt
echo "unset xlabel; unset ylabel" >> gp-tmp.txt
echo "set pm3d interpolate 2,2" >> gp-tmp.txt
echo "set palette defined ( 0 \"#000000\", 2 \"#FFFFFF\", 10 \"#FFFFFF\", 10 \"#FF0000\", 11 \"#FF0000\" )" >> gp-tmp.txt
echo "set cbrange [0:11]" >> gp-tmp.txt
echo "unset colorbox" >> gp-tmp.txt
echo "set output \"$2\"" >> gp-tmp.txt
echo "set pm3d map" >> gp-tmp.txt
#echo "set size square" >> gp-tmp.txt
echo "splot \"$1\" title \"\"" >> gp-tmp.txt

gnuplot gp-tmp.txt

rm gp-tmp.txt
