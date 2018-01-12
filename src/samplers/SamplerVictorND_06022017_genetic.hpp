#ifndef _UTK_SAMPLER_VICTOR_06022017_
#define _UTK_SAMPLER_VICTOR_06022017_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include "SamplerSobolIndices.hpp"
#include "SamplerVictor_06022017/SamplerVictorND_06022017_LUT_genetic.hpp"
#include "../io/imageIO.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

namespace utk
{
	
enum { LUT_SIZE, LUT_ACCESS, LUT_GENERATE, NB_LUT_INDICATORS };

template<uint D, uint I>
bool sortBitsetIDim(const Point<D, std::bitset<SamplerVictor06022017_BITSETSIZE> >& a, const Point<D, std::bitset<SamplerVictor06022017_BITSETSIZE> >& b)
{
	return a.pos()[I].to_ulong() < b.pos()[I].to_ulong();
}

class SamplerVictor06022017
{
public:

	SamplerVictor06022017() {
		subdiv_factor_K = 4;
		write_lut=false;
		
		srand48(std::chrono::system_clock::now().time_since_epoch().count());
		srand(std::chrono::system_clock::now().time_since_epoch().count());
		
		for(int i=0; i<10 ;i++)
		for(int j=0; j<NB_LUT_INDICATORS; j++)
			statsLUT[i][j] = 0;
		
		generates_new_patterns=false;
		
		sobol_ids.resize(4);
		sobol_ids[0] = 1;
		sobol_ids[1] = 2;
		sobol_ids[2] = 3;
		sobol_ids[3] = 7;
		
		optimize_patterns = true;
		null_patterns = false;
		
		silent=false;
	}
	
	void setSubdivK(uint arg_subdiv_factor_K) { subdiv_factor_K=arg_subdiv_factor_K; }
	//void setTileSize(uint arg_tilesize) { tilesize=arg_tilesize; }
	void setLookupTableInFile(std::string arg_lookuptable_file) { lookuptable_infile=arg_lookuptable_file; }
	void setLookupTableOutFile(std::string arg_lookuptable_file) { write_lut=true; lookuptable_outfile=arg_lookuptable_file; }
	void writeLUT(bool arg_write_lut) { write_lut=arg_write_lut; }
	void setSilent(bool arg_silent) { silent = arg_silent; }
	
