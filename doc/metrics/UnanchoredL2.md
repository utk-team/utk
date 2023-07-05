# UnanchoredL2

## Description

This tool computes the Unanchored L2 discrepancy of an input pointset using the following formula  
[![](../figs/unanchored_disc.png)](../figs/unanchored_disc.png)  
from [[Lem09]](http://www.springer.com/us/book/9780387781648).


## Files

```
src/metrics/UnanchoredL2.cpp  
include/utk/metrics/UnanchoredL2.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
UnanchoredL2Discrepancy calculator
Usage: ./build/src/metrics/UnanchoredL2 [OPTIONS]

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
#include <utk/metrics/UnanchoredL2Discrepancy.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = utk::UnanchoredL2Discrepancy().compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

d = pyutk.UnanchoredL2Discrepancy().compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License

BSD, see `UnanchoredL2Discrepancy.hpp`