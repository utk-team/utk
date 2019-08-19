# Integration graph

## Files

    src/scripts/generate_integration_graph_onfly.sh

## Description

This test executes a given sampler and integration test to generate a graph of variance and MSE in integration. This tests outputs the integration data, but it can also output a png graph using gnuplot. Note that the generated graph also plots the theoretical worst case 2D variance in integration for low discrepancy samplers and a pure random sampler. If plotting integration for higher dimensional samplers or for specific integrands, those references won't be pertinent.

## Execution

```
 Parameters:  

[Help]
-s | --sampler the sampler executable
WILL BE RUN AS ./sampler_exe -o [ouput file] -n [nb_points]
-i | --integrator the executable computing the integration
WILL BE RUN AS ./integration_exe -i [input pointset] -o [ouput file]
--min the min value of the resulting graph
--max the max value of the resulting graph
--step the step between tho points of the graph (regular stepping)
--log 			 if set, the min value will be 2^min, the max 2^max and the step 2^step
--silent 		 set the script to silent mode
--rewrite 		 if set, if a graph with the same name already exitsts, the results will be appened to it
--gnuplot		 if set, gnuplot will be called to generate a simple plot of the final graph
--sort 			 if set, the resulting graph will be sorted automatically
--sname 		 sets the sampler name. If not given, this name will be deduced from the sampler executable
```

## Results

     ../src/scripts/generate_integration_graph_onfly.sh -s "./src/samplers/Whitenoise_2dd -m 100"  -i "./src/integration/IntegrationNSphere_fromfile_2dd" --min 4 --max 18 --gnuplot --sort --step 0.5

File  
[pointsets_Whitenoise_2dd-m100_integration_graph_IntegrationNSphere_fromfile_2dd_11356.dat](data/integration_graph/pointsets_Whitenoise_2dd-m100_integration_graph_IntegrationNSphere_fromfile_2dd_11356.dat)

Graph MSE  
[![](data/integration_graph/pointsets_Whitenoise_2dd-m100_mse_integration_graph_IntegrationNSphere_fromfile_2dd_11356.png)](data/integration_graph/pointsets_Whitenoise_2dd-m100_mse_integration_graph_IntegrationNSphere_fromfile_2dd_11356.png)

Graph Variance  
[![](data/integration_graph/pointsets_Whitenoise_2dd-m100_var_integration_graph_IntegrationNSphere_fromfile_2dd_11356.png)](data/integration_graph/pointsets_Whitenoise_2dd-m100_var_integration_graph_IntegrationNSphere_fromfile_2dd_11356.png)

     ../src/scripts/generate_integration_graph_onfly.sh -s "./src/samplers/SobolIndices_2dd"  -i "./src/integration/IntegrationImage2D_fromfile_2dd --image ../data/HDR/HDRimage1600x1600_000.exr" --min 4 --max 18 --gnuplot --sort --step 0.25

File  
[pointsets_SobolIndices_2dd_integration_graph_HDRimage1600x1600_000.exr_4342.dat](data/integration_graph/pointsets_SobolIndices_2dd_integration_graph_HDRimage1600x1600_000.exr_4342.dat)

Graph MSE  
[![](data/integration_graph/pointsets_SobolIndices_2dd_mse_integration_graph_HDRimage1600x1600_000.exr_4342.png)](data/integration_graph/pointsets_SobolIndices_2dd_mse_integration_graph_HDRimage1600x1600_000.exr_4342.png)

     ../src/scripts/generate_integration_graph_onfly.sh -s "./src/samplers/Stratified_4dd -m 20" -i "./src/integration/IntegrationNSphere_fromfile_4dd" --min 4 --max 18 --gnuplot --sort --step 0.25

File  
[pointsets_Stratified_4dd-m20_integration_graph_IntegrationNSphere_fromfile_4dd_4405.dat](data/integration_graph/pointsets_Stratified_4dd-m20_integration_graph_IntegrationNSphere_fromfile_4dd_4405.dat)

Graph MSE  
[![](data/integration_graph/pointsets_Stratified_4dd-m20_mse_integration_graph_IntegrationNSphere_fromfile_4dd_4405.png)](data/integration_graph/pointsets_Stratified_4dd-m20_mse_integration_graph_IntegrationNSphere_fromfile_4dd_4405.png)

Graph Variance  
[![](data/integration_graph/pointsets_Stratified_4dd-m20_var_integration_graph_IntegrationNSphere_fromfile_4dd_4405.png)](data/integration_graph/pointsets_Stratified_4dd-m20_var_integration_graph_IntegrationNSphere_fromfile_4dd_4405.png)
