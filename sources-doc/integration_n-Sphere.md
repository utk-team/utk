Integration n-Sphere
--------------------

Files

src/integration/IntegrationNSphere.hpp  
src/bin/integration/IntegrationNSphere\_fromfile\_\*d\*.cpp.cpp

Description
===========

This test uses an input pointset to approximate the area of a n dimensional sphere using a Monte Carlo estimator. Note that the sphere is centered at 0^D (points are scaled to \[-0.5, 0.5\]^D), has a value of 1 inside, and 0 outside. This test is shown here in 2D but it exists also in 3D and 4D.

Execution
=========

Parameters:  

	\[HELP\]
	--radius \[double=0.25\]	Sets the radius of the n\_sphere. Must be in \]0, 0.5\[
	--outputfunc 	Outputs the integrated function in func.dat (plottable with gnuplot)
	--outputimg 	Outputs the integrated function in func.png
	-h \[string\]		Displays this help message
	-i \[string\]		The input pointsets
	-o \[string\]		The output integration results
	-s \[uint\]		The number of samples to read (if integrating from a sequence)
	--silent 		Silent mode
	--brute 		Output brute values instead of computing the statistics
			

To integrate over a function using an input 2D point set, we can use the following client line command:

 ./bin/integration/IntegrationNSphere\_fromfile\_2dd -i toto.dat 

Or one can use the following C++ code:

    
    IntegrationNSphere integration_test;
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
    			

Results
=======

Stochastic sampler

./bin/integration/IntegrationNSphere\_fromfile\_2dd -i stratified\_256.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
256	0.197266	2.11928e-05	0.191406	0.203125	0.19635		1.99127e-05	10 

[![](data/n-Sphere/integration_1_256.png)](data/n-Sphere/integration_1_256.png) [![](data/n-Sphere/integration_2_256.png)](data/n-Sphere/integration_2_256.png) [![](data/n-Sphere/integration_3_256.png)](data/n-Sphere/integration_3_256.png) ...

./bin/integration/IntegrationNSphere\_fromfile\_2dd -i stratified\_1024.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
1024	0.198926	1.0607e-05	0.194336	0.205078	0.19635		1.61833e-05	10 

[![](data/n-Sphere/integration_1_1024.png)](data/n-Sphere/integration_1_1024.png) [![](data/n-Sphere/integration_2_1024.png)](data/n-Sphere/integration_2_1024.png) [![](data/n-Sphere/integration_3_1024.png)](data/n-Sphere/integration_3_1024.png) ...

./bin/integration/IntegrationNSphere\_fromfile\_2dd -i stratified\_4096.dat
#Nbpts	#Mean		#Var		#Min		#Max		#Analytical	#MSE		#NbPtsets
4096	0.196167	1.02321e-06	0.194336	0.197998	0.19635		9.54216e-07	10 

[![](data/n-Sphere/integration_1_4096.png)](data/n-Sphere/integration_1_4096.png) [![](data/n-Sphere/integration_2_4096.png)](data/n-Sphere/integration_2_4096.png) [![](data/n-Sphere/integration_3_4096.png)](data/n-Sphere/integration_3_4096.png) ...

Deterministic sampler

./bin/integration/IntegrationNSphere\_fromfile\_2dd -i sobol\_1024.dat
#Nbpts	#Mean		#Var	#Min		#Max		#Analytical	#MSE		#NbPtsets
1024	0.203125	0	0.203125	0.203125	0.19635		4.59068e-05	1 

[![](data/n-Sphere/integration_1024.png)](data/n-Sphere/integration_1024.png)

./bin/integration/IntegrationNSphere\_fromfile\_2dd -i sobol\_4096.dat
#Nbpts	#Mean		#Var	#Min		#Max		#Analytical	#MSE		#NbPtsets
4096	0.196289	0	0.196289	0.196289	0.19635		3.65763e-09	1 

[![](data/n-Sphere/integration_4096.png)](data/n-Sphere/integration_4096.png)

Changing Radius

./bin/integration/IntegrationNSphere\_fromfile\_2dd --radius 0.1 -i sobol\_1024.dat
#Nbpts	#Mean		#Var	#Min		#Max		#Analytical	#MSE		#NbPtsets
1024	0.0332031	0	0.0332031	0.0332031	0.0314159	3.19408e-06	1 

[![](data/n-Sphere_rad_01/integration_1024.png)](data/n-Sphere_rad_01/integration_1024.png)

./bin/integration/IntegrationNSphere\_fromfile\_2dd --radius 0.4 -i sobol\_1024.dat
#Nbpts	#Mean	#Var	#Min	#Max	#Analytical	#MSE		#NbPtsets
1024	0.50293	0	0.50293	0.50293	0.502655	7.55496e-08	1 

[![](data/n-Sphere_rad_04/integration_1024.png)](data/n-Sphere_rad_04/integration_1024.png)