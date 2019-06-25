//
// Created by p1703185 on 09/04/19.
//

#ifndef SAMPLE_H
#define SAMPLE_H

#include <ostream>

class SamplePMJ {
private:
    int dimension;
    double* coordinates;
public:
    SamplePMJ(int dimension = 2);
    double& operator[](int index);
    void deleteSample();

    friend std::ostream &operator<<(std::ostream &os, const SamplePMJ &sample);
};


#endif //SAMPLE_H
