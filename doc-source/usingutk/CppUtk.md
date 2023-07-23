# Using UTK C++ library

## Linking and Building

Most of UTK is header only. Hence, setting include path to `include/` is sufficient. However, some code provided by authors are not header only and should be compiled along your project. 

If building with CMake, you may use the following lines to get all files required: 

```cmake
find_package(UTK)
# Depending on install location, the following line may be better
# find_package(UTK PATHS PATH_TO_UTK)
target_link_libraries(YOUR_TARGET UTK_LIBRARY)
```

See the documentation and `examples/` folder for more information. 

## Using UTK Library with externals Point formats

Points and pointsets have many different formats. UTK uses it's own, but have been designed to be integrated with many other.
Internally, the `utk::Poinset` is a `std::shared_ptr`. 
The class can be constructed in the following ways : 

* `utk::Poinset<Type>(uint n, uint d)` : Construct a pointset. The memory will be owned by the object. Copies will share the same memory.
* `utk::Poinset<Type>::View(T* data, uint n, uint d)` : Creates a wrapper around existing memory. The memory will not be deleted automatically, it is to the user to delete it properly. 
* `utk::Poinset<Type>::Copy(const T* data, uint n, uint d)` : Copies the data. The memory will be owned by the returning object. 

Only C-style linear buffers are supported.

**Important note** : Points might be stored in particular structures such as `struct Point{ float x, y; };` and arrays are built
upon such as : `std::vector<Point>`. *While technically undefined behavior (but so many codes depends on it, so it may be considered safe)*, it is possible to obtain an `utk::Pointset<float>` with the line `utk::Pointset<float>::View(static_cast<float*>(vector.data()), N, D)`; but be sure that your data are properly packed and aligned. In particular, make sure that there is no other attributes. For instance : `struct Point{ float* point; unsigned int N};` and `struct { std::vector<float> point; };` are not valid and copies will be necessary !

## Miscellaneous notes :

**How to control parallelism ?**: UTK parallelism is handled with the OpenMP library. Most sampler use only
a single thread (/!\ that does not mean they are thread-safe !!!) but metrics are multi-threaded. There are two main ways to control the number of threads :
using the openmp library directly in the code `omp_set_num_threads` or setting the `OMP_NUM_THREADS` environment variable. By defaults, all available threads are used. 

**How to control logging ?**: The library might be built without support for logging at all (with the `UTK_LOG` CMake option). This is discouraged though. The logger can be partially interacted with, using the following maccros : `UTK_LOG_DISABLE()`,  `UTK_LOG_ENABLE()`, `UTK_LOG_CONSOLE()` (which set log destination to standard output) and `UTK_LOG_FILE(filepath)` to log directly into a file. 

**Why is each Sampler / Metric a separate class without any base class ?** : Short answer : so that code can be extracted more easily. While UTK is a library in itself, it may serve as a sampler code repository. Technically, it is possible to have maccros that enables class dependancy on a common class and while it might be added latter, it is believed that it would add too much complexity for now...

**How to make a function that takes a sampler as parameter?**: Templates ! To run the same function
on multiple templates, consider variadic templates and parameter pack expansion (see examples !).
While not having a common base class, every sampler have the same generation method !