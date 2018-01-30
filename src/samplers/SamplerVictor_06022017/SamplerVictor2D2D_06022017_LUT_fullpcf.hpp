#ifndef _UTK_SAMPLER_VICTOR_LUT_06022017_
#define _UTK_SAMPLER_VICTOR_LUT_06022017_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include "../pointsets/Histogram.hpp"
#include "../SamplerSobolIndices.hpp"
#include "../io/histogramIO.hpp"
#include "../io/fileIO.hpp"
#include "../statistics/PCF.hpp"
#include <map>
#include <cstdlib>

#ifdef TBB_ON
#include <tbb/tbb.h>
#endif

#define PCF_FILE "step_4096pts_rmin15_rmax5_sigma01_nbbins200.dat"

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
struct evaluation
{
	Vector<D, unsigned long long int> permut;
	double Linf;
	double mindist;
	double L2;
};

template<uint D>
void evaluate(Pointset<D, uint, Point<D, uint> >& pts, evaluation<D>& i0, HistogramND<1, double, double>& pcf_ref, uint N, uint M, uint K)
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
		
		pcf_engine.compute< 2, uint, Point<2, uint> >(pts_proj, pcf_set, rmin, rmax, pcf_ref.size(), smoothing);
		
		double Linf_value = 0; 
		pcf_set.Linf(pcf_ref, Linf_value);
		
		if(i0.Linf < Linf_value)
			i0.Linf = Linf_value;
	}
	
	for(uint d=0; d<2; d++)
		free(permuts[d]);
}

template<uint D>	
bool exhaustiveSearch(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut, uint K=4, uint fullsearch=0)
{	
	if(D != 2)
	{
		ERROR("SamplerVictor06022017_LUT_fullpcf::exhaustiveSearch is only valid for D=2");
		exit(1);
	}
	
	HistogramND<1, double, double> pcf_ref;
	HistogramReader<1, double, double> pcfreader;
	
	pcfreader.open(PCF_FILE);
	pcfreader.readHistogram(pcf_ref);
	pcfreader.close();
	
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
	
	unsigned long long int M = tilesize-1;
	unsigned long long int N = M - ceil(log2(tilesize));
	
	evaluation<D> best_eval;
	best_eval.Linf=10;
	best_eval.mindist=1;
	for(unsigned long long int i=0; i<(unsigned long long int)(1<<N); i++)
	{
	for(unsigned long long int j=0; j<(unsigned long long int)(1<<N); j++)
	{
		evaluation<D> current_eval;
		current_eval.Linf=10;
		current_eval.mindist=1;
		current_eval.permut[0] = i;
		current_eval.permut[1] = j;
		
		evaluate<D>(pts, current_eval, pcf_ref, N, M, K);
		
		if(current_eval.Linf < best_eval.Linf)
		{
			best_eval.Linf = current_eval.Linf;
			best_eval.mindist = current_eval.mindist;
			best_eval.permut[0] = current_eval.permut[0];
			best_eval.permut[1] = current_eval.permut[1];
		}
	}
	
	std::cout << "                                                         \r" << std::flush;
	std::cout << i << "/ "<<  (unsigned long long int)(1<<N);
	std::cout << "   Best == " << best_eval.Linf << " " << best_eval.mindist << " " << best_eval.permut << "\r" << std::flush;
	
	}
	
	
	Vector<D, bool*> permuts;
	for(uint d=0; d<D; d++)
		permuts[d] = (bool*)malloc(sizeof(bool)*(tilesize-1));
	for(uint d=0; d<D; d++)
		for(uint i=0; i<tilesize-1; i++)
			permuts[d][i] = (best_eval.permut[d] >> i) & 1;
	
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
		exhaustiveSearch<D>(key, permuts, K, 0);
		
		data.insert(std::pair< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >(key, permuts));
		
		return true;
	}
	
	bool getANullPermut(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& permuts)
	{
		for(int d=0; d<D; d++)
			permuts[d] = 0;
		
		data.insert(std::pair< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >(key, permuts));
		
		return true;
	}
	
	bool computeAndGetAnOptimizedPermut(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut)
	{
		exhaustiveSearch<D>(key, resulting_permut, K);
		
		data.insert(std::pair< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >(key, resulting_permut));
		
        return true;
	}
};

}

#endif
