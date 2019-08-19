# Integration Centered Gaussian

## Files

    src/integration/IntegrationCenteredGaussian.hpp  
    src/bin/integration/IntegrationCenteredGaussian_fromfile_2dd.cpp.cpp

## Description

This test uses an input pointset to approximate the CDF of a Gaussian function using a Monte Carlo estimator. Note that in this test the gaussian is centered at $0.5^D$ (points are scaled to $[0, 1]^D$), for an analytical value of ~1. This test exists only in 2D. Note that due to our inability to find any analytical function to determine the area of a cropped gaussian, we devised independent tests for a centered uncropped gaussian and a 1/4 cropped gaussian (which both have exact analytical values) and a generic gaussian where the analytical value is estimated using a Monte Carlo estimator using 10e6 regular samples.

## License

BSD, see `IntegrationCenteredGaussian.hpp`

## Execution

```
Parameters:  

	[HELP]
	--outputfunc 	Outputs the integrated function in func.dat (plottable with gnuplot)
	--outputimg 	Outputs the integrated function in func.png
	-h [string]		Displays this help message
	-i [string]		The input pointsets
	-o [string]		The output integration results
	-s [uint]		The number of samples to read (if integrating from a sequence)
	--silent 		Silent mode
	--brute 		Output brute values instead of computing the statistics
```


To integrate over a function using an input 2D point set, we can use the following client line command:

     ./bin/integration/IntegrationCenteredGaussian_fromfile_2dd -i toto.dat

Or one can use the following C++ code:

``` cpp  
    IntegrationCenteredGaussian integration_test;
    IntegrationStatistics stats;
    Pointset< D, double, Point > pts;
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
./bin/integration/IntegrationCenteredGaussian_fromfile_2dd -i stratified_256.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
256	1.01654	0.000608383	0.979811	1.06037	1		0.000821097	10
```

[![](data/centered_gaussian/integration_1_256.png)](data/centered_gaussian/integration_1_256.png) [![](data/centered_gaussian/integration_2_256.png)](data/centered_gaussian/integration_2_256.png) [![](data/centered_gaussian/integration_3_256.png)](data/centered_gaussian/integration_3_256.png) ...

```
./bin/integration/IntegrationCenteredGaussian_fromfile_2dd -i stratified_1024.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
1024	0.99999	7.22086e-05	0.984586	1.01211	1		6.49878e-05	10
```

[![](data/centered_gaussian/integration_1_1024.png)](data/centered_gaussian/integration_1_1024.png) [![](data/centered_gaussian/integration_2_1024.png)](data/centered_gaussian/integration_2_1024.png) [![](data/centered_gaussian/integration_3_1024.png)](data/centered_gaussian/integration_3_1024.png) ...

```
./bin/integration/IntegrationCenteredGaussian_fromfile_2dd -i stratified_4096.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
4096	1.00112	3.73298e-06	0.998042	1.00392	1		4.61976e-06	10
```

[![](data/centered_gaussian/integration_1_4096.png)](data/centered_gaussian/integration_1_4096.png) [![](data/centered_gaussian/integration_2_4096.png)](data/centered_gaussian/integration_2_4096.png) [![](data/centered_gaussian/integration_3_4096.png)](data/centered_gaussian/integration_3_4096.png) ...

Deterministic sampler

```
./bin/integration/IntegrationCenteredGaussian_fromfile_2dd -i sobol_1024.dat
#Nbpts	#Mean		#Var	#Min		#Max		#Analytical	#MSE		#NbPtsets
1024	1.00034	0	1.00034	1.00034	1		1.18955e-07	1
```

[![](data/centered_gaussian/integration_1024.png)](data/centered_gaussian/integration_1024.png)

```
./bin/integration/IntegrationCenteredGaussian_fromfile_2dd -i sobol_4096.dat
#Nbpts	#Mean		#Var	#Min		#Max		#Analytical	#MSE		#NbPtsets
4096	0.999995	0	0.999995	0.999995	1		2.59188e-11	1
```

[![](data/centered_gaussian/integration_4096.png)](data/centered_gaussian/integration_4096.png)
