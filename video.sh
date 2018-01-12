
make
rm video_S_Q.mp4
rm video_Q_P.mp4



rm video_p1/*

mkdir video_p1
./bin/utils/makeVideo Slvl.dat Qlvl.dat color.dat video_p1; 

n=`ls video_p1/* | wc -l`; 

for i in video_p1/*.ps; 
do 
  echo $i/$n; 
  convert -density 50 $i ${i%%ps}png; 
done;

convert -delay 10 -loop 10 video_p1/frame*.png video_S_Q.gif

ffmpeg -r 30 -i video_p1/frame%6d.png video_S_Q.mp4




rm video_p2/*

mkdir video_p2
./bin/utils/makeVideo Qlvl.dat Plvl.dat color.dat video_p2; 

n=`ls video_p2/* | wc -l`; 

for i in video_p2/*.ps; 
do 
  echo $i/$n; 
  convert -density 50 $i ${i%%ps}png; 
done;

convert -delay 10 -loop 10 video_p2/frame*.png video_Q_P.gif

ffmpeg -r 30 -i video_p2/frame%6d.png video_Q_P.mp4