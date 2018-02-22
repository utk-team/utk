#ifndef _UTIL_H_
#define _UTIL_H_

#include <cmath>
#include <vector>
#include <string>

template <class T>
T compute_mean(const std::vector<T>& data)
{
    T mean = 0.0;
    for (unsigned i = 0; i < data.size(); ++i)
        mean += data[i];
    return (mean / data.size());
}

template <class T>
void remove_mean(std::vector<T>& data)
{
    T mean = compute_mean(data);
    for (unsigned i = 0; i < data.size(); ++i)
        data[i] -= mean;
}

template <class T>
double compute_norm(std::vector<T>& data)
{
    double norm2 = 0.0;
    for (unsigned i = 0; i < data.size(); ++i)
        norm2 += data[i]*data[i];
    return std::sqrt(norm2);
}

template <class T>
double compute_dot_prod(const std::vector<T>& data1, const std::vector<T>& data2)
{
    double dot_prod = 0.0;
    for (unsigned i = 0; i < data1.size(); ++i)
        dot_prod += data1[i]*data2[i];
    return dot_prod;
}

#endif
