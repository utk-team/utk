#include "../../pointsets/Pointset.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runRandomization.hpp"

using namespace utk;

#define D 2

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	long int param_seed = -1;
	std::array<int, D> param_shuffle;
	for(int d=0; d<D; d++)
	  param_shuffle[d] = -1;
	
	initParserRandomization(&parser);
	
	parser.addLongOption((char*)"shufflex", &param_shuffle[0], 1, assignInt, displayInt, (char*)"[uint]", (char*)"Shuffle X");
	parser.addLongOption((char*)"shuffley", &param_shuffle[1], 1, assignInt, displayInt, (char*)"[uint]", (char*)"Shuffle Y");
	parser.addLongOption((char*)"seed", &param_seed, 1, assignInt, displayInt, (char*)"[int]\t\t\trandom seed", (char*)"Seed");
	
	parser.parse(argc, argv);

	if(!dealParamParserRandomization(&parser))
	  return 1;
	
	if(param_seed == -1)
	  param_seed = std::chrono::system_clock::now().time_since_epoch().count();
	m_mersenneTwister.seed(param_seed);
	
	PointsetReader< D, uint, Point<D, uint> > stream_in;
	stream_in.open(param_input);
	
	PointsetWriter< D, uint, Point<D, uint> > stream_out;
	stream_out.open(param_output);
	
	Pointset< D, uint, Point<D, uint> > pts;
	std::array<int, D> shuffle;
	while(stream_in.readPointset(pts))
	{
	  for(int d=0; d<D; d++)
	    if(param_shuffle[d] == -1)
	      shuffle[d] = (int)( (double)getRandom01()*(double)pts.domain.pMax.pos()[d] );
		else
		  shuffle[d] = param_shuffle[d];
	    
	   pts.digit_xor(utk::Vector<D, unsigned int>(shuffle[0], shuffle[1]));

	  stream_out.writePointset(pts);
	  pts.clear();
	}
	
	stream_out.close();
	stream_in.close();
	
	return 0;
}
