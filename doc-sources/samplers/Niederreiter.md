# Sampler Niederreiter [[Nie88]](https://www.sciencedirect.com/science/article/pii/0022314X8890025X)

## Description


The Niederreiter sequence.  For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/Niederreiter.cpp  
include/utk/samplers/SamplerNiederreiter.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Niederreiter sampler
Usage: ./Niederreiter [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              UNUSED ! Here for compatibility with others.
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  -b,--basis UINT [2]         Sampler basis
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerNiederreiter.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerNiederreiter ndr(2 /* dimension */, 2 /* basis */);
    
    // Check for no errors
    if (ndr.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("ndr2d.dat", pts);
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

See Licence.md file. Author code is released under GNU LGPL licence. 

[GNU LGPL](https://people.sc.fsu.edu/~jburkardt/txt/gnu_lgpl.txt), code from [https://people.sc.fsu.edu/~jburkardt/cpp_src/niederreiter2/niederreiter2.html](https://people.sc.fsu.edu/~jburkardt/cpp_src/niederreiter2/niederreiter2.html)