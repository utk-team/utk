#include "../../pointsets/Pointset.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include "../../utils.hpp"
#include <chrono>

#include "runRandomization.hpp"

using namespace utk;

#define D 2
typedef unsigned long long int ullint;

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	ParamParser_getopt parser;
	std::array<ullint, D> param_shuffle;
	for(int d=0; d<D; d++)
	  param_shuffle[d] = 0;
	bool random = false;
	
	initParserRandomization(&parser);
	
	parser.addLongOption((char*)"random", &random, 0, assignBoolTrue, displayBool, (char*)"", (char*)"Random");
	parser.addLongOption((char*)"shufflex", &param_shuffle[0], 1, assignULLInt, displayULLInt, (char*)"[uint]", (char*)"Shuffle X");
	parser.addLongOption((char*)"shuffley", &param_shuffle[1], 1, assignULLInt, displayULLInt, (char*)"[uint]", (char*)"Shuffle Y");
	
	parser.parse(argc, argv);

	if(!dealParamParserRandomization(&parser))
	  return 1;
	
	PointsetReader< D, uint, Point<D, uint> > stream_in;
	stream_in.open(param_input);
	PointsetWriter< D, uint, Point<D, uint> > stream_out;
	stream_out.open(param_output);
	
	Pointset< D, uint, Point<D, uint> > pts;
	while(stream_in.readPointset(pts))
	{
		bool* shuffle_x;
		bool* shuffle_y;
		if(!random)
		{
			shuffle_x = toBitList(param_shuffle[0], pts.size());
			shuffle_y = toBitList(param_shuffle[1], pts.size());
		}
		else
		{
			shuffle_x = (bool*)malloc(sizeof(bool)*pts.size());
			shuffle_y = (bool*)malloc(sizeof(bool)*pts.size());
			for(uint i=0; i<pts.size(); i++)
			{
				shuffle_x[i] = rand()%2;
				shuffle_y[i] = rand()%2;
			}
		}

		pts.owen_scrambling(utk::Vector<D, bool*>(shuffle_x, shuffle_y));

		free(shuffle_x);
		free(shuffle_y);
		
		//Pointset< D, double, Point<D, double> > pts2;
		//pts.normalizePositions(pts2);

		stream_out.writePointset(pts);
		pts.clear();
	}
	
	stream_out.close();
	stream_in.close();
	
	return 0;
}
