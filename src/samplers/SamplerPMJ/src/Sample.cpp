//
// Created by p1703185 on 09/04/19.
//

#include "Sample.h"

Sample::Sample(int dimension) : dimension(dimension) {
    coordinates = new double[dimension];
}

double& Sample::operator[](int index)
{
    return coordinates[index];
}

void Sample::deleteSample() {
    delete[] coordinates;
}

std::ostream &operator<<(std::ostream &os, const Sample &sample) {
    os << "(";
    for(int i = 0; i < sample.dimension-1; i++)
        os << sample.coordinates[i] << " ; ";
    os << sample.coordinates[sample.dimension-1] << ")";
    return os;
}

