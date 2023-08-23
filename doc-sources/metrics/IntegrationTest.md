# Integration Test

## Description

Performs integration test on a pointset. The choice of integrand is crucial, and should be carefully choosen. UTK 
provides some basic test but one may design its own test. 

Examples may be found in :
- include/utk/metrics/Integrands/Heaviside.hpp, src/metrics/BuildHeavisideDatabase.cpp, src/metrics/HeavisideIntegrationTest.hpp 
- include/utk/metrics/Integrands/Gaussians.hpp, src/metrics/BuildGaussiansDatabase.cpp, src/metrics/GaussiansIntegrationTest.hpp

### Performing Integration Test

IntegrationTests have two main steps : creating a database and performing the test. First, 
a database of integrant is built. This is a caching mecanism allowing for reusing same integrands
with their integral already computated (this can be heavy if there is no close form and it should be approximated using MC methods with high sample count). Second, the database is read 
and the test is performed as usual. 

For each pointset tested against, an 'ErrorReport' is returned containing the following information : 

* min  : minimum error among all integrands in database
* max  : maximum error among all integrands in database
* mean : average error among all integrands in database
* var  : variance of error  among all integrands in database

### Creating Integrands

Integrands must derive from `utk::Integrand` and must implements the folowing methods : 

- Constructor: Takes as parameter the dimension. 
- GenerateRandom: The method takes a map of string to int, double or string and a seed. First argument is here to 
  guide generation (for exemple "scaling" of a function). The second is for reproducibility. 
- ReadFromStream: Reads parameters defining a function. 
- WriteToStream: Writes parameters defining a function. 
- Eval: Evaluate the function at a given point.

Additionnal method may also be derived controlling the computation of ground truth:

- hasCloseForm: Defaults return false.
- getIntegral: Will only be used if `hasCloseForm() == true`. 

When `hasCloseForm() == false`, the ground truth will be computed using QMC samples (likely from Sobol' + Owen).

## Files

```
include/utk/metrics/IntegrationTest.hpp

include/utk/metrics/Integrands/Heaviside.hpp
src/metrics/BuildHeavisideDatabase.cpp
src/metrics/HeavisideIntegrationTest.hpp 

include/utk/metrics/Integrands/Gaussians.hpp
src/metrics/BuildGaussiansDatabase.cpp
src/metrics/GaussiansIntegrationTest.hpp

include/utk/metrics/Integrands/BlinnPhong.hpp
src/metrics/BuildBlinnPhongDatabase.cpp
src/metrics/BlinnPhongIntegrationTest.hpp
```

## Usage (For gaussians)

<button class="tablink exebutton" onclick="openCode('exe', this)" markdown="1">Exe</button> 
<button class="tablink cppbutton" onclick="openCode('cpp', this)" markdown="1">C++</button> 
<button class="tablink pybutton" onclick="openCode('py', this)" markdown="1">Python</button> 
<br/>
  

<div class="exe tabcontent">

```bash
Integrand database builder
Usage: ./Integration/BuildGaussianIntegrandDatabase [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -o,--output TEXT REQUIRED   Output file
  -d,--dimension UINT REQUIRED
                              Dimension
  -n,--npts UINT REQUIRED     Number of points for integral computation.
  -m UINT REQUIRED            Number of integrands to compute
  -s UINT [0]                 Seed (0 means random)
  --sigmamin FLOAT [0.01]     Min value for variance
  --sigmamax FLOAT [1]        Max value for variance
  --silent                    Silence UTK logs

Gaussian Integration calculator
Usage: ./Integration/GaussianIntegrationTest [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE ... REQUIRED
                              Input file(s)
  -o,--output TEXT            Output file (empty is stdout)
  -f,--file,--integrands TEXT REQUIRED
                              Integrand File
  --silent                    Silence UTK logs
```

</div>

<div class="cpp tabcontent">

```  cpp
#include <utk/utils/PointsetIO.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/samplers/SamplerWhitenoise.hpp>
#include <utk/samplers/SamplerSobol.hpp>

#include <utk/metrics/IntegrationTest.hpp>
#include <utk/metrics/Integrands/GaussianIntegrands.hpp>


int main()
{
    utk::Pointset<double> ptsGT;
    utk::Pointset<double> ptsWN;

    utk::SamplerSobol sobol(2);
    sobol.generateSamples(ptsGT, 16384); // For integration 
    
    utk::SamplerWhitenoise wn(2);
    wn.generateSamples(ptsWN, 1024);

    utk::IntegrationTest test;

    // Build database
    test.BuildDatabase<utk::GaussianIntegrand>(
      "gaussians.db", // Output file
      2,              // Dimension 
      128,            // Number of integrands  
      0,              // seed   
      {{"smin", 0.01}, {"smax", 1.0}}, // Parameters (variance min and max)
      ptsGT           // Points to compute ground truth
    );

    // (Note: if build is first called, read is not necessary, but it may require heavy computation)
    // Perform test
    // Reads the Database
    test.ReadDatabase<utk::GaussianIntegrand>(
      "gaussians.db", // File to read
      2               // Dimension
    );

    // Compute the statistics
    auto report = test.compute(ptsWN);
    ostream << report.min << " " << report.mean << " " << report.var << " " << report.max << '\n';
}
```  

</div>

<div class="py tabcontent">

``` python
import pyutk 
import numpy as np

ptsGT = pyutk.Sobol(d=2, depth=32).sample(16384)

test = pyutk.IntegrationTest()

test.BuildGaussianDatabase(
  "gaussians.db",              # Output file
  2,                           # Dimension
  128,                         # Number of Integrands
  0,                           # Seed
  {"smin": 0.01, "smax": 1.0}, # Parameters (variance min and max)
  ptsGT                        # POints to compute ground truth
)

# Perform test :
test.ReadGaussianDatabse(
  "gaussians.db", # File to read
  2               # Dimension
)

report = test.compute(np.random.uniform(0, 1, (1024, 2)))
print(f"{report.min}, {report.mean}, {report.var}, {report.max}")
```  

</div>

## License

BSD