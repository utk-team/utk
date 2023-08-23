# Sampler Kronecker 

## Description

A Kronecker sequence is defined by an additive recurrence for each dimension:

S[i] = { S[i-1] + a }

Where _a_ is an irrational value and {_x_} denotes the fractional part of _x_. Multiple choices are
supported by default : 

* R2, R3, R4: [[Robets2018]](http://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/)
* K21_2, K21_3, K21_4 : [[Patel2022]](https://jcgt.org/published/0011/01/04/)
* K21b_2, K21b_3, K21b_4 : [[Patel2022]](https://jcgt.org/published/0011/01/04/)

## Files

```
src/samplers/Kronecker.cpp  
include/utk/samplers/SamplerKronecker.hpp
externals/Kronecker/*
```

## Usage

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Kronecker sampler
Usage: ./Kronecker [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT REQUIRED            Dimensions
  -s,--seed UINT              UNUSED ! Here for compatibility with others.
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
  --shifts TEXT               Shifts vectors. Supported [R2, R3, R4, K21_2, K21_3, K21_4, K21b_2, K21b_3, K21b_4, ]
  --silent                    Silence UTK logs

```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerKronecker.hpp>

int main()
{
    utk::Pointset<double> pts;
    utk::SamplerKronecker kron(2 /* dimension */);
    kron.setAlphas(
      utk::SamplerKronecker::ParseAlpha("K21_2", 2 /* dimension */)
    );

    // Check for no errors
    if (kron.generateSamples(pts, 1024 /* Number of points, must be a squared */))
    {
        write_text_pointset("kron2d.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

kron = pyutk.Kronecker(d=2)
kron.setAlphas("K21_2")
samples = kron.sample(1024)  # This is a numpy array !
```  

</div>

## Results

<div class="results"></div>
<script>
  window.addEventListener('DOMContentLoaded', function() { show_results(); }); 
</script>

## License

See Licence.md file.