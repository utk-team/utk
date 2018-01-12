#ifndef _UTK_SAMPLER_VICTOR_06022017_
#define _UTK_SAMPLER_VICTOR_06022017_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include "SamplerSobolIndices.hpp"
#include "SamplerVictor_06022017/SamplerVictorND_06022017_LUT_genetic.hpp"
#include "../io/imageIO.hpp"
#include <stack>
#include <queue>
#include <chrono>

#define LEVEL_MAX 15

namespace utk
{
	
enum{ LUT_MISS, LUT_FOUND, LUT_ACCESS, LUT_SIZE, NB_STATES_LUT };

bool sortBitsetX(const Point<2, std::bitset<SamplerVictor06022017_BITSETSIZE> >& a, const Point<2, std::bitset<SamplerVictor06022017_BITSETSIZE> >& b)
{
	return a.pos()[0].to_ullong() < b.pos()[0].to_ullong();
}
bool sortBitsetY(const Point<2, std::bitset<SamplerVictor06022017_BITSETSIZE> >& a, const Point<2, std::bitset<SamplerVictor06022017_BITSETSIZE> >& b)
{
	return a.pos()[1].to_ullong() < b.pos()[1].to_ullong();
}

class SamplerVictorND06022017
{
public:

	SamplerVictorND06022017() {
		subdiv_factor_K = 4;
		tilesize = 16;
		lookuptable.setK(subdiv_factor_K);
		
		write_lut=false;
		generates_new_patterns=false;
		
		setRandomSeedTime();
		
		sobol_ids.resize(4);
		sobol_ids[0] = 1;
		sobol_ids[1] = 2;
		sobol_ids[2] = 3;
		sobol_ids[3] = 7;
		
		optimize_patterns = true;
		null_patterns = false;
		
		silent = false;
		
		//srand48(std::chrono::system_clock::now().time_since_epoch().count());
		srand(std::chrono::system_clock::now().time_since_epoch().count());
	}
	
	void setSilent(bool arg_silent) { silent = arg_silent; }
	
	void useNullPermuts(bool arg_null) {  null_patterns = arg_null; }
	void useOptimizedPermuts(bool arg_optim) {  optimize_patterns = arg_optim; }
	
	void setSobolIds(const std::vector<uint>& ids) { sobol_ids.clear(); sobol_ids.resize( ids.size() ); for(uint i=0; i<ids.size(); i++) sobol_ids[i] = ids[i]; }
	void generatesNewPatterns(bool arg_generates_new_patterns) { generates_new_patterns = arg_generates_new_patterns; }
	void setRandomSeed( long unsigned int arg_seed ) { srand48(arg_seed); }
	void setRandomSeedTime() { srand48(std::chrono::system_clock::now().time_since_epoch().count()); }
	
	void setLookupTableInFile(std::string arg_lookuptable_file) { lookuptable_infile=arg_lookuptable_file; }
	void setLookupTableOutFile(std::string arg_lookuptable_file) { write_lut=true; lookuptable_outfile=arg_lookuptable_file; }
	void writeLUT(bool arg_write_lut) { write_lut=arg_write_lut; }
	
