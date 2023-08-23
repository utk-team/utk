# MinDist

## Description

Compute minimum distance between points. 

## Files

```
src/metrics/MinDist.cpp  
include/utk/metrics/MinDist.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
MinDist calculator
Usage: ./MinDist [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--output TEXT            Output file (empty is stdout)
  --euclidean [0]             Use euclidean distance (default is wrap around)
  --silent                    Silence UTK logs
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
        auto rslt = utk::MinDist(true /* Toroidal / euclidean distance */).compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

d = pyutk.MinDist(
  True # Toroidal / Euclidean Distance
).compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License

BSD, see `MinDist.hpp`