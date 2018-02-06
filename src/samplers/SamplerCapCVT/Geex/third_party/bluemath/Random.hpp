/*
  Random.hpp

  Li-Yi Wei
  06/28/2007

*/

#ifndef _RANDOM_HPP
#define _RANDOM_HPP

#include <Geex/third_party/misc/linkage.h>

class GEEX_API Random
{
public:
    static void InitRandomNumberGenerator(void);
    static void InitRandomNumberGenerator(const unsigned long seed);
    
    // a uniform random number between [0 1]
    static double UniformRandom(void);

    // a Gaussian with 0 mean and 1 variance
    static double GaussianRandom(void);

    // Box-Muller transform
    static void BoxMuller(const double u1, const double u2, double & n1, double & n2); 
};

#include <vector>
using namespace std;
#include <math.h>

template<class T>
void Randomize(vector<T> & inout)
{
    vector<T> input(inout);
    vector<T> output;

    while(input.size() > 0)
    {
        const int selection = static_cast<int>(floor(Random::UniformRandom()*input.size()))%input.size();

        output.push_back(input[selection]);
        input[selection] = input[input.size()-1];
        input.pop_back();
    }

    inout = output;
};

#endif