	void setSubdivFactorK(uint arg_K) { subdiv_factor_K=arg_K; tilesize=pow(arg_K, 2); lookuptable.setK(arg_K); }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
	{
		if(arg_points == 0)
			return true;
		
		if(D%2 != 0)
		{
			ERROR("SamplerVictor06022017::generateSamples only in (2k)D for now");
			return false;
		}

		if (!arg_pts.empty())
		{
			WARNING("SamplerVictor06022017::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		if (generates_new_patterns && !write_lut)
		{
			WARNING("SamplerVictor06022017::generateSamples will compute new LUT entries but will not write them ...");
		}
		if (!generates_new_patterns && write_lut)
		{
			WARNING("SamplerVictor06022017::generateSamples will NOT compute new LUT entries but will write a new LUT anyway ...");
		}
		if (write_lut && !optimize_patterns)
		{
			WARNING("SamplerVictor06022017::generateSamples will fill a LUT with random patterns, Is this the desired behaviour ? y/n");
			char c = ' ';
			while(1)
			{
				std::cin >> c;
				if (c == 'n')
					return false;
				if (c == 'y')
					break;
				std::cout << "press y or n" << std::endl;
			}
		}
		
		if(!lookuptable.fill(lookuptable_infile.c_str()))
		{
			ERROR("Failed to open LUT");
			return false;
		}
		
		for(uint current_dim=0; current_dim<D; current_dim+=2)
		{
			uint level_max = ceil( log(arg_points)/log(tilesize) );
			unsigned long long int nextpoweroftilesize = pow(tilesize, level_max);
		
			Pointset< 2, uint, Point<2, uint> > input_pts;
			
			int indices[2];
			indices[0] = sobol_ids[current_dim];
			indices[1] = sobol_ids[current_dim+1];
			sobol.setIndices(indices, 2);

			if(!silent)
				std::cout << "Generating " << nextpoweroftilesize << " Sobol samples ..." << std::flush;
			if(!sobol.generateSamples< 2, uint, Point<2, uint> >(input_pts, nextpoweroftilesize))
			{
				ERROR("SamplerVictor06022017::generateSamples Failed to generates sobol samples ...");
				return false;
			}
			if(!silent)
				std::cout << "Done" << std::endl;
			
			Pointset< 2, uint, Point<2, uint> > output_pts;
			output_pts.resize(nextpoweroftilesize);
			
			/*if(level_max >= LEVEL_MAX)
			{
				ERROR("SamplerVictor06022017::generateSamples can only generate up to 16^" + std::to_string(LEVEL_MAX-1) + " samples ...");
				return false;
			}*/
			
			if(!initializeStrata(input_pts, level_max))
				return false;
			if(!recursivePermutations(level_max, input_pts, output_pts))
				return false;

			arg_pts.resize(arg_points);
			arg_pts.toricity = 1;
			
			//Returns the pointset with uint values in [0:nextpoweroftilesize)
			if(isIntegerType<T>())
			{
				for(uint d=current_dim; d<current_dim+2; d++)
				{
					arg_pts.domain.pMin.pos()[d] = 0;
					arg_pts.domain.pMax.pos()[d] = nextpoweroftilesize;
				}

				for(unsigned long long int i=0; i<arg_points; i++)
				{
					for(int d=0; d<2; d++)
					{
						arg_pts[i].pos()[current_dim+d] = output_pts[i].pos()[d];
					}
				}
			}
			//Returns the pointset with float values in [0:1)
			else if(isFloatingType<T>())
			{
				for(uint d=current_dim; d<current_dim+2; d++)
				{
					arg_pts.domain.pMin.pos()[d] = 0;
					arg_pts.domain.pMax.pos()[d] = 1;
				}

				for(unsigned long long int i=0; i<arg_points; i++)
				{
					for(int d=0; d<2; d++)
					{
						arg_pts[i].pos()[current_dim+d] = output_pts[i].pos()[d]  / (double)nextpoweroftilesize;
					}
				}
			}
			else
			{
				ERROR("SamplerVictor06022017::generateSamples cannot generates samples of the given type. Can only generate samples as integers of floating values");
				return false;
			}
			
			std::string filename;
			filename += "stats_lut_K"+std::to_string(subdiv_factor_K)+"_sobol";
			for(uint i=0; i<D; i++)
				filename += "_"+std::to_string(sobol_ids[i]);
			filename += ".txt";
			std::ofstream file_stats_lut(filename);
			file_stats_lut << "#NbPts\tNbUniquePatterns-1\tNbUniquePatterns\tNbMiss\tNbFound\tNbAccess" << std::endl;
			file_stats_lut << 0 << "\t" << 0 << "\t" << statsLUT[0][LUT_SIZE] <<"\t" << statsLUT[0][LUT_MISS] <<  "\t" << statsLUT[0][LUT_FOUND] <<  "\t" << statsLUT[0][LUT_ACCESS] << std::endl;
			for(uint i=1; i<level_max; i++)
			{
				uint nbpts = pow(subdiv_factor_K, D*(i+1));
				if(nbpts > 1000000)
					file_stats_lut << nbpts/1000000 << "M\t";
				else if(nbpts > 1000)
					file_stats_lut << nbpts/1000 << "K\t";
				else
					file_stats_lut << nbpts << "\t";
				file_stats_lut << statsLUT[i-1][LUT_SIZE] << "\t" << statsLUT[i][LUT_SIZE] <<"\t" << statsLUT[i][LUT_MISS] <<  "\t" << statsLUT[i][LUT_FOUND] <<  "\t" << statsLUT[i][LUT_ACCESS] << std::endl;
			}
			file_stats_lut.close();
		}
		
		if(!silent)
			std::cout << "Done" << std::endl;
		
		if(write_lut)
		{
			if(!lookuptable.write(lookuptable_outfile))
			{
				ERROR("SamplerVictor06022017::generateSamples Failed to write LUT");
				return false;
			}
		}
			
		lookuptable.clear();
		
		return true;
	}

private:
    
	struct Recursive
	{
		uint level;
		Vector<2, uint> cornerStrata;
		Vector<2, uint> digital_shift;
	};

	bool recursivePermutations(uint levelmax, Pointset< 2, uint, Point<2, uint> >& input_pts, Pointset< 2, uint, Point<2, uint> >& output_pts)
	{
		/***
		 * Variables
		 ***/
		uint kd = tilesize;
		uint logkd = log2(kd);
		uint logk = log2(subdiv_factor_K);
		uint bitmax = logkd*levelmax;
		unsigned long long int nb_pts_total = 1;//pow(k, levelmax);
		nb_pts_total <<= (logkd*levelmax);
		
		std::queue<Recursive> stack;
		
		Recursive r0;
		r0.level = 0;
		r0.cornerStrata = input_pts[0].pos();
		r0.digital_shift[0] = 0;
		r0.digital_shift[1] = 0;
		stack.push(r0);
		
		while(!stack.empty())
		{
			Recursive r = stack.front();
			stack.pop();
		
			uint level = r.level;
			
			if(!silent && (statsLUT[level][LUT_ACCESS]%100 == 0))
				std::cout << "Generating samples ... Level " << level << "/" << levelmax << "\t" <<  (double)statsLUT[level][LUT_ACCESS]/pow(16, level) * 100.0 << "%...\r" << std::flush;

			//If I just do nb_strata_axis = pow(sqrt(k),level); I will face an int overflow when generating 4G samples
			unsigned long long int nb_strata_axis = 1;
			nb_strata_axis <<= (logk*level);

			//Used for retrieving the LUT key from the digits of the selected points
			uint bit1 = (logkd*levelmax)-(level*logk);
			
			/***
			* Select all samples within the current strata
			* Also saves their sobol indices to be able to properly reorder them
			***/
			std::vector<unsigned long long int> selected_ids;
			selected_ids.resize(kd);
			Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > selected_pts; // = keyPattern16BITSTRING
			selected_pts.resize(kd);
		
			double x = r.cornerStrata[0];
			double y = r.cornerStrata[1];
			x /= nb_pts_total;
			y /= nb_pts_total;
			unsigned long long int stratum_x = x * nb_strata_axis;
			unsigned long long int stratum_y = y * nb_strata_axis;
			for(uint i=0; i<kd; i++)
			{
				selected_ids[i] = strata[level][stratum_x*nb_strata_axis+stratum_y][i];
				selected_pts[i].pos()[0] = input_pts[ selected_ids[i] ].pos()[0];
				selected_pts[i].pos()[1] = input_pts[ selected_ids[i] ].pos()[1];
			}
			
			/*{
				std::string str;
				str = "sv_stratum";
				
				str += "_" + std::to_string(r.level);
				
				str += "_" + std::to_string(r.cornerStrata[0]);
				str += "_" + std::to_string(r.cornerStrata[1]);
				
				str += ".dat";
					
				std::ofstream file;
				file.open(str);
				for(uint i=0; i<kd; i++)
					file << selected_pts[i].pos()[0].to_ulong() << " " << selected_pts[i].pos()[1].to_ulong() << std::endl;
				
				file.close();
			}*/

			/**
			* We apply the accumulated XOR values to reposition the 0 point as it would have been premutted in the previous level
			***/
			Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > xored_selected_pts;
			xored_selected_pts.resize(kd);
			for(uint i=0; i<selected_pts.size(); i++)
			{
				xored_selected_pts[i].pos()[0] = selected_pts[i].pos()[0].to_ulong() ^ r.digital_shift[0];
				xored_selected_pts[i].pos()[1] = selected_pts[i].pos()[1].to_ulong() ^ r.digital_shift[1];
			}
			
			/*{
				std::string str;
				str = "sv_xored";
				
				str += "_" + std::to_string(r.level);
				
				str += "_" + std::to_string(r.cornerStrata[0]);
				str += "_" + std::to_string(r.cornerStrata[1]);
				
				str += ".dat";
					
				std::ofstream file;
				file.open(str);
				for(uint i=0; i<kd; i++)
					file << "\\filldraw[black] (" << xored_selected_pts[i].pos()[0].to_ulong() << "," << xored_selected_pts[i].pos()[1].to_ulong() << ") circle (16pt)" << std::endl;
				
				file.close();
			}*/
			
			/**
			* Retrieving the LUT key from the digits of the xored points
			***/
			Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > key_pts;
			key_pts.resize(kd);
			for(uint i=0; i<xored_selected_pts.size(); i++)
				for(uint b=0; b<logkd; b++)
				{
					key_pts[i].pos()[0][logkd-1-b] = xored_selected_pts[i].pos()[0][bit1-b-1]; //we read the logk most significant bits from the position (bitmax-bit0) to (bitmax-bit0-logk)
					key_pts[i].pos()[1][logkd-1-b] = xored_selected_pts[i].pos()[1][bit1-b-1];
				}
				
			/*{
				std::string str;
				str = "sv_key";
				
				str += "_" + std::to_string(r.level);
				
				str += "_" + std::to_string(r.cornerStrata[0]);
				str += "_" + std::to_string(r.cornerStrata[1]);
				
				str += ".dat";
					
				std::ofstream file;
				file.open(str);
				for(uint i=0; i<kd; i++)
					file << key_pts[i].pos()[0].to_ulong() << " " << key_pts[i].pos()[1].to_ulong() << std::endl;
				
				file.close();
			}*/
			
			/**
			* Retrieving the permutation from the LUT
			***/
			Vector<2, unsigned long long int> permuts;
			SamplerVictor06022017_LUT_Key<2> key(key_pts);
			
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
					return false;
				}
				statsLUT[level][LUT_MISS]++;
				
				if (null_patterns)
					lookuptable.getANullPermut(key, permuts);
				else if(optimize_patterns)
					lookuptable.computeAndGetAnOptimizedPermut(key, permuts);
				else
					lookuptable.getARandomPermutPerservingZero(key, permuts);
			}
			else
				statsLUT[level][LUT_FOUND]++;
			
			statsLUT[level][LUT_SIZE] = lookuptable.size();
			
			/**
			* Apply the permutation to the pointset
			***/
			Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > permutted_selected_pts;
			permutted_selected_pts.resize(kd);
			applyPermut(xored_selected_pts, permutted_selected_pts, permuts, level, levelmax);
			
			/*{
				std::string str;
				str = "sv_permutted";
				
				str += "_" + std::to_string(r.level);
				
				str += "_" + std::to_string(r.cornerStrata[0]);
				str += "_" + std::to_string(r.cornerStrata[1]);
				
				str += ".dat";
					
				std::ofstream file;
				file.open(str);
				for(uint i=0; i<kd; i++)
					file << permutted_selected_pts[i].pos()[0].to_ulong() << " " << permutted_selected_pts[i].pos()[1].to_ulong() << std::endl;
				
				file.close();
			}*/
				
			
			if((level+1) == levelmax)
			{
				/**
				* Return the permutted pointset
				***/
				for(uint i=0; i<permutted_selected_pts.size(); i++)
				for(int d=0; d<2; d++)
					output_pts[selected_ids[i]].pos()[d] = permutted_selected_pts[i].pos()[d].to_ulong();
			}
			else
			{
				for(uint i=0; i<kd; i++)
				{
					Recursive rprime;
					rprime.level = level+1;
					
					/**
					* Computes the new digital shift
					***/
					rprime.digital_shift[0] = r.digital_shift[0] ^ ( permutted_selected_pts[i].pos()[0].to_ulong() ^ xored_selected_pts[i].pos()[0].to_ulong() );
					rprime.digital_shift[1] = r.digital_shift[1] ^ ( permutted_selected_pts[i].pos()[1].to_ulong() ^ xored_selected_pts[i].pos()[1].to_ulong() );
					
					/**
					* Computes the corner of the point's stratum
					***/
					Vector<2, uint> newcornerstrata;
					std::bitset<SamplerVictor06022017_BITSETSIZE> pt;
					for(uint b=0; b<(level+1)*logk; b++)
						pt[(level+1)*logk - 1 -b] = selected_pts[i].pos()[0][bitmax-1-b];
					uint nb_pts_strata = nb_pts_total / pow(sqrt(kd), level+1);
					rprime.cornerStrata[0] = pt.to_ulong() * nb_pts_strata;
					for(uint b=0; b<(level+1)*logk; b++)
						pt[(level+1)*logk - 1 -b] = selected_pts[i].pos()[1][bitmax-1-b];
					rprime.cornerStrata[1] = pt.to_ulong() * nb_pts_strata;
					
					/**
					* Recurse
					***/
					stack.push(rprime);
				}
			}
		}
		
		return true;
		
	}
	
