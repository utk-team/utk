# OwenTree Scrambling [[Owen1996]](https://link.springer.com/chapter/10.1007/978-1-4612-2552-2_19)

## Description 

Perform Owen's scrambling on a given Pointset.


Unlike [Owen's scramler](./Owen.md), this scrambler partially stores the trees to allow user to specify bits. The key function is `setBitPattern` which allows to adress specific dyadic intervals and have multiple parameters:

  * Dimension: Which tree to modify
  * The pattern: A string of '0', '1' or '*'. First characters are the most significant bits !
  * Value: Sets the corresponding bits to True or False (swap or not).

For instance, the pattern '0*1' addresses 2 dyadic intervals : [0.125, 0.250] and [0.375, 0.500].
Specfically : 

* '0': Inside [0.000, 0.500]
* '*': Either [0.000, 0.250] or [0.250, 0.500]
* '1': Inside [0.125, 0.250] or [0.375, 0.500]

Therefore, if `value` is set to `true`, it is guaranteed to swap points in the intervals: 

* [0.1250, 0.1875] with [0.1875, 0.2500]
* [0.3750, 0.4375] with [0.4375, 0.5000]

To swap [0.0, 0.5] with [0.5, 1.0] use the pattern `""` (ie. swap points independently of 
their first bit). Use it last as it is always included in the other non-empty pattern.

The trees are not grown unless necessary. By default, they starts with no leaves.
Setting the pattern `0*1` will grow a FULL tree of height 3 (2^(3+1) - 1 elements). Setting 
afterwards `0*1*` will expand the tree to be of height 4 (2^(4+1) elements). 

The `max_height` parameter in the constructor allows the scrambling to go beyond
the stored trees without memory overhead. Trees are then generated randomly in the same
way [Owen's scrambler](./Owen.md) does. 


* By it's very nature, it only operates on 'integer pointsets', meaning that
points are not in [0, 1], but integers [[0, INT_MAX]]. There are no check on the type
in the executable version and double are cast to integers (and will likely be converted to 0).


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