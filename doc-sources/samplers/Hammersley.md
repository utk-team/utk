# Sampler Hammersley [[Ham60]](http://onlinelibrary.wiley.com/doi/10.1111/j.1749-6632.1960.tb42846.x/abstract)

## Description

The Hammersley sampler. This sampler is only 2D, with the first coordinate obtained by dividing the index of the sample by the total number of samples, and the second coordinate obtained by reversing the binary representation of the index.  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/Hammersley.cpp  
include/utk/samplers/SamplerHammersley.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Hammersley sampler
Usage: ./Hammersley [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              UNUSED ! Here for compatibility with others.
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerHammersley.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerHammersley ham;

    // Check for no errors
    if (ham.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("ham.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

halton = pyutk.Hammersley()
samples = halton.sample(1024)  # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.
