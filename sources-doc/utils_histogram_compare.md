# Histogram Compare

## Files

    src/bin/utils/histogram_compare_2dd.cpp  

## Description

An executable that returns the maximal distance (Linf distance) and the sum of squared distances (L2 distance) between two 2D histograms.

## Execution

```
Parameters:  

	[HELP]
	--h1 	 Histogram 1
	--h2 	 Histogram 2
```

Note that the comparison between histograms can only be performed if both histograms contain the same number of bins, with the same x coordinates.

To compute the difference between 2 histograms, we can use the following client line command:

     ./bin/utils/histogram_compare_2dd --h1 pcf.dat --h2 pcf2.dat

Or the following C++ code:

``` cpp    
    Histogram1dd histo1;
    Histogram1dd histo2;
    //we assume histograms are filled

    double l2, linf;
    histo1.Linf(histo2, linf);
    histo1.L2(histo2, l2);
``` 


## Results

For the following histogram, representing PCFs: ![](data/histo_cmp/pcfs.png)

```
 ./bin/utils/histogram_compare_2dd --h1 pcf.dat --h2 pcf2.dat
Distance between Histograms:
		Linf:0.0884439
		L2:0.225963
```
