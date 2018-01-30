#ifndef _UTK_BNLDS_LUT_06022017_
#define _UTK_BNLDS_LUT_06022017_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include "../pointsets/Histogram.hpp"
#include "../SamplerSobolIndices.hpp"
#include "../io/histogramIO.hpp"
#include "../io/fileIO.hpp"
#include "../statistics/PCF.hpp"
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
void permut(Pointset<D, uint, Point<D, uint> >& pts, Pointset<D, uint, Point<D, uint> >& out_pts, Vector<D, unsigned long long int> permuts, uint K) {
	
	uint tilesize = pow(K, D);
	
	int ind;
	std::bitset<SamplerBNLDS_BITSETSIZE> xpermuts[D];
	for(uint d=0; d<D; d++)
		xpermuts[d] = permuts[d];

	uint NBITS = log2(tilesize);
	uint PERMUTS_NBITS = tilesize-1;
	
	for(uint ipt=0; ipt<tilesize; ipt++) {
		std::bitset<SamplerBNLDS_BITSETSIZE> xresdigits[D];
		std::bitset<SamplerBNLDS_BITSETSIZE> thisSobolPt[D];
		for(uint d=0; d<D; d++)
			thisSobolPt[d] = pts[ipt].pos()[d];

		for(uint ilevel=0; ilevel<NBITS; ilevel++) {
			
			for(uint d=0; d<D; d++)
			{
				ind = (pts[ipt].pos()[d] >> (NBITS-ilevel)) + (pow(2,ilevel)-1);
				xresdigits[d].set(NBITS-ilevel-1, (xpermuts[d][PERMUTS_NBITS-ind-1] ^ thisSobolPt[d][NBITS-ilevel-1]) );
			}
		}
		
		for(uint d=0; d<D; d++)
			out_pts[ipt].pos()[d] = xresdigits[d].to_ulong();
	}
}

template<uint D>
double maximalMinimumDistance(Pointset<D, uint, Point<D, uint> > pts) {
	
	double mindist = 10000000;
	double tilesize = pts.size();
	
	for (uint ipts = 0; ipts < pts.size(); ipts++) {
		for (uint ipts_to_compare = ipts+1; ipts_to_compare < pts.size(); ipts_to_compare++) {
			double dist_sq = 0;
			for(uint d=0; d<D; d++)
			{
				double dx = (double)pts[ipts].pos()[d]/tilesize - (double)pts[ipts_to_compare].pos()[d]/tilesize;
				dist_sq += dx*dx;
			}
			if (dist_sq < mindist) mindist = dist_sq;
		}
	}
	return sqrt(mindist);
}

#define PCF_FILE "../data/BNLDS/PCF/pcfLDBN1024.dat"
template<uint D>
double PCFDistance(Pointset<D, uint, Point<D, uint> > pts) 
{
	double tilesize = pts.size();
	Pointset<2, double, Point<2, double> > pts2;
	pts2.resize(tilesize);
	for(uint i=0; i<tilesize; i++)
	{
		pts2[i].pos()[0] = (double)pts[i].pos()[0] / tilesize;
		pts2[i].pos()[1] = (double)pts[i].pos()[1] / tilesize;
	}
	
	if(SamplerBNLDS_targetpcf.size() == 0)
	{
		HistogramReader<1, double, double> histo_reader;
		histo_reader.open(PCF_FILE);
		histo_reader.readHistogram(SamplerBNLDS_targetpcf);
		histo_reader.close();
	}
	PCF pcf_engine;
	
	double d = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*tilesize) );
	double smoothing = 0.1*d;
	uint nbbins = SamplerBNLDS_targetpcf.size();
	double rmin = 1.5*smoothing;
	double rmax = 2.5*d;
	
	Histogram1dd pcf_data;
	if(!pcf_engine.compute< 2, double, Point<2, double> >(pts2, pcf_data, rmin, rmax, nbbins, smoothing, true))
		return 1;
	
	double linf=0;
	pcf_data.Linf(SamplerBNLDS_targetpcf, linf);
	return linf;
}

