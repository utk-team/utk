//
// Created by p1703185 on 11/04/19.
//

#ifndef PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PMJ02SAMPLESEQUENCEGENERATOR_H
#define PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PMJ02SAMPLESEQUENCEGENERATOR_H

#include <random>
#include <fstream>
#include <iostream>
#include "SamplePMJ.h"

class PMJ02SampleSequenceGenerator {
public:
    PMJ02SampleSequenceGenerator(int seed = 42);
    ~PMJ02SampleSequenceGenerator();
    void ProgressiveMultiJittered02Algorithm2D(int numberOfSamplesToGenerate);
    void exportSampleSet(std::string &outputPath);

private:
    std::mt19937 randomNumberGenerator;
    int numberOfSamplesToGenerate;
    SamplePMJ* generatedSamples;
    bool** occupiedStrata;
    int* xhalves;
    int* yhalves;
    int numSamples;

    void extendSequenceEven(int alreadyGeneratedSamples);
    void extendSequenceOdd(int alreadyGeneratedSamples);
    void markOccupiedStrata(int alreadyGeneratedSamples);
    void markOccupiedStrata1(SamplePMJ &pt, int NN);
    void generateSamplePoint(int i, int j, int xhalf, int yhalf, double n, int N);
    bool isOccupied(SamplePMJ &pt, int NN);
    double minDist(SamplePMJ &reference);
    SamplePMJ* instantiateArray(int size);
    double generateRandomDouble();
};;


#endif //PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PMJ02SAMPLESEQUENCEGENERATOR_H
