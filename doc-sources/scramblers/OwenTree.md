# OwenTree Scrambling [[Owen1996]](https://link.springer.com/chapter/10.1007/978-1-4612-2552-2_19)

## Description 

Perform Owen's scrambling on a given Pointset.

* By it's very nature, it only operates on 'integer pointsets', meaning that
points are not in [0, 1], but integers [[0, INT_MAX]]. There are no check on the type
in the executable version and double are cast to integers.
* This implementation partially stores the tree. The setBitPattern function allows to 
  quickly toggle on / off bits. A tree of height 'number of bits' is then stored. 
* If max_depth is higher than tree height, the end is randomly selected (and not stored).


## Files

```
include/utk/scrambling/ScramblingOwenTree.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
No CLI interface. 
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
        utk::ScramblingOwenTree owen(32 /* max depth : no tree stored yet */);
        owen.setBitPattern(
            0,      // Dimension 
            "0*1",  // Patterns. 
                    // '0': Inside [0.000, 0.500]
                    // '*': Either [0.000, 0.250] or [0.250, 0.500]
                    // '1': Inside [0.125, 0.250] or [0.375, 0.500]
                    // Therefore : Guarentees to swap or not  
                    //   [0.1250, 0.1875] with [0.1875, 0.2500]
                    //   [0.3750, 0.4375] with [0.4375, 0.5000]
            true    // True means 'swap' the intervals
        ); // Tree up to depth 3 alreay stores, so no allocation


        // In place:
        owen.Scramble(pts);
        owen.setRandomSeed();
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
sc = pyutk.OwenTree(
  depth = 32                      # max depth
)
sc.setBitPattern(
    0, 
    "0*1", 
    False
)

sc.scramble(samples) # This is a double array
```  

</div>