	void useNullPermuts(bool arg_null) {  null_patterns = arg_null; }
	void useOptimizedPermuts(bool arg_optim) {  optimize_patterns = arg_optim; }
	void setSobolIds(const std::vector<uint>& ids) { sobol_ids.clear(); sobol_ids.resize( ids.size() ); for(uint i=0; i<ids.size(); i++) sobol_ids[i] = ids[i]; }
	void generatesNewPatterns(bool arg_generates_new_patterns) { generates_new_patterns = arg_generates_new_patterns; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if(D>4)
		{
			ERROR("SamplerVictor06022017::generateSamples Sobol Indices up to 4 only");
			return false;
		}
		if (!arg_pts.empty())
		{
			WARNING("SamplerVictor06022017::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		uint tilesize = pow(subdiv_factor_K, D);
		uint nextpoweroftilesize = pow(tilesize, ceil( log(arg_points)/log(tilesize) ));
		
		SamplerVictor06022017_LUT<D> lookuptable;
		
		lookuptable.setK(subdiv_factor_K);
		if(!lookuptable.fill(lookuptable_infile.c_str()))
		{
			ERROR("Failed to open LUT");
			return false;
		}
		
		int indices[D];
		for(int d=0; d<D; d++)
			indices[d] = sobol_ids[d];
		sobol.setIndices(indices, D);
		
		Pointset< D, uint, Point<D, uint> > input_pts;
		if(!silent)
			std::cout << "Generating " << nextpoweroftilesize << " Sobol samples ..." << std::flush;
		if(!sobol.generateSamples< D, uint, Point<D, uint> >(input_pts, nextpoweroftilesize))
		{
			ERROR("SamplerVictorND06022017::generateSamples Failed to generates sobol samples ...");
			return false;
		}
		if(!silent)
			std::cout << "Done" << std::endl;
		
		uint levelmax = log(nextpoweroftilesize)/log(tilesize);
		
		nb_done = 0;
		Vector<D, uint> digital_shift(0, 0);
		Pointset< D, uint, Point<D, uint> > output_pts;
		output_pts.resize(nextpoweroftilesize);
		recursivePermutations<D>(0, levelmax, input_pts[0].pos(), digital_shift, input_pts, output_pts, lookuptable);

		std::ofstream file;
		file.open("lutStats_sobol"+std::to_string(indices[0])+"_"+std::to_string(indices[1])+"_K"+std::to_string(subdiv_factor_K)+".dat");
		file << "#NbPts LutSize LutGenerated LutAccess" << std::endl;
		for(uint i=0; i<levelmax; i++)
		{
			statsLUT[i+1][LUT_SIZE] = statsLUT[i][LUT_SIZE] + statsLUT[i][LUT_GENERATE];
			file << pow(subdiv_factor_K, D*i) << statsLUT[i][LUT_SIZE] << " " << statsLUT[i][LUT_GENERATE] << " " << statsLUT[i][LUT_ACCESS] << std::endl;
		}
		file.close();
		
		arg_pts.resize(arg_points);
		arg_pts.toricity = 1;
		
		//Returns the pointset with uint values in [0:nextpoweroftilesize)
		if(isIntegerType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = nextpoweroftilesize;
			}

			for(unsigned int i=0; i<arg_points; i++)
			{
				for(uint d=0;d<D;d++)
				{
					arg_pts[i].pos()[d] = output_pts[i].pos()[d];
				}
			}
		}
		//Returns the pointset with float values in [0:1)
		else if(isFloatingType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}

			for(unsigned int i=0; i<arg_points; i++)
			{
				for(uint d=0;d<D;d++)
				{
					arg_pts[i].pos()[d] = output_pts[i].pos()[d] / (double)nextpoweroftilesize;
				}
			}
		}
		else
		{
			ERROR("SamplerVictor06022017::generateSamples cannot generates samples of the given type. Can only generate samples as integers of floating values");
			return false;
		}
		
		if(write_lut)
		{
			if(!lookuptable.write(lookuptable_outfile))
			{
				ERROR("SamplerVictor06022017::generateSamples Failed to write LUT");
				return false;
			}
		}
		
		return true;
	}

private:

