UTKPATH="~/Desktop/Recherche/UnicornTK2/src/build"

eval "${UTKPATH}/bin/spectral/stkfourier_2dd -i ${1} -P ${1%%.dat}_fourier.png --Rdat ${1%%.dat}_radial.dat"
eval "${UTKPATH}/../scripts/plotRadialSpectrum.sh ${1%%.dat}_radial.dat ${1%%.dat}_radial.pdf"

