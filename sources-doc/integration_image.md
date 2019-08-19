# Integration Image

## Files

    src/integration/IntegrationImage.hpp  
    src/bin/integration/IntegrationImage_fromfile_2dd.cpp.cpp

## Description

This test uses an input pointset to approximate the integration of an image using a Monte Carlo estimator. Note that since there are no analytical function for this integration, we define this analytical function as the sum of the values of all pixels of the image. We usually integrate over HDR images. This test exists only in 2D.

## License

BSD, see `IntegrationImage.hpp`

## Execution


```
Parameters:  

	[HELP]
	--image [string]	The image to integrate
	-h [string]		Displays this help message
	-i [string]		The input pointsets
	-o [string]		The output integration results
	-s [uint]		The number of samples to read (if integrating from a sequence)
	--silent 		Silent mode
	--brute 		Output brute values instead of computing the statistics
```


To integrate over a function using an input 2D point set, we can use the following client line command:

     ./bin/integration/IntegrationImage2D_fromfile_2dd --image ../../data/HDR/HDRimage1600x1600_017.exr -i toto.dat

Or one can use the following C++ code:

``` cpp    
    IntegrationImage integration_test;
    IntegrationStatistics stats;
    Pointset< D, double, Point > pts;
    integration_test.setImage("../../data/HDR/HDRimage1600x1600_017.exr");

    //We assume pts is filled
    stats.nbpts = pts.size();

    //Can be done several times if we need to average over
    //several stochastic pointsets
    double analytical = 0;
    double integration = 0;
    integration_test.compute< D, double, Point >(pts, integration, analytical)
    stats.setAnalytical(analytical);
    stats.addValue(integration);

    stats.computeStatistics();
    std::cout << stats << std::endl;
```

## Results

Stochastic sampler

```
./bin/integration/IntegrationImage2D_fromfile_2dd --image ../../data/HDR/HDRimage1600x1600_017.exr -i stratified_256.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
256	0.00526645	5.08625e-08	0.00490972	0.00564206	0.00532679	4.94174e-08	10
```

[![](data/image/integration_1_256.png)](data/image/integration_1_256.png) [![](data/image/integration_2_256.png)](data/image/integration_2_256.png) [![](data/image/integration_3_256.png)](data/image/integration_3_256.png) ...

```
./bin/integration/IntegrationImage2D_fromfile_2dd --image ../../data/HDR/HDRimage1600x1600_017.exr -i stratified_1024.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
1024	0.00533107	7.88106e-09	0.00521723	0.0054546	0.00532679	7.1113e-09	10
```

[![](data/image/integration_1_1024.png)](data/image/integration_1_1024.png) [![](data/image/integration_2_1024.png)](data/image/integration_2_1024.png) [![](data/image/integration_3_1024.png)](data/image/integration_3_1024.png) ...

```
./bin/integration/IntegrationImage2D_fromfile_2dd --image ../../data/HDR/HDRimage1600x1600_017.exr -i stratified_4096.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
4096	0.00532906	7.15467e-10	0.00527126	0.00536509	0.00532679	6.49065e-10	10
```

[![](data/image/integration_1_4096.png)](data/image/integration_1_4096.png) [![](data/image/integration_2_4096.png)](data/image/integration_2_4096.png) [![](data/image/integration_3_4096.png)](data/image/integration_3_4096.png) ...

Deterministic sampler

```
./bin/integration/IntegrationImage2D_fromfile_2dd --image ../../data/HDR/HDRimage1600x1600_017.exr -i sobol_1024.dat
#Nbpts	#Mean		#Var	#Min		#Max		#Analytical	#MSE		#NbPtsets
1024	0.00531459	0	0.00531459	0.00531459	0.00532679	1.48854e-10	1
```

[![](data/image/integration_1024.png)](data/image/integration_1024.png)

```
./bin/integration/IntegrationImage2D_fromfile_2dd --image ../../data/HDR/HDRimage1600x1600_017.exr -i sobol_4096.dat
#Nbpts	#Mean		#Var	#Min		#Max		#Analytical	#MSE		#NbPtsets
4096	0.00531362	0	0.00531362	0.00531362	0.00532679	1.73295e-10	1
```

[![](data/image/integration_4096.png)](data/image/integration_4096.png)
