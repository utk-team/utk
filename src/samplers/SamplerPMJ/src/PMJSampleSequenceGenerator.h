//
// Created by p1703185 on 10/04/19.
//

#ifndef UNTITLED1_PMJSAMPLESEQUENCEGENERATOR_H
#define UNTITLED1_PMJSAMPLESEQUENCEGENERATOR_H

#include <random>
#include <fstream>
#include <iostream>
#include "Sample.h"

class PMJSampleSequenceGenerator {
public:
    PMJSampleSequenceGenerator(int seed = 42);
    ~PMJSampleSequenceGenerator();
    void ProgressiveMultiJitteredAlgorithm2D(int numberOfSamplesToGenerate, int numberOfCandidates = 10);
    void exportSampleSet(std::string outputPath);

private:
    std::mt19937 randomNumberGenerator;
    int numberOfSamplesToGenerate;
    Sample* generatedSamples;
    bool* occupied1Dx;
    bool* occupied1Dy;
    int* xhalves;
    int* yhalves;
    int numSamples;
    int numberOfCandidates;

    void extendSequenceEven(int alreadyGeneratedSamples);
    void extendSequenceOdd(int alreadyGeneratedSamples);
    void markOccupiedStrata(int alreadyGeneratedSamples);
    void generateSamplePoint(int i, int j, int xhalf, int yhalf, double n, int N);
    double minDist(Sample reference);
    Sample* instantiateArray(int size);
    double generateRandomDouble();
};


#endif //UNTITLED1_PMJSAMPLESEQUENCEGENERATOR_H
