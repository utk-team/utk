/*
 * David Coeurjolly <david.coeurjolly@liris.cnrs.fr>
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
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
 * either expressed or implied, of the Halton project.
 */
#include "SamplerLloyd.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 3
typedef Point<D, T> P;
typedef SamplerLloyd S;

int main(int argc, char** argv)
{
  ParamParser_getopt parser;
  S sampler;
	
  //PARSE PARAM
  initParserSampler(&parser);
  unsigned int param_nbSteps=100;
  parser.addShortOption('s', &param_nbSteps, 1, utk::assignUInt, utk::displayUInt, "[int=100]\t\t\tNumber of Lloyd's steps", "Nb Lloyd's steps");

  bool param_isToric=false;
  parser.addShortOption('t', &param_isToric, 0, utk::assignBoolTrue, utk::displayBool, "[bool=false]\t\t\tActivate Toroidal domain","Toroidal domain");
  
  //PARSING
  parser.parse(argc, argv);
	
  if(!dealParamParserSampler(&parser))
    return 0;
	
  sampler.setToricity(param_isToric);
  
  PointsetWriter<D, T, P> writer;
  writer.open(param_output.c_str());
  while(param_nbrealisations>0)
    {
      //SAMPLE
      Pointset<D, T, P> pts;
		
      std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
      if(!sampler.generateSamples<D, T, P>(pts, param_nbsamples, param_nbSteps))
        return 1;
      std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
      if(param_verbose)
        std::cout << std::fixed << std::setprecision(5) << "Generated " << pts.size() << " samples in " << time_span.count() << " secs" << std::endl;
		
      //WRITE
      writer.writePointset(pts);
      param_nbrealisations--;
    }
  writer.close();
	
  return 0;
}
