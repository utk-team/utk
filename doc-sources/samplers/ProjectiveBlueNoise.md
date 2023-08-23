# Sampler Dart Throwing [[Reinert15]](https://resources.mpi-inf.mpg.de/ProjectiveBlueNoise/ProjectiveBlueNoise.pdf)

## Description

Implementation of [Projective Blue-Noise Sampling](https://resources.mpi-inf.mpg.de/ProjectiveBlueNoise/ProjectiveBlueNoise.pdf) in N-D.

For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/ProjectiveBlueNoise.cpp  
include/utk/samplers/SamplerProjectiveBlueNoise.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Projective Dart Throwing sampler
Usage: ./ProjectiveBlueNoise [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --relax [1]                 Enable relaxation
  --euclidean [0]             Use euclidean distance (default is wrap around)
  --relax_factor FLOAT [0.9]  Amount of relaxation applied after too many unsucessful trials
  --trials UINT [1000]        Number of trials before relaxation occurs
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerDartThrowing.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerProjectiveBlueNoise dt(
        2 /* dimension */
        true,  /* true = relaxed, false = not relaxed */
        false, /* false = euclidean distance, true = toroidal*/
        1000,  /* trials before relaxing distance */ 
        0.9,   /* Relaxation factor */
    );
    dt.setRandomSeed(/* Or a seed */);
    // Check for no errors
    if (dt.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("dt2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

dt = pyutk.ProjectiveBlueNoise(
    2,     # dimension, 
    True,  # true = relaxed, false = not relaxed
    False, # false = euclidean distance, true = toroidal
    1000,  # trials before relaxing distance 
    0.9,   # Relaxation factor
)
samples = dt.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.