# Diaphony

## Description

Diaphony is a metric of point set uniformity closely related to discrepancy.
This tool computes the diaphony in s(N^2) time, with N points in dimension s.

from
ZINTERHOF, P. 1976. Uber einige Absch~itzungen bei der Approximation von Funktionen mit Gleichverteilungsmethoden. Sitzungsber. Osterr. Akad. Wiss. Math.-Natur. K1. H 185, 121-132.


## Files

```
src/metrics/Diaphony.cpp  
include/utk/metrics/Diaphony.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Diaphony calculator
Usage: ./build/src/metrics/Diaphony [OPTIONS]

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
#include <utk/metrics/Diaphony.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = utk::Diaphony().compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

d = pyutk.Diaphony().compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License

BSD, see `Diaphony.hpp`