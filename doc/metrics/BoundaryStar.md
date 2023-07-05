# Fast bounds on discrepancy

## Description

Code from [[Thi01]](https://www.sciencedirect.com/science/article/pii/S0885064X01906004). This tool approximates upper and lower boundaries on the Linf discrepancy. As those approximations can be computed faster than the true value, it allows to approximate the Linf discrepancy of higher dimensional point sets. Can be used in higher dimensions.

## Files

```
src/metrics/BoundariesStar.cpp  
include/utk/metrics/BoundariesStarDiscrepancy.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
BoundariesStarDiscrepancy calculator
Usage: ./BoundariesStar [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE REQUIRED
                              Input file
  -o,--output TEXT            Output file (empty is stdout)
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerWhitenoise.hpp>
#include <utk/metrics/BoundariesStarDiscrepancy.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = utk::BoundariesStarDiscrepancy().compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

dmin, dmax = pyutk.BoundariesStarDiscrepancy().compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License
