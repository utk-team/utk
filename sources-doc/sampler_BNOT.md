# Sampler BNOT


## Files

    externals/bnot/*

## Description


This sampler is a wrapper around the BNOT sampler from [de Goes et al 2012]. Due to its code complexity, this sampler was not ported within the UTK framework. It is therefore provided as an external party (in the externals folder). It was however slightly modified to allow using this sampler with similar parameters as other samples from UTK (very useful for scripting purposes). To compile this sampler, please do the following:

```
cd externals/bnot
mkdir build
cd build
cmake ..
make
```

For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## License

BSD for the UTK wrapper, GPL for bnot (see [http://www.geometry.caltech.edu/BlueNoise/](http://www.geometry.caltech.edu/BlueNoise/)).

## Execution


Parameters:  

	[HELP]
	-o [string]			Output file
	-m [int=1]			Number of poinset realisations
	-n [int=1024]		Number of samples to generate
	-h 				Displays this help message


To generate a 2D point set of 1024 samples using BNOT, we can use the following client line command (from the buold folder):

     ../externals/bnot/build/pbnot -n 1024 -o toto.dat

Results
=======

      ../externals/bnot/build/pbnot -o BNOT_1024.edat -n 1024

File  
[BNOT_1024.edat](data/BNOT/BNOT_1024.edat)

Pointset  
[![](data/BNOT/BNOT_1024.png)](data/BNOT/BNOT_1024.png)

Fourier  
[![](data/BNOT/BNOT_1024_fourier.png)](data/BNOT/BNOT_1024_fourier.png)

     ../externals/bnot/build/pbnot -o BNOT_4096.edat -n 4096

File  
[BNOT_4096.edat](data/BNOT/BNOT_4096.edat)

Pointset  
[![](data/BNOT/BNOT_4096.png)](data/BNOT/BNOT_4096.png)

Fourier  
[![](data/BNOT/BNOT_4096_fourier.png)](data/BNOT/BNOT_4096_fourier.png)
