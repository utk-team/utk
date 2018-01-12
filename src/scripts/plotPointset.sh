if [ -z "$1" ] || [ -z "$2" ]
then
echo "Usage: ./plotPointset.sh <pointset> <outputfile> [radius]"
exit
fi

if [ -z "$3" ]
then
radius="0.1"
else
radius="$3"
fi

echo "set lmargin 0; set rmargin 0; set tmargin 0; set bmargin 0; unset border; unset xtics; unset ytics;" >> gp-tmp.txt;
ext="${2##*.}"
if [ $ext = "png" ]
then
echo "set term png size 2048,2048" >> gp-tmp.txt;
elif [ $ext = "pdf" ]
then
echo "set term pdf enhanced size 5,5" >> gp-tmp.txt;
else
echo "Unknown output file format. Accepted format are .png, .pdf"
rm gp-tmp.txt
exit
fi
echo "set output \"$2\"" >> gp-tmp.txt
echo "plot \"$1\" w p pt 7 ps ${radius} lc rgb \"black\" notitle" >> gp-tmp.txt

gnuplot gp-tmp.txt
rm gp-tmp.txt
