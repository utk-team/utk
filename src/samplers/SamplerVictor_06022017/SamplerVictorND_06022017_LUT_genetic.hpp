#ifndef _UTK_SAMPLER_VICTOR_LUT_06022017_
#define _UTK_SAMPLER_VICTOR_LUT_06022017_

#include "../../utils.hpp"
#include "../../pointsets/Pointset.hpp"
#include "../../pointsets/Histogram.hpp"
#include "../SamplerSobolIndices.hpp"
#include "../../io/histogramIO.hpp"
#include "../../io/fileIO.hpp"
#include "../../statistics/PCF.hpp"
#include <map>
#include <cstdlib>

#ifdef TBB_ON
#include <tbb/tbb.h>
#endif

#define PCF_FILE "step_256pts_rmin15_rmax5_sigma01_nbbins200.dat"
//#define PCF_FILE "bnot_256pts_rmin15_rmax5_sigma01_nbbins100.dat"

namespace utk
{

const uint SamplerVictor06022017_BITSETSIZE=64;

template<uint D>
class SamplerVictor06022017_LUT_Key
{
public:
	Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point<D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > pts;
	
	SamplerVictor06022017_LUT_Key() {}
	SamplerVictor06022017_LUT_Key(Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point<D, std::bitset<SamplerVictor06022017_BITSETSIZE> > >& arg_pts) { pts=arg_pts; }

	/**
	 * Needed by std::map
	 ***/
	bool operator<(const SamplerVictor06022017_LUT_Key<D>& k) const
	{
		for(uint i=0; i<pts.size(); i++)
		{
			for(uint d=0; d<D; d++)
			{
				if(pts[i].pos()[d].to_ulong() < k.pts[i].pos()[d].to_ulong())
					return true;

				if(pts[i].pos()[d].to_ulong() > k.pts[i].pos()[d].to_ulong())
					return false;
			}
		}
		return false;
	}
};


template<uint D>
struct individual
{
	Vector<D, unsigned long long int> permut;
	double Linf;
	double mindist;
	double L2;
};

template<uint D>
bool compare_individuals(const individual<D>& i0, const individual<D>& i1)
{
	return (i0.Linf < i1.Linf);
}

template<uint D>
void evaluate_individual(Pointset<D, uint, Point<D, uint> >& pts, individual<D>& i0, HistogramND<1, double, double>& pcf_ref, uint N, uint M, uint K)
{
	uint nbpts = pts.size();
	Vector<D, bool*> permuts;
	
	for(uint d=0; d<D; d++)
		permuts[d] = (bool*)malloc(sizeof(bool)*(nbpts-1));
	
	for(uint d=0; d<D; d++)
		for(uint i=0; i<nbpts-1; i++)
			permuts[d][i] = (i0.permut[d] >> i) & 1;
	
	uint logKD=log2(nbpts);
	std::vector< std::array<uint, D> > x;
	x.resize(logKD-1);
	for(uint d=0; d<D; d++)
	{
		for(uint i=0; i<logKD-1; i++)
		{
			uint sum = (1 << (i+1))-1;
			x[i][d] = sum + pts[0].pos()[d]/(nbpts/pow(2, i+1));
		}
	}
	for(uint d=0; d<D; d++)
	{
		permuts[d][0] = 0;
		for(uint i=0; i<logKD-1; i++)
			permuts[d][x[i][d]] = 0;
	}
	Pointset< D, uint, Point<D, uint> > scrambledset;
	pts.owen_scrambling(permuts, scrambledset);
		
	uint maxD = D/2;
	maxD *= 2;
	
	i0.Linf = 0;
 	for(uint dim=0; dim<maxD; dim+=2)
	{
		PCF pcf_engine;
		HistogramND<1, double, double> pcf_set;
	
		double d = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*scrambledset.size()) );
		double smoothing = 0.1*d;
		double rmin = 1.5*smoothing;
		double rmax = 5*d;
	
		Pointset<2, uint, Point<2, uint> > pts_proj;
		pts_proj.resize(nbpts);
		for(uint p=0; p<scrambledset.size(); p++)
		{
			pts_proj[p].pos()[0] = scrambledset[p].pos()[dim];
			pts_proj[p].pos()[1] = scrambledset[p].pos()[dim+1];
			
		}
		pts_proj.domain.pMax.pos()[0] = nbpts;
		pts_proj.domain.pMax.pos()[1] = nbpts;
		
