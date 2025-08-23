# Sampler OneTwoSobol [BCIO25](https://perso.liris.cnrs.fr/david.coeurjolly/publication/one-two-seq-25/)

A sobol sequence with guarenteed quality 2D projections. 

## Files

```
src/samplers/OneTwoSobol.cpp  
include/utk/samplers/OneTwoSobol.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Sobol sampler
Usage: ./OneTwoSobol [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              Seed (unspecified means 'random')
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerOneTwoSobol.hpp>

int main()
{
    utk::Pointset<double> pts;

    // If needed, can add template to use 64bits integers
    utk::OneTwoSobol sobol(2 /* dimension */);
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
sobol = pyutk.OneTwoSobol(d=2) 
samples = sobol.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. 
