#! /bin/bash

mkdir bu
mv *.html bu

cd posts

for i in *.html
do

cat ../template/header.html >> tmp.html
cat ../template/navbar.html >> tmp.html
cat $i >> tmp.html
cat ../template/footer.html >> tmp.html

mv tmp.html ../$i

done
