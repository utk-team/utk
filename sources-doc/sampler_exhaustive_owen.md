# Sampler Exhaustive Owen


## Files

    src/samplers/SamplerExhaustiveOwen.hpp  
    src/bin/samplers/ExaustiveOwen_2dd.cpp.cpp

## Description


Owen's scrambling uses a permutation tree in each dimension. This tree is usually randomly filled. For a set containing n samples, it contains N flags, with N=n-1. This sampler tests all 2^N possible trees (in each dimension) and returns the trees that generates the most Blue Noise like set. As this exhaustive search is very computationally expensive, this sampler can only generate point sets of size n lesser than 32.

## License

BSD, see `SamplerExhaustiveOwen.hpp`

## Execution

```
Parameters:  

	[HELP]
	-o [string=output_pts.dat]	Output file
	-m [int=1]			Number of poinset realisations
	-n [ullint=1024]		Number of samples to generate
	--silent 			Silent mode
	-h 				Displays this help message
  ```


To generate a 2D point set of 1024 samples with a exhaustive_owen distribution, we can use the following client line command:

     ./bin/samplers/ExhautiveOwen_2dd -n 1024 -o toto.dat

Or one can use the following C++ code:

``` cpp    
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerExhaustiveOwen s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
```


Results
=======

     ./bin/samplers/ExhautiveOwen_2dd -o exhaustive_owen_16.edat -n 16

File  
[exhaustive_owen_16.edat](data/exhaustive_owen/exhaustive_owen_16.edat)

Pointset  
[![](data/exhaustive_owen/exhaustive_owen_16.png)](data/exhaustive_owen/exhaustive_owen_16.png)

Fourier  
[![](data/exhaustive_owen/exhaustive_owen_16_fourier.png)](data/exhaustive_owen/exhaustive_owen_16_fourier.png)
