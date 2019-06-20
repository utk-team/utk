#include "./SamplerPMJ/src/PJSampleSequenceGenerator.h"
#include <string>
#include <iostream>

std::string getArg(std::string identifier, int argc, char* argv[], std::string defaultValue)
{
    for (int i = 0; i < argc; ++i) {
        if(argv[i] == identifier && argc > i+1)
            return argv[i+1];
    }
    return defaultValue;
}

int main(int argc, char* argv[]) {
    if(argc == 1)
    {
        std::cout << "Usage :" << std::endl
        << "-n      number of samples to generate   default : 16" << std::endl
        << "-o      output file                     default : output_PJ.dat" << std::endl
        << "-s      seed                            default : 0" << std::endl;
    }

    int numberOfSamplesToGenerate = atoi(getArg("-n", argc, argv, "16").c_str());
    std::string outputPath = getArg("-o", argc, argv, "output_PJ.dat");
    int seed = atoi(getArg("-s", argc, argv, "0").c_str());

    PJSampleSequenceGenerator g = PJSampleSequenceGenerator(seed);
    g.progressiveJitteredAlgorithm2D(numberOfSamplesToGenerate);
    g.exportSampleSet(outputPath);

    return 0;
}
