# Sampler Rank 1 [[Keller04]](https://link.springer.com/chapter/10.1007%2F978-3-642-18743-8_18).  

## Description

The Rank 1 lattices from [[Keller04]](https://link.springer.com/chapter/10.1007%2F978-3-642-18743-8_18).  

For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/R1.cpp  
include/utk/samplers/SamplerR1.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
R1 sampler
Usage: ./Rank1 [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              UNUSED ! Here for compatibility with others.
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  -e,--exhaustive [0]         Enable exhaustive search
  -f,--fibonacci [0]          Enable fibonnaci lattice
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerR1.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerR1 r1(false /* exhaustive search */, false /* fibonnaci lattice */);
    // Check for no errors
    if (r1.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("r1.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

r1 = pyutk.R1(
    False,  # Ehaustive search
    False, # Fibonnaci lattice
)
samples = r1.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.