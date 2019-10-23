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

#include "PMJ02SampleSequenceGenerator.h"

PMJ02SampleSequenceGenerator::PMJ02SampleSequenceGenerator(int seed) {
    randomNumberGenerator = std::mt19937(seed);
}

PMJ02SampleSequenceGenerator::~PMJ02SampleSequenceGenerator() {
    for(int i = 0; i < numberOfSamplesToGenerate; i++)
        generatedSamples[i].deleteSample();
    delete[](generatedSamples);
    delete[](xhalves);
    delete[](yhalves);
    for(int i = 0; i < 4*numberOfSamplesToGenerate; i++)
        delete[](occupiedStrata[i]);
    delete[](occupiedStrata);
}

void PMJ02SampleSequenceGenerator::ProgressiveMultiJittered02Algorithm2D(int numberOfSamplesToGenerate,  
                                                                         int numberOfCandidates) {
    this->numberOfSamplesToGenerate = numberOfSamplesToGenerate;
    int arraySize = 4*numberOfSamplesToGenerate;
    generatedSamples = instantiateArray(arraySize);
    occupiedStrata = new bool*[4*numberOfSamplesToGenerate];
    for(int i = 0; i < 4*numberOfSamplesToGenerate;i++)
        occupiedStrata[i] = new bool[4*numberOfSamplesToGenerate];
    xhalves = new int[4*numberOfSamplesToGenerate];
    yhalves = new int[4*numberOfSamplesToGenerate];
    generatedSamples[0][0] = generateRandomDouble();
    generatedSamples[0][1] = generateRandomDouble();

    this->numberOfCandidates = numberOfCandidates;
    numSamples = 1;
    int currentlyGeneratedSamples = 1;
    while(currentlyGeneratedSamples < numberOfSamplesToGenerate)
    {
        extendSequenceEven(currentlyGeneratedSamples);
        extendSequenceOdd(currentlyGeneratedSamples*2);
        currentlyGeneratedSamples*=4;
    }
}

void PMJ02SampleSequenceGenerator::exportSampleSet(std::string &outputPath) {
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

void PMJ02SampleSequenceGenerator::extendSequenceEven(int alreadyGeneratedSamples) {
    double n = sqrt((double)alreadyGeneratedSamples);
    markOccupiedStrata(alreadyGeneratedSamples);
    for(int s = 0; s < alreadyGeneratedSamples; s++)
    {
        SamplePMJ oldSample = generatedSamples[s];
        int i = (int)(n*oldSample[0]);
        int j = (int)(n*oldSample[1]);
        unsigned int xhalf = (int)(2.0*(n*oldSample[0] - i));
        unsigned int yhalf = (int)(2.0*(n*oldSample[1] - j));
        xhalf = 1-xhalf;
        yhalf = 1-yhalf;
        generateSamplePoint(i,j,xhalf,yhalf,n,alreadyGeneratedSamples);
    }
}

void PMJ02SampleSequenceGenerator::extendSequenceOdd(int alreadyGeneratedSamples) {

    double n = sqrt((double)alreadyGeneratedSamples/2.0f);
    markOccupiedStrata(alreadyGeneratedSamples);
    for(int s  = 0; s < alreadyGeneratedSamples/2; s++)
    {
        SamplePMJ oldSample = generatedSamples[s];
        int i = (int)(n*oldSample[0]);
        int j = (int)(n*oldSample[1]);
        unsigned int xhalf = (int)(2.0*(n*oldSample[0] - i));
        unsigned int yhalf = (int)(2.0*(n*oldSample[1] - j));
        if(generateRandomDouble() > 0.5)
            xhalf = 1-xhalf;
        else
            yhalf = 1-yhalf;
        xhalves[s] = xhalf;
        yhalves[s] = yhalf;
        generateSamplePoint(i,j,xhalf,yhalf,n,alreadyGeneratedSamples);
    }
    for(int s = 0; s < alreadyGeneratedSamples/2; s++)
    {
        SamplePMJ oldSample = generatedSamples[s];
        int i = (int)(n*oldSample[0]);
        int j = (int)(n*oldSample[1]);
        int xhalf = 1-xhalves[s];
        int yhalf = 1-yhalves[s];
        generateSamplePoint(i,j,xhalf,yhalf,n,alreadyGeneratedSamples);
    }

}

void PMJ02SampleSequenceGenerator::markOccupiedStrata(int alreadyGeneratedSamples) {
    int NN = 2*alreadyGeneratedSamples;
    for(int i = 0; i <= log2(NN); i++)
        for(int j = 0; j < NN; j++)
            occupiedStrata[i][j] = false;

    for(int s = 0; s < alreadyGeneratedSamples; s++)
    {
        markOccupiedStrata1(generatedSamples[s], NN);
    }
}

void PMJ02SampleSequenceGenerator::markOccupiedStrata1(SamplePMJ &pt, int NN) {
    int shape = 0;
    int xdivs = NN;
    int ydivs = 1;
    do{
        int xstratum = (int)(xdivs * pt[0]);
        int ystratum = (int)(ydivs * pt[1]);
        occupiedStrata[shape][ystratum*xdivs+xstratum] = true;
        shape ++;
        xdivs /= 2;
        ydivs *= 2;
    }while(xdivs != 0);
}

void PMJ02SampleSequenceGenerator::generateSamplePoint(int i, int j, int xhalf, int yhalf, double n, int N) {
    int NN = 2*N;
    SamplePMJ pt = SamplePMJ();
    do{
        pt[0] = (i+0.5*(xhalf+generateRandomDouble()))/n;
        pt[1] = (j+0.5*(yhalf+generateRandomDouble()))/n;
    }while(isOccupied(pt, NN));
    markOccupiedStrata1(pt, NN);
    generatedSamples[numSamples] = pt;
    numSamples++;
}

bool PMJ02SampleSequenceGenerator::isOccupied(SamplePMJ &pt, int NN) {
    int shape = 0;
    int xdivs = NN;
    int ydivs = 1;
    do{
        int xstratum = (int)(xdivs * pt[0]);
        int ystratum = (int)(ydivs * pt[1]);
        if(occupiedStrata[shape][ystratum*xdivs+xstratum])
            return true;
        shape++;
        xdivs/=2;
        ydivs*=2;
    }while(xdivs != 0);
    return false;
}

double PMJ02SampleSequenceGenerator::minDist(SamplePMJ& reference) {
    double minSquareDist = pow(reference[0] - generatedSamples[0][0],2) + pow(reference[0] - generatedSamples[0][1],2);
    for(int i = 1; i < numSamples; i++)
    {
        double currentSquareDist = pow(reference[0] - generatedSamples[i][0],2) + pow(reference[0] - generatedSamples[i][1],2);
        if(currentSquareDist < minSquareDist)
        {
            minSquareDist = currentSquareDist;
        }
    }
    return sqrt(minSquareDist);
}

SamplePMJ *PMJ02SampleSequenceGenerator::instantiateArray(int size) {
    SamplePMJ* toReturn  = new SamplePMJ  [size];
    for(int i = 0; i < size; i++)
        toReturn[i] = SamplePMJ(2);
    return toReturn;
}

double PMJ02SampleSequenceGenerator::generateRandomDouble() {
    std::uniform_real_distribution<> uniform(0.0,1.0);
    return uniform(randomNumberGenerator);
}
