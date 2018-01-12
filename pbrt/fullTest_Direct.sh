sampler_list="bnldseq16 bnldseq64 halton stratified sobol"
pts_list="4 16 64 256"
dimensions="light"
cropping="100x100+400+300"

scene="../pbrt-v3-scenes/dragon/scene.pbrt"
name="dragon"

if [[ -z "$scene" ]]
then
	echo "[ERROR] No scene specified"
	echo "Usage " $0 " <scene> <name>"
	exit
fi

if [[ -z "$name" ]]
then
	echo "[ERROR] No name specified"
	echo "Usage " $0 " <scene> <name>"
	exit
fi

imgsize=512
bounces=1
tilesize=262144

rm MSE.txt

./testConfig.sh ${scene} stratified 512 ${imgsize} ${bounces}
mv ${name}.png ${name}_stratified_512spp_GT.png
convert ${name}_stratified_512spp_GT.png -crop ${cropping} +repage thumb_${cropping}_${name}_stratified_512spp_GT.png

for s in ${sampler_list}
do

for n in ${pts_list}
do

./catPointsets_${s}_${dimensions}.sh pts2.dat ${tilesize}
./testConfig.sh ${scene} utkpixel ${n} ${imgsize} ${bounces} pts2.dat ${tilesize} Global

file=${name}.png
filename=${name}_${s}_${n}spp_${tilesize}tilesize_${bounces}bounces_Global_direct.png
mv ${file} ${filename}
convert ${filename} -crop ${cropping} +repage thumb_${cropping}_${filename}

echo "./imgtool diff ${name}_stratified_512spp_GT.png ${filename}" >> MSE.txt
./imgtool diff ${name}_stratified_512spp_GT.png ${filename} >> MSE.txt
echo >> MSE.txt

done

done

mkdir renderings_${name}_direct
mv *.png renderings_${name}_direct
mv MSE.txt renderings_${name}_direct