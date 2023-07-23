# Sampler ART [[ANH*17]](http://abdallagafar.com/publications/art/)

## Description

The sampler from [[ANH*17]](http://abdallagafar.com/publications/art/)  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/ART.cpp  
include/utk/samplers/SamplerART.hpp
externals/ART/*
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
ART sampler
Usage: ./samplers/ART [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file
  --tfile TEXT                Path to table file. Default (empty) is the one provided by the author.
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerART.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::ART art;
    // Check for no errors
    if (art.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("art.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

aa = pyutk.ART()
samples = aa.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. Author code from 'Abdalla Ahmed' is provided without license. 