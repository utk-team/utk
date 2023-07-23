# Sampler Halton [[Hal64]](https://dl.acm.org/citation.cfm?id=365104)

## Description

The Halton sequence. Samples are generated from reversing the n-ary representation of their index. Each dimension has a different basis and the low discrepancy is ensured if all basis are distinct and are prime numbers.  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/Halton.cpp  
include/utk/samplers/SamplerHalton.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Halton sampler
Usage: ./samplers/Halton [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerHalton.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerHalton halton(2 /* dimension */);
    
    // Default are the first 100 primes numbers
    unsigned int basis[] = {3, 5, 7} // Skip base 2
    halton.setBasisList(basis, 3);

    // Check for no errors
    if (halton.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("halton2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

halton = pyutk.Halton(d=2)
# Default is the first 100 primes numbers
halton.setBasisList([3, 5, 7]) # Skip base 2
samples = halton.sample(1024)  # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See licence.md file. 