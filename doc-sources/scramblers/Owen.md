# Owen's Scrambling [[Owen1996]](https://link.springer.com/chapter/10.1007/978-1-4612-2552-2_19)

## Description 

Perform Owen's scrambling on a given Pointset.

* By it's very nature, it only operates on 'integer pointsets', meaning that
points are not in [0, 1], but integers [[0, INT_MAX]]. There are no check on the type
in the executable version and double are cast to integers.
* This implementation does not store the full permutation tree. It is recomputed
by seeding a PRNG. Hence, it is recommended to use a depth of 32

## Files

```
src/scramblers/Owen.cpp  
include/utk/scrambling/ScramblingOwen.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Owen scrambler
Usage: ./Owen [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  -s,--seed UINT              Seed (unspecified means 'random')
  --silent                    Silence UTK logs
  --depth UINT [32]           Depth of scrambling
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerSobol.hpp>
#include <utk/scrambling/ScramblingOwen.hpp>

int main()
{
    utk::Pointset<uint32_t> pts;

    utk::SamplerSobol sobol(2 /* dimension */);
    sobol.setRandomSeed(/* empty means random, can also pass a number */);

    // Check for no errors
    if (sobol.generateSamples(pts, 1024 /* Number of points */))
    {
        utk::ScramblingOwen owen(32 /* depth */);
        owen.setRandomSeed();
        
        // In place:
        owen.Scramble(pts);
        // Results in another pointset. If double, it is converted appropriatly
        utk::Pointset<double> pts2;
        owen.Scramble(pts, pts2);        
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
samples = pyutk.Sobol(d=2, depth=0).isample(1024) # isample returns integers
sc = pyutk.Owen(
  depth = 32                      # Recommended. 0: means no scrambling, log2(N) is common, but 32 is even better !
).scramble(samples)               # returns a double array
```  

</div>