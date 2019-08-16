/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
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
 * either expressed or implied, of the Halton project.
 */
#ifndef _UTK_SAMPLER_EXHAUSTIVEOWEN_
#define _UTK_SAMPLER_EXHAUSTIVEOWEN_

#include "../pointsets/Pointset.hpp"
#include "SamplerSobolIndices.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"
#include "../statistics/MinimalDistance.hpp"

namespace utk
{

class SamplerExhaustiveOwen
{
protected:
public:

	SamplerExhaustiveOwen() { setRandomSeedTime(); }

	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerExhaustiveOwen::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		if (D != 2)
		{
			ERROR("SamplerExhaustiveOwen::generateSamples generates only 2D samples.");
			return false;
		}
		
		std::vector<uint> permutation_tree[D];
		
		Pointset<D, unsigned int, Point<D, unsigned int> > arg_pts_int;
		SamplerSobolIndices sobol;
		sobol.generateSamples< D, unsigned int, Point<D, unsigned int> >(arg_pts_int, arg_points);
		
		tree_depth = ceil( log2(arg_points) );
		tree_size = (1 << tree_depth) - 1;
		
		for(int d=0; d<D; d++)
			permutation_tree[d].resize(tree_size);
		std::cout << tree_size << " flags required for " << arg_points << " samples" << std::endl;
		
		optimizePermutationTrees<D>(arg_pts_int, permutation_tree);
		
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
			ERROR("SamplerGeneticOwen::generateSamples The samples corrdinates must be of numeric value");
			return false;
		}
		
		return true;
	};

protected:
	
	MinimalDistance mmd_engine;
    std::mt19937 m_mersenneTwister;
	uint tree_size;
	uint tree_depth;
	
    uint getRandom01()
    {
        return (uint)m_mersenneTwister()%2;
    }
    
    uint getRandom(uint min, uint max)
    {
        return (uint)m_mersenneTwister()%(max+1) + min;
    }
    
    uint flagID(uint depth, std::vector<uint>& binary_pt)
	{
		uint id = 0;
		for(uint i=0; i<depth; i++)
		 id += binary_pt[i] * (1 << i);
		uint flag = (1 << depth)-1 + id;
		return flag;
	}
    
    template<unsigned int D>
    void owenScrambling(Pointset<D, unsigned int, Point<D, unsigned int> >& arg_pts_int, uint dim, std::vector<uint>& permutation_tree)
	{
		uint point_binary_size = tree_depth;
		std::vector<uint> binary_pt;
		binary_pt.resize(point_binary_size);
		
		for(uint pt=0; pt<arg_pts_int.size(); pt++)
		{
			//binary_pt[0] -> highest digit
			for(int i=0; i<point_binary_size; i++)
				binary_pt[point_binary_size-i-1] = (arg_pts_int[pt].pos()[dim] >> i) & 1;
			
			for(int i=0; i<point_binary_size; i++)
				binary_pt[i] = binary_pt[i] ^ permutation_tree[flagID(i, binary_pt)];
			
			uint scrambled_pt = 0;
			for(int i=0; i<point_binary_size; i++)
				scrambled_pt += binary_pt[point_binary_size-i-1] * (1 << i);

			
			arg_pts_int[pt].pos()[dim] = scrambled_pt;
		}
	}
	
	template<unsigned int D>
	struct PopulationItem
	{
		std::vector<uint> permutation_tree[D];
		double score;
	};
	
	template<unsigned int D>
	void scoreIndividual(Pointset<D, unsigned int, Point<D, unsigned int> > arg_pts_int, PopulationItem<D>& individual)
	{
		for(int d=0; d<D; d++)
			owenScrambling<D>(arg_pts_int, d, individual.permutation_tree[d]);
		mmd_engine.compute< D, unsigned int, Point<D, unsigned int> >(arg_pts_int, true, individual.score);
	}
	
	template<unsigned int D>
	void optimizePermutationTrees(Pointset<D, unsigned int, Point<D, unsigned int> >& arg_pts_int, std::vector<uint> permutation_tree[D])
	{
		uint NB_ITER = pow(2, tree_size);

		PopulationItem<D> current_tree;
		PopulationItem<D> best_tree; best_tree.score = 0;
		for(int d=0; d<D; d++) {
			best_tree.permutation_tree[d].resize(tree_size);
			current_tree.permutation_tree[d].resize(tree_size);
		}
		
		for(int i=0; i<NB_ITER; i++)
		{
			//if(tree_size==15 && best_tree.score > 3.6) break;
			
			std::cout << "Iter " << i << " / " << NB_ITER << " - Score " << best_tree.score <<  "          \r" << std::flush;
			for(int j=i; j<NB_ITER; j++)
			{
				for(int t=0; t<tree_size; t++)
				{
					current_tree.permutation_tree[0][t] = (i >> t) & 1;
					current_tree.permutation_tree[1][t] = (j >> t) & 1;
				}
				scoreIndividual<D>(arg_pts_int, current_tree);
				if(current_tree.score > best_tree.score)
				{
					for(int t=0; t<tree_size; t++)
						for(int d=0; d<D; d++)
							best_tree.permutation_tree[d][t] = current_tree.permutation_tree[d][t] ;
					best_tree.score = current_tree.score;
				}
			}
		}
		std::cout << "Final iter - Score " << best_tree.score << "                      " << std::endl;
		
		//Return best candidate
		for(int i=0; i<tree_size; i++)
		{
			for(int d=0; d<D; d++)
				permutation_tree[d][i] = best_tree.permutation_tree[d][i] ;
		}
	}
    
};

} //end namespace utk


#endif
