#ifndef _UTK_SAMPLER_VICTOR_OWENPRIME_
#define _UTK_SAMPLER_VICTOR_OWENPRIME_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include "SamplerSobolIndices.hpp"
#include <random>
#include <chrono>


namespace utk
{
	
class SamplerVictorOwenPrime
{
public:

	SamplerVictorOwenPrime() {
		leveldepth = 6;
		setRandomSeedTime();
		
		int indices[10];
		for(int i=0; i<10; i++)
			indices[i] = i+1;
		sobol.setIndices(indices, 10);
	}
	
	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }
	void setLevelDepth(uint arg_leveldepth) { leveldepth = arg_leveldepth; }
	void setIndices(int* arg_indices, uint arg_dimension){ sobol.setIndices(arg_indices, arg_dimension); }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
	{
		//setRandomSeed(1434);
		
		if(arg_points == 0)
			return true;

		if (!arg_pts.empty())
		{
			WARNING("SamplerVictorOwenPrime::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		treedepth = ceil( log2(arg_points) );
		treedepth = ceil( (double)treedepth / (double)leveldepth ) * leveldepth;
		treenbflags = pow(2, treedepth);
		
		std::cout << "To generate " << arg_points << " samples, i need a tree of depth " << treedepth << " with " << treenbflags << " flags" << std::endl;
		
		Vector<D, bool*> permutations;
		
		computeTree<D>(permutations);
		
		/*for(int d=0; d<D; d++)
		{
			for(int l=0; l<treedepth; l++)
			{
				uint begin_level = pow(2, l)-1;
				uint end_level = pow(2,l+1)-1;
				
				std::cout << "L" << begin_level << "\t";
				for(int i=begin_level; i<end_level; i++)
					std::cout << "[" << permutations[d][i] << "]";
				std::cout << std::endl;
			}
		}*/
		
		Pointset<D, uint, Point<D, uint> > int_pts;
		sobol.generateSamples<D, uint, Point<D, uint> >(int_pts, treenbflags);
		int_pts.resize(arg_points);
		
		owenscrambling<D, uint, Point<D, uint> >(int_pts, permutations);
		
		arg_pts.resize(arg_points);
		if(isIntegerType<T>())
		{
			for(uint d=0; d<D; d++){
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = treenbflags;
			}
			
			for(uint i=0; i<arg_points; i++)
			{
				for(uint d=0; d<D; d++)
					arg_pts[i].pos()[d] = int_pts[i].pos()[d];
			}
		}
		else if (isFloatingType<T>())
		{
			for(uint d=0; d<D; d++){
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			
			for(uint i=0; i<arg_points; i++)
			{
				for(uint d=0; d<D; d++)
					arg_pts[i].pos()[d] = ((T)int_pts[i].pos()[d] / (T)treenbflags);
			}
		}
		
		for(int d=0; d<D; d++)
			delete [] permutations[d];
		
		return true;
	}

private:
	
	template<unsigned int D>
	void computeTree(Vector<D, bool*>& permutations)
	{
		for(int d=0; d<D; d++)
			permutations[d] = new bool[treenbflags];
	
		for(int i=0; i<treenbflags; i++)
			for(int d=0; d<D; d++)
				permutations[d][i] = (uint)(getRandom01()*1000)%2;
		
		for(int d=0; d<D; d++)
		{
			for(int l=0; l<treedepth; l++)
			{
				uint begin_level = pow(2, l)-1;
				uint end_level = pow(2,l+1)-1;
				uint increm = pow(2, l%leveldepth);
				
				for(int i=begin_level; i<end_level; i+=increm)
					permutations[d][i] = 0;
			}
		}
	}
	
	template<unsigned int D, typename T, typename P>
	void owenscrambling(Pointset<D, T, P>& arg_pts, Vector<D, bool*> permutations)
	{
		int ind;
		int nbBits = treedepth;

		bool* pt_digits = new bool[nbBits];//(bool*)malloc(sizeof(bool)*nbBits);
		bool* res_digits = new bool[nbBits];//(bool*)malloc(sizeof(bool)*nbBits);
	  
		for(uint d=0; d<D; d++)
		{
			bool* permut = permutations[d];

			for(uint ipt=0; ipt<arg_pts.size(); ipt++) 
			{
				uint pt = arg_pts.at(ipt).pos()[d];

				for(int i=0; i<nbBits; i++)
					pt_digits[i] = (pt >> i) & 1;
				
				/*std::cout << "ptsDigits ";
				for(int i=0; i<nbBits; i++)
					std::cout << pt_digits[i];
				std::cout << std::endl;*/

				for(int ilevel=0; ilevel<nbBits; ilevel++) {
					int begin_level = pow(2, ilevel)-1;
					ind = (pt >> (nbBits-ilevel)) + begin_level;
					res_digits[ilevel] = permut[ind] ^ pt_digits[nbBits-ilevel-1];
				}

				uint tmp = 0;
				for(int i=0; i<nbBits; i++) {
					tmp <<= 1;
					tmp |= res_digits[i];
				}

				arg_pts[ipt].pos()[d] = tmp;
			}
		}

		//free(pt_digits);
		//free(res_digits);
		delete [] pt_digits;
		delete [] res_digits;
	}
	
	uint treedepth;
	uint treenbflags;
	uint leveldepth;
	SamplerSobolIndices sobol;
	
	std::mt19937 m_mersenneTwister;
    double getRandom01()
    {
        return (double)m_mersenneTwister()/(double)m_mersenneTwister.max();
    }
};

} //end namespace utk

#endif
