//
// Created by p1703185 on 15/04/19.
//
#include <string>
#include "../PJSampleSequenceGenerator.h"
#include "../PMJSampleSequenceGenerator.h"
#include "../PMJ02SampleSequenceGenerator.h"
#include <iostream>

std::string standardize(int totalSize, int number)
{
    std::string stringNumber = std::to_string(number);
    while (stringNumber.length() < totalSize)
        stringNumber += "_";
    return stringNumber;
}


int main()
{//lenght = 7
    /*int[] numberOfSamples = {8,9,10,10,11,12,13,15,16,17,19,21,23,25,27,29,32,35,38,41,45,
                             49,54,59,64,70,76,83,91,99,108,117,128,140,152,166,181,197,215,
                             235,256,279,304,332,362,395,431,470,512,558,609,664,724,790,
                             861,939,1024,1117,1218,1328,1448,1579,1722,1878,2048,2233,2435,
                             2656,2896,3158,3444,3756,4096,4467,4871,5312,5793,6317,6889,7512,
                             8192,8933,9742,10624,11585,12634,13777,15024,16384,17867,19484,21247,
                             23170,25268,27554,30048,32768,35734,38968,42495,46341,50535,55109,
                             60097,65536,71468,77936,84990,92682,101070,110218,120194,131072,
                             142935,155872,169979,185364,202141,220436,240387,262144,285870,
                             311744,339959,370728,404281,440872,480774,524288,571740,623487,
                             679917,741455,808563,881744,961548,1048576};
    int numberOfSeeds = 100;
    for(int j = 0; j < numberOfSeeds; j++) {
        int seed = rand();
        std::string seedString = standardize()

        int numberOfNumberOfSamples = 100;
        for (int i = 0; i < numberOfNumberOfSamples: i++)
        {
            int numberOfSamplesToGenerate = numberOfNumberOfSamples[i];

            PJSampleSequenceGenerator g = PJSampleSequenceGenerator(seed);
            g.progressiveJitteredAlgorithm2D(numberOfSamplesToGenerate);
            std::string outputPath = "/home/etu/p1703185/tmp/PJ/";
            outputPath += numberOfSamplesToGenerate;
            outputPath += ".dat";
            g.exportSampleSet(outputPath);

            PMJSampleSequenceGenerator h = PMJSampleSequenceGenerator(seed);
            h.progressiveMultiJitteredAlgorithm2D(numberOfSamplesToGenerate);
            std::string outputPath = "/home/etu/p1703185/tmp/PMJ/N";
            outputPath += numberOfSamplesToGenerate;
            outputPath += ".dat";
            h.exportSampleSet(outputPath);

            PMJ02SampleSequenceGenerator k = PMJ02SampleSequenceGenerator(seed);
            k.progressiveMultiJittered02Algorithm2D(numberOfSamplesToGenerate);
            std::string outputPath = "/home/etu/p1703185/tmp/PMJ02/N";
            outputPath += numberOfSamplesToGenerate;
            outputPath += ".dat";
            k.exportSampleSet(outputPath);
        }

    }

*/
    for(int i = 0; i < 100; i++)
        std::cout << rand();

    return 0;
}
