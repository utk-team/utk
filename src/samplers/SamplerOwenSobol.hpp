#ifndef _UTK_SAMPLER_GENETICOWEN_
#define _UTK_SAMPLER_GENETICOWEN_

#include "../pointsets/Pointset.hpp"
#include "SamplerSobolIndices.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"

namespace utk
{

class SamplerOwenSobol
{
protected:
public:

	SamplerOwenSobol() { setRandomSeedTime(); }

	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerOwenSobol::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		std::vector<uint> permutation_tree[D];
		
		Pointset<D, unsigned int, Point<D, unsigned int> > arg_pts_int;
		SamplerSobolIndices sobol;
		sobol.generateSamples< D, unsigned int, Point<D, unsigned int> >(arg_pts_int, arg_points);
		
		uint tree_size = (1 << (uint)(ceil( log2(arg_points) ))) - 1;
		for(int d=0; d<D; d++)
			permutation_tree[d].resize(tree_size);
		//std::cout << tree_size << " flags required for " << arg_points << " samples" << std::endl;
		
		for(int i=0; i<tree_size; i++)
		{
			for(int d=0; d<D; d++)
				permutation_tree[d][i] = getRandom01();
		}
		
		for(int d=0; d<D; d++)
			owenScrambling<D>(arg_pts_int, d, permutation_tree[d]);
		
		arg_pts.toricity = 1;
		arg_pts.resize(arg_points);
		
		if(isFloatingType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			for(int i=0; i<arg_points; i++)
			{
				for(int d=0; d<D; d++)
					arg_pts[i].pos()[d] = (T)arg_pts_int[i].pos()[d] / (T)(tree_size+1);
			}
		}
		else if(isIntegerType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = tree_size+1;
			}
			for(int i=0; i<arg_points; i++)
			{
				for(int d=0; d<D; d++)
					arg_pts[i].pos()[d] = (T)arg_pts_int[i].pos()[d];
			}
		}
		else
		{
			ERROR("SamplerOwenSobol::generateSamples The samples corrdinates must be of numeric value");
			return false;
		}
		
		return true;
	};

protected:
	
    std::mt19937 m_mersenneTwister;
	
    uint getRandom01()
    {
        return (uint)m_mersenneTwister()%2;
    }
    
    uint flagID(uint depth, std::vector<uint>& binary_pt)
	{
		uint id = 0;
		for(uint i=0; i<depth; i++)
		 id += binary_pt[i] * (1 << i);
		uint flag = (1 << depth)-1 + id;
		//std::cout << "Flag" << depth << " " << id << "(" << flag << ")" << std::endl;
		return flag;
	}
    
    template<unsigned int D>
    void owenScrambling(Pointset<D, unsigned int, Point<D, unsigned int> >& arg_pts_int, uint dim, std::vector<uint>& permutation_tree)
	{
		uint point_binary_size = (uint)(ceil( log2(arg_pts_int.size()) ));
		std::vector<uint> binary_pt;
		binary_pt.resize(point_binary_size);
		
		for(uint pt=0; pt<arg_pts_int.size(); pt++)
		{
			//binary_pt[0] -> highest digit
			for(int i=0; i<point_binary_size; i++)
				binary_pt[point_binary_size-i-1] = (arg_pts_int[pt].pos()[dim] >> i) & 1;
			
 			/*std::cout << "Point " << pt << " dim " << dim << std::endl;
 			std::cout << arg_pts_int[pt].pos()[dim] << " = ";
 			for(int i=0; i<point_binary_size; i++)
 				std::cout << binary_pt[i];
 			std::cout << std::endl;*/
			
			for(int i=0; i<point_binary_size; i++)
				binary_pt[i] = binary_pt[i] ^ permutation_tree[flagID(i, binary_pt)];
			
			uint scrambled_pt = 0;
			for(int i=0; i<point_binary_size; i++)
				scrambled_pt += binary_pt[point_binary_size-i-1] * (1 << i);
			
			/*std::cout << "New Point " << pt << " dim " << dim << std::endl;
 			std::cout << scrambled_pt << " = ";
 			for(int i=0; i<point_binary_size; i++)
 				std::cout << binary_pt[i];
 			std::cout << std::endl;*/
			
			arg_pts_int[pt].pos()[dim] = scrambled_pt;
		}
	}
    
};

} //end namespace utk


#endif