template<uint D>
bool permutFromNbitsToMbits(uint N, uint M, const Vector<D, uint>& fixed_point, const Vector<D, unsigned long long int>& permutN, Vector<D,unsigned long long int>& permutM, uint K)
{
	int tilesize = pow(K, D);
	int LOGK= log2(tilesize);
	
	uint fixed[D];
	for(uint d=0; d<D; d++)
		fixed[d] = fixed_point[d];
	
	
	//For K=4
	//ind
	//
	//0              14
	//1         13        12
	//2      11   10    9    8
	//3     7 6  5 4   3 2   1 0
	//
	//corresponding to sample pos
	//      1 3  5 7   9 11  13 15
	//      0 2  4 6   8 10  12 14
	
	std::vector< std::array<uint, D> > x;
	x.resize(LOGK-1);
	for(uint d=0; d<D; d++)
	{
		int nb_flags = 2;
		int orig_lvl = tilesize-2 -1;
		for(uint i=1; i<LOGK; i++)
		{
			x[i-1][d] = orig_lvl - (fixed[d]/(tilesize/nb_flags));
			orig_lvl -= nb_flags;
			nb_flags*=2;
		}
	}
	std::bitset<SamplerBNLDS_BITSETSIZE> xpermutsNbits[D];
	for(uint d=0; d<D; d++)
		xpermutsNbits[d] = permutN[d];
	
	std::bitset<SamplerBNLDS_BITSETSIZE> xpermutsMbits[D];
	int xcount[D];
	for(uint d=0; d<D; d++)
		xcount[d] = tilesize-LOGK-2; // bitset indexing: 0==least significant bit
	for(uint i=1; i<tilesize-1; i++) { // start form the second i because xpermuts15bits[0].x = xpermuts15bits[0].y = 0;
		int ind = tilesize-2-i;
		
		for(uint d=0; d<D; d++)
		{
			bool isFlag=true;
			for(uint i=0; i<LOGK-1; i++)
				isFlag &= (ind != x[i][d]);
			
			if( isFlag ) {
				xpermutsMbits[d][ind] = xpermutsNbits[d][xcount[d]];
				xcount[d]--;
			}
		}
	}
	
	for(uint d=0; d<D; d++)
		permutM[d] = xpermutsMbits[d].to_ullong();
	
	return true;
}

void show_tree(Vector<2, unsigned long long int>& permut, uint K)
{
	uint tilesize = pow(K, 2);
	uint depth = log2(tilesize);
	

	std::bitset<SamplerBNLDS_BITSETSIZE> tree = permut[0];
	std::cout << "Permut " << permut[0] << " = " << tree.to_ullong() << std::endl;

	int n=tilesize-2;
	for(uint i=0; i<depth; i++)
	{
		for(int j=0; j<(1<<i); j++)
		{
			std::cout << tree[n] << " ";
			n--;
		}
		std::cout << std::endl;
	}
}

