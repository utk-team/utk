# GaussianKernels

## Description

Compute a weighted average of distances among points. Weights are given by : $w_ij = e^{-\frac{d(x_i, x_j)^2}{2\sigma^2}}$ and correspond to a gaussian centered of variance $\sigma$ around each points.

## Files

```
src/metrics/GaussianKernels.cpp  
include/utk/metrics/GaussianKernels.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
GaussianKernels calculator
Usage: ./GaussianKernels [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--output TEXT            Output file (empty is stdout)
  --silent                    Silence UTK logs
  --euclidean [0]             Use euclidean distance (default is wrap around)
  --sigma [1]                 Variance of gaussians (relative to nominal grid N^(1/D))

```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerWhitenoise.hpp>
#include <utk/metrics/MinDist.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = utk::MinDist(true /* Toroidal / euclidean distance */, 1.0 /* sigma */).compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

d = pyutk.GaussianKernels(
  True, # Toroidal / Euclidean Distance, 
  1.0   # Sigma (relative to nominal grid !)
).compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License

BSD, see `MinDist.hpp`