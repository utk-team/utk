# File format .ps


## Files

```
src/io/imageIO.hpp  
src/io/imageIO_model.hpp  
src/io/imageIO_ps.hpp
```

## Description


UnicornTK relies on the extension of the input file to determine its type. When the extension is `.ps`, the pointset is drawn into a postscript file.

## Examples


Input pointset | Output .ps file | Image
---------------|-----------------|----
[sobol_128.edat](../data/graphics/sobol_128.edat) | [sobol_128.ps](../data/graphics/sobol_128.ps) | [![](../data/graphics/sobol_128_ps.png)](../data/graphics/sobol_128_ps.png)
[stratified\_25.dat](../data/graphics/stratified_25.dat)| [stratified\_25.ps](../data/graphics/stratified_25.ps)| [![](../data/graphics/stratified_25_ps.png)](../data/graphics/stratified_25_ps.png)
[toric\_bnot\_50.dat](../data/graphics/toric_bnot_50.dat)| [toric\_bnot\_50.ps](../data/graphics/toric_bnot_50.ps)|[![](../data/graphics/toric_bnot_50_ps.png)](../data/graphics/toric_bnot_50_ps.png)
