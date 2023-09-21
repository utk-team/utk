# Sampler LDBN [[APC*16]](https://liris.cnrs.fr/ldbn/)

## Description

The LDBN sampler from [[APC*16]](https://liris.cnrs.fr/ldbn). This sampler is only 2D.  
For a more precise description of this sampler and its performances in terms of aliasing and discrepancy, please refer to the following web bundle [https://liris.cnrs.fr/ldbn/HTML_bundle/index.html](https://liris.cnrs.fr/ldbn/HTML_bundle/index.html).

There are two version : 

1. A version that re-compute permutation
2. A lookup table version (recommended)

## Files

```
src/samplers/LDBN.cpp  
include/utk/samplers/SamplerLDBN.hpp
```

```
src/samplers/LutLDBN.cpp  
include/utk/samplers/LutLDBN.hpp
externals/LutLDBN/*
```

## Usage (1)

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
LDBN sampler
Usage: ./LDBN [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --tilesize UINT [128]       Tile size. WARNING: Other values than 128 might not be properly supported for random generation.
  --tileFile TEXT             File containing tiles. If empty a new one will be generated on the fly. Tilesize must be set accordingly.
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerLDBN.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerLDBN ldbn;
    ldbn.setPermutFile("a file");
    ldbn.setTileSize(128);
    ldbn.setRandomSeed(/* empty means random, can also pass a number */);

    // Check for no errors
    if (ldbn.generateSamples(pts, 1024 /* Number of points, must be a squared */))
    {
        write_text_pointset("ldbn.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
# No pyutk version supported
```  

</div>

## Usage (2)

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
LDBN(Lut) sampler
Usage: ./LutLDBN [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              UNUSED ! Here for compatibility with others.
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --target TEXT [BNOT]        Target to use. Available (other will be treaded as BNOT): BNOT, STEP,
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerLutLDBN.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerLutLDBN ldbn;
    ldbn.setTarget("BNOT" /* or step */);

    // Check for no errors
    if (ldbn.generateSamples(pts, 1024 /* Number of points, must be a squared */))
    {
        write_text_pointset("ldbn.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

ldbn = pyutk.LDBN()
samples = ldbn.sample(1024)  # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results("LutLDBN"); }); 
</script>

## License

See Licence.md file.