template<uint D>
bool optimizePermutMinDist(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut, uint K, uint filltype=0)
{
	uint tilesize = pow(K, D);
	
	assert(key.pts.size() == tilesize);

	Pointset<2, uint, Point<2, uint> > pts;
	pts.resize(tilesize);
	for(uint i=0; i<tilesize; i++)
	{
		pts[i].pos()[0] = key.pts[i].pos()[0].to_ulong();
		pts[i].pos()[1] = key.pts[i].pos()[1].to_ulong();
	}
	
	uint N, M;
	M = tilesize-1;
	N = M - log2(tilesize);
	
	uint free_digits = std::min(N, 11u);
	//std::cout << N << " total digits " << free_digits << " free digits" << std::endl;

	uint rand_offset_x = drand48()*(1 << free_digits);
	uint rand_offset_y = drand48()*(1 << free_digits);
	
	double max_mindist = 0;
	for (int ix = 0; ix < (1 << free_digits); ix++) {
		
		std::cout << ix << "/" <<  (1 << free_digits) << "                  \r" << std::flush;
		for (int iy = 0; iy < (1 << free_digits); iy++) {
			
#define FASTOPTIM_MINDIST
#ifdef FASTOPTIM_MINDIST
			if(max_mindist > 0.2 && K==4)
				break;
			if(max_mindist > 0.08 && K==8)
				break;
#endif
			
			Vector<2, unsigned long long int> permutsNbits, permutsMbits;
			
			uint indexx = (ix + rand_offset_x)%(1 << free_digits);
			uint indexy = (iy + rand_offset_y)%(1 << free_digits);
			permutsNbits[0] = (unsigned long long int)((unsigned long long int)indexx << (unsigned long long int)(N-free_digits));
			permutsNbits[1] = (unsigned long long int)((unsigned long long int)indexy << (unsigned long long int)(N-free_digits));
			//std::cout << "permut " << permutsNbits << std::endl;
			
			Vector<2, uint> fixed_point = pts[0].pos();
			permutFromNbitsToMbits<D>(N, M, fixed_point, permutsNbits, permutsMbits, K);
			
			//show_tree(permutsMbits, K);
			
			Pointset<2, uint, Point<2, uint> > out_pts;
			out_pts.resize(tilesize);
			permut<D>(pts, out_pts, permutsMbits, K);
			double mindist = maximalMinimumDistance<D>(out_pts);
			
//#define DEBUG
#ifdef DEBUG
			{
				std::stringstream file;
				file << "sv_truepermut_" << ix <<"_" << iy << "_" << mindist << ".dat";
				std::ofstream iofile (file.str());
				for(int i=0; i<tilesize; i++)
					iofile << out_pts[i].pos() << std::endl;
				iofile.close();
			}
#endif

			if(mindist > max_mindist)
			{
				std::cout << "Dist " << mindist << " / " << 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*N) ) << std::endl;
				max_mindist = mindist;
				resulting_permut = permutsNbits;
			}
		}
	}
	
	Vector<2, unsigned long long int> permutsNbits, permutsMbits;
	if(filltype == FILLTREE_RANDOMOPTI)
	{
		double mindist = max_mindist;
		if(N > free_digits)
		{
			int loops= 10000;
			int iter = 100;
			while(mindist <= max_mindist && loops > 0)
			{
				if(iter-- == 0)
				{
					loops--;
					iter=100;
					std::cout << "(" << loops << ")" << mindist << "             \r" << std::flush;
				}
				
				unsigned long long int addx = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << (N-free_digits)));
				unsigned long long int addy = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << (N-free_digits)));
				
				permutsNbits[0] = resulting_permut[0] + addx;
				permutsNbits[1] = resulting_permut[1] + addy;
				
				Vector<2, uint> fixed_point = pts[0].pos();
				permutFromNbitsToMbits<D>(N, M, fixed_point, permutsNbits, permutsMbits, K);
			
				//show_tree(permutsMbits, K);
				
				Pointset<2, uint, Point<2, uint> > out_pts;
				out_pts.resize(tilesize);
				permut<D>(pts, out_pts, permutsMbits, K);
				mindist = maximalMinimumDistance<D>(out_pts);
				
				//std::cout << permutsNbits << " -- " << mindist << std::endl;
			}
		}
		if(mindist <= max_mindist)
			permutsNbits = resulting_permut;
		else
		{
			max_mindist = mindist;
			std::cout << "Dist " << max_mindist << " / " << 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*N) ) << std::endl;
		}
	}
	else if (filltype == FILLTREE_RANDOM)
	{
		unsigned long long int addx = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << (N-free_digits)));
		unsigned long long int addy = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << (N-free_digits)));
		permutsNbits[0] = resulting_permut[0] + addx;
		permutsNbits[1] = resulting_permut[1] + addy;
	}
	else if (filltype == FILLTREE_ZERO)
	{
		permutsNbits[0] = resulting_permut[0];
		permutsNbits[1] = resulting_permut[1];
	}
	else
	{
		ERROR("Wrong choice for filling the bottom of the optimized permutation tree");
		exit(1);
	}
	
	Vector<2, uint> fixed_point = pts[0].pos();
	permutFromNbitsToMbits<D>(N, M, fixed_point, permutsNbits, permutsMbits, K);
	resulting_permut = permutsMbits;
	
	return true;
}