		double mindist = 1;
		for(uint i=0; i<nbpts; i++)
		for(uint j=(i+1); j<nbpts; j++)
		{	
			double posx = (double)pts_proj[i].pos()[0]/(double)nbpts;
			double posy = (double)pts_proj[i].pos()[1]/(double)nbpts;
				
			double neighbour_posx = (double)pts_proj[j].pos()[0]/(double)nbpts;
			double neighbour_posy = (double)pts_proj[j].pos()[1]/(double)nbpts;
			
			double diffx = neighbour_posx-posx;
			double diffy = neighbour_posy-posy;
				
			double dist = sqrt(diffx*diffx + diffy*diffy);
			if(dist < mindist)
				mindist = dist;
		}
		i0.mindist = mindist;
		
		if(mindist < d*0.5)
		{
			i0.Linf = 10;
			break;
		}
			
		pcf_engine.compute< 2, uint, Point<2, uint> >(pts_proj, pcf_set, rmin, rmax, pcf_ref.size(), smoothing);
		
		double Linf_value = 0; 
		/*if(!pcf_set.isLinfUnderThreshold(1, pcf_ref, Linf_value))
		{
			i0.Linf = 10;
			break;
		}*/
		pcf_set.Linf(pcf_ref, Linf_value);
		
			
		
		if(i0.Linf < Linf_value)
			i0.Linf = Linf_value;
	}
	
	for(uint d=0; d<2; d++)
		free(permuts[d]);
		
	//i0.L2 = L2_value;
}

template<uint D>
void cross_mutate_individual(int index, std::vector< individual<D> >& population, std::vector< individual<D> >& population_cross, uint SIZE_ELITE, uint nbpts)
{
	uint depth = 0;
	uint chunck_size = pow(2, depth);
	uint end_level = 1;
	uint begin_level = 0;

	int id_parent[D];
	for(uint d=0; d<D; d++)
		id_parent[d]=rand()%SIZE_ELITE;
	
	for(uint c=0; c<nbpts-1; c++)
	{
		if((c-begin_level)%chunck_size == 0)
		{
			for(uint d=0; d<2; d++)
				id_parent[d]=rand()%SIZE_ELITE;
		}
		
		if(c+1 == end_level)
			depth++;
		begin_level = c+1;
		end_level += pow(2, depth);
		chunck_size = 2;
		
		for(uint d=0; d<D; d++)
		{
			population_cross[index].permut[d] <<= 1;
			population_cross[index].permut[d] |= (population[id_parent[d]].permut[d] >> c) & 1;
		}
	}
	
	if(rand()%3 == 0)
	{
		uint nb_mutations = 2;
		for(uint c=0; c<nb_mutations; c++)
		{
			int id[D];
			for(uint d=0; d<D; d++)
				id[d] = rand()%(nbpts-1);
		
			for(uint d=0; d<D; d++)
				population_cross[index].permut[d] ^= (1 << id[d]);
		}
	}
	
	population_cross[index].mindist = 1;
	population_cross[index].Linf = 10;
	population_cross[index].L2 = 10;
}

template<uint D>
void init_individual(uint index, std::vector< individual<D> >& population, uint nbpts)
{
	for(uint d=0; d<D; d++)
	{
		for(uint i=0; i<nbpts-1; i++)
		{
			population[index].permut[d] <<= 1;
			population[index].permut[d] |= rand()%2;
		}
	}
	
	population[index].mindist = 1;
	population[index].Linf = 10;
	population[index].L2 = 10;
}

template<uint D>
void init_population(std::vector< individual<D> >& population, uint nbpts)
{
	uint SIZE_POPULATION = population.size();
	
	for(uint p=0; p<SIZE_POPULATION; p++)
	{
		for(uint d=0; d<D; d++)
		{
			for(uint i=0; i<nbpts-1; i++)
			{
				population[p].permut[d] <<= 1;
				population[p].permut[d] |= rand()%2;
			}
		}
		
		population[p].mindist = 1;
		population[p].Linf = 10;
		population[p].L2 = 10;
	}
}

