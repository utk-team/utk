# Bridson Poisson Disk [[Bridson07]](https://dl.acm.org/doi/10.1145/1278780.1278807)

## Description

Implementation of Bridson algorithm to generate Poisson Disc samples. Instead of a grid, a hash_map is used 
to emulate the grid. Hence the algorithm does not use exponential amount of memory when generating samples. 
The complexity is $O(2^d N log(N))$ (instead of $O(2^d N)$ if a grid is used.)

## Files

```
src/samplers/Bridson.cpp  
include/utk/samplers/SamplerBridson.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Bridson algorithm for Poisson disk sampling
Usage: ./src/samplers/Bridson [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --silent                    Silence UTK logs
  --euclidean [0]             Use euclidean distance (default is wrap around)
  --trials UINT [30]          Number of trials candidates to try
  --packing FLOAT [-1]        Set packing density to use. If negative use precomputed values (meaningfull only when D < 9)

```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerBridson.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerBridson dt(
        2 /* dimension */
        false, /* false = euclidean distance, true = toroidal*/
        30,    /* Number of candidates to use for each try */ 
        -1     /* Distance (-1 = precomputed values) */
    );
    dt.setRandomSeed(/* Or a seed */);
    // Check for no errors
    if (dt.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("dt2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

dt = pyutk.Bridson(
    2,     # dimension, 
    False, # false = euclidean distance, true = toroidal
    30,    # Number of candidates 
    -1     # Distance (-1 = precomputed sphere packing values)

)
samples = dt.sample(1024) # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.