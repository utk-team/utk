# Sampler Sobol [[JK03]](http://web.maths.unsw.edu.au/~fkuo/sobol/)


##  Files

```
src/samplers/SamplerSobol.hpp  
src/bin/samplers/Sobol_*dd.cpp.cpp
```

## Description


The Sobol sequence. Samples are generated from a binary product between the binary representation of their index and a matrix, generated from primitive polynomials. The code from this sampler is from [[JK03]](http://web.maths.unsw.edu.au/~fkuo/sobol/).  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).


## License

BSD-like, see [https://web.maths.unsw.edu.au/~fkuo/sobol/](https://web.maths.unsw.edu.au/~fkuo/sobol/).

## Execution

```
Parameters:  

	[HELP]
	-o [string=output_pts.dat]	Output file
	-m [int=1]			Number of poinset realisations
	-n [ullint=1024]		Number of samples to generate
	--silent 			Silent mode
	-h 				Displays this help message
	-d ‹string›			File containing the direction numbers
```			

To generate a 2D point set of 1024 samples with a sobolkuo distribution, we can use the following client line command:

         ./bin/samplers/Sobol_2dd -d ../../data/sobol/new-joe-kuo-6.21201 -n 1024 -o toto.dat 

Or one can use the following C++ code:

```cpp    
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerSobol s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
```    			

##Results


         ./bin/samplers/Sobol_2dd -d ../../data/sobol/new-joe-kuo-6.21201 -o sobolkuo_1024.edat -n 1024 

File  
[sobolkuo_1024.edat](data/sobolkuo/sobolkuo_1024.edat)

Pointset  
[![](data/sobolkuo/sobolkuo_1024.png)](data/sobolkuo/sobolkuo_1024.png)

Fourier  
[![](data/sobolkuo/sobolkuo_1024_fourier.png)](data/sobolkuo/sobolkuo_1024_fourier.png)

         ./bin/samplers/Sobol_2dd -d ../../data/sobol/new-joe-kuo-6.21201 -o sobolkuo_4096.edat -n 4096 

File  
[sobolkuo_4096.edat](data/sobolkuo/sobolkuo_4096.edat)

Pointset  
[![](data/sobolkuo/sobolkuo_4096.png)](data/sobolkuo/sobolkuo_4096.png)

Fourier  
[![](data/sobolkuo/sobolkuo_4096_fourier.png)](data/sobolkuo/sobolkuo_4096_fourier.png)
