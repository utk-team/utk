Sampler N-Rooks [\[Shi91\]](http://www.cs.utah.edu/~shirley/papers/euro91.pdf)
------------------------------------------------------------------------------

Files

src/samplers/SamplerNRooks.hpp  
src/bin/samplers/NRooks\_\*dd.cpp.cpp

Description
===========

The N-Rooks sampler.  
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
			

To generate a 2D point set of 1024 samples with a nrooks distribution, we can use the following client line command:

 ./bin/samplers/NRooks\_2dd -n 1024 -o toto.dat 

Or one can use the following C++ code:

    
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerNRooks s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
    			

Results
=======

 ./bin/samplers/NRooks\_2dd -o nrooks\_1024.edat -n 1024 

File  
[nrooks\_1024.edat](data/nrooks/nrooks_1024.edat)

Pointset  
[![](data/nrooks/nrooks_1024.png)](data/nrooks/nrooks_1024.png)

Fourier  
[![](data/nrooks/nrooks_1024_fourier.png)](data/nrooks/nrooks_1024_fourier.png)

 ./bin/samplers/NRooks\_2dd -o nrooks\_4096.edat -n 4096 

File  
[nrooks\_4096.edat](data/nrooks/nrooks_4096.edat)

Pointset  
[![](data/nrooks/nrooks_4096.png)](data/nrooks/nrooks_4096.png)

Fourier  
[![](data/nrooks/nrooks_4096_fourier.png)](data/nrooks/nrooks_4096_fourier.png)