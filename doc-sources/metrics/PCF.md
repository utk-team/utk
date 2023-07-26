# PCF

## Description


The Pair Correlation Function expresses the distribution of distances between pairs of points in a point set. Intuitively, it expresses the number of samples that are at a distance r from a "typical" sample. [Oztireli](https://graphics.ethz.ch/publications/papers/paperOzt12b.php) devised a simplified estimator for this measure in the particular case of isotropic and stationary point processes. It's this function that is implemented here.

Note : When multiples pointset are given to 'compute', the results are averaged.

## Files

```
src/metrics/PCF.cpp  
include/utk/metrics/PCF.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
PCF calculator
Usage: ./PCF [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--output TEXT            Output file (empty is stdout)
  -t,--toroidal [0]           When set, use toroidal distance
  -b,--nbbins UINT [200]      Number of bins
  --rmin FLOAT [0.01]         Minimal radius to consider
  --rmax FLOAT [0.01]         Maximal radius to consider
  -s,--smoothin FLOAT [0.001] 
                              Smoothing value
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerWhitenoise.hpp>
#include <utk/metrics/PCF.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors

    auto pcf = utk::PCF(
      true, // Toroidal 
      0.01, // Min radius 
      0.15, // Max radius
      200,  // Number of bins 
      0.001 // Kernel smoothing
    );
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = pcf.compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

pcf = PCF(
    True, # Toroidal 
    0.01, # Min radius 
    0.15, # Max radius
    200,  # Number of bins 
    0.001 # Kernel smoothing
  
)
pcf = pcf.compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License

BSD, see `UnanchoredL2Discrepancy.hpp`