template<uint D>	
bool geneticOptimizer(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut, uint K=4, uint NB_GENERATIONS=120)
{	
	HistogramND<1, double, double> pcf_ref;
	HistogramReader<1, double, double> pcfreader;
	
	if(NB_GENERATIONS > 0)
	{
		pcfreader.open(PCF_FILE);
		pcfreader.readHistogram(pcf_ref);
		pcfreader.close();
	}
	
	uint SIZE_POPULATION = 100;
	uint SIZE_KEPT = 10;
	uint SIZE_ELITE = 50;
	
	uint tilesize = pow(K, D);
	assert(key.pts.size() == tilesize);
	Pointset<D, uint, Point<D, uint> > pts;
	pts.resize(tilesize);
	for(uint i=0; i<tilesize; i++)
	{
		for(uint d=0; d<D; d++)
			pts[i].pos()[d] = key.pts[i].pos()[d].to_ulong();
	}
	for(uint d=0; d<D; d++)
		pts.domain.pMax.pos()[d] = tilesize;
	
	std::ofstream  file("initpts.dat");
	for(uint i=0; i<tilesize; i++)
		file << pts[i] << std::endl;
	file.close();
	
	uint M = tilesize-1;
	uint N = M - ceil(log2(tilesize));

	std::vector< individual<D> > population;
	std::vector< individual<D> > population_cross;
	population.resize(SIZE_POPULATION);
	population_cross.resize(SIZE_POPULATION);
	
	//INIT POPULATION
	init_population<D>(population, tilesize);
	/*for(int i=0; i<SIZE_POPULATION; i++)
	{
		std::cout << "Init " << i << "/" << SIZE_POPULATION << "...\r" << std::flush;
		
		evaluate_individual<D>(pts, population[i], pcf_ref, N, M, K);
		if(population[i].Linf == 10)
		{
			init_individual<D>(i, population, tilesize);
			i--;
		}
	}*/
	
	//std::cout << "Init " << std::endl;
	//for(int i=0; i<10; i++)
	//	std::cout << population[i].Linf << " " << population[i].mindist << " " << population[i].permut << std::endl;
	
	std::sort(population.begin(), population.end(), compare_individuals<D>);
	
	//std::cout << "First sort " << std::endl;
	//for(int i=0; i<10; i++)
	//	std::cout << population[i].Linf << " " << population[i].mindist << " " << population[i].permut << std::endl;
	
	int NB_GENERATIONS_orig = NB_GENERATIONS;
	for(uint i=0; i<NB_GENERATIONS; i++)
	{
		for(uint i=SIZE_KEPT; i<SIZE_POPULATION; i++)
			cross_mutate_individual<D>(i, population, population_cross, SIZE_ELITE, tilesize);
		for(uint i=SIZE_KEPT; i<SIZE_POPULATION; i++)
			population[i] = population_cross[i];
		
		for(uint i=0; i<SIZE_POPULATION; i++)
			evaluate_individual<D>(pts, population[i], pcf_ref, N, M, K);
		
		std::sort(population.begin(), population.end(), compare_individuals<D>);
		
		double mean = 0;
		for(uint p=0; p<SIZE_KEPT; p++)
			mean += population[p].Linf;
		mean /= (double)SIZE_KEPT;
		//std::cout << "Generation " << i << "/" << NB_GENERATIONS << "     Avg Linf=" << mean << "       Min Linf= " << population[0].Linf << "        MinDist=" << population[0].mindist << "      \r" << std::flush;
		
		//std::cout << "                                                                                                                     \r" << std::flush;
		std::cout << "Generation " << i << "/" << NB_GENERATIONS << " == " << population[0].Linf << " " << population[0].mindist << " " << population[0].permut << "\r" << std::flush;
		//for(int i=0; i<1; i++)
		///	std::cout << population[i].Linf << " " << population[i].mindist << " " << population[i].permut << std::endl;
		
		if(population[0].Linf > 1.0 && NB_GENERATIONS < NB_GENERATIONS_orig*2)
			NB_GENERATIONS++;
	}
	//std::cout << "Permut    Linf=" << population[0].Linf << "    Min Dist=" << population[0].mindist << "                                                                           " << std::endl;
	
	
	//std::cout << std::endl << "#Linf_value\tL2_value\tmindist\tpermutx\tpermuty" << std::endl;
	/*for(int i=0; i<1; i++)
	{
		//std::cout << population[i].Linf << " " << population[i].L2 << " " << population[i].mindist << " " << population[i].permut << std::endl;

		Vector<D, bool*> permuts;
		for(uint d=0; d<D; d++)
			permuts[d] = (bool*)malloc(sizeof(bool)*(tilesize-1));
		for(uint d=0; d<D; d++)
			for(int b=0; b<tilesize-1; b++)
				permuts[d][b] = (population[i].permut[d] >> b) & 1;
		
		uint logKD=log2(tilesize);
		std::vector< std::array<uint, D> > x;
		x.resize(logKD-1);
		for(uint d=0; d<D; d++)
		{
			for(uint i=0; i<logKD-1; i++)
			{
				uint sum = (1 << (i+1))-1;
				x[i][d] = sum + pts[0].pos()[d]/(tilesize/pow(2, i+1));//tilesize -2 - sum - pts[0].pos()[d]/(tilesize/pow(2, i+1));
			}
		}
		for(uint d=0; d<D; d++)
		{
			permuts[d][0] = 0;
			for(uint i=0; i<logKD-1; i++)
				permuts[d][x[i][d]] = 0;
		}
		//for(uint d=0; d<D; d++)
		//{
		//	for(int b=0; b<tilesize-1; b++)
		//		std::cout << permuts[d][b] << " ";
		//	std::cout << std::endl;
		//}
		Pointset< D, uint, Point<D, uint> > scrambledset;
		pts.owen_scrambling(permuts, scrambledset);
		
		double d = 0;
		if(D==2)
			d = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*scrambledset.size()) );
		else if(D==3)
		{
			double maxpack = M_PI/(3.0*sqrt(2.0));
			d = 2.0 * cbrt( (maxpack/(double)tilesize) * (tgamma((double)D/2.0 + 1)/pow(M_PI, (double)D/2.0)) );
		}
	
		double smoothing = 0.1*d;
		double rmin = 0.15*d;//1.5*smoothing;
		double rmax = 5*d;

		PCF pcfengine;
		HistogramND<1, double, double> pcf_ps;
		pcfengine.compute<D, uint, Point<D, uint> >(scrambledset, pcf_ps, rmin, rmax, 100, smoothing);
		
		HistogramWriter<1, double, double> pcf_writer;
		pcf_writer.open("optipcf_"+std::to_string(population[i].permut[0])+"_"+std::to_string(population[i].permut[1])+".dat");
		pcf_writer.writeHistogram(pcf_ps);
		pcf_writer.close();
		
		HistogramND<1, double, double> pcf_diff;
		pcf_diff = pcf_ps - pcf_ref;
		pcf_writer.open("diffpcf_"+std::to_string(population[i].permut[0])+"_"+std::to_string(population[i].permut[1])+".dat");
		pcf_writer.writeHistogram(pcf_diff);
		pcf_writer.close();
		
		PointsetWriter< D, uint, Point<D, uint> > pts_writer;
		pts_writer.open("optipts_"+std::to_string(population[i].permut[0])+"_"+std::to_string(population[i].permut[1])+".dat");
		pts_writer.writePointset(scrambledset);
		pts_writer.close();
		
		for(uint d=0; d<D; d++)
			free(permuts[d]);
	}*/
	
	
	Vector<D, bool*> permuts;
	for(uint d=0; d<D; d++)
		permuts[d] = (bool*)malloc(sizeof(bool)*(tilesize-1));
	for(uint d=0; d<D; d++)
		for(uint i=0; i<tilesize-1; i++)
			permuts[d][i] = (population[0].permut[d] >> i) & 1;
	
	uint logKD=log2(tilesize);
	std::vector< std::array<uint, D> > x;
	x.resize(logKD-1);
	for(uint d=0; d<D; d++)
	{
		for(uint i=0; i<logKD-1; i++)
		{
			uint sum = (1 << (i+1))-1;
			x[i][d] = sum + pts[0].pos()[d]/(tilesize/pow(2, i+1));//tilesize -2 - sum - pts[0].pos()[d]/(tilesize/pow(2, i+1));
		}
	}
	for(uint d=0; d<D; d++)
	{
		permuts[d][0] = 0;
		for(uint i=0; i<logKD-1; i++)
			permuts[d][x[i][d]] = 0;
	}
	for(uint d=0; d<D; d++)
	{
		for(uint c=0; c<tilesize-1; c++)
		{
			resulting_permut[d] <<= 1;
			resulting_permut[d] |= permuts[d][c];
		}
	}
	for(uint d=0; d<D; d++)
		free(permuts[d]);

	return true;
}

