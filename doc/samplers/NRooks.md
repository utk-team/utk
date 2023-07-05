# Sampler N-Rooks [[Shi91]](http://www.cs.utah.edu/~shirley/papers/euro91.pdf)

## Description

The N-Rooks sampler.
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle https://liris.cnrs.fr/ldbn/HTML_bundle/index.html.

## Files

```
src/samplers/NRooks.cpp  
include/utk/samplers/SamplerNRooks.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">
```bash
NRooks sampler
Usage: ./samplers/NRooks [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file
</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerNiederreiter.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerNRooks nrooks(2 /* dimension */);
    nrooks.setRandomSeed(/* empty means random, can also pass a number */);

    // Check for no errors
    if (nrooks.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("nrooks2d.dat", pts);
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

See Licence.md file.