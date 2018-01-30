#include "../../pointsets/Pointset.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runRandomization.hpp"

using namespace utk;

#define D 3

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	long int param_seed = -1;
	std::array<double, D> param_shuffle;
	for(int d=0; d<D; d++)
	  param_shuffle[d] = -1;
	
	initParserRandomization(&parser);
	
	parser.addLongOption((char*)"shufflex", &param_shuffle[0], 1, assignDouble, displayDouble, (char*)"[double]", (char*)"Shuffle X");
	parser.addLongOption((char*)"shuffley", &param_shuffle[1], 1, assignDouble, displayDouble, (char*)"[double]", (char*)"Shuffle Y");
	parser.addLongOption((char*)"shufflez", &param_shuffle[2], 1, assignDouble, displayDouble, (char*)"[double]", (char*)"Shuffle Z");
	parser.addLongOption((char*)"seed", &param_seed, 1, assignInt, displayInt, (char*)"[int]\t\t\trandom seed", (char*)"Seed");
	
	parser.parse(argc, argv);

	if(!dealParamParserRandomization(&parser))
	  return 1;
	
	if(param_seed == -1)
	  param_seed = std::chrono::system_clock::now().time_since_epoch().count();
	m_mersenneTwister.seed(param_seed);
	
	PointsetReader< D, double, Point<D, double> > stream_in;
	stream_in.open(param_input);
	
	PointsetWriter< D, double, Point<D, double> > stream_out;
	stream_out.open(param_output);
	
	Pointset< D, double, Point<D, double> > pts;
	std::array<double, D> shuffle;
	while(stream_in.readPointset(pts))
	{
	  for(int d=0; d<D; d++)
	    if(param_shuffle[d] == -1)
	      shuffle[d] = getRandom01();
		else
		  shuffle[d] = param_shuffle[d];
	    
	   pts.cranley_patterson(utk::Vector<D, double>(shuffle[0], shuffle[1]));

	  stream_out.writePointset(pts);
	  pts.clear();
	}
	
	stream_out.close();
	stream_in.close();
	
	return 0;
}
