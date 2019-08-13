Sampler Niederreiter [\[Nie88\]](https://www.sciencedirect.com/science/article/pii/0022314X8890025X)
----------------------------------------------------------------------------------------------------

Files

src/samplers/SamplerNiederreiter.hpp  
src/bin/samplers/Niederreiter\_2dd.cpp.cpp

Description
===========

The Niederreiter sequence.  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML\_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

Execution
=========

Parameters:  

	\[HELP\]
	-o \[string=output\_pts.dat\]	Output file
	-m \[int=1\]			Number of poinset realisations
	-n \[ullint=1024\]		Number of samples to generate
	--silent 			Silent mode
	-h 				Displays this help message
	-b \[uint=2\]			Basis
			

To generate a 2D point set of 1024 samples with a nieddereiter distribution, we can use the following client line command:

 ./bin/samplers/Niederreiter\_2dd -n 1024 -o toto.dat 

Or one can use the following C++ code:

    
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerNiederreiter s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
    			

Results
=======

 ./bin/samplers/Niederreiter\_2dd -o nieddereiter\_1024.edat -n 1024 

File  
[nieddereiter\_1024.edat](data/nieddereiter/nieddereiter_1024.edat)

Pointset  
[![](data/nieddereiter/nieddereiter_1024.png)](data/nieddereiter/nieddereiter_1024.png)

Fourier  
[![](data/nieddereiter/nieddereiter_1024_fourier.png)](data/nieddereiter/nieddereiter_1024_fourier.png)

 ./bin/samplers/Niederreiter\_2dd -o nieddereiter\_4096.edat -n 4096 

File  
[nieddereiter\_4096.edat](data/nieddereiter/nieddereiter_4096.edat)

Pointset  
[![](data/nieddereiter/nieddereiter_4096.png)](data/nieddereiter/nieddereiter_4096.png)

Fourier  
[![](data/nieddereiter/nieddereiter_4096_fourier.png)](data/nieddereiter/nieddereiter_4096_fourier.png)

 ./bin/samplers/Niederreiter\_2dd -b 3 -o nieddereiter\_b3\_1024.edat -n 1024 

File  
[nieddereiter\_b3\_1024.edat](data/nieddereiter_b3/nieddereiter_b3_1024.edat)

Pointset  
[![](data/nieddereiter_b3/nieddereiter_b3_1024.png)](data/nieddereiter_b3/nieddereiter_b3_1024.png)

Fourier  
[![](data/nieddereiter_b3/nieddereiter_b3_1024_fourier.png)](data/nieddereiter_b3/nieddereiter_b3_1024_fourier.png)

 ./bin/samplers/Niederreiter\_2dd -b 5 -o nieddereiter\_b5\_1024.edat -n 1024 

File  
[nieddereiter\_b5\_1024.edat](data/nieddereiter_b5/nieddereiter_b5_1024.edat)

Pointset  
[![](data/nieddereiter_b5/nieddereiter_b5_1024.png)](data/nieddereiter_b5/nieddereiter_b5_1024.png)

Fourier  
[![](data/nieddereiter_b5/nieddereiter_b5_1024_fourier.png)](data/nieddereiter_b5/nieddereiter_b5_1024_fourier.png)