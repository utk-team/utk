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

## Miscellaneous notes

* All samplers have common parameters `-n` for number of points, `-d` for dimension, 
`-m` for numbers of point sets to generate and `-s` for seeds. Note that some option 
might be ignored for samples that have fixed dimension (2D only) or are deterministic
(Sobol' without scrambling). 

* All metrics have common parameters `-i` for the input file or `-o` for output. The 
behaviour when multiples point sets are given is the same file might differ : `Spectrum` 
will return an average of all point sets but `Discrepancy` a list of values for each one. 