# UTK Examples

## Pointset Evaluation

Under the `Evaluation` directory, small programs running evaluation 
of most samplers provided with UTK. More specifically, it creates a
csv file containing integration error for gaussian / heaviside 
functions as well as Generalized Discrepancy values. 

Dimension, samples size can be controlled by modifying defaults
values of the `Config` class (in both C++ and Python version).

|name |N   |dim|metric|value                 |
|-----|----|---|------|----------------------|
|WN   |16  |2  |gl2   |0.15174487523539448   |
|WN   |16  |2  |gauss |0.0022725382131632355 |
|WN   |16  |2  |heavi |0.008519915227374     |
|WN   |256 |2  |gl2   |0.0468264181226269    |
|WN   |256 |2  |gauss |0.0002603261545549384 |
|WN   |256 |2  |heavi |0.0008560392083814568 |
|WN   |4096|2  |gl2   |0.01010079721546089   |
|WN   |4096|2  |gauss |1.010091707784436e-05 |
|WN   |4096|2  |heavi |3.710433769122564e-05 |
|Strat|16  |2  |gl2   |0.08587384765207495   |
|Strat|16  |2  |gauss |0.00032150732132570076|
|Strat|16  |2  |heavi |0.002430274168367333  |
|Strat|256 |2  |gl2   |0.011131451801241758  |
|Strat|256 |2  |gauss |1.400627403479704e-06 |
|Strat|256 |2  |heavi |3.790720704501276e-05 |
|Strat|4096|2  |gl2   |0.0013682774247619773 |
|Strat|4096|2  |gauss |5.211024300506443e-09 |
|Strat|4096|2  |heavi |5.980191025808278e-07 |

----------------------------------------------

Both version should produce similar results (up to seeding which is
not properly done). Code might be long to run depending on config
options (which should be set in the code directly). Parrallelization can be
controlled by setting `OMP_NUM_THREADS` environment variable. 

Data may be plotted with the `python plots.py path_to_csv` command. 

### Python version 

Requires libraries :

* pyutk 
* numpy
* pandas
* tqdm

Run code with command : `python evaluate.py` 

### Cpp version

Building is done via CMake and the following command lines :

```
mkdir build
cd build
cmake ..
make -j 4
./Evaluation/C++/Evaluate
```

## Samples

A small bash script that produces images of pointset and their respective 
spectrum. A few option are to be set directly in the code. 

It can be run with the following command `bash sample.sh`

This is the code that is used to generate documentation figures. 