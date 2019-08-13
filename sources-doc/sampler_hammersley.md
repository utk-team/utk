# Sampler Hammersley [[Ham60]](http://onlinelibrary.wiley.com/doi/10.1111/j.1749-6632.1960.tb42846.x/abstract)


## Files

```
src/samplers/SamplerHammersley.hpp  
src/bin/samplers/Hammersley_2d*.cpp.cpp
```


## Description


The Hammersley sampler. This sampler is only 2D, with the first coordinate obtained by dividing the index of the sample by the total number of samples, and the second coordinate obtained by reversing the binary representation of the index.  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## License

BSD, see `SamplerHammersley.hpp`.

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

To generate a 2D point set of 1024 samples with a hammersley distribution, we can use the following client line command:

    ./bin/samplers/Hammersley_2dd -n 1024 -o toto.dat 

Or one can use the following C++ code:

```cpp   
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerHammersley s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
```   			

## Results


     ./bin/samplers/Hammersley_2dd -o hammersley_1024.edat -n 1024 

File  
[hammersley_1024.edat](data/hammersley/hammersley_1024.edat)

Pointset  
[![](data/hammersley/hammersley_1024.png)](data/hammersley/hammersley_1024.png)

Fourier  
[![](data/hammersley/hammersley_1024_fourier.png)](data/hammersley/hammersley_1024_fourier.png)

     ./bin/samplers/Hammersley_2dd -o hammersley_4096.edat -n 4096 

File  
[hammersley_4096.edat](data/hammersley/hammersley_4096.edat)

Pointset  
[![](data/hammersley/hammersley_4096.png)](data/hammersley/hammersley_4096.png)

Fourier  
[![](data/hammersley/hammersley_4096_fourier.png)](data/hammersley/hammersley_4096_fourier.png)