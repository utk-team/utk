# Radial SPectrum

## Description

Computes Radial Average of the fourier spectrum.

When the `UTK_USE_FFTW` cmake flag is set to true, the Fourier transform is computed using the [fftw](https://www.fftw.org) library (faster for high resolution spectra).

Note : C++ API also supports computing the average from an already computed spectrum. 

Note : When multiples pointset are given to 'compute', the results are averaged.

## Files

```
src/metrics/Spectrum.cpp  
include/utk/metrics/PCF.hpp
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
RadialSpectrum calculator
Usage: ./RadialSpectrum [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE REQUIRED
                              Input file
  -o,--output TEXT            Output file (empty is stdout)
  -r,--res UINT [0]           Spectrum resolution (0 means automatic)
  --canceldc [0]              When set, cancel the DC peak
  -s,--scale FLOAT [0.5]      Scale for distance to origin
  -b,--bins UINT [0]          Number of bins (0 means automatic)
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerWhitenoise.hpp>
#include <utk/metrics/RadialSpectrum.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerWhitenoise wn(2 /* dimension */);
    wn.setRandomSeed(/* empty means random, can also pass a number */);
    // Check for no errors

    auto rad = utk::RadialSpectrum(
       75,  // Number of bins
       0.5, // Scale for distances (compress / expands bins size)
       101, // Resolution for underlying fourier spectrum, only odd number. If even will use res - 1
       true // Cancel DC
    );
    if (wn.generateSamples(pts, 1024 /* Number of points */))
    {
        auto rslt = rad.compute(pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk
import numpy as np

rad = pyutk.RadialSpectrum(
  75,  # Number of bins
  0.5, # Scale for distances (compress / expands bins size)
  101, # Resolution for underlying fourier spectrum, only odd number. If even will use res - 1
  True # Cancel DC
)
rad = rad.compute(np.random.uniform(0, 1, (128, 2)))
```  

</div>

## License

BSD, see `Spectrum.hpp`