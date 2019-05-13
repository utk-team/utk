if [ -z "$1" ] || [ -z "$2" ]
then
echo "Usage: ./plotPCF.sh <pointset> <outputfile> [radius]"
exit
fi

if [ -z "$3" ]
then
radius="0.1"
else
radius="$3"
fi

ext="${2##*.}"
if [ $ext = "png" ]
then
echo "set term png size 1200,600" >> gp-tmp.txt;
elif [ $ext = "pdf" ]
then
echo "set term pdf enhanced size 5,2.25" >> gp-tmp.txt;
else
echo "Unknown output file format. Accepted format are .png, .pdf"
rm gp-tmp.txt
exit
fi
echo "set output \"$2\"" >> gp-tmp.txt
echo "set xrange [0:3]" >> gp-tmp.txt
echo "set yrange [0:3]" >> gp-tmp.txt
echo "set xlabel \"Radius\"; set ylabel \"PCF\"" >> gp-tmp.txt
echo "plot \"$1\" w l lw 2 notitle" >> gp-tmp.txt

gnuplot gp-tmp.txt
rm gp-tmp.txt
