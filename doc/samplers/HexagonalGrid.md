# Sampler Hexagonal Grid

## Description

An hexagonal grid.  

For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/HexagonalGrid.cpp  
include/utk/samplers/SamplerHexagonalGrid.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
HexGrid sampler
Usage: ./samplers/HexagonalGrid [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerHexagonalGrid.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerHexagonalGrid hex;

    // Check for no errors
    if (hex.generateSamples(pts, 1024 /* Number of points, must be a squared */))
    {
        write_text_pointset("hex.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

# Number of samples must be a square
hexgrid = pyutk.HexagonalGrid()
samples = hexgrid.sample(1024)  # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.