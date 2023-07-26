# Sampler Regular Grid 

## Description

A regular grid.

For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/RegularGrid.cpp  
include/utk/samplers/SamplerRegularGrid.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
RegularGrid sampler
Usage: ./RegularGrid [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              UNUSED ! Here for compatibility with others.
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.

```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerRegularGrid.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerRegularGrid rg(2 /* dimension */);
    // Check for no errors
    if (rg.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("rg2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

rg = pyutk.RegularGrid(d=2)
samples = rg.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.