	template<unsigned int D>
	void recursivePermutations(uint level, uint levelmax, Vector<D, uint>& cornerStrata, Vector<D, uint>& digital_shift, 
							   Pointset< D, uint, Point<D, uint> >& input_pts, Pointset< D, uint, Point<D, uint> >& output_pts,
							   SamplerVictor06022017_LUT<D>& lookuptable)
	{
		/***
		 * Variables
		 ***/
		uint kd = pow(subdiv_factor_K, D);
		uint logkd = log2(kd);
		uint logk = log2(subdiv_factor_K);
		//uint halflogk = logk/2;
		//uint halflogkd = logkd/2;
		uint bitmax = logkd*levelmax;

		uint nb_pts_total = pow(kd, levelmax);
		uint nb_pts = pow(kd, level+1);
		uint nb_stratas_axis = pow(subdiv_factor_K,level);
		uint strata_size_axis = pow(kd, levelmax)/nb_stratas_axis; //number of samples within each stratum
		//Used for retrieving the LUT key from the digits of the selected points
		//uint bit0 = halflogk*level;
		uint bit1 = (logkd*levelmax)-(level*logk);
		//uint length = bit1-bit0;
		
		if(!silent)
			std::cout << nb_done << "/" << pow(subdiv_factor_K, D*(levelmax-1)) << "     Level " << level << "  Stratum " << cornerStrata << "\r" << std::flush;
		
		/***
		 * Select all samples within the current strata
		 * Also saves their sobol indices to be able to properly reorder them
		 ***/
		std::vector<uint> selected_ids;
		selected_ids.resize(kd);
		Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > selected_pts; // = keyPattern16BITSTRING
		selected_pts.resize(kd);
		uint nb=0;
		for(uint i=0; i<nb_pts; i++)
		{
			bool inside = true;
			for(uint d=0; d<D; d++)
				inside &= (input_pts[i].pos()[d] >= cornerStrata[d] && input_pts[i].pos()[d] < cornerStrata[d]+strata_size_axis);
			
			if(inside)
			{
				for(uint d=0; d<D; d++)
					selected_pts.at(nb).pos()[d] = input_pts[i].pos()[d];
				selected_ids.at(nb) = i;
				nb++;
			}
		}
		if(nb != kd)
		{
			ERROR("SamplerVictor06022017::recursivePermutations Failed to build stratum");
			exit(1);
		}
		
		
		/*{
			PointsetWriter< D, uint, Point<D, uint> > writer;
			Pointset< D, uint, Point<D, uint> > to_write;
			to_write.resize(selected_pts.size());
			for(uint i=0; i<selected_pts.size(); i++)
			{
				for(uint d=0; d<D; d++)
					to_write[i].pos()[d] = selected_pts[i].pos()[d].to_ulong();
			}
			
			std::string filename = "sv_stratum";
			filename += "_" + std::to_string(level);
			for(uint d=0; d<D; d++)
				filename += "_" + std::to_string(cornerStrata[d]);
			writer.open(filename+".dat");
			writer.writePointset(to_write);
			writer.close();
		}*/
		
		/***
		 * If we don't have k samples, the pointset is not dyadic
		 ***/
		assert(nb == kd);
		
		/**
		 * We apply the accumulated XOR values to reposition the 0 point as it would have been premutted in the previous level
		 ***/
		Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > xored_selected_pts;
		xored_selected_pts.resize(kd);
		for(uint i=0; i<selected_pts.size(); i++)
		{
			for(uint d=0; d<D; d++)
				xored_selected_pts[i].pos()[d] = selected_pts[i].pos()[d].to_ulong() ^ digital_shift[d];
		}
		
		/**
		 * Retrieving the LUT key from the digits of the xored points
		 ***/
		Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > key_pts;
		key_pts.resize(kd);
		for(uint i=0; i<xored_selected_pts.size(); i++)
			for(uint b=0; b<logkd; b++)
			{
				for(uint d=0; d<D; d++)
					key_pts[i].pos()[d][logkd-1-b] = xored_selected_pts[i].pos()[d][bit1-b-1]; //we read the logk most significant bits from the position (bitmax-bit0) to (bitmax-bit0-logk)
			}
			
		/*{
			PointsetWriter< D, uint, Point<D, uint> > writer;
			Pointset< D, uint, Point<D, uint> > to_write;
			to_write.resize(xored_selected_pts.size());
			for(uint i=0; i<xored_selected_pts.size(); i++)
			{
				for(uint d=0; d<D; d++)
					to_write[i].pos()[d] = xored_selected_pts[i].pos()[d].to_ulong();
			}
			
			std::string filename = "sv_xored";
			filename += "_" + std::to_string(level);
			for(uint d=0; d<D; d++)
				filename += "_" + std::to_string(cornerStrata[d]);
			writer.open(filename+".dat");
			writer.writePointset(to_write);
			writer.close();
		}
		{
			PointsetWriter< D, uint, Point<D, uint> > writer;
			Pointset< D, uint, Point<D, uint> > to_write;
			to_write.resize(key_pts.size());
			for(uint i=0; i<key_pts.size(); i++)
			{
				for(uint d=0; d<D; d++)
					to_write[i].pos()[d] = key_pts[i].pos()[d].to_ulong();
			}
			
			std::string filename = "sv_key";
			filename += "_" + std::to_string(level);
			for(uint d=0; d<D; d++)
				filename += "_" + std::to_string(cornerStrata[d]);
			writer.open(filename+".dat");
			writer.writePointset(to_write);
			writer.close();
		}*/
		
		/**
		 * Retrieving the permutation from the LUT
		 ***/
		Vector<D, unsigned long long int> permuts;
		SamplerVictor06022017_LUT_Key<D> key(key_pts);
		statsLUT[level][LUT_ACCESS]++;
		if(!lookuptable.getAnOptimizedPermut(key, permuts))
		{
			if(!generates_new_patterns)
			{
				std::string str;
				str += "{";
				for(uint i=0; i<key_pts.size(); i++)
				{
					str += "{";
					str += std::to_string(key_pts[i].pos()[0].to_ulong());
					str += ",";
					str += std::to_string(key_pts[i].pos()[1].to_ulong());
					str += "}";
				}
				str += "}";
				ERROR("SamplerVictor06022017::recursivePermutations Couldn't find a LUT entry for pattern " + str);
				exit(1);
			}
			statsLUT[level][LUT_GENERATE]++;
			
			if (null_patterns)
				lookuptable.getANullPermut(key, permuts);
			else if(optimize_patterns)
				lookuptable.computeAndGetAnOptimizedPermut(key, permuts);
			else
				lookuptable.getARandomPermutPerservingZero(key, permuts);
		}
		
		/**
		 * Apply the permutation to the pointset
		 ***/
		Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > permutted_selected_pts;
		permutted_selected_pts.resize(kd);
		applyPermut<D>(xored_selected_pts, permutted_selected_pts, permuts, level, levelmax);
		
		/*{
			PointsetWriter< D, uint, Point<D, uint> > writer;
			Pointset< D, uint, Point<D, uint> > to_write;
			to_write.resize(permutted_selected_pts.size());
			for(uint i=0; i<permutted_selected_pts.size(); i++)
			{
				for(uint d=0; d<D; d++)
					to_write[i].pos()[d] = permutted_selected_pts[i].pos()[d].to_ulong();
			}
			
			std::string filename = "sv_permutted";
			filename += "_" + std::to_string(level);
			for(uint d=0; d<D; d++)
				filename += "_" + std::to_string(cornerStrata[d]);
			writer.open(filename+".dat");
			writer.writePointset(to_write);
			writer.close();
		}*/
		
		
		if((level+1) == levelmax)
		{
			/**
			* Return the permutted pointset
			***/
			nb_done++;
			for(uint i=0; i<permutted_selected_pts.size(); i++)
			for(uint d=0; d<D; d++)
				output_pts[selected_ids[i]].pos()[d] = permutted_selected_pts[i].pos()[d].to_ulong();
		}
		else
		{
			for(uint i=0; i<kd; i++)
			{
				/**
				* Computes the new accumulated xor
				* TOFIX: LOTS of useless variables and computations here
				***/
				Vector<D, uint> newdigitalshift;
				for(uint d=0; d<D; d++)
					newdigitalshift[d] = digital_shift[d] ^ ( permutted_selected_pts[i].pos()[d].to_ulong() ^ xored_selected_pts[i].pos()[d].to_ulong() );
				
				/**
				* Computes the corner of the point's stratum
				***/
				Vector<D, uint> newcornerstrata;
				std::bitset<SamplerVictor06022017_BITSETSIZE> pt;
				for(uint d=0; d<D; d++)
				{
					for(uint b=0; b<(level+1)*logk; b++)
						pt[(level+1)*logk - 1 -b] = selected_pts[i].pos()[d][bitmax-1-b];
					uint nb_pts_strata = nb_pts_total / pow(subdiv_factor_K, level+1);
					newcornerstrata[d] = pt.to_ulong() * nb_pts_strata;
				}
				
				/**
				 * Recurse
				 ***/
				recursivePermutations<D>(level+1, levelmax, newcornerstrata, newdigitalshift, input_pts, output_pts, lookuptable);
			}
		}
		
	}
	