	void applyPermut(Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > in_pts, 
					 Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > >& out_pts,
					 Vector<2, unsigned long long int> permuts,
					 uint level,
				     uint level_max
					)
	{
		uint N = in_pts.size();
		Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > pts2;
		pts2.resize(N);
		
		uint kd = tilesize;
		//uint nbbits = log2(N);
		uint logkd = log2(tilesize);
		//uint logk = log2(subdiv_factor_K);
		//uint bitmax = logk*level_max;
		
		//uint bit0 = halflogk*level;
		//uint bit1 = (logkd*level_max)-(level*logk);
		//uint length = bit1-bit0;
		
		Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > in_pts_sortx;
		in_pts_sortx.resize(N);
		Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > in_pts_sorty;
		in_pts_sorty.resize(N);
		for(uint i=0; i<N; i++)
		{
			in_pts_sortx[i] = in_pts[i];
			in_pts_sorty[i] = in_pts[i];
		}
		std::sort(in_pts_sortx.begin(), in_pts_sortx.end(), sortBitsetX);
		std::sort(in_pts_sorty.begin(), in_pts_sorty.end(), sortBitsetY);
		
		std::bitset<SamplerVictor06022017_BITSETSIZE> xpermuts, ypermuts;
		xpermuts = permuts[0];
		ypermuts = permuts[1];
		
		Pointset< 2, std::bitset<SamplerVictor06022017_BITSETSIZE>, Point< 2, std::bitset<SamplerVictor06022017_BITSETSIZE> > > pts_indices;
		pts_indices.resize(in_pts.size());
		for(uint ipt=0; ipt<in_pts.size(); ipt++) 
		{
			uint idx = 0;
			for(idx=0; idx<N && in_pts[ipt].pos()[0] != in_pts_sortx[idx].pos()[0]; idx++);
			uint idy = 0;
			for(idy=0; idy<N && in_pts[ipt].pos()[1] != in_pts_sorty[idy].pos()[1]; idy++);
			
			pts_indices[ipt].pos()[0] = idx;
			pts_indices[ipt].pos()[1] = idy;
		}
		
		for(uint ipt=0; ipt<N; ipt++)
		{
			std::bitset<SamplerVictor06022017_BITSETSIZE> xresdigits, yresdigits;
			
			uint pts0 = pts_indices[ipt].pos()[0].to_ulong();
			uint pts1 = pts_indices[ipt].pos()[1].to_ulong();

			int ind=0;
			for(uint ilevel=0; ilevel<logkd; ilevel++) {
				ind = (pts0 >> (logkd-ilevel)) + (pow(2,ilevel)-1);
				xresdigits.set(logkd-ilevel-1, (xpermuts[kd-1-ind-1] ^ pts_indices[ipt].pos()[0][logkd-ilevel-1]) );
				ind = (pts1 >> (logkd-ilevel)) + (pow(2,ilevel)-1);
				yresdigits.set(logkd-ilevel-1, (ypermuts[kd-1-ind-1] ^ pts_indices[ipt].pos()[1][logkd-ilevel-1]) );
			};
			
			pts_indices[ipt].pos()[0] = xresdigits;
			pts_indices[ipt].pos()[1] = yresdigits;
		}
		
		
		for(uint ipt=0; ipt<N; ipt++)
			out_pts[ipt].pos()[0] = in_pts_sortx[pts_indices[ipt].pos()[0].to_ullong()].pos()[0];
		
		
		for(uint ipt=0; ipt<N; ipt++)
			out_pts[ipt].pos()[1] = in_pts_sorty[pts_indices[ipt].pos()[1].to_ullong()].pos()[1];
	}
	
