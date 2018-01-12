#!/bin/bash

generatePointset=${1}
name=${2}
if [ -z "${name}" ]
then
	name="quicktest"
fi
drawPointset="../../../UnicornTK/src/build/utils/drawPointset2D"
fourier="./bin/spectral/stkfourier_2dd"

rm *${name}*

cmd="${generatePointset} -n 256 -o ${name}_256.dat"
echo ${cmd}
eval ${cmd}
cmd="${generatePointset} -n 1024 -o ${name}_1024.dat"
echo ${cmd}
eval ${cmd}
cmd="${generatePointset} -n 4096 -o ${name}_4096.dat"
echo ${cmd}
eval ${cmd}

cmd="${drawPointset} -i ${name}_256.dat -o ${name}_256_drawpointset.ps --numbered"
echo ${cmd}
eval ${cmd}
cmd="${drawPointset} -i ${name}_1024.dat -o ${name}_1024_drawpointset.ps --radius 0.001"
echo ${cmd}
eval ${cmd}
cmd="${drawPointset} -i ${name}_4096.dat -o ${name}_4096_drawpointset.ps --radius 0.001"
echo ${cmd}
eval ${cmd}

cmd="${fourier} -i ${name}_256.dat -P ${name}_256_powerspectrum.png -R ${name}_256_radial.png"
echo ${cmd}
eval ${cmd}
cmd="${fourier} -i ${name}_1024.dat -P ${name}_1024_powerspectrum.png -R ${name}_1024_radial.png"
echo ${cmd}
eval ${cmd}
cmd="${fourier} -i ${name}_4096.dat -P ${name}_4096_powerspectrum.png -R ${name}_4096_radial.png"
echo ${cmd}
eval ${cmd}
