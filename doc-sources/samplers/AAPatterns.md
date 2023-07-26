# Sampler AA Patterns [[Ahm16]](http://abdallagafar.com/abdalla/wp-content/uploads/2017/03/Sampling-with-AA-Patterns.pdf)

## Description

The AA Pattern sampler from [[Ahm16]](http://abdallagafar.com/abdalla/wp-content/uploads/2017/03/Sampling-with-AA-Patterns.pdf).  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/AAPatterns.cpp  
include/utk/samplers/SamplerAAPatterns.hpp
externals/AAPatterns/*
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
AAPatterns sampler
Usage: ./AAPatterns [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --vfile TEXT [blue]         Path to vector file, or use one of default [ blue, fpo-like, green, pink, step, ]

```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerAAPatterns.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerAAPatterns aa; // Default target is 'blue'
    aa.setRandomSeed(/* empty means random, can also pass a number */);
    aa.setVectorFile("green" /* Or one of supported types*/)
    
    // Check for no errors
    if (aa.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("aapatterns.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

aa = pyutk.AAPatterns()
aa.setVectorFile("green")
samples = aa.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. Author code from 'Abdalla Ahmed' has no licence but is provided with following header.

```
/* Generate a distribution based on an AA pattern and a loadable displacement map
 * Values which depend on pattern parameters are read from a header file so
 *  that different parameters can be used with this code.
 * We could also load these dynamically, but hard-coding them gives substantial speedup
 * Created by Abdalla Ahmed
 * 2014-11-11
 * Last revision 2015-09-10
 */
```