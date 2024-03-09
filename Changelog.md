# CHANGELOG.md

### 2.0.1

- 2024 Feb (David Coeurjolly)
    - cmake CPM instead of `FETCH_CONTENT` for dependencies 
- 2024 Fev (Bastien DOIGNIES):
    - Binary file format support
    - Updated documentation

- 2024 Janv (Bastien DOIGNIES):
    - Cascaded Sobol' sampler
    - Sampler from Generative Matrix (arbitrary base + Owen)
    - Sliced Optimal Transport Sampling
    - Fix GaussianIntegrands to generate proper SPD matrix.

- 2023 Oct: 
    - Adding a new option to Sobol' (c++) to load an alternative to [JK03] init table (David Coeurjolly, 2023-Oct).

## 2.0.0 

- 2023-Jul 2.0 released : 
    - Code refactoring
    - Dimension templating removed
    - Automatic dependancy handling
    - CMake find_package
    - Python Binding

## 1.0.0

* 2022-Mar: Adding the reference for the Kronecker K21-2 sampler
* 2021-Dec: Adding Kronecker K21-2 and R2 sample generators
* 2021-Aug: Adding code and documentation for Diaphony
* 2020-Nov: Cleanup in the Generalized L2 Discrepancy code
* 2020-Jan: Bugfix FFTW_INCLUDES headers
* 2020-Jan: More Jittered samplers
* 2019-Nov: Adding Matt Pharr's variant of PMJ02 (faster)
* 2019-Sep: Adding Lloyd's relaxation sampler in 3D (toroidal and non-toroidal) domains using Geogram
* 2019-Aug: Global refactoring of the website
* 2019-Jun: Adding Progressive MultiJittered Samplers (PM,PMJ, PMJ02)
