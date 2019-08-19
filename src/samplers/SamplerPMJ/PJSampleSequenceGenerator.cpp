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

#include "PJSampleSequenceGenerator.h"

PJSampleSequenceGenerator::PJSampleSequenceGenerator(int seed) {
    randomNumberGenerator = std::mt19937(seed);
}

PJSampleSequenceGenerator::~PJSampleSequenceGenerator() {
    for(int i = 0; i < numberOfSamplesToGenerate; i++)
        generatedSamples[i].deleteSample();
    delete[](generatedSamples);
}

void PJSampleSequenceGenerator::progressiveJitteredAlgorithm2D(int numberOfSamplesToGenerate) {
    this->numberOfSamplesToGenerate = numberOfSamplesToGenerate;
    int arraySize = 4*numberOfSamplesToGenerate;
    generatedSamples = instanciateArray(arraySize);
    generatedSamples[0][0] = generateRandomDouble();
    generatedSamples[0][1] = generateRandomDouble();
    int currentlyGeneratedSamples = 1;
    while(currentlyGeneratedSamples < numberOfSamplesToGenerate)
    {
        PJExtendSequence(currentlyGeneratedSamples);
        currentlyGeneratedSamples*=4;
    }
}

void PJSampleSequenceGenerator::exportSampleSet(std::string &outputPath) {
    std::ofstream outputStream(outputPath);
    outputStream.precision(16);
    if(outputStream) {
        for (int i = 0; i < numberOfSamplesToGenerate; i++) {
            outputStream << generatedSamples[i][0];
            outputStream << " ";
            outputStream << generatedSamples[i][1];
            outputStream << "\n";
        }
        outputStream.close();
    }
    else
        std::cout << "ERREUR d'ouverture de fichier" << std::endl;
}

void PJSampleSequenceGenerator::PJExtendSequence(int currentlyGeneratedSamples) {
    int n = std::sqrt(currentlyGeneratedSamples);
    for(int s = 0; s < currentlyGeneratedSamples; s++)
    {
        SamplePMJ oldSample = generatedSamples[s];
        unsigned int i = (int)(n*oldSample[0]);
        unsigned int j = (int)(n*oldSample[1]);
        double xhalf = (int)(2*(n*oldSample[0]-i));
        double yhalf = (int)(2*(n*oldSample[1]-j));
        xhalf = 1-xhalf;
        yhalf = 1-yhalf;
        generatedSamples[currentlyGeneratedSamples+s] = PJGenerateSamplePoint(i,j,xhalf,yhalf,n);
        if(generateRandomDouble() > 0.5)
            xhalf = 1-xhalf;
        else
            yhalf = 1- yhalf;
        generatedSamples[2*currentlyGeneratedSamples+s] = PJGenerateSamplePoint(i,j,xhalf,yhalf,n);
        xhalf = 1-xhalf;
        yhalf = 1-yhalf;
        generatedSamples[3*currentlyGeneratedSamples+s] = PJGenerateSamplePoint(i,j,xhalf,yhalf,n);
    }
}

SamplePMJ PJSampleSequenceGenerator::PJGenerateSamplePoint(double i, double j, double xhalf, double yhalf, double n) {
    SamplePMJ toReturn = SamplePMJ(2);
    toReturn[0] = (i+0.5*(xhalf + generateRandomDouble()))/n;
    toReturn[1] = (j+0.5*(yhalf + generateRandomDouble()))/n;
    return toReturn;
}

SamplePMJ *PJSampleSequenceGenerator::instanciateArray(int size) {
    SamplePMJ* toReturn = new SamplePMJ[size];
    for(int i = 0; i < size; i++)
        toReturn[i] = SamplePMJ(2);
    return toReturn;
}

double PJSampleSequenceGenerator::generateRandomDouble() {
    std::uniform_real_distribution<> uniform(0.0,1.0);
    return uniform(randomNumberGenerator);
}
