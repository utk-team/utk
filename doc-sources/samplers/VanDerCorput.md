# Sampler Hammersley [[VDC35]](https://dwc.knaw.nl/DL/publications/PU00014607.pdf)

## Description

Van Der Corput sequence (1D only).

For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

## Files

```
src/samplers/VanDerCorput.cpp  
include/utk/samplers/SamplerVanDerCorput.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Van Der Corput sampler
Usage: ./VanDerCorput [OPTIONS]

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
#include <utk/samplers/SamplerVanDerCorput.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerVanDerCorput vdc;

    // Check for no errors
    if (vdc.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("vdc.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
# No python binding
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.