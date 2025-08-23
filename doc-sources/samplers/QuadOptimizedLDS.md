# Sampler Quad Optimized LDS [OBCI24](https://perso.liris.cnrs.fr/david.coeurjolly/publication/quadquad-24/)

A base 3 Sobol'-based sampler that provides optimized table for quadruplet of consecutive dimensions. 
Samples are scrambled using Owen scrambling at depth log(N). This sampler only works if N is a
power of 3.

## Files

```
src/samplers/QuadOptimizedLDS.cpp  
include/utk/samplers/QuadOptimizedLDS.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Sobol sampler
Usage: ./QuadOptimizedLDS [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              Seed (unspecified means 'random')
  --table TEXT                Init file (Joe&Kuo format). If not specified the [VO24] table is used
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerQuadOptimized.hpp>

int main()
{
    utk::Pointset<double> pts;

    // If needed, can add template to use 64bits integers
    utk::QuadOptimized sobol(2 /* dimension */);
    sobol.setRandomSeed(args->seed);
    // Check for no errors
    if (sobol.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("sobol2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

# Only 32 bits integers supported
sobol = pyutk.QuadOptimizedLDS(d=2) 
samples = sobol.sample(2187) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. 
