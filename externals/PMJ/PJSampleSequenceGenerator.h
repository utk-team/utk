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

    /**
     * Get pointer to samples
     */
    const SamplePMJ* GetSamples() const { return generatedSamples; }
    /**
     * Get number of generated samples
     */
    int GetSampleCount() const { return numberOfSamplesToGenerate; }

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
