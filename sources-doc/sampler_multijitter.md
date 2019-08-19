# Sampler Multi Jittered [[CSW94]](https://dl.acm.org/citation.cfm?id=180895.180927)


## Files

```
src/samplers/SamplerMultiJitter.hpp  
src/bin/samplers/MultiJitter_2dd.cpp
```

## Description


The Multi-Jitter sampler from [[CSW94]](https://www.researchgate.net/publication/262372120_Multi-Jittered_Sampling).  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Execution


Parameters:  

	[HELP]
	-o [string=output_pts.dat]	Output file
	-m [int=1]			Number of poinset realisations
	-n [ullint=1024]		Number of samples to generate
	--silent 			Silent mode
	-h 				Displays this help message
			

To generate a 2D point set of 1024 samples with a multijitter distribution, we can use the following client line command:

     ./bin/samplers/MultiJitter_2dd -n 1024 -o toto.dat 

Or one can use the following C++ code:

    
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerMultiJitter s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
    			

Results
=======

     ./bin/samplers/MultiJitter_2dd -o multijitter_1024.edat -n 1024 

File  
[multijitter_1024.edat](data/multijitter/multijitter_1024.edat)

Pointset  
[![](data/multijitter/multijitter_1024.png)](data/multijitter/multijitter_1024.png)

Fourier  
[![](data/multijitter/multijitter_1024_fourier.png)](data/multijitter/multijitter_1024_fourier.png)

     ./bin/samplers/MultiJitter_2dd -o multijitter_4096.edat -n 4096 

File  
[multijitter_4096.edat](data/multijitter/multijitter_4096.edat)

Pointset  
[![](data/multijitter/multijitter_4096.png)](data/multijitter/multijitter_4096.png)

Fourier  
[![](data/multijitter/multijitter_4096_fourier.png)](data/multijitter/multijitter_4096_fourier.png)
