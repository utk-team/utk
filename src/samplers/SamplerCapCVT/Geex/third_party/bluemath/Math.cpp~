/*
  Math.cpp

  Li-Yi Wei
  05/25/2008

*/

#include <math.h>

#include "Math.hpp"

const double Math::PI = 3.14159265;
const double Math::INF = 1.0/sin(0.0);
const double Math::NAN = 0.0/sin(0.0);

int Math::AllInteger(const vector<float> & input)
{
    int all_integer = 1;

    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(input[i] != floor(input[i])) all_integer = 0;
    }

    return all_integer;
}

int Math::GreatestCommonDivisor(const int a_in, const int b_in)
{
    const int a = max(a_in, b_in);
    const int b = min(a_in, b_in);

    if(b == 0) return a;
    else return GreatestCommonDivisor(b, a%b);
}

int Math::GreatestCommonDivisor(const vector<int> & input)
{
    if(input.size() <= 0) return 1;

    int gcd = input[0];

    for(unsigned int i = 1; i < input.size(); i++)
    {
        gcd = GreatestCommonDivisor(input[i], gcd);
    }

    return gcd;
}

float Math::MaxPackingDensity(const int dimension)
{
    float packing_density = -1;

    // from  http://mathworld.wolfram.com/HyperspherePacking.html
    
    switch(dimension)
    {
    case 1:
        packing_density = 1.0;
        break;

    case 2:
        packing_density = PI*sqrt(3.0)/6;
        break;
        
    case 3:
        packing_density = PI*sqrt(2.0)/6;
        break;
        
    case 4:
        packing_density = PI*PI/16;
        break;
        
    case 5:
        packing_density = PI*PI*sqrt(2.0)/30;
        break;
        
    case 6:
        packing_density = PI*PI*PI*sqrt(3.0)/144;
        break;
        
    case 7:
        packing_density = PI*PI*PI/105;
        break;
        
    case 8:
        packing_density = PI*PI*PI*PI/384;
        break;
        
    default:
        packing_density = -1;
    }

    return packing_density;
}
    
float Math::ComputeMaxMinDistance(const int dimension, const float num_samples)
{
    const float packing_density = MaxPackingDensity(dimension);
    
    const float answer = (packing_density > 0 ? 2*pow(packing_density/(PI*num_samples), 1.0/dimension) : -1);
    
    return answer;
}

int Math::ComputeMaxNumSamples(const int dimension, const float min_distance)
{ 
    const float packing_density = MaxPackingDensity(dimension);

    const int answer = (packing_density > 0 ? static_cast<int>(floor(packing_density/PI*pow(min_distance/2, -dimension))) : -1);

    return answer;
}

double Math::HyperSphereVolume(const int dimension, const float radius)
{
    if(dimension < 2)
    {
        return -1;
    }
    else
    {
        return pow(PI, 0.5*dimension)*pow(radius, dimension)/Gamma(dimension);
    }
}

double Math::HyperSphereRadius(const int dimension, const float volume)
{
    if(dimension < 2)
    {
        return -1;
    }
    else
    {
        return pow(volume*Gamma(dimension), 1.0/dimension)*pow(PI, -0.5);
    }
}

double Math::Gamma(const int n)
{
    if(n <= 0)
    {
        return -1;
    }
    else
    {
        if(n%2 == 0)
        {
            // even case
            return Factorial(n/2);
        }
        else
        {
            // odd case
            return sqrt(PI)*DoubleFactorial(n)/pow(2.0, (n+1)/2);
        }
    }
}
    
double Math::Factorial(const int n)
{
    double answer = 1;

    for(int i = 1; i <= n; i++)
    {
        answer *= i;
    }

    return answer;
}

double Math::DoubleFactorial(const int n)
{
    double answer = 1;

    for(int i = n; i > 0; i-=2)
    {
        answer *= i;
    }

    return answer;
}
