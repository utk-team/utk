# Sampler Forced Random [[CTS*17]](https://link.springer.com/article/10.1007/s00371-017-1392-7)


## Files

    src/samplers/SamplerForcedRandom.hpp  
    src/bin/samplers/ForcedRandom_2dd.cpp.cpp

## Description


A sampler based on dithering matrices from [[CTS*17]](https://link.springer.com/article/10.1007/s00371-017-1392-7).

## License

Open *Unlicense*, see source code:

```
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
```

## Execution


Parameters:  

	[HELP]
	-o [string=output_pts.dat]	Output file
	-m [int=1]			Number of poinset realisations
	-n [ullint=1024]		Number of samples to generate
	--silent 			Silent mode
	-h 				Displays this help message
	--matrix [int=512] 		the matrix size [512 | 1024 | 2048]
	--sparsity [int=8] 		the matrix sparsity


To generate a 2D point set of 1024 samples with a forced_random distribution, we can use the following client line command:

     ./bin/samplers/ForcedRandom_2dd -n 1024 -o toto.dat

Or one can use the following C++ code:


    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerForcedRandom s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();


Results
=======

     ./bin/samplers/ForcedRandom_2dd -o forced_random_1024.edat -n 1024

File  
[forced_random_1024.edat](data/forced_random/forced_random_1024.edat)

Pointset  
[![](data/forced_random/forced_random_1024.png)](data/forced_random/forced_random_1024.png)

Fourier  
[![](data/forced_random/forced_random_1024_fourier.png)](data/forced_random/forced_random_1024_fourier.png)

     ./bin/samplers/ForcedRandom_2dd -o forced_random_4096.edat -n 4096

File  
[forced_random_4096.edat](data/forced_random/forced_random_4096.edat)

Pointset  
[![](data/forced_random/forced_random_4096.png)](data/forced_random/forced_random_4096.png)

Fourier  
[![](data/forced_random/forced_random_4096_fourier.png)](data/forced_random/forced_random_4096_fourier.png)