	bool initializeStrata(Pointset< 2, uint, Point<2, uint> >& input_pts, uint level_max)
	{
		
		strata.resize(level_max);
		uint logKd = log2(tilesize);
		uint logK = log2(subdiv_factor_K);
		unsigned long long int nb_pts_total = 1;
		nb_pts_total <<= (logKd*level_max);//pow(tilesize, level_max);
		
		for(uint l=0; l<level_max; l++)
		{	
			uint nb_strata = 1;
			nb_strata <<= (logKd*l);
			
			//unsigned long long int nb_strata_axis = sqrt( nb_strata );
			uint nb_strata_axis = 1;
			nb_strata_axis <<= (logK*l);
			
			strata[l].resize( nb_strata );
			for(uint i=0; i<nb_strata; i++)
				strata[l][i].resize(tilesize);
			
			std::vector< uint > cpt_strata;
			cpt_strata.resize(nb_strata);
			
			unsigned long long int nbpts = 1;//pow(tilesize,l+1);
			nbpts <<= (logKd*(l+1));//pow(tilesize, level_max);
			for(unsigned long long int i=0; i<nbpts; i++)
			{
				if(!silent && (i%100 == 0))
					std::cout << "Init stratas ... Level " << l << "/" << level_max << "\t" <<  (double)i/(double)nbpts * 100.0 << "%...\r" << std::flush;
				
				double x = input_pts[i].pos()[0];
				double y = input_pts[i].pos()[1];
				x /= nb_pts_total;
				y /= nb_pts_total;
				unsigned long long int stratum_x = x * nb_strata_axis;
				unsigned long long int stratum_y = y * nb_strata_axis;
				
				uint& n = cpt_strata[stratum_x*nb_strata_axis + stratum_y];
				if(n >= tilesize)
				{
					ERROR("SamplerVictor06022017::initializeStrata non dyadic point repartition");
					return false;
				}
				strata[l][stratum_x*nb_strata_axis + stratum_y][n] = i;
				n++;
			}
			
			for(unsigned long long int i=0; i<nb_strata; i++)
				if( cpt_strata[i] != tilesize )
				{
					ERROR("SamplerVictor06022017::initializeStrata non dyadic point repartition");
					return false;
				}
		}
		
		for(int i=0; i<LEVEL_MAX; i++)
			for(int s=0; s<NB_STATES_LUT; s++)
				statsLUT[i][s] = 0;
			
		if(!silent)
				std::cout << "Done" << std::endl;
			
		return true;
	}
	
	uint subdiv_factor_K;
	uint tilesize;
	std::string lookuptable_infile;
	std::string lookuptable_outfile;
	bool write_lut;
	bool generates_new_patterns;
	bool optimize_patterns;
	bool silent;
	bool null_patterns;
	std::vector<uint> sobol_ids;
	
	uint statsLUT[LEVEL_MAX][NB_STATES_LUT];
	
	SamplerSobolIndices sobol;
	std::vector< std::vector< std::vector<uint> > > strata;
	SamplerVictor06022017_LUT<2> lookuptable;
};

} //end namespace utk

#endif
