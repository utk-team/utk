# Centered L2 Discrepancy

## Description

This tool computes the Centered L2 discrepancy of an input pointset using the following formula  
[![](../figs/centered_discrepancy.png)](../figs/centered_discrepancy.png)  
from [[Hic98]](http://www.ams.org/journals/mcom/1998-67-221/S0025-5718-98-00894-1/S0025-5718-98-00894-1.pdf).

## Files

```
src/metrics/CenteredL2.cpp  
include/utk/metrics/CenteredL2.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
GeneralizedL2 calculator
Usage: ./build/src/metrics/GL2 [OPTIONS]

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
#include <utk/metrics/CenteredL2.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = utk::CenteredL2Discrepancy().compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

d = pyutk.CenteredL2Discrepancy().compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License

BSD, see `L2.hpp`