# Using PyUTK library

## Building

A conda (or venv) environnment is recommended. There is no requirements for the pyutk
library to be built. But to be used, numpy must be available.

If using a conda environnment, make sure the proper compiler is detected. It is 
recommanded to install GCC via `conda install -c conda-force gxx`. If the wrong
compiler, setting CC and CXX environment variables might help. 

To build the library run the command `pip install -v -e .` (dot is part of command) in 
the root directory. Everything will be built and linked with your current python interpreter. 
A simple test command would be `python -c "import pyutk; print(help(pyutk))"` which shows all
samplers and functions availables. 

Most usage of library is given in the documentation with the python code snippets. A more 
elaborate example may be find in the `examples/` directory. 

## Miscellaneous Notes

* Samplers are not thread safes !

* Metrics uses parallel code. Number of thread might be controlled by setting `OMP_NUM_THREADS` 
this can be done by setting `os.environ['OMP_NUM_THREADS']`

* Documentation is not exhaustive. Use `help(pyutk)` to have access to available functions 

* Logs can be enabled and disabled with the functions `pyutk.disableLogs()`, `pyutk.enableLogs()`, 
`pyutk.setLogConsole()` (which set log destination to standard output) and `pyutk.setLogFile(filepath)` 
(which set log destination to specified file). 

* The binding is done with the awesome `pybind11` library. There is no code directly written
in python within the library, hence performance are almost the same.  

* The binding is built to be interoperable with the wonderfull `numpy` library. Hence there 
is no intermediate data representation (such as UTKPointset or equivalent). To be more
precised, the library returns numpy array, but accepts any object supporting [python buffer
protocol](https://docs.python.org/3/c-api/buffer.html) API.  

* The interface is a slighly modified version of CppUTK to be more pythonic. While CppUTK 
uses returns code to indicate error, PyUTK returns array and throws exception when something
is wrong. 

* Why is my favorite sampler missing in the binding ? : We might have forgot it... Request the
addition :) ! 