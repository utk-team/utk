# Sampler Lloyd 3D

## Description


This sampler is a wrapper around the
[geogram](http://alice.loria.fr/software/geogram/doc/html/index.html)
library for an efficient implementation of periodic and non-periodic
Delaunay triangulation in 3D. The `Lloyd_3dd` sampler is a standard
implementation of Lloyd's relaxation algorithm.


## Files

```
src/samplers/Lloyd.cpp  
include/utk/samplers/Lloyd.hpp
externals/Lloyd/*
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Lloyd sampler
Usage: ./samplers/Lloyd [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file
  --steps [100]               Number of steps to perform
  --toroidal [0]              When set, use toroidal version.
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerLloyd.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerLloyd lloyd(100 /* nbSteps */, true /* True for toroidal */ );
    lloyd.setRandomSeed(/* empty means random, can also pass a number */);

    // Check for no errors
    if (lloyd.generateSamples(pts, 1024 /* Number of points, must be a squared */))
    {
        write_text_pointset("lloyd.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

lloyd = pyutk.Lloyd()
samples = lloyd.sample(1024)  # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.