#  File format .bin

## Files

```
src/io/fileIO.hpp  
src/io/fileIO_model.hpp  
src/io/fileIO_bin.hpp
```

##Description

UnicornTK relies on the extension of the input file to determine its type. When the extension is `.bin`, the pointsets are stored in binary mode, without any header. It is much more efficient than ascii based format like .dat or `.edat`, and is usually less memory expensive.