	template<unsigned int D>
	void applyPermut(Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > in_pts, 
					 Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > >& out_pts,
					 Vector<D, unsigned long long int> permuts,
					 uint level,
				     uint level_max
					)
	{
		uint N = in_pts.size();
		Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > pts2;
		pts2.resize(N);
		
		uint kd = pow(subdiv_factor_K, D);
		//uint nbbits = log2(N);
 		//uint logk = log2(subdiv_factor_K);
		//uint halflogk = logk/2;
		
		uint logkd = log2(kd);
		//uint halflogkd = logkd/2;
		//uint bitmax = logk*level_max;
		
		//uint bit0 = halflogk*level;
		//uint bit1 = (logkd*level_max)-(level*logk);
		//uint length = bit1-bit0;
		
		Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > in_pts_sort[D];
		for(uint d=0; d<D; d++)
			in_pts_sort[d].resize(N);
		
		for(uint i=0; i<N; i++)
		{
			for(uint d=0; d<D; d++)
				in_pts_sort[d][i] = in_pts[i];
		}
		
		if(D>=2)
		{
			std::sort(in_pts_sort[0].begin(), in_pts_sort[0].end(), sortBitsetIDim<D, 0>);
			std::sort(in_pts_sort[1].begin(), in_pts_sort[1].end(), sortBitsetIDim<D, 1>);
		}
		if(D>=3)
		{
			std::sort(in_pts_sort[2].begin(), in_pts_sort[2].end(), sortBitsetIDim<D, 2>);
		}
		if(D>=4)
		{
			std::sort(in_pts_sort[3].begin(), in_pts_sort[3].end(), sortBitsetIDim<D, 3>);
		}
		
		std::bitset<SamplerVictor06022017_BITSETSIZE> xpermuts[D];
		for(uint d=0; d<D; d++)
			xpermuts[d] = permuts[d];
		
		Pointset< D, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< D, std::bitset<SamplerVictor06022017_BITSETSIZE> > > pts_indices;
		pts_indices.resize(in_pts.size());
		
		for(int ipt=0; ipt<in_pts.size(); ipt++) 
		{
			uint id[D];
			
			for(uint d=0; d<D; d++)
			{
				id[d]= 0;
				for(id[d]=0; id[d]<N && in_pts[ipt].pos()[d] != in_pts_sort[d][id[d]].pos()[d]; id[d]++);
				//std::cout << "Id[" << d << "]" << id[d] << std::endl;
				pts_indices[ipt].pos()[d] = id[d];
			}
			
			/*for(int i=0; i<logkd; i++)
			{
				for(uint d=0; d<D; d++)
					pts_indices[ipt].pos()[d][logkd-1-i] = in_pts[ipt].pos()[d][bit1-i-1]; //we read the logkd most significant bits from the position (bitmax-bit0) to (bitmax-bit0-logkd)
			}*/
		}
		
		for(uint ipt=0; ipt<N; ipt++)
		{
			std::bitset<SamplerVictor06022017_BITSETSIZE> xresdigits[D];
			
			uint pts[D];
			for(uint d=0; d<D; d++)
				pts[d] = pts_indices[ipt].pos()[d].to_ulong();

			int ind=0;
			for(uint ilevel=0; ilevel<logkd; ilevel++) {
				
				for(uint d=0; d<D; d++)
				{
					ind = (pts[d] >> (logkd-ilevel)) + (pow(2,ilevel)-1);
					xresdigits[d].set(logkd-ilevel-1, (xpermuts[d][kd-1-ind-1] ^ pts_indices[ipt].pos()[d][logkd-ilevel-1]) );
				}
			};
			
			for(uint d=0; d<D; d++)
				pts_indices[ipt].pos()[d] = xresdigits[d];
		}
		
		if(D>=2)
		{
			for(int ipt=0; ipt<N; ipt++)
				out_pts[ipt].pos()[0] = in_pts_sort[0][pts_indices[ipt].pos()[0].to_ulong()].pos()[0];

			for(int ipt=0; ipt<N; ipt++)
				out_pts[ipt].pos()[1] = in_pts_sort[1][pts_indices[ipt].pos()[1].to_ulong()].pos()[1];
		}
		if(D>=3)
		{
			for(int ipt=0; ipt<N; ipt++)
				out_pts[ipt].pos()[2] = in_pts_sort[2][pts_indices[ipt].pos()[2].to_ulong()].pos()[2];
		}
		if(D>=4)
		{
			for(int ipt=0; ipt<N; ipt++)
				out_pts[ipt].pos()[3] = in_pts_sort[3][pts_indices[ipt].pos()[3].to_ulong()].pos()[3];
		}
	}
	
	SamplerSobolIndices sobol;
	
	//uint tilesize;
	uint subdiv_factor_K;
	std::string lookuptable_infile;
	std::string lookuptable_outfile;
	bool silent;
	bool write_lut;
	
	bool generates_new_patterns;
	bool optimize_patterns;
	bool null_patterns;
	std::vector<uint> sobol_ids;
	
	uint statsLUT[10][NB_LUT_INDICATORS];
	
	uint nb_done;
};

} //end namespace utk

#endif
