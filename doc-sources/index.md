# Uni{corn|form} Tool Kit

![](figs/logo_utk2.jpg) The UTK tool kit aims at providing executables to generate and analyze point sets in unit domains $[0,1)^s$. It is originally meant to help researchers developing sampling patterns in a numerical integration using Monte Carlo estimators. More precisely, it was developed with the precise question of optimizing image synthesis via Path tracing algorithms.

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

See `Using UTK/*` documentation pages for mor information and building options. 

## External libraries

Main external libraries are downloaded automatically by CMake ([spdlog](https://github.com/gabime/spdlog), [cli11](https://cliutils.github.io/CLI11/book/)). When the python binding is enabled (cmake flag `UTK_PYTHON` set to true), [pybind11](https://pybind11.readthedocs.io/en/stable/) is fetched.

If `UTK_USE_FFTW` cmake flag is set to true (the default setting) and if the fftw library is available on your system, the Fourier tools will use it.

Finally, if `UTK_USE_CGAL` is set to true, samplers with [CGAL](http://cgal.org) as dependency will be built. CGAL and SuiteSparse must be installed on your system.

## Authors

The UTK library was developed mainly by

*   David Coeurjolly [david.coeurjolly@liris.cnrs.fr](mailto:david.coeurjolly@liris.cnrs.fr)
*   Hélène Perrier [helene.perrier@liris.cnrs.fr](mailto:helene.perrier@liris.cnrs.fr)
*   Bastien Doignies [bastien.doignies@liris.cnrs.fr](mailto:bastien.doignies@liris.cnrs.fr)

But was also contributed to by

*   Victor Ostromoukhov [victor.ostromoukhov@liris.cnrs.fr](mailto:victor.ostromoukhov@liris.cnrs.fr)
*   Adrien Pilleboue
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
