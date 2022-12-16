#ifndef _UTK_SAMPLER_ART_
#define _UTK_SAMPLER_ART_

/*
 * Code from 
 * "An Adaptive Point Sampler on a Regular Lattice"
 * Abdalla G. M. Ahmed, Till Niese, Hui Huang, Oliver Deussen
 * SIGGRAPH 2017
 * 
 * http://abdallagafar.com/publications/art/
 */

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"

#include <cstdlib>
#include "SamplerART/self-similar.h"
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <algorithm>

namespace art
{

inline Point getSample(unsigned tileID, unsigned sampleNo, Matrix m = IDENTITY) {
    while (sampleNo > 0) {
        unsigned childNo = tiles[tileID].order[ sampleNo & mask ];
        tileID = tiles[tileID].children[childNo];
        m = m.concat( M[childNo] );
        sampleNo = (sampleNo >> shift);
    }
    return transform(tiles[tileID].p, m);
}

void generate(Points &samples) {
    auto n = samples.size();
    int tileID = rand() % tileCount;
    for (auto i = 0; i < n; i++) {
        samples[i] = getSample(tileID, i);
    }
}

}

namespace utk
{

class SamplerART
{
protected:
public:

	SamplerART() { srand((unsigned)time(NULL)); m_tilefile=""; }
	
	void setTileFile(std::string arg_tilefile) { m_tilefile = arg_tilefile; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerART::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerART::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}
		
		if (D != 2)
		{
			ERROR("SamplerART::generateSamples only generates 2D samples");
			return false;
		}
		
		std::fstream file(m_tilefile);
		art::loadTileSet(file);
		file.close();
		
		int n = arg_points;
		art::Points s(n);
		
		art::generate(s);

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;

		arg_pts.resize(n);
		for (int i = 0; i < n; i++) 
		{
			arg_pts[i].pos()[0] = s[i].x;
			arg_pts[i].pos()[1] = s[i].y;
		}

		return true;
	};

protected:
	std::string m_tilefile;
};

} //end namespace utk


#endif
