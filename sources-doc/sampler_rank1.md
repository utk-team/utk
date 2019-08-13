Sampler Rank 1 [\[DK08\]](https://www.uni-ulm.de/fileadmin/website_uni_ulm/iui.inst.100/institut/mitarbeiterbereiche/sabrina/ImgSynthPrePrint.pdf)
--------------------------------------------------------------------------------------------------------------------------------------------------

Files

src/samplers/SamplerRank1.hpp  
src/bin/samplers/Rank1\_2dd.cpp.cpp

Description
===========

The Rank 1 lattices from [\[Keller04\]](https://link.springer.com/chapter/10.1007%2F978-3-642-18743-8_18).  
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
	--fibonacci 			Uses the fibonnacci sequence as indices
			

To generate a 2D point set of 1024 samples with a rank1 distribution, we can use the following client line command:

 ./bin/samplers/Rank1\_2dd -n 1024 -o toto.dat 

Or one can use the following C++ code:

    
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerRank1 s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
    			

Results
=======

 ./bin/samplers/Rank1\_2dd -o rank1\_1024.edat -n 1024 

File  
[rank1\_1024.edat](data/rank1/rank1_1024.edat)

Pointset  
[![](data/rank1/rank1_1024.png)](data/rank1/rank1_1024.png)

Fourier  
[![](data/rank1/rank1_1024_fourier.png)](data/rank1/rank1_1024_fourier.png)

 ./bin/samplers/Rank1\_2dd -o rank1\_4096.edat -n 4096 

File  
[rank1\_4096.edat](data/rank1/rank1_4096.edat)

Pointset  
[![](data/rank1/rank1_4096.png)](data/rank1/rank1_4096.png)

Fourier  
[![](data/rank1/rank1_4096_fourier.png)](data/rank1/rank1_4096_fourier.png)

 ./bin/samplers/Rank1\_2dd --fibonacci -o rank1\_fib\_1024.edat -n 1024 

File  
[rank1\_fib\_1024.edat](data/rank1_fib/rank1_fib_1024.edat)

Pointset  
[![](data/rank1_fib/rank1_fib_1024.png)](data/rank1_fib/rank1_fib_1024.png)

Fourier  
[![](data/rank1_fib/rank1_fib_1024_fourier.png)](data/rank1_fib/rank1_fib_1024_fourier.png)

 ./bin/samplers/Rank1\_2dd --fibonacci -o rank1\_fib\_4096.edat -n 4096 

File  
[rank1\_fib\_4096.edat](data/rank1_fib/rank1_fib_4096.edat)

Pointset  
[![](data/rank1_fib/rank1_fib_4096.png)](data/rank1_fib/rank1_fib_4096.png)

Fourier  
[![](data/rank1_fib/rank1_fib_4096_fourier.png)](data/rank1_fib/rank1_fib_4096_fourier.png)