template<uint D>
bool optimizePermutPCF(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut, uint K, int filltype=0)
{
	uint tilesize = pow(K, D);
	
	assert(key.pts.size() == tilesize);

	Pointset<2, uint, Point<2, uint> > pts;
	pts.resize(tilesize);
	for(uint i=0; i<tilesize; i++)
	{
		pts[i].pos()[0] = key.pts[i].pos()[0].to_ulong();
		pts[i].pos()[1] = key.pts[i].pos()[1].to_ulong();
	}
	
	uint N, M;
	M = tilesize-1;
	N = M - log2(tilesize);
	
	uint free_digits = std::min(N, 11u);

	uint rand_offset_x = drand48()*(1 << free_digits);
	uint rand_offset_y = drand48()*(1 << free_digits);
	
	double min_pcfdist = 10000;
	for (int ix = 0; ix < (1 << free_digits); ix++) {
		
		std::cout << ix << "/" <<  (1 << free_digits) << "                  \r" << std::flush;
		for (int iy = 0; iy < (1 << free_digits); iy++) {
			
#define FASTOPTIM_PCF
#ifdef FASTOPTIM_PCF
			if(min_pcfdist < 0.45 && K==4)
				break;
			if(min_pcfdist < 0.45 && K==8)
				break;
#endif
			
			Vector<2, unsigned long long int> permutsNbits, permutsMbits;
			
			uint indexx = (ix + rand_offset_x)%(1 << free_digits);
			uint indexy = (iy + rand_offset_y)%(1 << free_digits);
			permutsNbits[0] = (unsigned long long int)((unsigned long long int)indexx << (unsigned long long int)(N-free_digits));
			permutsNbits[1] = (unsigned long long int)((unsigned long long int)indexy << (unsigned long long int)(N-free_digits));
			//std::cout << "permut " << permutsNbits << std::endl;
			
			Vector<2, uint> fixed_point = pts[0].pos();
			permutFromNbitsToMbits<D>(N, M, fixed_point, permutsNbits, permutsMbits, K);
			
			//show_tree(permutsMbits, K);
			
			Pointset<2, uint, Point<2, uint> > out_pts;
			out_pts.resize(tilesize);
			permut<D>(pts, out_pts, permutsMbits, K);
			
			double mindist_pt = maximalMinimumDistance<D>(out_pts);
			if(K==8 && mindist_pt < 0.08)
				continue;
			if(K==4 && mindist_pt < 0.15)
				continue;
			
			double pcfdist = PCFDistance<D>(out_pts);

#ifdef DEBUG
			{
				std::stringstream file;
				file << "sv_truepermut_" << ix <<"_" << iy << "_" << mindist << ".dat";
				std::ofstream iofile (file.str());
				for(int i=0; i<tilesize; i++)
					iofile << out_pts[i].pos() << std::endl;
				iofile.close();
			}
#endif

			if(pcfdist < min_pcfdist)
			{
				std::cout << "Dist " << pcfdist << std::endl;
				min_pcfdist = pcfdist;
				resulting_permut = permutsNbits;
			}
		}
	}
	
	Vector<2, unsigned long long int> permutsNbits, permutsMbits;
	if(filltype == FILLTREE_RANDOMOPTI)
	{
		double pcfdist = min_pcfdist;
		if(N > free_digits)
		{
			int loops= 10000;
			int iter = 100;
			while(pcfdist >= min_pcfdist && loops > 0)
			{
				if(iter-- == 0)
				{
					loops--;
					iter=100;
					std::cout << "(" << loops << ")" << pcfdist << "             \r" << std::flush;
				}
				
				unsigned long long int addx = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << (N-free_digits)));
				unsigned long long int addy = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << (N-free_digits)));
				
				permutsNbits[0] = resulting_permut[0] + addx;
				permutsNbits[1] = resulting_permut[1] + addy;
				
				Vector<2, uint> fixed_point = pts[0].pos();
				permutFromNbitsToMbits<D>(N, M, fixed_point, permutsNbits, permutsMbits, K);
				
				Pointset<2, uint, Point<2, uint> > out_pts;
				out_pts.resize(tilesize);
				permut<D>(pts, out_pts, permutsMbits, K);
				
				double mindist_pt = maximalMinimumDistance<D>(out_pts);
				if(mindist_pt < 0.07)
					continue;
				
				pcfdist = PCFDistance<D>(out_pts);
			}
		}
		if(pcfdist >= min_pcfdist)
			permutsNbits = resulting_permut;
		else
		{
			min_pcfdist = pcfdist;
			std::cout << "Dist " << min_pcfdist << std::endl;
		}
	}
	else if (filltype == FILLTREE_RANDOM)
	{
		unsigned long long int addx = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << (N-free_digits)));
		unsigned long long int addy = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << (N-free_digits)));
		permutsNbits[0] = resulting_permut[0] + addx;
		permutsNbits[1] = resulting_permut[1] + addy;
	}
	else if (filltype == FILLTREE_ZERO)
	{
		permutsNbits[0] = resulting_permut[0];
		permutsNbits[1] = resulting_permut[1];
	}
	else
	{
		ERROR("Wrong choice for filling the bottom of the optimized permutation tree");
		exit(1);
	}
	
	Vector<2, uint> fixed_point = pts[0].pos();
	permutFromNbitsToMbits<D>(N, M, fixed_point, permutsNbits, permutsMbits, K);
	resulting_permut = permutsMbits;
	
	return true;
}



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
	
	bool computeAnOptimizedPermut(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut)
	{
		int filltype = 123;
		if(fillEndTreeZero) filltype = FILLTREE_ZERO;
		else if(fillEndTreeRandom) filltype = FILLTREE_RANDOM;
		else if(fillEndTreeRandomOpti) filltype = FILLTREE_RANDOMOPTI;
		else 
		{
			std::cout << "No fill tree heuristic specified" << std::endl;
			exit(1);
		}
		
		if(optiMinDist)
		{
			std::cout << "Optimize pattern using Min Dist, Filling with " << filltype << " ..." << std::endl;
			optimizePermutMinDist<D>(key, resulting_permut, K, filltype);
		}
		else if(optiPCF)
		{
			std::cout << "Optimize pattern using PCF, Filling with " << filltype << " ..." << std::endl;
			optimizePermutPCF<D>(key, resulting_permut, K, filltype);
		}
		else 
		{
			std::cout << "No optimisation heuristic specified" << std::endl;
			exit(1);
		}
		
		data.insert(std::pair< SamplerBNLDS_LUT_Key<D>, Vector<D, unsigned long long int> >(key, resulting_permut));

        return true;
	}
	
	bool getRandomPermut(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut)
	{
		std::cout << "Random pattern ..." << std::endl;
		
		uint tilesize = pow(K, D);
		assert(key.pts.size() == tilesize);
		
		Point<2, uint> pt;
		pt.pos()[0] = key.pts[0].pos()[0].to_ulong();
		pt.pos()[1] = key.pts[0].pos()[1].to_ulong();

		uint N, M;
		M = tilesize-1;
		N = M - log2(tilesize);
	
		Vector<2, unsigned long long int> permutsNbits;
		unsigned long long int addx = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << N));
		unsigned long long int addy = (unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << N));
		permutsNbits[0] = addx;
		permutsNbits[1] = addy;

		permutFromNbitsToMbits<D>(N, M, pt.pos(), permutsNbits, resulting_permut, K);
		
		data.insert(std::pair< SamplerBNLDS_LUT_Key<D>, Vector<D, unsigned long long int> >(key, resulting_permut));

        return true;
	}
	
	bool getSpecificPermut(SamplerBNLDS_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut, unsigned long long int permut_val)
	{
		std::cout << "Specific pattern ..." << std::endl;
		
		uint tilesize = pow(K, D);
		assert(key.pts.size() == tilesize);
		
		Point<2, uint> pt;
		pt.pos()[0] = key.pts[0].pos()[0].to_ulong();
		pt.pos()[1] = key.pts[0].pos()[1].to_ulong();

		uint N, M;
		M = tilesize-1;
		N = M - log2(tilesize);
	
		Vector<2, unsigned long long int> permutsNbits;
		unsigned long long int addx = permut_val;//(unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << N));
		unsigned long long int addy = 0;//(unsigned long long int)(drand48()*(unsigned long long int)((unsigned long long int)1 << N));
		permutsNbits[0] = addx;
		permutsNbits[1] = addy;

		permutFromNbitsToMbits<D>(N, M, pt.pos(), permutsNbits, resulting_permut, K);
		
		data.insert(std::pair< SamplerBNLDS_LUT_Key<D>, Vector<D, unsigned long long int> >(key, resulting_permut));

        return true;
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
