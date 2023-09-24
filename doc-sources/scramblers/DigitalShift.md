# Digital Shift Scrambling 

## Description 

Perform DigitalShift scrambling on a given Pointset. This is done by XORing the sample components by a random binary word.

* By it's very nature, it only operates on 'integer pointsets', meaning that
points are not in [0, 1], but integers [[0, INT_MAX]]. There are no check on 
the type in the executable version and double are cast to integers.

## Files

```
src/scramblers/DigitalShift.cpp  
include/utk/scrambling/ScramblingDigitalShift.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Digital shift scrambler
Usage: DigitalShift [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  -s,--seed UINT              Seed (unspecified means &apos;random&apos;)
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerSobol.hpp>
#include <utk/scrambling/ScramblingDigitalShift.hpp>

int main()
{
    utk::Pointset<uint32_t> pts;

    utk::SamplerSobol sobol(2 /* dimension */);
    sobol.setRandomSeed(/* empty means random, can also pass a number */);

    // Check for no errors
    if (sobol.generateSamples(pts, 1024 /* Number of points */))
    {
        utk::ScramblingDigitalShift scrambler;
        scrambler.setRandomSeed();
        // In place:
        scrambler.Scramble(pts);
        // Results in another pointset. If double, it is converted appropriatly
        utk::Pointset<double> pts2;
        scrambler.Scramble(pts, pts2);        
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
samples = pyutk.Sobol(d=2, depth=0).isample(1024) # isample returns integers
sc = pyutk.DigitalShift().scramble(samples)       # returns a double array
```  

</div>