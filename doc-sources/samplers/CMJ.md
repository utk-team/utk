# Sampler Correlated Multi Jittered [[Ken03]](http://graphics.pixar.com/library/MultiJitteredSampling/paper.pdf)

## Description

The Correlated Multi-Jitter sampler from [[Kens03]](http://graphics.pixar.com/library/MultiJitteredSampling/paper.pdf).  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/CMJ.cpp  
include/utk/samplers/SamplerCMJ.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
CMJ sampler
Usage: ./CMJ [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerCMJ.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerCMJ cmj;
    cmj.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors
    if (art.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("cmj.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

cmj = pyutk.CMJ()
samples = cmj.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.
