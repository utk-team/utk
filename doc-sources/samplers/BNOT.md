# Sampler BNOT [[de Goes12]](http://www.geometry.caltech.edu/BlueNoise/)

## Description

Blue noise through Optimal Transport sampler. Two implementation are given : one in the preriodic
domain and the other in the bounded domain. The source code is modified to work with latest versions of CGal (actually: 5.4-1).

This sampler requires UTK to be linked against CGal and SuiteSparse. 

* For executable: CMake option UTK_USE_CGAL=ON.
* For C++ library: CGal and SuiteSparse (AMD, CHOLMOD, SPQR) 
* For python library : CMake option UTK_USE_CGAL=ON. See `setup.py` line 26.

## Files

```
src/samplers/PeriodicBNOT.cpp  
src/samplers/BoundedBNOT.cpp
include/utk/samplers/BoundedBNOT.hpp
include/utk/samplers/PeriodicBNOT.hpp
externals/BoundedBNOT/*
externals/PeriodicBNOT/*
```

## Usage


<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Sampler BoundedBNOT sampler
Usage: ./src/samplers/BoundedBNOT [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
```

```bash
Sampler PeriodicBNOT sampler
Usage: ./src/samplers/PeriodicBNOT [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n UINT REQUIRED            Number of points
  -d UINT                     UNUSED ! Here for compatibility with others.
  -s,--seed UINT              Seed (unspecified means 'random')
  -m UINT [1]                 Number of pointsets
  -o,--out TEXT [out.dat]     Output file (format). {i} splits outputs in multiple files and token is replaced by index.
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerPeriodicBNOT.hpp>
#include <utk/samplers/SamplerBoundedBNOT.hpp>

int main()
{
    utk::Pointset<double> pts;

    utk::SamplerPeriodicBNOT pbnot;
    utk::SamplerPeriodicBNOT bbnot;
    pbnot.setRandomSeed(/* empty means random, can also pass a number */);
    bbnot.setRandomSeed(/* empty means random, can also pass a number */);
   
    // Check for no errors
    if (pbnot.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("pbnotpatterns.dat", pts);
    }

    if (bbnot.generateSamples(pts, 1024 /* Number of points */))
    {
        write_text_pointset("pbnotpatterns.dat", pts);
    }
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk

pbnot = pyutk.PBNOT()
bbnot = pyutk.BBNOT()

samples = pbnot.sample(1024) # This is a numpy array !
samples = bbnot.sample(1024) # This is a numpy array !
```  

</div>

## License

See Licence.md file. Author code from 'F. De Goes' is provided under GPL license ((see [http://www.geometry.caltech.edu/BlueNoise/](http://www.geometry.caltech.edu/BlueNoise/))).
