/*
 * Adrien Hamers
 * David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
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
    void ProgressiveMultiJittered02Algorithm2D(int numberOfSamplesToGenerate,
                                               int numberOfCandidates = 10);
    void exportSampleSet(std::string &outputPath);

private:
    std::mt19937 randomNumberGenerator;
    int numberOfSamplesToGenerate;
    SamplePMJ* generatedSamples;
    bool** occupiedStrata;
    int* xhalves;
    int* yhalves;
    int numSamples;
    int numberOfCandidates;

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
