# Cranley Patterson [[CP76](https://www.jstor.org/stable/2156452)]

## Descrition

Apply small modular shift vector to all points. It perfors : $y_i = (x_i + v) mod 1.0$.

* Shifts vector are uniformly selected in [-1, 1]. Hence, shift vector coordinates are uniform, 
  but the direction vector are not uniform in the sphere. 
* The methods only operates on points in [0, 1].

## Files 

```
src/scramblers/CranleyPatterson.cpp  
include/utk/scrambling/ScramblingCranleyPatterson.hpp
```


## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
CranleyPatterson scrambler
Usage: ./CranleyPatterson [OPTIONS]

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
#include <utk/scrambling/ScramblingCranleyPatterson.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerSobol sobol(2 /* dimension */);
    sobol.setRandomSeed(/* empty means random, can also pass a number */);

    // Check for no errors
    if (sobol.generateSamples(pts, 1024 /* Number of points */))
    {
        utk::ScramblingCranleyPatterson sc;
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
samples = pyutk.Sobol(d=2, depth=0).isample(1024) # isample returns integers
sc = pyutk.CranleyPatterson()
).scramble(samples)               # returns a double array
```  
