#ifndef _UTK_BNLDS_LUT_06022017_
#define _UTK_BNLDS_LUT_06022017_

#include "../../utils.hpp"
#include "../../pointsets/Pointset.hpp"
#include "../../pointsets/Histogram.hpp"
#include "../SamplerSobolIndices.hpp"
#include "../../io/histogramIO.hpp"
#include "../../io/fileIO.hpp"
#include "../../statistics/PCF.hpp"
#include "SamplerBNLDS_GPU_optim.hpp"
#include <map>
#include <cstdlib>


namespace utk
{
	
enum { FILLTREE_ZERO, FILLTREE_RANDOM, FILLTREE_RANDOMOPTI };

Histogram1dd SamplerBNLDS_targetpcf;
const uint SamplerBNLDS_BITSETSIZE=64;

template<uint D>
class SamplerBNLDS_LUT_Key
{
public:
	Pointset< D, std::bitset<SamplerBNLDS_BITSETSIZE>, Point<D, std::bitset<SamplerBNLDS_BITSETSIZE> > > pts;
	
	SamplerBNLDS_LUT_Key() {}
	SamplerBNLDS_LUT_Key(Pointset< D, std::bitset<SamplerBNLDS_BITSETSIZE>, Point<D, std::bitset<SamplerBNLDS_BITSETSIZE> > >& arg_pts) { pts=arg_pts; }

	/**
	 * Needed by std::map
	 ***/
	bool operator<(const SamplerBNLDS_LUT_Key<D>& k) const
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
class SamplerBNLDS_LUT
{
private:
	std::map< SamplerBNLDS_LUT_Key<D>, Vector<D, unsigned long long int> >  data; //map linking key to permutation
	uint K; //subdiv factor
	
public:
	SamplerBNLDS_LUT() { K=4; }
	SamplerBNLDS_LUT(uint arg_K, uint arg_NBPERMUTS) { K=arg_K; }
	
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
		{
			std::cout << "Empty LUT . . ." << std::endl;
			return false;
		}
		
		std::cout << "Filling LUT . . ." << std::endl;
		while(!file.eof())
		{
			uint tilesize = pow(K, D);
			SamplerBNLDS_LUT_Key<D> key;
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
			data.insert(std::pair< SamplerBNLDS_LUT_Key<D>, Vector<D, unsigned long long int> >(key, p));
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
	
	#define PCF_FILE "../../data/BNLDS/PCF/pcfLDBN1024.dat"
	bool computeAnOptimizedPermut(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut)
	{
		/*int id = drand48()*10000;
		std::stringstream filename;
		filename << "pattern_tmp_" << id << ".dat";
		std::ofstream file(filename.str());
		for(int i=0; i<key.pts.size(); i++)
			file << key.pts[i].pos()[0].to_ulong() << " " << key.pts[i].pos()[1].to_ulong() << std::endl;
		file.close();
		
		std::stringstream cmd;
		cmd << "./bin/samplers/BNLDS_optimize_pattern_2dd " << filename.str() << " ../../data/BNLDS/PCF/pcfLDBN1024.dat";
		std::cout << cmd.str() << std::endl;
		system(cmd.str().c_str());
		
		std::stringstream filename2;
		filename2 << filename.str() << ".res";
		std::ifstream file2(filename2.str());
		file2 >> resulting_permut[0] >> resulting_permut[1];
		file2.close();
		
		std::stringstream cmd2;
		cmd << "rm " << filename.str();
		system(cmd2.str().c_str());
		std::stringstream cmd3;
		cmd << "rm " << filename.str() << ".res";
		system(cmd3.str().c_str());*/
		
		if(SamplerBNLDS_targetpcf.size() == 0)
		{
			HistogramReader<1, double, double> histo_reader;
			histo_reader.open(PCF_FILE);
			histo_reader.readHistogram(SamplerBNLDS_targetpcf);
			histo_reader.close();
		}
		
		Pointset<2, unsigned int, Point<2, unsigned int> > pts;
		pts.resize(key.pts.size());
		for(int i=0; i<key.pts.size(); i++)
		{
			pts[i].pos()[0] = key.pts[i].pos()[0].to_ulong();
			pts[i].pos()[1] = key.pts[i].pos()[1].to_ulong();
		}
		optimizeGPU(pts, SamplerBNLDS_targetpcf, resulting_permut);
		
		data.insert(std::pair< SamplerBNLDS_LUT_Key<D>, Vector<D, unsigned long long int> >(key, resulting_permut));

        return true;
	}
	
	bool getRandomPermut(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut)
	{
		ERROR("getRandomPermut not implemented yet");
		exit(1);
	}
	
	bool getSpecificPermut(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut, unsigned long long int permut_val)
	{
		ERROR("getSpecificPermut not implemented yet");
		exit(1);
	}
	
	bool getAnOptimizedPermut(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut)
	{
		auto it = data.find(key);
		if(it == data.end())
			return false;
		resulting_permut = it->second;
		return true;
	}
	
	bool optiMinDist = 0;
	bool optiPCF = 0;
	bool fillEndTreeZero = 0;
	bool fillEndTreeRandom = 0;
	bool fillEndTreeRandomOpti = 0;
};

}

#endif