template<uint D>
class SamplerVictor06022017_LUT
{
private:
	std::map< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >  data; //map linking key to permutation
	uint K; //subdiv factor
	
public:
	SamplerVictor06022017_LUT() { K=4; }
	SamplerVictor06022017_LUT(uint arg_K, uint arg_NBPERMUTS) { K=arg_K; }
	
	void setK(uint arg_K) { K=arg_K; }
	
	uint size() { return data.size(); }
	void clear() { data.clear(); }
	bool fill(const std::string& name)
	{
		std::ifstream file(name);
		if(!file.is_open())
		{
			std::ofstream newfile(name);
			newfile.close();
			file.open(name);
			if(!file.is_open())
				return false;
		}
		
		if(file.eof())
			return false;
		
		std::cout << "Filling LUT . . ." << std::endl;
		while(!file.eof())
		{
			uint tilesize = pow(K, D);
			SamplerVictor06022017_LUT_Key<D> key;
			key.pts.resize(tilesize);
			
			std::vector<uint> keyui;
			keyui.resize(tilesize*D);
			for(uint i=0; i<tilesize*D; i++)
				file >> keyui[i];
			
			for(uint i=0; i<tilesize; i++)
			{
				for(uint d=0; d<D; d++)
					key.pts[i].pos()[d] = keyui[D*i+d];
			}
			
			if(file.eof())
				break;
			
			Vector<D, unsigned long long int> p;
			for(uint d=0; d<D; d++)
				file >> p[d];
			data.insert(std::pair< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >(key, p));
		}
		
		std::cout << "Filled LUT with " << data.size() << " patterns" << std::endl;
		file.close();
		return true;
	}
	bool write(const std::string& name)
	{
		std::ofstream file(name);
		if(!file.is_open())
			return false;
		
		std::cout << "Writing LUT . . ." << std::endl;
		for(auto it=data.begin(); it!=data.end(); it++)
		{
			std::vector<uint> keyui;
			uint tilesize = pow(K, D);
			keyui.resize(tilesize*D);
			
			for(uint i=0; i<tilesize; i++)
			{
				for(uint d=0; d<D; d++)
					keyui[D*i+d] = it->first.pts[i].pos()[d].to_ulong();
			}

			for(uint i=0; i<tilesize*D; i++)
				file << keyui[i] << "\t";
			
			Vector<D, uint> p;
			for(uint d=0; d<D-1; d++)
				file << it->second[d] << "\t";
			file << it->second[D-1] << std::endl;
		}
		
		std::cout << "Written LUT with " << data.size() << " patterns" << std::endl;
		file.close();
		return true;
	}
	bool getAnOptimizedPermut(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut)
	{
		//std::map< SamplerVictor06022017_LUT_Key<D>, Vector<D, uint> >::iterator it;
		auto it = data.find(key);
		if(it == data.end())
			return false;
		resulting_permut = it->second;
		return true;
	}
	
	bool getARandomPermutPerservingZero(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& permuts)
	{
		geneticOptimizer<D>(key, permuts, K, 0);
		
		data.insert(std::pair< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >(key, permuts));
		
		return true;
	}
	
	bool getANullPermut(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& permuts)
	{
		//geneticOptimizer<D>(key, permuts, K, 0);
		for(int d=0; d<D; d++)
			permuts[d] = 0;
		
		data.insert(std::pair< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >(key, permuts));
		
		return true;
	}
	
	bool computeAndGetAnOptimizedPermut(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut)
	{
		/*if(D==2)
			optimizePermut2D(key, resulting_permut, K);
		if(D==3)
			optimizePermut3D(key, resulting_permut, K);*/
		geneticOptimizer<D>(key, resulting_permut, K);
		
		data.insert(std::pair< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >(key, resulting_permut));
		
		//std::cout << "Not implemented yet" << std::endl;
		//exit(1);
        return true;
	}
};

}

#endif
