Sampler whitenoise
------------------

Files

src/samplers/SamplerWhitenoise.hpp  
src/bin/samplers/Whitenoise_*.cpp.cpp

Description
===========

A pure random sampler  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

Execution
=========

Parameters:  

	[HELP]
	-o [string=output_pts.dat]	Output file
	-m [int=1]			Number of poinset realisations
	-n [ullint=1024]		Number of samples to generate
	--silent 			Silent mode
	-h 				Displays this help message
			

To generate a 2D point set of 1024 samples with a whitenoise distribution, we can use the following client line command:

     ./bin/samplers/Whitenoise_2dd -n 1024 -o toto.dat 

Or one can use the following C++ code:

    
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerWhitenoise s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
    			

Results
=======

     ./bin/samplers/Whitenoise_2dd -o whitenoise_1024.edat -n 1024 

File  
[whitenoise_1024.edat](data/whitenoise/whitenoise_1024.edat)

Pointset  
[![](data/whitenoise/whitenoise_1024.png)](data/whitenoise/whitenoise_1024.png)

Fourier  
[![](data/whitenoise/whitenoise_1024_fourier.png)](data/whitenoise/whitenoise_1024_fourier.png)

     ./bin/samplers/Whitenoise_2dd -o whitenoise_4096.edat -n 4096 

File  
[whitenoise_4096.edat](data/whitenoise/whitenoise_4096.edat)

Pointset  
[![](data/whitenoise/whitenoise_4096.png)](data/whitenoise/whitenoise_4096.png)

Fourier  
[![](data/whitenoise/whitenoise_4096_fourier.png)](data/whitenoise/whitenoise_4096_fourier.png)
