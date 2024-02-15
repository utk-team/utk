# GenerataiveMatrices


## Description

Sampler that produce samples using algebraic matrices of any base. A Owen Scrambling is also
performed in the provided base.  

The matrices should be given through a file, whose format is simply the list of the coefficients of the matrices. It can be formatted matrices by matrices or as a single
line. The sampler expect the file and the provided arguments (m = size of matrices, 
p = basis and d = dimension) to be coherents.

## Files

```
src/samplers/GenerativeMatrices.cpp  
include/utk/samplers/SamplerGenerativeMatrices.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Generative Matrix sampler
Usage: ./GenerativeMatrices [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --silent                    Silence UTK logs
  --file TEXT                 Matrices file
  --depth INT [0]             Owen scrambling depth
  --base INT                  Base of matrices
  --size INT                  Size of matrices
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerGenerativeMatrices.hpp>

int main()
{
    utk::Pointset<double> pts;

    // If needed, can add template to use 64bits integers
    utk::SamplerGenerativeMatrices sobol(
        "matrices.txt", // file, 
        10            , // m = size of matrices, 
        3             , // p = Base of matrices,
        5             , // d = Dimension
        10            , // Depth (at most 'size')
    );
    sobol.setRandomSeed(args->seed);
    // Check for no errors
    if (sobol.generateSamples(pts, 1024 /* Number of points */)) // At most p^m
    {
        write_text_pointset("sobol2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

sobol = pyutk.GenerativeMatrices(
    "matrices.txt", # file, 
    10            , # size of matrices, 
    3             , # Base of matrices,
    5             , # Dimension
    10            , # Depth (at most 'size')
)
samples = sobol.sample(1024) # This is a numpy array !
```  

</div>

## Results ([[Larcher and Pillichshammer 2003]](https://eudml.org/doc/278839))

Matrices: [matrices.txt](../figs/samplers/LP_matrices.txt)

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results("LP"); }); 
</script>

## License

See Licence.md file. Author code from 'Abdalla Ahmed' has no licence but is provided with following header.