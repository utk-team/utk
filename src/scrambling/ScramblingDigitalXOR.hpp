#ifndef UTK_SCRAMBLER_XOR_HPP_
#define UTK_SCRAMBLER_XOR_HPP_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"
#include <cstdlib>
#include <random>
#include <chrono>


namespace utk
{

class ScramblingDigitalXOR
{
public:
	ScramblingDigitalXOR() { setRandomSeedTime(); }
	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }
	
	template<unsigned int D, typename T, typename P>
	bool scramble(const Pointset< D, T, P >& pts_input, Pointset< D, T, P >& pts_output)
	{
		if(pts_input.toricity!=1)
			WARNING("ScramblingDigitalXOR:compute applying on a non-toric point set");
		if (isFloatingType<T>())
			WARNING("ScramblingDigitalXOR:compute applying on floating type coordinates. Clamping to integer values.");
		
		Vector<D, unsigned int> randvector;
		for(uint d=0; d<D; d++)
			randvector[d] = getRandom01()*(pts_input.domain.pMax.pos()[d]-pts_input.domain.pMin.pos()[d]);
		
		pts_output.resize(pts_input.size());
		for(uint d=0; d<D; d++)
		{
			pts_output.domain.pMin.pos()[d] = pts_input.domain.pMin.pos()[d];
			pts_output.domain.pMax.pos()[d] = pts_input.domain.pMax.pos()[d];
		}
		pts_output.toricity=pts_input.toricity;
		
		for(uint i=0; i<pts_input.size(); i++)
		{
			for(uint d=0; d<D; d++)
			{
				pts_output[i].pos()[d] = (unsigned int)(pts_input[i].pos()[d]) ^ (unsigned int)(randvector[d]);
			}
		}
		
		return true;
	}
	
	template<unsigned int D, typename T, typename P>
	bool scramble(const Pointset< D, T, P >& pts_input, Pointset< D, T, P >& pts_output, Vector<D, unsigned int> randvector)
	{
		if(pts_input.toricity!=1)
			WARNING("ScramblingDigitalXOR:compute applying on a non-toric point set");
		if (isFloatingType<T>())
			WARNING("ScramblingDigitalXOR:compute applying on floating type coordinates. Clamping to integer values.");
		
		pts_output.resize(pts_input.size());
		for(uint d=0; d<D; d++)
		{
			pts_output.domain.pMin.pos()[d] = pts_input.domain.pMin.pos()[d];
			pts_output.domain.pMax.pos()[d] = pts_input.domain.pMax.pos()[d];
		}
		pts_output.toricity=pts_input.toricity;
		
		for(uint i=0; i<pts_input.size(); i++)
		{
			for(uint d=0; d<D; d++)
			{
				pts_output[i].pos()[d] = (unsigned int)(pts_input[i].pos()[d]) ^ (unsigned int)(randvector[d]);
			}
		}
		
		return true;
	}
	
	
protected:
	std::mt19937 m_mersenneTwister;
    double getRandom01()
    {
        return (double)m_mersenneTwister()/(double)m_mersenneTwister.max();
    }

};
	
}

#endif