//
// Created by p1703185 on 09/04/19.
//

#include "SamplePMJ.h"

SamplePMJ::SamplePMJ(int dimension) : dimension(dimension) {
    coordinates = new double[dimension];
}

double& SamplePMJ::operator[](int index)
{
    return coordinates[index];
}

void SamplePMJ::deleteSample() {
    delete[] coordinates;
}

std::ostream &operator<<(std::ostream &os, const SamplePMJ &sample) {
    os << "(";
    for(int i = 0; i < sample.dimension-1; i++)
        os << sample.coordinates[i] << " ; ";
    os << sample.coordinates[sample.dimension-1] << ")";
    return os;
}

