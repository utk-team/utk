# Using UTK Executables

## Linking and Building

To build executable, the following commands are necessary : 

```
mkdir build
cd build
# Install to specified location, every executable will be located in the 
# YOUR_INSTALL_DIR/bin folder. 
cmake -DCMAKE_INSTALL_PREFIX=. .. 
make install -j 8 # Or whatever number of threads you want
```

Executable will be located in `src/samplers` and `src/metrics folders` and, if 
`make install` is run, to the specified directory (without distinction). 

## File formats

Currently, two differents file formats are supported : text and binary. 

* Text files: No header. Each line represents a point, and coordinates are separated by spaces. 
Pointsets are separated by '#'.
* Binary files: The first 20 bytes are the header: 4 bytes for the magic number, 4 bytes 
for the meta data, 4 bytes for the number of pointsets, 4 bytes for the number of points in each 
pointset and 4 bytes for the number of dimensions. After the header, data is stored consecutively. 

Executables identifies file formats by their extension: ".bin" for binary and everything else will
be treated as text files.

### Details for binary file format:

* Magic number is : 0x214B5455 ("UTK!" in binary)
* Metadata is composed of 4 bytes : 0: data type code, 1: unused, 2: unused, 3: unused
* Data type codes are : 0: int, 1: long int, 2: uint, 3: long uint, 4: float, 5: double  

## Miscellaneous notes

* All samplers have common parameters `-n` for number of points, `-d` for dimension, 
`-m` for numbers of point sets to generate and `-s` for seeds. Note that some option 
might be ignored for samples that have fixed dimension (2D only) or are deterministic
(Sobol' without scrambling). 

* All metrics have common parameters `-i` for the input file or `-o` for output. The 
behaviour when multiples point sets are given is the same file might differ : `Spectrum` 
will return an average of all point sets but `Discrepancy` a list of values for each one. 

* UTK uses common names for most samplers and metrics. If that conflicts with other libraries,
one can use `cmake -DUTK_EXE_PREFIX=utk_ ..` to add prefix to executable names. 

* Silent option is not total : First, only utk logs are silenced. If the sampler or metrics
depends on an external code, the output are not silenced... Still, there is one log that is
not silenced : the failure of a sampler. 