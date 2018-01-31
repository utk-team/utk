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
void permut(Pointset<D, uint, Point<D, uint> >& pts, Pointset<D, uint, Point<D, uint> >& out_pts, Vector<D, unsigned long long int> permuts, uint K=4) {
	
	uint tilesize = pow(K, D);
	
	int ind;
	std::bitset<SamplerVictor06022017_BITSETSIZE> xpermuts[D];
	for(uint d=0; d<D; d++)
		xpermuts[d] = permuts[d];

	uint NBITS = log2(tilesize);
	uint PERMUTS_NBITS = tilesize-1;
	
	for(uint ipt=0; ipt<tilesize; ipt++) {
		std::bitset<SamplerVictor06022017_BITSETSIZE> xresdigits[D];
		std::bitset<SamplerVictor06022017_BITSETSIZE> thisSobolPt[D];
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

/*template<uint D>
bool permutFrom11bitsTo15bits(const Vector<D, uint>& fixed_point, const Vector<D, unsigned long long int>& permut11, Vector<D,unsigned long long int>& permut15)
{
	int K = pow(4, D);
	int LOGK= log2(K);
	
	uint fixed[D];
	for(uint d=0; d<D; d++)
		fixed[d] = fixed_point[d];
	
	uint x1[D];
	uint x2[D];
	uint x3[D];
	
	for(uint d=0; d<D; d++)
	{
		x1[d] = 14 - 1 - fixed[d]/8, x2[d] = 14 - 1 - 2 - fixed[d]/4, x3[d] = 14 - 1 - 2 - 4 - fixed[d]/2;
	}
	std::bitset<SamplerVictor06022017_BITSETSIZE> xpermuts11bits[D];
	for(uint d=0; d<D; d++)
		xpermuts11bits[d] = permut11[d];
	
	std::bitset<SamplerVictor06022017_BITSETSIZE> xpermuts15bits[D];
	int xcount[D];
	for(uint d=0; d<D; d++)
		xcount[d] = K-LOGK-2; // bitset indexing: 0==least significant bit
	for(uint i=1; i<K-1; i++) { // start form the second i because xpermuts15bits[0].x = xpermuts15bits[0].y = 0;
		int ind = K-2-i;
		
		for(uint d=0; d<D; d++)
		{
			if( (ind != x1[d]) && (ind != x2[d]) && (ind != x3[d]) ) {
				xpermuts15bits[d][ind] = xpermuts11bits[d][xcount[d]];
				xcount[d]--;
			}
		}
	}
	
	for(uint d=0; d<D; d++)
		permut15[d] = xpermuts15bits[d].to_ulong();
	
	return true;
}*/

template<uint D>
bool permutFromNbitsToMbits(uint N, uint M, const Vector<D, uint>& fixed_point, const Vector<D, unsigned long long int>& permut11, Vector<D,unsigned long long int>& permut15, uint K=4)
{
	int tilesize = pow(K, D);
	
	//std::cout << "N=" << N << " M=" << M << " D=" << D << " tilesize=" << tilesize << std::endl;

	int LOGTILESIZE= log2(tilesize);
	
	uint fixed[D];
	for(uint d=0; d<D; d++)
		fixed[d] = fixed_point[d];
	
	//std::cout << "Fix " << fixed_point << std::endl;
	
	std::vector< std::array<uint, D> > x;
	x.resize(LOGTILESIZE-1);
	for(uint d=0; d<D; d++)
	{
		for(uint i=0; i<LOGTILESIZE-1; i++)
		{
			uint sum = (1 << (i+1))-1;
			uint length = tilesize/pow(2, i+1);
			
			x[i][d] = (tilesize-2) -sum -(fixed[d]/length); //tilesize-2 = index of the last flag; sum = index first flag at level i+1; length = lenght of the level i+1
		}
	}
	
	/*for(uint i=0; i<LOGTILESIZE-1; i++)
	for(uint d=0; d<D; d++)
		std::cout << "Locked " << d << " " << x[i][d] << std::endl;*/
	
	std::bitset<SamplerVictor06022017_BITSETSIZE> xpermuts11bits[D];
	for(uint d=0; d<D; d++)
		xpermuts11bits[d] = permut11[d];
	
	std::bitset<SamplerVictor06022017_BITSETSIZE> xpermuts15bits[D];
	int xcount[D];
	for(uint d=0; d<D; d++)
		xcount[d] = tilesize-LOGTILESIZE-2; // bitset indexing: 0==least significant bit
	for(uint i=1; i<tilesize-1; i++) { // start form the second i because xpermuts15bits[0].x = xpermuts15bits[0].y = 0;
		int ind = tilesize-2-i;
		
		for(uint d=0; d<D; d++)
		{
			bool isFlag=true;
			for(uint i=0; i<LOGTILESIZE-1; i++)
				isFlag &= (ind != x[i][d]);
			
			if( isFlag ) {
				xpermuts15bits[d][ind] = xpermuts11bits[d][xcount[d]];
				xcount[d]--;
			}
		}
	}
	
	for(uint d=0; d<D; d++)
		permut15[d] = xpermuts15bits[d].to_ullong();
	
	return true;
}

template<uint D>
bool optimizePermut(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& resulting_permut, uint tilesize=16)
{
	//std::cout << "Optimizing ..." << std::endl;
	
	assert(key.pts.size() == tilesize);
	uint K = log(tilesize) / log(D);
		
	
	/*bool mmd_5_found = 0;
	bool mmd_8_found = 0;
	bool mmd_10_found = 0;
	
	Vector<2, uint> mmd_5;
	Vector<2, uint> mmd_8;
	Vector<2, uint> mmd_10;
	Vector<2, uint> mmd_13;*/
	
	int current_min = 1000;
	Vector<2, unsigned long long int> current_permut;
	
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
	
	//uint x_random_array_switch = 1000*drand48();
	//uint y_random_array_switch = 1000*drand48();
	
	uint x_random_ind_switch = 1000*drand48();
	uint y_random_ind_switch = 1000*drand48();
	
	for (int ix = 0; ix < (1 << N); ix++) {
	for (int iy = 0; iy < (1 << N); iy++) {
		
		Vector<2, unsigned long long int> permutsNbits, permutsMbits;
		
		//Variante A
		/*permuts11bits[0] = ix;
		permuts11bits[1] = iy;*/
		
		//Variante B
		/*permuts11bits[0] = (x_random_ind_switch+ix)%2048;
		permuts11bits[1] = (y_random_ind_switch+iy)%2048;*/
		
		//Variante C
		//permutsNbits[0] = ind_tabs[x_random_array_switch][ix];    // random access <<<<<<<<<<<<<<
		//permutsMbits[1] = ind_tabs[y_random_array_switch][iy];    // random access <<<<<<<<<<<<<<
		
		permutsNbits[0] = (x_random_ind_switch+ix)%(1 << N);
		permutsNbits[1] = (y_random_ind_switch+iy)%(1 << N);
		
		Vector<2, uint> fixed_point = pts[0].pos();
		permutFromNbitsToMbits<D>(N, M, fixed_point, permutsNbits, permutsMbits, K);
		
		Pointset<2, uint, Point<2, uint> > out_pts;
		out_pts.resize(tilesize);
		permut<D>(pts, out_pts, permutsMbits, log2(tilesize));
		double mindist = maximalMinimumDistance<D>(out_pts);

		if(current_min > mindist)
		{
			current_min = mindist;
			current_permut = permutsMbits;
			
			/*std::ofstream titi("titi.dat");
			for(int i=0; i<K; i++)
				titi << out_pts[i] << std::endl;
			titi.close();
			
			std::cout << resulting_permut << std::endl;*/
		}
		if(tilesize == 16 && mindist >= 0.20)
		{
			/*std::ofstream titi("titi.dat");
			for(int i=0; i<K; i++)
				titi << out_pts[i] << std::endl;
			titi.close();
			
			std::cout << resulting_permut << std::endl;*/
			
			resulting_permut = permutsMbits;
			return true;
		}
		
		/*if(mindist >= 5) {
			mmd_5_found = 1;
			mmd_5 = permutsMbits;
		}
		if(mindist >= 8) {
			mmd_8_found = 1;
			mmd_8 = permutsMbits;
		}
		if(mindist >= 10) {
			mmd_10_found = 1;
			mmd_10 = permutsMbits;
		}
		if(mindist >= 13) {
			resulting_permut = permutsMbits;
			//std::cout << "Found permut " << resulting_permut << " (MMD 13)" << std::endl;
			return true;
		}*/
	}
	}
	
	/*if(mmd_10_found)
	{
		resulting_permut = mmd_10;
		//std::cout << "Found permut " << resulting_permut << " (MMD 10)" << std::endl;
		return true;
	}
	if(mmd_8_found)
	{
		resulting_permut = mmd_8;
		//std::cout << "Found permut " << resulting_permut << " (MMD 8)" << std::endl;
		return true;
	}
	if(mmd_5_found)
	{
		resulting_permut = mmd_5;
		//std::cout << "Found permut " << resulting_permut << " (MMD 5)" << std::endl;
		return true;
	}*/
			
	resulting_permut = current_permut;
	
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
		auto it = data.find(key);
		if(it == data.end())
			return false;
		resulting_permut = it->second;
		return true;
	}
	
	bool getARandomPermutPerservingZero(SamplerVictor06022017_LUT_Key<D>& key, Vector<D, unsigned long long int>& permuts)
	{
		uint tilesize = pow(K, D);

		uint N, M;
		M = tilesize-1;
		N = M - log2(tilesize);
		
		//std::cout << N << "   " << M << std::endl;
		
		Vector<2, unsigned long long int> permutsNbits;
		permutsNbits[0] = drand48()*(1<<N);
		permutsNbits[1] = drand48()*(1<<N);
		
		Vector<2, uint> fixed_point;
		for(int d=0; d<2; d++)
			fixed_point[d] = key.pts[0].pos()[d].to_ulong();
		
		permutFromNbitsToMbits<D>(N, M, fixed_point, permutsNbits, permuts, K);
		//std::cout << permutsNbits << " -> " << permuts << std::endl << std::endl;
	
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
		//std::cout << "Optimizing ..." << std::flush;
		uint tilesize = pow(K, D);
		optimizePermut<D>(key, resulting_permut, tilesize);
		
		data.insert(std::pair< SamplerVictor06022017_LUT_Key<D>, Vector<D, unsigned long long int> >(key, resulting_permut));
		
		//std::cout << "Done" << std::endl;
        return true;
	}
};

}

#endif
