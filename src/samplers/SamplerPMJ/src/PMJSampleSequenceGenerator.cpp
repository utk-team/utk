//
// Created by p1703185 on 10/04/19.
//

#include "PMJSampleSequenceGenerator.h"

PMJSampleSequenceGenerator::PMJSampleSequenceGenerator(int seed) {
    randomNumberGenerator = std::mt19937(seed);
}

PMJSampleSequenceGenerator::~PMJSampleSequenceGenerator() {
    delete[](occupied1Dx);
    delete[](occupied1Dy);
    delete[](xhalves);
    delete[](yhalves);
    for(int i = 0; i < numberOfSamplesToGenerate; i++)
        generatedSamples[i].deleteSample();
    delete[](generatedSamples);
}

void PMJSampleSequenceGenerator::ProgressiveMultiJitteredAlgorithm2D(int numberOfSamplesToGenerate, int numberOfCandidates) {
    this->numberOfSamplesToGenerate = numberOfSamplesToGenerate;
    int arraySize = 4*numberOfSamplesToGenerate;
    generatedSamples = instantiateArray(arraySize);
    occupied1Dx = new bool[4*numberOfSamplesToGenerate];
    occupied1Dy = new bool[4*numberOfSamplesToGenerate];
    xhalves = new int[4*numberOfSamplesToGenerate];
    yhalves = new int[4*numberOfSamplesToGenerate];
    generatedSamples[0][0] = generateRandomDouble();
    generatedSamples[0][1] = generateRandomDouble();
    numSamples = 1;
    this->numberOfCandidates = numberOfCandidates;
    int currentlyGeneratedSamples = 1;
    while(currentlyGeneratedSamples < numberOfSamplesToGenerate)
    {
        extendSequenceEven(currentlyGeneratedSamples);
        extendSequenceOdd(currentlyGeneratedSamples*2);
        currentlyGeneratedSamples*=4;
    }
}

void PMJSampleSequenceGenerator::exportSampleSet(std::string outputPath) {
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

void PMJSampleSequenceGenerator::extendSequenceEven(int alreadyGeneratedSamples) {
    double n = sqrt((double)alreadyGeneratedSamples);
    markOccupiedStrata(alreadyGeneratedSamples);
    for(int s = 0; s < alreadyGeneratedSamples; s++)
    {
        Sample oldSample = generatedSamples[s];
        int i = (int)(n*oldSample[0]);
        int j = (int)(n*oldSample[1]);
        unsigned int xhalf = (int)(2.0*(n*oldSample[0] - i));
        unsigned int yhalf = (int)(2.0*(n*oldSample[1] - j));
        xhalf = 1-xhalf;
        yhalf = 1-yhalf;
        generateSamplePoint(i,j,xhalf,yhalf,n,alreadyGeneratedSamples);
    }
}

void PMJSampleSequenceGenerator::extendSequenceOdd(int alreadyGeneratedSamples) {

    double n = sqrt((double)alreadyGeneratedSamples/2.0f);
    markOccupiedStrata(alreadyGeneratedSamples);
    for(int s  = 0; s < alreadyGeneratedSamples/2; s++)
    {
        Sample oldSample = generatedSamples[s];
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
        Sample oldSample = generatedSamples[s];
        int i = (int)(n*oldSample[0]);
        int j = (int)(n*oldSample[1]);
        int xhalf = 1-xhalves[s];
        int yhalf = 1-yhalves[s];
        generateSamplePoint(i,j,xhalf,yhalf,n,alreadyGeneratedSamples);
    }

}

void PMJSampleSequenceGenerator::markOccupiedStrata(int alreadyGeneratedSamples) {
    int NN = 2*alreadyGeneratedSamples;
    for(int i = 0; i < NN; i++) {
        occupied1Dx[i] = false;
        occupied1Dy[i] = false;
    }

    for(int s = 0; s < alreadyGeneratedSamples; s++)
    {
        int xstratum = (int)(NN*generatedSamples[s][0]);
        int ystratum = (int)(NN*generatedSamples[s][1]);
        occupied1Dx[xstratum] = true;
        occupied1Dy[ystratum] = true;
    }
}

void PMJSampleSequenceGenerator::generateSamplePoint(int i, int j, int xhalf, int yhalf, double n, int N) {
    int NN = 2*N;
    double bestDist = 0.0;
    Sample pt;
    int xstratum;
    int ystratum;
    for(int t = 1; t <= numberOfCandidates; t++ )
    {
        Sample candpt = Sample(2);

        do
        {
            candpt[0] = (i+0.5*(xhalf + generateRandomDouble()))/n;
            xstratum = (int)(NN*candpt[0]);
        }
        while(occupied1Dx[xstratum]);
        do
        {
            candpt[1] = (j+0.5*(yhalf + generateRandomDouble()))/n; //pour le 15e sample, devrait generer entre 0.1875 et 0.25 mais genere entre 0.25 et 0.5
            ystratum = (int)(NN*candpt[1]);
        }
        while(occupied1Dy[ystratum]);
        double d = minDist(candpt);
        if(d>bestDist)
        {
            bestDist = d;
            pt = candpt;
        }
    }
    xstratum = (int)(NN*pt[0]);
    ystratum = (int)(NN*pt[1]);
    occupied1Dx[xstratum] = true;
    occupied1Dy[ystratum] = true;
    generatedSamples[numSamples] = pt;
    numSamples++;
}

/** classical euclidian minimum distance between reference and all other samples
 *
 * @param reference
 * @return the minimal distance found
 */
double PMJSampleSequenceGenerator::minDist(Sample reference) {
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

Sample *PMJSampleSequenceGenerator::instantiateArray(int size) {
    Sample* toReturn  = new Sample[size];
    for(int i = 0; i < size; i++)
        toReturn[i] = Sample(2);
    return toReturn;
}

double PMJSampleSequenceGenerator::generateRandomDouble() {
    std::uniform_real_distribution<> uniform(0.0,1.0);
    return uniform(randomNumberGenerator);
}
