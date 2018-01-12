#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]
then
	echo "Usage ./profile.sh <nom executable> \"<liste des arguments>\""
	exit
fi

rm gprofstats.png
rm gmon.out
rm gprof.stats

echo "CPU Profiling " $1 $2 " ..."
cmd="$1 $2"
echo $cmd
eval $cmd
cmd="gprof $1 gmon.out >> gprof.stats"
echo $cmd
eval $cmd
cmd="~/Desktop/Installed/gprof2dot/gprof2dot.py gprof.stats | dot -Tpng -o gprofstats.png"
echo $cmd
eval $cmd
eog gprofstats.png
