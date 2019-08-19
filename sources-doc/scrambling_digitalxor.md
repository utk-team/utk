Scrambling Digital XOR
----------------------

Files

src/scrambling/ScramblingDigitalXOR.hpp  
src/scrambling/DigitalXOR\_\*di.cpp

Description
===========

Randomize a point set with integer coordinates. Picks D random values wich are each xored to the D^th coordinate of each sample of the input point set.

Execution
=========

Parameters:  

	\[HELP\]
	-i \[string\]	Input file
	-o \[string=output\_pts.dat\]	Output file
	--silent 			Silent mode
	-h 				Displays this help message
			

To generate a 2D point set of 1024 samples with a whitenoise distribution, we can use the following client line command:

 ./src/scrambling/CranleyPatterson\_2dd -i pointset.edat -o scrambledset.edat 

Or one can use the following C++ code:

    
    ScramblingDigitalXOR scrambler;
    Pointset< D, T, P > pts;
    Pointset< D, T, P > pts_scrambled;
    //we assume pts is filled
    scrambler.scramble< D, T, P >(pts, pts_scrambled);
    PointsetWriter< D, T, P > writer;
    writer.open("scrambledset.edat");
    writer.writePointset(pts_scrambled);
    writer.close();
    			

Results
=======

 ./src/scrambling/DigitalXOR\_2di -i sobol\_m3\_1024.edat -o scrambled.edat 

Input  
[sobol\_m3\_1024.edat](data/digitalxor/sobol_1024.edat)  
[![](data/digitalxor/sobol_1024.png)](data/digitalxor/sobol_1024.png)

Output  
[scrambled\_m3.edat](data/digitalxor/scrambled_m3.edat)  
[![](data/digitalxor/scrambled1.png)](data/digitalxor/scrambled1.png) [![](data/digitalxor/scrambled2.png)](data/digitalxor/scrambled2.png) [![](data/digitalxor/scrambled3.png)](data/digitalxor/scrambled3.png)
