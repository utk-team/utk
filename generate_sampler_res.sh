#echo "The Sobol sequence. Samples are generated from a binary product between the binary representation of their index and a matrix, generated from primitive polynomials. The primitive polynomials are indiced based on OEIS" >> desc.txt; ./generate_sampler_res.sh sobolindiced_56 "./bin/samplers/SobolIndices_2dd -x 5 -y 6" SamplerSobolIndices SobolIndices_*d*.cpp desc.txt; rm desc.txt

rm tmp.html

samplername=$1
myexec=$2
hppfile=$3
cppfile=$4
desc=$5

if [ -e $1 ]
then
	echo "Usage: ./generate_sampler_res.sh <sampler_name> <exec_cmd> <hppfile> <cppfile> <description file>"
	exit
fi

echo "<div id=\"post\">" >> tmp.html
echo "			<h2> Sampler ${samplername} </h2>" >> tmp.html
echo "			<table class="files">" >> tmp.html
echo "			<tr>" >> tmp.html
echo "				<td class="files" valign="middle">Files</td>" >> tmp.html
echo "				<td class="files" valign="middle">" >> tmp.html
echo "					src/samplers/${hppfile}.hpp<br/>" >> tmp.html
echo "					src/bin/samplers/${cppfile}.cpp" >> tmp.html
echo "				</td>" >> tmp.html
echo "			</tr>" >> tmp.html
echo "			</table>" >> tmp.html
echo >> tmp.html			
echo "			<h1>Description</h1>" >> tmp.html
echo "			<p>" >> tmp.html
cat $desc >> tmp.html
echo "			<br/>For a more precise description of this sampler and its performances in terms of aliasing and discrepancy," >> tmp.html
echo "			please refer to the following web bundle <a href=\"https://liris.cnrs.fr/ldbn/HTML_bundle/index.html\">https://liris.cnrs.fr/ldbn/HTML_bundle/index.html</a>." >> tmp.html
echo "		    </p>" >> tmp.html
echo >> tmp.html			    
echo "		    <h1> Execution </h1>" >> tmp.html
echo >> tmp.html
echo "		    <p>" >> tmp.html
echo "				Parameters:<br/>" >> tmp.html
echo "				<pre>" >> tmp.html
echo -n "	" >> tmp.html;
eval "$myexec 1>> tmp.html"
cp tmp.html tmp.html.tmp
sed '$ d' tmp.html.tmp > tmp.html
rm -f tmp.html.tmp
echo "			</pre>" >> tmp.html
echo "			</p>" >> tmp.html
echo >> tmp.html	
echo "			<p>To generate a 2D point set of 1024 samples with a ${samplername} distribution, we can use the following" >> tmp.html 
echo "			client line command:" >> tmp.html
echo "			<pre><cli>" >> tmp.html
echo "${myexec} -n 1024 -o toto.dat" >> tmp.html
echo "			</cli></pre>" >> tmp.html
echo "			</p>" >> tmp.html
echo >> tmp.html	
echo "			<p>" >> tmp.html
echo "			Or one can use the following C++ code:" >> tmp.html
echo "			<pre><code>" >> tmp.html
echo "<b>PointsetWriter< 2, double, Point<2, double> ></b> writer;" >> tmp.html
echo "writer.open(\"toto.dat\");" >> tmp.html
echo "<b>Pointset< 2, double, Point<2, double> ></b> pts;" >> tmp.html
echo "<b>${hppfile}</b> s;" >> tmp.html
echo "<b>unsigned int</b> param_nbsamples = 1024;" >> tmp.html
echo "s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);" >> tmp.html
echo "writer.writePointset(pts);" >> tmp.html
echo "writer.close();" >> tmp.html
echo "			</code></pre>" >> tmp.html
echo "			</p>" >> tmp.html
echo >> tmp.html
echo "			<h1> Results </h1>" >> tmp.html
echo >> tmp.html
mkdir ~/Desktop/Recherche/utk/docs/data/${samplername}
for n in 1024 4096
do

filename=${samplername}_${n}.edat
cmd="${myexec} -o ${filename} -n ${n}"
eval $cmd >> /dev/null

imagename=${samplername}_${n}.png
imgcmd="./bin/utils/draw_fromfile_2dd -i ${filename} -o ${samplername}_${n}.ps"
eval $imgcmd >> /dev/null
convert -density 150 ${samplername}_${n}.ps ${imagename}

fouriername=${samplername}_${n}_fourier.png
eval "${myexec} -o tmp_gen_res_html.bin -n ${n} -m 10"
fourcmd="./bin/spectral/stkfourier_2dd -i tmp_gen_res_html.bin -P fourier_tmp.png"
eval $fourcmd >> /dev/null
rm tmp_gen_res_html.bin
convert -set colorspace RGB fourier_tmp.png ${fouriername}
rm fourier_tmp.png

echo "			<pre><cli>" >> tmp.html
echo $cmd >> tmp.html
echo "			</cli></pre>" >> tmp.html
echo "			<table style=\"width:100%\">" >> tmp.html
echo "				<tr class=\"images\">" >> tmp.html
echo "					<td>File<br/><a href=\"data/${samplername}/${filename}\">${filename}</a></td>" >> tmp.html
echo "					<td>Pointset<br/><a href=\"data/${samplername}/${imagename}\"><img class=\"mylogo\" width=\"150\" src=\"data/${samplername}/${imagename}\"></a></td>" >> tmp.html
echo "					<td>Fourier<br/><a href=\"data/${samplername}/${fouriername}\"><img class=\"mylogo\" width=\"150\" src=\"data/${samplername}/${fouriername}\"></a></td>" >> tmp.html
echo "				</tr>" >> tmp.html
echo "			</table>" >> tmp.html

mv ${imagename} ~/Desktop/Recherche/utk/docs/data/${samplername}
mv ${filename} ~/Desktop/Recherche/utk/docs/data/${samplername}
mv ${fouriername} ~/Desktop/Recherche/utk/docs/data/${samplername}

done;

mv tmp.html ~/Desktop/Recherche/utk/docs/posts/sampler_${samplername}.html
cd ~/Desktop/Recherche/utk/docs/
./generate.sh
cd ~/Desktop/Recherche/utk/src/build
