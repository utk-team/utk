# Micro Jittering

## Descrition

Apply small modular shift vector to all points. Unlike Cranley Patterson, points are clamped if 
not in their regular strata (width $N ^{- 1 / D}$ around the point by default). 

* Shifts vector are uniformly selected in $[-N^{-1/D} / 2, N^{-1/D} / 2]$. Hence, shift vector coordinates are uniform, 
  but the direction vectors are not uniform in the sphere. 
* The methods only operates on points in [0, 1].

## Files 

```
src/scramblers/MicroJittering.cpp  
include/utk/scrambling/ScramblingMicroJitteringon.hpp
```


## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
MicroJittering scrambler
Usage: ./MicroJittering [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  -s,--seed UINT              Seed (unspecified means 'random')
  --silent                    Silence UTK logs

```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerSobol.hpp>
#include <utk/scrambling/ScramblingMicroJittering.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerSobol sobol(2 /* dimension */);
    sobol.setRandomSeed(/* empty means random, can also pass a number */);

    // Check for no errors
    if (sobol.generateSamples(pts, 1024 /* Number of points */))
    {
        utk::ScramblingMicroJittering sc;
        sc.setRandomSeed();

        // In place:
        sc.Scramble(pts);
        // Results in another pointset
        utk::Pointset<double> pts2;
        sc.Scramble(pts, pts2);        
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
samples = pyutk.Sobol(d=2, depth=0).sample(1024) 
sc = pyutk.MicroJittering().scramble(samples)
```  
