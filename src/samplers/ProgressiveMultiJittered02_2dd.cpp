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
#include "./SamplerPMJ/PMJ02SampleSequenceGenerator.h"
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
                  << "-o      output file                     default : output_PMJ02.dat" << std::endl
                  << "-s      seed                            default : 0" << std::endl
                  << "-c      number of candidates            default : 10" << std::endl;
    }

    int numberOfSamplesToGenerate = atoi(getArg("-n", argc, argv, "16").c_str());
    std::string outputPath = getArg("-o", argc, argv, "output_PMJ02.dat");
    int seed = atoi(getArg("-s", argc, argv, "0").c_str());
    int numberOfCandidates = atoi(getArg("-c", argc, argv, "10").c_str());

    PMJ02SampleSequenceGenerator g = PMJ02SampleSequenceGenerator(seed);
    g.ProgressiveMultiJittered02Algorithm2D(numberOfSamplesToGenerate, numberOfCandidates);
    g.exportSampleSet(outputPath);

    return 0;
}
