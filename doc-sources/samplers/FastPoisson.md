# Sampler Fast Poisson [[Bri07]](https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf)

## Description


The Fast Poisson sampler from [[DH06]](https://dl.acm.org/citation.cfm?doid=1141911.1141915).  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/DartThrowing.cpp  
include/utk/samplers/SamplerFastPoisson.hpp
externals/FastPoisson/*
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
FastPoisson sampler
Usage: ./FastPoisson [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              UNUSED ! Here for compatibility with others.
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --method TEXT [DartThrowing] 
                              Method. [DartThrowing, BestCandidate, BoundarySampler, Pure, LinearPure, Penrose, Uniform, ]
  -r,--radius FLOAT [0]       Radius in [0.0005, 0.2]. If outside the range, will attempt to find a radius given N.
  --throwMult UINT [1]        Multiplier for number of throw (for DartThrowing & BestCandidate methods)
  --throws UINT [1000]        Number of throws ().
  --maximize                  Enable maximization
  --tiled                     Enable tiling. Might be ignored for tiled-only methods
  --relaxCount [0]            Number of relaxation to perform
  --silent                    Silence UTK logs

```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerFastPoisson.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerFastPoisson fp(
        "DartThrowing", /* method */
        0.01,           /* radius */
        1,              /* Throws multiplier */
        1000,           /* Number of throws */
        false,          /* Maximize or not */
        true,           /* True for tiling, might be ignored */
        10,             /* Number of relaxation steps */
    );
    fp.setRandomSeed(/* Or a seed */);
    // Check for no errors
    if (fp.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("fastpoisson.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

fp = pyutk.FastPoisson(
    "DartThrowing", # method
    0.01,           # radius
    1,              # Throws multiplier
    1000,           # Number of throws
    False,          # Maximize or not
    True,           # True for tiling, might be ignored
    10,             # Number of relaxation steps
)
samples = fp.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. Author code from 'Daniel Dunbar' is released into the public domain. 