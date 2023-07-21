// Implementation of the Park-Miller Pseudo-Random Number Generator. For an explanation, see:
// Park, Steven K. and Miller, Keith W., Random Number Generators: Good Ones are Hard to Find,
// Communications of the ACM, volume 31, number 10, pp. 1192 - 1201, 1988.

#include "Random.h"

#define A 16807LL
#define M 2147483647LL
#define INTMAXD 2147483647.0
#define MAXVALUE 2147483648.0
#define INITIALRUNS 42

Random::Random(int seed)
{
    mSeed = ParkMillerPRNG(seed, INITIALRUNS);
}

Random::Random(int seed, int initialRuns)
{
    mSeed = ParkMillerPRNG(seed, initialRuns);
}

double Random::NextDouble()
{
    return static_cast<double>(this->NextInt()) / INTMAXD;
}

int Random::NextInt()
{
    mSeed = ParkMillerPRNG(mSeed, 1);

    return mSeed;
}

int Random::NextInt(int maxValue)
{
    mSeed = ParkMillerPRNG(mSeed, 1);

    return mSeed % maxValue;
}

int Random::ParkMillerPRNG(int seed, int runs)
{
    for (int run = 0; run < runs; run++)
    {
        seed = static_cast<int>(static_cast<int>(seed) * A % M);
    }

    return seed;
}

int Random::RandomizeSeed(int seed)
{
    return ParkMillerPRNG(seed, INITIALRUNS);
}
