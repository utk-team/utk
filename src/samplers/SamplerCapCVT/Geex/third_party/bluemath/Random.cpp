/*
  Random.cpp

  Li-Yi Wei
  06/28/2007

*/

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Random.hpp"
#include "Math.hpp"
//#include "rand48.h"

// Li-Yi: this is not working yet.... 07/07/2007
#define _WIN32_USE_RAND48

#ifdef WIN32
#ifdef _WIN32_USE_RAND48
extern "C" {
void srand48(long seed);
double drand48(void);
}
#endif
#endif

void Random::InitRandomNumberGenerator(void)
{
#ifdef WIN32
#ifndef _WIN32_USE_RAND48
    srand(time(0));
    // srandom(time(0));
#else
    srand48(time(0));
#endif
#else
    srand48(time(0));
#endif
}

void Random::InitRandomNumberGenerator(const unsigned long seed)
{
#ifdef WIN32
#ifndef _WIN32_USE_RAND48
    srand(seed);
    // srandom(time(0));
#else
    srand48(seed);
#endif
#else
    srand48(seed);
#endif
}
    
double Random::UniformRandom(void)
{
#ifdef WIN32
#ifndef _WIN32_USE_RAND48
    return static_cast<double>(rand())/RAND_MAX;
    // return static_cast<double>(random())/0xffffffff; // 2^32-1
#else
    return drand48();
#endif
#else
    return drand48();
#endif
}
 
double Random::GaussianRandom(void)
{
    const double u1 = UniformRandom();
    const double u2 = UniformRandom();

    double n = 0;

    BoxMuller(u1, u2, n, n);

    return n;
}

void Random::BoxMuller(const double u1, const double u2, double & n1, double & n2)
{
    n1 = sqrt(-2 * log(u1)) * sin(2*Math::PI*u2);
    n2 = sqrt(-2 * log(u1)) * cos(2*Math::PI*u2);
}
