# Semidiscrete Optimal transport to uniform distribution

## Description

Compute the semidiscrete optimal transport to uniform distribution distance of a pointset. 

The code originates from : [https://github.com/nbonneel/semi_discrete_ot_2d](https://github.com/nbonneel/semi_discrete_ot_2d). A patch is applied so that it is compatible with CMake. 

This metrics is only available for 2D pointsets . 

## Files

```
src/metrics/SemidiscreteOT.cpp  
include/utk/metrics/SemidiscreteOT.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Semidiscrete Optimal Transport to uniform distribution calculator (2D)
Usage: ./SemidiscreteOT [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--output TEXT            Output file (empty is stdout)
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerWhitenoise.hpp>
#include <utk/metrics/SemidiscreteOT.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = utk::SemidiscreteOT().compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

distance = pyutk.SemidiscreteOT().compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License
