# Uni{corn|form} Tool Kit

![](data/logo_utk2.jpg) The UTK tool kit aims at providing executables to generate and analyze point sets in unit domains $[0,1)^s$. It is originally meant to help researchers developing sampling patterns in a numerical integration using Monte Carlo estimators. More precisely, it was developed with the precise question of optimizing image synthesis via Path tracing algorithms.

UTK is a C++ library that implements a large variety of samplers and tools to analyze and compare them (discrepancy evaluation, spectral analysis, numerical integration tests...).

## License

The core of the library is available under the [BSD](license) license. For some samplers, the library is just a wrapper on some codes with specific license (see the [samplers](samplers) page for details).


## Clone and Build


UTK is hosted on a git repository. To install it, the first step is to clone the repository on your computer. This can be done using the following command line:

    git clone https://github.com/utk-team/utk.git

The code can be build on a Linux or a Mac environment. The Windows build is not yet supported. To build it, one can use the following commands, assuming the current working directory is the cloned utk repository.

```
mkdir build
cd build
cmake ..
make
```


## External libraries


The UTK repository provides the files for two external libraries:

*   [CImg](http://cimg.eu)(for image processing)
*   [dCpp](https://zigasajovic.github.io/dCpp/) (for automatic differentiation).

Some specific tools may require additional dependencies:

*   [CGAL](http://cgal.org) (to generate analytical shapes arrangements)
*   [FFTW3](http://fftw.org) (to compute Fourier spectra)

Finally, it also strongly relies on the Gnuplot framework to generate graphs.

Note that none of those libraries are mandatory. When building the UTK framework, the external dependancies can be either activated or deactivated. Each tool of this framework is compiled independently. If a dependency is marked as unrequired, all the tools that do not require this dependency are still compiled.

Originally, only CImg and dCpp are activated, since they are very light libraries and are provided with the framework.

To activate or deactivate a library, one can either edit the file `utk/CMakeLists.txt` to set to true/false the variables `[LIBRARY]_ON`. Alternatively, you can pass some options at the cmake step (or use `ccmake` or `cmake-gui`), e.g.:

    cmake .. -DUSE_FFTW=true

## Authors


The UTK library was developed mainly by

*   Hélène Perrier [helene.perrier@liris.cnrs.fr](mailto:helene.perrier@liris.cnrs.fr)
*   David Coeurjolly [david.coeurjolly@liris.cnrs.fr](mailto:david.coeurjolly@liris.cnrs.fr)

But was also contributed to by

*   Victor Ostromoukhov [victor.ostromoukhov@liris.cnrs.fr](mailto:victor.ostromoukhov@liris.cnrs.fr)
*   Jean David Genevaux

If you want to refer to UTK in your publications, please use the following bibtex entry:

```
@Misc{UTK,
title = {Uni(corn|form) tool kit},
note = {url{https://utk-team.github.io/utk/}}
}
```
## Contributing

UTK is an open-source, community driven, project. If you want to help us (typos, documentations issues, new samplers, new tools...), feel free to create a Pull-Request on the Github project.
