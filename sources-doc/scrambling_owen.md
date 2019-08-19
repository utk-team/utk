# Scrambling Owen

## Files

    src/scrambling/ScramblingOwen.hpp  
    src/scrambling/OwenScrambling_*di.cpp

## Description

Randomize a point set with integer coordinates using Owen's scrambling.

## License

BSD, see `ScramblingOwen.hpp`

## Execution

```
Parameters:  

	[HELP]
	-i [string]	Input file
	-o [string=output_pts.dat]	Output file
	--silent 			Silent mode
	-h 				Displays this help message
```


To generate a 2D point set of 1024 samples with a whitenoise distribution, we can use the following client line command:

     ./src/scrambling/CranleyPatterson_2dd -i pointset.edat -o scrambledset.edat

Or one can use the following C++ code:

``` cpp    
    ScramblingOwen scrambler;
    Pointset< D, T, P > pts;
    Pointset< D, T, P > pts_scrambled;
    //we assume pts is filled
    scrambler.scramble< D, T, P >(pts, pts_scrambled);
    PointsetWriter< D, T, P > writer;
    writer.open("scrambledset.edat");
    writer.writePointset(pts_scrambled);
    writer.close();
```    			

## Results

     ./src/scrambling/DigitalXOR_2di -i sobol_m3_1024.edat -o scrambled.edat

Input  
[sobol_m3_1024.edat](data/owenscrambler/sobol_1024.edat)  
[![](data/owenscrambler/sobol_1024.png)](data/owenscrambler/sobol_1024.png)

Output  
[scrambled_m3.edat](data/owenscrambler/scrambled_m3.edat)  
[![](data/owenscrambler/scrambled1.png)](data/owenscrambler/scrambled1.png)

[![](data/owenscrambler/scrambled2.png)](data/owenscrambler/scrambled2.png)

 [![](data/owenscrambler/scrambled3.png)](data/owenscrambler/scrambled3.png)
