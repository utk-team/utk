# Cascaded Sobol Sampling with owen permutation [[Paulin21]](https://projet.liris.cnrs.fr/cascaded/)


## Description

The Cascaded Sobol' construction from [[Paulin21]](https://projet.liris.cnrs.fr/cascaded/).

## Files

```
src/samplers/CascadedSobol.cpp  
include/utk/samplers/SamplerCascadedSobol.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
CascadedSobol sampler
Usage: ./CascadedSobol [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --depth UINT [0]            Owen depth (0: no randomness, 32: recommended).
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerCascadedSobol.hpp>

int main()
{
    utk::Pointset<double> pts;

    // If needed, can add template to use 64bits integers
    utk::SamplerCascadedSobol<uint32_t> sobol(2 /* dimension */, 32 /* depth */);
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
sobol = pyutk.CascadedSobol(d=2, depth=32) 
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