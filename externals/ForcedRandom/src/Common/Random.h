#ifndef RANDOM_H
#define RANDOM_H
#include <cmath>
#define MULTILINE(...) #__VA_ARGS__

class Random
{
public:
    Random(int seed);
    Random(int seed, int initialRuns);

    double NextDouble();
    int NextInt();
    int NextInt(int maxValue);
    int RandomizeSeed(int seed);

private:
    int ParkMillerPRNG(int seed, int runs);

    int mSeed;
};

#endif
