# Sampler Penrose [[ODJ04]](https://cseweb.ucsd.edu/~ravir/6160/papers/importancesampling.pdf)

## Description

The N-Rooks sampler.
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle https://liris.cnrs.fr/ldbn/HTML_bundle/index.html.

## Files

```
src/samplers/SamplerPenrose.cpp  
include/utk/samplers/SamplerPenrose.hpp
externals/Penrose/*
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">
```bash
Penrose sampler
Usage: ./build/src/samplers/Penrose [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file
</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerPenrose.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerPenrose penrose;

    // Check for no errors
    if (penrose.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("penrose.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

ndr = pyutk.Niederreiter(d=2, basis=2)
samples = ndr.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. Author code from 'Charles Donohue' has no licence but is provided with following header.

```
File: quasisampler_prototype.h
 Quasisampler prototype.

 This is a toy (non-optimized) implementation of the importance sampling
 technique proposed in the paper:
 "Fast Hierarchical Importance Sampling with Blue Noise Properties",
 by Victor Ostromoukhov, Charles Donohue and Pierre-Marc Jodoin,
 to be presented at SIGGRAPH 2004.

 Implementation by Charles Donohue,
Based on Mathematica code by Victor Ostromoukhov.
Universite de Montreal
18.08.04
```