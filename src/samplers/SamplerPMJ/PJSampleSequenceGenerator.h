//
// Created by p1703185 on 09/04/19.
//

#ifndef PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PJSAMPLESEQUENCEGENERATOR_H
#define PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PJSAMPLESEQUENCEGENERATOR_H

#include <math.h>
#include <fstream>
#include <iostream>
#include <random>
#include "SamplePMJ.h"

/** Generates sample sequences using the Progressive Jittered algorithm
 *
 */
class PJSampleSequenceGenerator {
public:
    /**Constructor of the class
     *
     * @param seed the seed of the random double generator (uses mt19937)
     */
    PJSampleSequenceGenerator(int seed = 42);
    ~PJSampleSequenceGenerator();

    /** Using this method fills the generatedSample array using the progressive jittered algorithm
     *
     * @param numberOfSamplesToGenerate The number of samples wanted.
     */
    void progressiveJitteredAlgorithm2D(int numberOfSamplesToGenerate);
    /**
     * Creates a file at outputPath containing the coordinates of the generated samples.
     * @param outputPath The path of the created file.
     */
    void exportSampleSet(std::string &outputPath);
private:
    std::mt19937 randomNumberGenerator;
    SamplePMJ* generatedSamples;
    int numberOfSamplesToGenerate;

    /**
     * For each already generated sample, creates 3 new samples that satisfy the ProgressiveJittered constraints.
     * @param currentlyGeneratedSamples The number of samples already generated.
     */
    void PJExtendSequence(int currentlyGeneratedSamples);
    SamplePMJ PJGenerateSamplePoint(double i, double j, double xhalf, double yhalf, double n);
    SamplePMJ* instanciateArray(int size);
    double generateRandomDouble();
};


#endif //PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PJSAMPLESEQUENCEGENERATOR_H
