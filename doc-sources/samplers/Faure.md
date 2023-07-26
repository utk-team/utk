# Sampler Faure [[Fau82]](https://eudml.org/doc/205851)

## Description

The Faure sequence (2D only). For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/Faure.cpp  
include/utk/samplers/SamplerFaure.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Faure sampler
Usage: ./Faure [OPTIONS]

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
#include <utk/samplers/SamplerFaure.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerFaure faure(2 /* dimension */);
    
    // Check for no errors
    if (faure.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("faure2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

faure = pyutk.Faure(d=2)
samples = faure.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file. Author code is provided with the following license : 

[GNU LGPL](https://people.sc.fsu.edu/~jburkardt/txt/gnu_lgpl.txt), code from [https://people.sc.fsu.edu/~jburkardt/cpp_src/faure/faure.html](https://people.sc.fsu.edu/~jburkardt/cpp_src/faure/faure.html)