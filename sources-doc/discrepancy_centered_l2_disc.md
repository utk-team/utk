# Centered L2 Discrepancy

## Files

    src/discrepancy/CenteredL2Discrepancy.hpp
    src/bin/discrepancy/CenteredL2Discrepancy_2dd.cpp

## Description

This tool computes the Centered L2 discrepancy of an input pointset using the following formula  
[![](data/centered_l2_disc/centered_discrepancy.png)](data/centered_l2_disc/centered_discrepancy.png)  
from [[Hic98]](http://www.ams.org/journals/mcom/1998-67-221/S0025-5718-98-00894-1/S0025-5718-98-00894-1.pdf).

## License

BSD, see `CenteredL2Discrepancy.hpp`

## Execution

```
Parameters:  

	[HELP]
	-h [string]		Displays this help message
	-i [string]		The input pointsets
	-o [string]		The output discrepancies
	-s [uint]		The number of samples to read (if computing from a sequence)
	--silent 		Silent mode
	--brute 		Output brute values instead of computing the statistics
```			

To evaluate the discrepancy of an input 2D point set, one can use the following client line command:

     ./bin/discrepancy/CenteredL2Discrepancy_fromfile_2dd -i toto.dat

Or one can use the following C++ code:

```cpp    
    CenteredL2Discrepancy.hpp discrepancy_test;
    DiscrepancyStatistics stats;
    Pointset< D, double, Point > pts;
    //We assume pts is filled
    stats.nbpts = pts.size();

    //Can be done several times if we need to average over
    //several stochastic pointsets
    double discrepancy = 0;
    discrepancy_test.compute< D, double, Point >(pts, discrepancy)
    stats.addValue(discrepancy);

    stats.computeStatistics();
    std::cout << stats << std::endl;
```    			

## Results

Stochastic sampler

```    			
./bin/discrepancy/CenteredL2Discrepancy_fromfile_2dd -i stratified_256.dat
#Nbpts		#Mean		#Var		#Min		#Max		#NbPtsets
256		0.0100191		2.32342e-07		0.00927838		0.0108118		10
```    			

[![](data/centered_l2_disc/stratified_256_1.png)](data/centered_l2_disc/stratified_256_1.png) [![](data/centered_l2_disc/stratified_256_2.png)](data/centered_l2_disc/stratified_256_2.png) [![](data/centered_l2_disc/stratified_256_3.png)](data/centered_l2_disc/stratified_256_3.png) ...

```    			
./bin/discrepancy/CenteredL2Discrepancy_fromfile_2dd -i stratified_1024.dat
#Nbpts		#Mean		#Var		#Min		#Max		#NbPtsets
1024		0.00358071		2.12656e-08		0.00332768		0.00376758		10
```    			

[![](data/centered_l2_disc/stratified_1024_1.png)](data/centered_l2_disc/stratified_1024_1.png) [![](data/centered_l2_disc/stratified_1024_2.png)](data/centered_l2_disc/stratified_1024_2.png) [![](data/centered_l2_disc/stratified_1024_3.png)](data/centered_l2_disc/stratified_1024_3.png) ...

```    			
./bin/discrepancy/CenteredL2Discrepancy_fromfile_2dd -i stratified_4096.dat
#Nbpts		#Mean		#Var		#Min		#Max		#NbPtsets
4096		0.0012567		2.50781e-09		0.00120009		0.00134903		10
```    			

[![](data/centered_l2_disc/stratified_4096_1.png)](data/centered_l2_disc/stratified_4096_1.png) [![](data/centered_l2_disc/stratified_4096_2.png)](data/centered_l2_disc/stratified_4096_2.png) [![](data/centered_l2_disc/stratified_4096_3.png)](data/centered_l2_disc/stratified_4096_3.png) ...

Deterministic sampler

```    			
./bin/discrepancy/CenteredL2Discrepancy_fromfile_2dd -i sobol_1024.dat
#Nbpts		#Mean		#Var		#Min		#Max		#NbPtsets
1024		0.00105202		0		0.00105202		0.00105202		1
```    			

[![](data/centered_l2_disc/sobol_1024.png)](data/centered_l2_disc/sobol_1024.png)

```    			
./bin/discrepancy/CenteredL2Discrepancy_fromfile_2dd -i sobol_4096.dat
#Nbpts		#Mean		#Var		#Min		#Max		#NbPtsets
4096		0.000276215		0		0.000276215		0.000276215		1
```    			

[![](data/centered_l2_disc/sobol_4096.png)](data/centered_l2_disc/sobol_4096.png)
