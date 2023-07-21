# Sampler PMJ [[PMJ]](https://graphics.pixar.com/library ProgressiveMultiJitteredSampling/paper.pdf)

## Description


The Progressive Multi-Jittered sampler. This sampler is divided in 3 techniques, each giving better results than the previous one:  

Progressive Jittered (PJ): Divides the unit square into 4 equal squares. Randomly throws a point into each subsquare. Then divide those subsquares into subsubsquares. Randomly throws points in them, and so on.  

Progressive Multi-Jittered (PMJ): Adds to PJ one property: Using PJ, each sample falls into its own square subdivision. With PMJ, each sample falls into its own square subdivision AND its own strata. When throwing points, uses a best candidate algorithm.  

Progressive Multi-Jittered(0,2) (PMJ02): Generalizes the previous techniques. If we subdivide the unit square into equal rectangles, each sample falls into its own rectangle.  

C++ Implementation of [[PMJ]](https://graphics.pixar.com/library/ProgressiveMultiJitteredSampling/paper.pdf)

`PMJ02_Pharr` implements Matt Pharr's variant
from the JCGT article "Efficient Generation of Points that Satisfy
Two-Dimensional Elementary Intervals" focusing on fast PMJ02 sampling.

## Files

```
src/samplers/PMJ.cpp  
include/utk/samplers/PMJ.hpp
externals/PMJ/*
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
PMJ sampler
Usage: ./build/src/samplers/PMJ [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file
  --method TEXT [PMJ02]       Method to use. Available (other will be treaded as PMJ02): PJ, PMJ, PMJ02, PMJ02_Pharr,
  -c,--candidates UINT [10]   Number of candidates for PMJ02* methods
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerPMJ.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerPMJ pmj("PMJ02" /* method */, 10 /* Number of candidates */);
    pmj.setRandomSeed(/* Or a seed */);
    // Check for no errors
    if (pmj.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("pmj02d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

pmj = pyutk.PMJ(
    "PMJ02", # Method
    10,      # Candidates for PMJ02* methods
)
samples = pmj.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. Licence may differ by methods. See corresponding author files. 