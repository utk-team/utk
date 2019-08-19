# File format .ebin

## Files

```
src/io/fileIO.hpp  
src/io/fileIO_model.hpp  
src/io/fileIO_ebin.hpp
```

##  Description


UnicornTK relies on the extension of the input file to determine its type. When the extension is `.ebin`, the pointsets are stored in binary mode, with a header containing the extend of the pointset domain and the toricity of the pointset. It is much more efficient than ascii based format like .dat or `.edat`, and is usually less memory expensive.
