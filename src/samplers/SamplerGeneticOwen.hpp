#ifndef _UTK_SAMPLER_GENETICOWEN_
#define _UTK_SAMPLER_GENETICOWEN_

#include "../pointsets/Pointset.hpp"
#include "SamplerSobolIndices.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"
#include "../statistics/MinimalDistance.hpp"

namespace utk
{

class SamplerGeneticOwen
{
protected:
public:

	SamplerGeneticOwen() { setRandomSeedTime(); m_nbiter=1000; } //{ setRandomSeed(123456); m_nbiter=1000; }

	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }
	void setNbIter(int arg_nbiter) { m_nbiter = arg_nbiter; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerGeneticOwen::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		std::vector<uint> permutation_tree[D];
		
		Pointset<D, unsigned int, Point<D, unsigned int> > arg_pts_int;
		SamplerSobolIndices sobol;
		sobol.generateSamples< D, unsigned int, Point<D, unsigned int> >(arg_pts_int, arg_points);
		
		tree_depth = ceil( log2(arg_points) );
		tree_size = (1 << tree_depth) - 1;
		
		for(int d=0; d<D; d++)
			permutation_tree[d].resize(tree_size);
		//std::cout << tree_size << " flags required for " << arg_points << " samples" << std::endl;
		
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
	uint m_nbiter;
	
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
		
		bool operator<(const PopulationItem<D>& id1) const { return score > id1.score; }
	};
	

	template<unsigned int D>
	void initPopulation(std::vector< PopulationItem<D> >& population, uint size_population)
	{
		for(int p=0; p<size_population; p++)
		for(int d=0; d<D; d++)
		{
			population[p].permutation_tree[d].resize(tree_size);
			for(int i=0; i<tree_size; i++)
				population[p].permutation_tree[d][i] = getRandom01();
		}
	}
	
	template<unsigned int D>
	void scoreIndividual(Pointset<D, unsigned int, Point<D, unsigned int> > arg_pts_int, PopulationItem<D>& individual)
	{
		for(int d=0; d<D; d++)
			owenScrambling<D>(arg_pts_int, d, individual.permutation_tree[d]);
		
		mmd_engine.compute< D, unsigned int, Point<D, unsigned int> >(arg_pts_int, true, individual.score);
	}
	
	template<unsigned int D>
	void sortPopulation(Pointset<D, unsigned int, Point<D, unsigned int> >& arg_pts_int, std::vector< PopulationItem<D> >& population, uint size_population)
	{
		for(int p=0; p<size_population; p++)
			scoreIndividual<D>( arg_pts_int, population[p] );
		
		std::sort(population.begin(), population.end());
		
		/*std::cout << "Best scores:" << std::endl;
		for(int p=0; p<10; p++)
			std::cout << population[p].score << std::endl;*/
	}
	
	template<unsigned int D>
	void crossPopulation(std::vector< PopulationItem<D> >& population, uint size_population)
	{
		uint SIZE_ELITE=(size_population/10)*2;
		for(int p=size_population-1; p>0; p--)
		{
			for(int i=0; i<tree_depth; i++)
			{
				int flag_min = (1 << i);
				int flag_max = (1 << (i+1)) -1;
				int chunck_size = getRandom(1, (1<<i));
				
				while(flag_min < flag_max)
				{
					
					int mutation;
					mutation = getRandom(0, 4);
					
					uint elite = getRandom(0, SIZE_ELITE-1);
					
					for(int f=flag_min; f<flag_min+chunck_size; f++)
					{
						if(f > flag_max)
							break;
						for(int d=0; d<D; d++)
						{
							if(mutation == 3 ) //random fill
								population[p].permutation_tree[d][f] = getRandom01();
							else
								population[p].permutation_tree[d][f] = population[elite].permutation_tree[d][f];
						}
					}
					flag_min+=chunck_size;
				}
			}
		}
	}
	
	template<unsigned int D>
	void optimizePermutationTrees(Pointset<D, unsigned int, Point<D, unsigned int> >& arg_pts_int, std::vector<uint> permutation_tree[D])
	{
		uint SIZE_POPULATION = 100;
		uint NB_ITER = m_nbiter;
		
		std::cout << "Population " << SIZE_POPULATION << " -- Iter " << NB_ITER << std::endl;
		
		std::vector< PopulationItem<D> > population;
		population.resize(SIZE_POPULATION);

		//Random init population
		initPopulation<D>(population, SIZE_POPULATION);
		//Test population
		sortPopulation<D>(arg_pts_int, population, SIZE_POPULATION);
		
		for(int i=0; i<NB_ITER; i++)
		{
			if(arg_pts_int.size() >= 64)
				std::cout << "Iter " << i << " / " << NB_ITER << " - Score " << population[0].score <<  "          \r" << std::flush;
			//Random init population
			crossPopulation<D>(population, SIZE_POPULATION);
			//Test population
			sortPopulation<D>(arg_pts_int, population, SIZE_POPULATION);			
		}
		std::cout << "Final iter - Score " << population[0].score << "                      " << std::endl;
		
		//Return best candidate
		for(int i=0; i<tree_size; i++)
		{
			for(int d=0; d<D; d++)
				permutation_tree[d][i] = population[0].permutation_tree[d][i] ;
		}
	}
    
};

} //end namespace utk


#endif
