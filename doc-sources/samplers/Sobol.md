# Sampler Sobol [[JK03]](http://web.maths.unsw.edu.au/~fkuo/sobol/) [[Owe95]](https://statistics.stanford.edu/sites/default/files/EFS%20NSF%20464.pdf)


## Description

The Sobol sequence. Samples are generated from a binary product between the binary representation of their index and a matrix, generated from primitive polynomials. By default, uses Juo & Kuo tables [[JK03]](http://web.maths.unsw.edu.au/~fkuo/sobol/).

The sequence is scrambled with Owen permutation [[Owe95]](https://statistics.stanford.edu/sites/default/files/EFS%20NSF%20464.pdf).

For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/Sobol.cpp  
include/utk/samplers/SamplerSobol.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Sobol sampler
Usage: ./Sobol [OPTIONS]

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
#include <utk/samplers/SamplerStep.hpp>

int main()
{
    utk::Pointset<double> pts;

    // If needed, can add template to use 64bits integers
    utk::SamplerSobol sobol(2 /* dimension */, 32 /* depth */);
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
sobol = pyutk.Sobol(d=2, depth=32) 
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