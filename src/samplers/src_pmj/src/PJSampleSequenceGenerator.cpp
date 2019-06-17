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

void PJSampleSequenceGenerator::exportSampleSet(std::string outputPath) {
    std::ofstream outputStream(outputPath);
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
        Sample oldSample = generatedSamples[s];
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

Sample PJSampleSequenceGenerator::PJGenerateSamplePoint(double i, double j, double xhalf, double yhalf, double n) {
    Sample toReturn = Sample(2);
    toReturn[0] = (i+0.5*(xhalf + generateRandomDouble()))/n;
    toReturn[1] = (j+0.5*(yhalf + generateRandomDouble()))/n;
    return toReturn;
}

Sample *PJSampleSequenceGenerator::instanciateArray(int size) {
    Sample* toReturn = new Sample[size];
    for(int i = 0; i < size; i++)
        toReturn[i] = Sample(2);
    return toReturn;
}

double PJSampleSequenceGenerator::generateRandomDouble() {
    std::uniform_real_distribution<> uniform(0.0,1.0);
    return uniform(randomNumberGenerator);
}
