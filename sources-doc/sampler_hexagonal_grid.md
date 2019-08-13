Sampler Hexagonal Grid
----------------------

Files

src/samplers/SamplerHexagonalGrid.hpp  
src/bin/samplers/HexagonalGrid\_2dd.cpp.cpp

Description
===========

An hexagonal grid.  
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
			

To generate a 2D point set of 1024 samples with a hexagonal\_grid distribution, we can use the following client line command:

 ./bin/samplers/HexagonalGrid\_2dd -n 1024 -o toto.dat 

Or one can use the following C++ code:

    
    PointsetWriter< 2, double, Point<2, double> > writer;
    writer.open("toto.dat");
    Pointset< 2, double, Point<2, double> > pts;
    SamplerHexagonalGrid s;
    unsigned int param_nbsamples = 1024;
    s.generateSamples< 2, double, Point<2, double> >(pts, param_nbsamples);
    writer.writePointset(pts);
    writer.close();
    			

Results
=======

 ./bin/samplers/HexagonalGrid\_2dd -o hexagonal\_grid\_1024.edat -n 1024 

File  
[hexagonal\_grid\_1024.edat](data/hexagonal_grid/hexagonal_grid_1024.edat)

Pointset  
[![](data/hexagonal_grid/hexagonal_grid_1024.png)](data/hexagonal_grid/hexagonal_grid_1024.png)

Fourier  
[![](data/hexagonal_grid/hexagonal_grid_1024_fourier.png)](data/hexagonal_grid/hexagonal_grid_1024_fourier.png)

 ./bin/samplers/HexagonalGrid\_2dd -o hexagonal\_grid\_4096.edat -n 4096 

File  
[hexagonal\_grid\_4096.edat](data/hexagonal_grid/hexagonal_grid_4096.edat)

Pointset  
[![](data/hexagonal_grid/hexagonal_grid_4096.png)](data/hexagonal_grid/hexagonal_grid_4096.png)

Fourier  
[![](data/hexagonal_grid/hexagonal_grid_4096_fourier.png)](data/hexagonal_grid/hexagonal_grid_4096_fourier.png)