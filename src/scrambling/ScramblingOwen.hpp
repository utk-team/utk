/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
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
#ifndef UTK_SCRAMBLER_OWEN_HPP_
#define UTK_SCRAMBLER_OWEN_HPP_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"
#include <cstdlib>
#include <random>
#include <chrono>


namespace utk
{

class ScramblingOwen
{
public:
	ScramblingOwen() { setRandomSeedTime(); }
	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }
	
	template<unsigned int D, typename T, typename P>
	bool scramble(const Pointset< D, T, P >& pts_input, Pointset< D, T, P >& pts_output)
	{
		if (isFloatingType<T>())
			WARNING("ScramblingOwen:compute applying on floating type coordinates. Clamping to integer values.");
		
		std::vector<uint> permutation_tree[D];
		uint arg_points = pts_input.size();

		uint tree_size = (1 << (uint)(ceil( log2(arg_points) ))) - 1;
		for(int d=0; d<D; d++)
			permutation_tree[d].resize(tree_size);

		for(int i=0; i<tree_size; i++)
			for(int d=0; d<D; d++)
				permutation_tree[d][i] = (bool)(getRandom01()>0.5);
					
		pts_output.resize(pts_input.size());
		for(uint d=0; d<D; d++)
		{
			pts_output.domain.pMin.pos()[d] = pts_input.domain.pMin.pos()[d];
			pts_output.domain.pMax.pos()[d] = pts_input.domain.pMax.pos()[d];
		}
		pts_output.toricity=pts_input.toricity;
		
		for(int i=0; i<arg_points; i++)
			for(int d=0; d<D; d++)
				pts_output[i].pos()[d] = (unsigned int)pts_input[i].pos()[d];
				
		for(int d=0; d<D; d++)
			owenScrambling<D, T>(pts_output, d, permutation_tree[d]);
		
		return true;
	}
	
	template<unsigned int D, typename T, typename P>
	bool scramble(const Pointset< D, T, P >& pts_input, Pointset< D, T, P >& pts_output, std::vector<uint> permutation_tree[D])
	{
		if (isFloatingType<T>())
			WARNING("ScramblingOwen:compute applying on floating type coordinates. Clamping to integer values.");
		
		uint arg_points = pts_input.size();
	
		pts_output.resize(pts_input.size());
		for(uint d=0; d<D; d++)
		{
			pts_output.domain.pMin.pos()[d] = pts_input.domain.pMin.pos()[d];
			pts_output.domain.pMax.pos()[d] = pts_input.domain.pMax.pos()[d];
		}
		pts_output.toricity=pts_input.toricity;
		
		for(int i=0; i<arg_points; i++)
			for(int d=0; d<D; d++)
				pts_output[i].pos()[d] = (unsigned int)pts_input[i].pos()[d];
				
		for(int d=0; d<D; d++)
			owenScrambling<D, T>(pts_output, d, permutation_tree[d]);
		
		return true;
	}
	
	
protected:
	std::mt19937 m_mersenneTwister;
    double getRandom01()
    {
        return (double)m_mersenneTwister()/(double)m_mersenneTwister.max();
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
    
    template<unsigned int D, typename T>
    void owenScrambling(Pointset<D, T, Point<D, T> >& arg_pts_int, uint dim, std::vector<uint>& permutation_tree)
	{
		uint point_binary_size = (uint)(ceil( log2(arg_pts_int.size()) ));
		std::vector<uint> binary_pt;
		binary_pt.resize(point_binary_size);
		
		for(uint pt=0; pt<arg_pts_int.size(); pt++)
		{
			//binary_pt[0] -> highest digit
			for(int i=0; i<point_binary_size; i++)
				binary_pt[point_binary_size-i-1] = (((unsigned int)arg_pts_int[pt].pos()[dim]) >> i) & 1;
			
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
	
}

#endif