# t-value 

## Description

Compute the t-value associated with given pointset. The t value is smallest positive integer such that every b-adic intervals of size $\frac{1}{b^{\log_b(N)-t}}$ contains exactly $b^t$ points. 

* b is the (prime) basis, and t values can only be computed if $\log_b(N)$ is an integer. 
* t is therefore an integer and lower value is better and implies good discrepancy propeties.
* Complexity is roughly: $dN\log(N)P(N, d)(1 + N\log(N))$, which is asymtotically $dN^d$... 

More in [[Lem 2009]](https://link.springer.com/book/10.1007/978-0-387-78165-5)

## Files

```
src/metrics/TValue.cpp  
include/utk/metrics/TValue.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
t-value calculator
Usage: ./src/metrics/TValue [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--output TEXT            Output file (empty is stdout)
  --silent                    Silence UTK logs
  -b,--basis UINT [2]         Basis of intervalls
```
</div>

<div class="cpp tabcontent">

```cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerWhitenoise.hpp>
#include <utk/metrics/TValue.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = utk::TValue(2 /* basis */).compute(pts); // should be around log(N) !
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

t = pyutk.TValue(
  2 # Basis
).compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License

BSD, see `TValue.hpp`