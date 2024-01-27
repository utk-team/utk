# Sliced Optimal Transport Sampling [Paulin 2021](https://perso.liris.cnrs.fr/lpaulin/Publications/paulin2020.html)


## Description

Sliced optimal transport sampling distributes samples to be as uniform (in the sens of optimal transport to uniform distribution) as possible any random 1D slice (in the sphere). See paper for more details.

## Files

```
src/samplers/SOT.cpp  
include/utk/samplers/SamplerSOT.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
SOT sampler
Usage: ./SOT [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --silent                    Silence UTK logs
  -p,--projections UINT [64]  Number of slices (projection) to use.
  -i,--iterations UINT [4096] 
                              Number of iterations.
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerSOT.hpp>

int main()
{
    utk::Pointset<double> pts;

    // If needed, can add template to use 64bits integers
    utk::SamplerSOT sot(
        5,    // Dimension, 
        4096, // Number of iterations
        64    // Number of slices ("BatchSize")
    );
    sot.setRandomSeed(args->seed);
    // Check for no errors
    if (sot.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("sobol2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

sot = pyutk.SOT(
  5,    // Dimension, 
  4096, // Number of iterations
  64    // Number of slices ("BatchSize")
)
samples = sot.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. Author code from 'Abdalla Ahmed' has no licence but is provided with following header.