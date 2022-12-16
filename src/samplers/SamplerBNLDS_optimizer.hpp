/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
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
 * either expressed or implied, of the Halton project.
 */
#ifndef _UTK_SAMPLER_BNLDS_06022017_
#define _UTK_SAMPLER_BNLDS_06022017_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include "SamplerSobolIndices.hpp"
#include "SamplerBNLDS/SamplerBNLDS_LUT.hpp"
#include "../io/imageIO.hpp"
#include <stack>
#include <queue>
#include <chrono>

#define LEVEL_MAX 15
//#define DEBUG

namespace utk
{
	
//enum{ LUT_MISS_2D2D, LUT_FOUND_2D2D, LUT_ACCESS_2D2D, LUT_SIZE_2D2D, NB_STATES_LUT_2D2D };

bool sortBitsetX_2D2D(const Point<2, std::bitset<SamplerBNLDS_BITSETSIZE> >& a, const Point<2, std::bitset<SamplerBNLDS_BITSETSIZE> >& b)
{
	return a.pos()[0].to_ullong() < b.pos()[0].to_ullong();
}
bool sortBitsetY_2D2D(const Point<2, std::bitset<SamplerBNLDS_BITSETSIZE> >& a, const Point<2, std::bitset<SamplerBNLDS_BITSETSIZE> >& b)
{
	return a.pos()[1].to_ullong() < b.pos()[1].to_ullong();
}

class SamplerBNLDS_optimizer
{
public:

	SamplerBNLDS_optimizer() {
		subdiv_factor_K = 4;
		lookuptable.setK(subdiv_factor_K);
		
		tilesize = 16;
		
		sobol_ids.resize(2);
		sobol_ids[0] = 1;
		sobol_ids[1] = 2;

		srand(std::chrono::system_clock::now().time_since_epoch().count());
		srand48(std::chrono::system_clock::now().time_since_epoch().count());
	}
	
	unsigned long long int permut_val;
	bool optiMinDist;
	bool optiPCF;
	bool fillEndTreeZero;
	bool fillEndTreeRandom;
	bool fillEndTreeRandomOpti;
	
	void setSobolIds(const std::vector<uint>& ids) { 
		sobol_ids.clear();
		sobol_ids.resize( ids.size() );
		for(uint i=0; i<ids.size(); i++)
			sobol_ids[i] = ids[i];
	}
	
	void setRandomSeed( long unsigned int arg_seed ) { 
		srand48(arg_seed); 
		srand(arg_seed);
	}
	void setRandomSeedTime() { 
		srand(std::chrono::system_clock::now().time_since_epoch().count());
		srand48(std::chrono::system_clock::now().time_since_epoch().count()); 
	}
	
	void setLookupTableInfile(std::string arg_lookuptable_file) { lookuptable_infile=arg_lookuptable_file; }
	void setLookupTableOutfile(std::string arg_lookuptable_file) { lookuptable_outfile=arg_lookuptable_file; }
	void setSubdivFactorK(uint arg_K) { subdiv_factor_K=arg_K; tilesize=pow(arg_K, 2); lookuptable.setK(arg_K); }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
	{
		lookuptable.optiMinDist = optiMinDist;
		lookuptable.optiPCF = optiPCF;
		lookuptable.fillEndTreeRandom = fillEndTreeRandom;
		lookuptable.fillEndTreeRandomOpti = fillEndTreeRandomOpti;
		lookuptable.fillEndTreeZero = fillEndTreeZero;
		
		if(arg_points == 0)
			return true;
		
		if(D != 2)
		{
			ERROR("SamplerBNLDS::generateSamples only 2D samples");
			return false;
		}

		if (!arg_pts.empty())
		{
			WARNING("SamplerBNLDS::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		if(!lookuptable.fill(lookuptable_infile.c_str()))
		{
			ERROR("SamplerBNLDS::generateSamples Failed to open LUT");
			return false;
		}

		uint level_max = ceil( log(arg_points)/log(tilesize) );
		unsigned long long int nextpoweroftilesize = pow(tilesize, level_max);
	
		Pointset< 2, uint, Point<2, uint> > input_pts;
		
		int indices[2];
		indices[0] = sobol_ids[0];
		indices[1] = sobol_ids[1];
		sobol.setIndices(indices, 2);

		std::cout << "Generating " << nextpoweroftilesize << " Sobol samples ..." << std::flush;
		
		if(!sobol.generateSamples< 2, uint, Point<2, uint> >(input_pts, nextpoweroftilesize))
		{
			ERROR("SamplerBNLDS::generateSamples Failed to generates sobol samples ...");
			return false;
		}
		
		std::cout << "Done" << std::endl;
		
		Pointset< 2, uint, Point<2, uint> > output_pts;
		output_pts.resize(nextpoweroftilesize);
		
		std::cout << "Initializing Stratas  ..." << std::flush;
		
		if(!initializeStrata(input_pts, level_max))
			return false;
		
		std::cout << "Done" << std::endl;
		
		std::cout << "Permuting Samples  ..." << std::flush;
		
		if(!recursivePermutations(level_max, input_pts, output_pts))
			return false;
		
		std::cout << "Done" << std::endl;

		arg_pts.resize(arg_points);
		arg_pts.toricity = 1;
		
		if(isIntegerType<T>())
		{
			for(uint d=0; d<2; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = nextpoweroftilesize;
			}

			for(unsigned long long int i=0; i<arg_points; i++)
			{
				for(int d=0; d<2; d++)
				{
					arg_pts[i].pos()[d] = output_pts[i].pos()[d];
				}
			}
		}
		//Returns the pointset with float values in [0:1)
		else if(isFloatingType<T>())
		{
			for(uint d=0; d<2; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}

			for(unsigned long long int i=0; i<arg_points; i++)
			{
				for(int d=0; d<2; d++)
				{
					arg_pts[i].pos()[d] = output_pts[i].pos()[d]  / (double)nextpoweroftilesize;
				}
			}
		}
		else
		{
			ERROR("SamplerVictor06022017::generateSamples cannot generates samples of the given type. Can only generate samples as integers of floating values");
			return false;
		}
		
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
		uint logkd = log2(tilesize);
		uint logk = log2(subdiv_factor_K);
		uint bitmax = logkd*levelmax;
		unsigned long long int nb_pts_total = 1;
		nb_pts_total <<= (subdiv_factor_K*levelmax);
		
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
			
			/*std::cout << "r:" << std::endl;
			std::cout << "- r.level " << r.level << std::endl;
			std::cout << "- r.cornerstrata " << r.cornerStrata << std::endl;*/
			
		
			uint level = r.level;
			
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
			selected_ids.resize(tilesize);
			Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > selected_pts; // = keyPattern16BITSTRING
			selected_pts.resize(tilesize);
		
			double x = r.cornerStrata[0];
			double y = r.cornerStrata[1];
			x /= nb_pts_total;
			y /= nb_pts_total;
			unsigned long long int stratum_x = x * nb_strata_axis;
			unsigned long long int stratum_y = y * nb_strata_axis;
			for(uint i=0; i<tilesize; i++)
			{
				selected_ids[i] = strata[level][stratum_x*nb_strata_axis+stratum_y][i];
				selected_pts[i].pos()[0] = input_pts[ selected_ids[i] ].pos()[0];
				selected_pts[i].pos()[1] = input_pts[ selected_ids[i] ].pos()[1];
			}
			
#ifdef DEBUG
			{
				std::string str;
				str = "sv_stratum";
				
				str += "_" + std::to_string(r.level);
				
				str += "_" + std::to_string(r.cornerStrata[0]);
				str += "_" + std::to_string(r.cornerStrata[1]);
				
				str += ".dat";
					
				std::ofstream file;
				file.open(str);
				for(uint i=0; i<tilesize; i++)
					file << selected_pts[i].pos()[0].to_ulong() << " " << selected_pts[i].pos()[1].to_ulong() << std::endl;
				
				file.close();
			}
#endif

			/**
			* We apply the accumulated XOR values to reposition the 0 point as it would have been premutted in the previous level
			***/
			Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > xored_selected_pts;
			xored_selected_pts.resize(tilesize);
			for(uint i=0; i<selected_pts.size(); i++)
			{
				xored_selected_pts[i].pos()[0] = selected_pts[i].pos()[0].to_ulong() ^ r.digital_shift[0];
				xored_selected_pts[i].pos()[1] = selected_pts[i].pos()[1].to_ulong() ^ r.digital_shift[1];
			}
			
#ifdef DEBUG
			{
				std::string str;
				str = "sv_xored";
				
				str += "_" + std::to_string(r.level);
				
				str += "_" + std::to_string(r.cornerStrata[0]);
				str += "_" + std::to_string(r.cornerStrata[1]);
				
				str += ".dat";
					
				std::ofstream file;
				file.open(str);
				for(uint i=0; i<tilesize; i++)
					file << xored_selected_pts[i].pos()[0].to_ulong() << " " << xored_selected_pts[i].pos()[1].to_ulong() << std::endl;
				
					//file << "\\filldraw[black] (" << xored_selected_pts[i].pos()[0].to_ulong() << "," << xored_selected_pts[i].pos()[1].to_ulong() << ") circle (16pt)" << std::endl;
				
				file.close();
			}
#endif
			
			/**
			* Retrieving the LUT key from the digits of the xored points
			***/
			Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > key_pts;
			key_pts.resize(tilesize);
			for(uint i=0; i<xored_selected_pts.size(); i++)
				for(uint b=0; b<logkd; b++)
				{
					key_pts[i].pos()[0][logkd-1-b] = xored_selected_pts[i].pos()[0][bit1-b-1]; //we read the logk most significant bits from the position (bitmax-bit0) to (bitmax-bit0-logk)
					key_pts[i].pos()[1][logkd-1-b] = xored_selected_pts[i].pos()[1][bit1-b-1];
				}
				
#ifdef DEBUG
			{
				std::string str;
				str = "sv_key";
				
				str += "_" + std::to_string(r.level);
				
				str += "_" + std::to_string(r.cornerStrata[0]);
				str += "_" + std::to_string(r.cornerStrata[1]);
				
				str += ".dat";
					
				std::ofstream file;
				file.open(str);
				for(uint i=0; i<tilesize; i++)
					file << key_pts[i].pos()[0].to_ulong() << " " << key_pts[i].pos()[1].to_ulong() << std::endl;
				
				file.close();
			}
#endif
			
			/**
			* Retrieving the permutation from the LUT
			***/
			Vector<2, unsigned long long int> permuts;
			SamplerBNLDS_LUT_Key<2> key(key_pts);

			if(!lookuptable.getAnOptimizedPermut(key, permuts))
			{
				if(!lookuptable.computeAnOptimizedPermut(key, permuts))
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
					ERROR("SamplerBNLDS::recursivePermutations Couldn't find or generate an entry for pattern " + str);
					return false;
				}
				
				time_t rawtime;
				struct tm * timeinfo;
				char buffer [80];
				time (&rawtime);
				timeinfo = localtime (&rawtime);
				strftime (buffer,80,"%F-%R:%S",timeinfo);
				lookuptable.write("lut_"+std::string(buffer)+".dat");
			}
			//lookuptable.getRandomPermut(key, permuts);
			
			/**
			* Apply the permutation to the pointset
			***/
			//std::cout << "Permutation " << permuts << std::endl;
			Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > permutted_selected_pts;
			permutted_selected_pts.resize(tilesize);
			applyPermut(xored_selected_pts, permutted_selected_pts, permuts, level, levelmax);
			
			
// 			std::stringstream filename;
// 			filename << "permut_";
// 			int N = tilesize-1-1;
// 			for(int i=0; i<logkd; i++)
// 			{
// 				for(int j=0; j<(1 << i); j++)
// 				{
// 					filename << ((permuts[0] >> N) & 1);
// 					N--;
// 				}
// 				filename << "_";
// 			}
// 			filename << "x.tex";
// 			std::ofstream file;
// 			file.open(filename.str());
// 			file << "\\documentclass[10pt,a4paper]{article}\n\\usepackage[paperheight=10in,paperwidth=10in, margin=1in]{geometry} \n\\usepackage[utf8]{inputenc}\n\\usepackage{amsmath}\n\\usepackage{amsfonts}\n\\usepackage{tikz}\n\\usepackage{amssymb}\n" << std::endl;
// 			file << "\\begin{document}" << std::endl;
// 			
// 			
// 			file << "\\thispagestyle{empty} \n \\begin{tikzpicture}[scale=1.25]" << std::endl;
// 			file << "\\draw (0, 0) rectangle (16,16);" << std::endl;
// 			
// 			if(permut_val <= 64)
// 			{
// 				for(int i=0; i<16; i++)
// 					file << "\\draw (" << i <<", 0) -- (" << i << ",16);" << std::endl;
// 			}
// 			else if (permut_val <= 512)
// 			{
// 				for(int i=0; i<16; i+=2)
// 					file << "\\draw (" << i <<", 0) -- (" << i << ",16);" << std::endl;
// 			}
// 			else if (permut_val <= 1024)
// 			{
// 				for(int i=0; i<16; i+=4)
// 					file << "\\draw (" << i <<", 0) -- (" << i << ",16);" << std::endl;
// 			}
// 			else
// 			{
// 				for(int i=0; i<16; i+=8)
// 					file << "\\draw (" << i <<", 0) -- (" << i << ",16);" << std::endl;
// 			}
// 			
// 			for(uint i=0; i<tilesize; i++)
// 			{
// 				file << "\\filldraw[lightgray] (" << selected_pts[i].pos()[0].to_ulong() << "," << selected_pts[i].pos()[1].to_ulong()<< ") circle (6pt);" << std::endl;
// 			}
// 			for(uint i=0; i<tilesize; i++)
// 			{
// 				file << "\\draw[->, very thick, red] (" << permutted_selected_pts[i].pos()[0].to_ulong() << "," << permutted_selected_pts[i].pos()[1].to_ulong()<< ") -- (" << selected_pts[i].pos()[0].to_ulong() << "," << selected_pts[i].pos()[1].to_ulong() << ");" << std::endl;
// 			}
// 			for(uint i=0; i<tilesize; i++)
// 			{
// 				file << "\\filldraw (" << permutted_selected_pts[i].pos()[0].to_ulong() << "," << permutted_selected_pts[i].pos()[1].to_ulong()<< ") circle (6pt);" << std::endl;
// 			}
// 			file << "\\end{tikzpicture}" << std::endl;
// 			file << "\\newpage" << std::endl;
			
			
// 			N = tilesize-1-1;
// 			file << "\\thispagestyle{empty} \n \\begin{tikzpicture}[scale=1.25]" << std::endl;
// 			for(int i=0; i<logkd; i++)
// 			{
// 				for(int j=0; j<(1 << i); j++)
// 				{
// 					if(i+1 < logkd)
// 					{
// 						int i2 = i+1;
// 						for(int j2=2*j; j2<2*j+2; j2++)
// 							file << "\\draw (" << (1.0/pow(2,(i+1)))*16 + j*((1.0/pow(2,i))*16) << "," << (2*(logkd-i)) << ") -- (" << (1.0/pow(2,(i2+1)))*16 + j2*((1.0/pow(2,i2))*16) << "," << (2*(logkd-i2)) << ");" << std::endl;
// 					}
// 					
// 					int flag = ((permuts[0] >> N) & 1);
// 					N--;
// 					
// 					file << "\\filldraw";
// 					if(flag == 1)
// 						file << "[draw=black, fill=red!30]";
// 					else
// 						file << "[draw=black, fill=white]";
// 					file << " (" << (1.0/pow(2,(i+1)))*16 + j*((1.0/pow(2,i))*16) << "," << (2*(logkd-i)) << ") circle (10pt);" << std::endl;
// 					file << "\\draw (" << (1.0/pow(2,(i+1)))*16 + j*((1.0/pow(2,i))*16) << "," << (2*(logkd-i)) << ") node {" << flag << "};" << std::endl;
// 					
// 					
// 				}
// 				
// 				}
// 			file << "\\end{tikzpicture}" << std::endl;
// 			
// 			file << "\\end{document}" << std::endl;
// 			
// 			file.close();
			
#ifdef DEBUG
			{
				std::string str;
				str = "sv_permutted";
				
				str += "_" + std::to_string(r.level);
				
				str += "_" + std::to_string(r.cornerStrata[0]);
				str += "_" + std::to_string(r.cornerStrata[1]);
				
				str += ".dat";
					
				std::ofstream file;
				file.open(str);
				for(uint i=0; i<tilesize; i++)
					file << permutted_selected_pts[i].pos()[0].to_ulong() << " " << permutted_selected_pts[i].pos()[1].to_ulong() << std::endl;
				
				file.close();
			}
#endif
				
			
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
				for(uint i=0; i<tilesize; i++)
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
					std::bitset<SamplerBNLDS_BITSETSIZE> pt;
					for(uint b=0; b<(level+1)*logk; b++)
						pt[(level+1)*logk - 1 -b] = selected_pts[i].pos()[0][bitmax-1-b];
					uint nb_side_strata = nb_pts_total / pow(sqrt(tilesize), level+1);
					//std::cout << pt.to_ullong() << " * " << nb_side_strata << std::endl;
					rprime.cornerStrata[0] = pt.to_ulong() * nb_side_strata;
					for(uint b=0; b<(level+1)*logk; b++)
						pt[(level+1)*logk - 1 -b] = selected_pts[i].pos()[1][bitmax-1-b];
					rprime.cornerStrata[1] = pt.to_ulong() * nb_side_strata;
					
					/**
					* Recurse
					***/
					stack.push(rprime);
				}
			}
		}
		
		lookuptable.write(lookuptable_outfile);
		
		return true;
		
	}
	
	void applyPermut(Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > in_pts, 
					 Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > >& out_pts,
					 Vector<2, unsigned long long int> permuts,
					 uint level,
				     uint level_max
					)
	{
		auto N = in_pts.size();
		Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > pts2;
		pts2.resize(N);
		
		uint logkd = log2(tilesize);
		
		Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > in_pts_sortx;
		in_pts_sortx.resize(N);
		Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > in_pts_sorty;
		in_pts_sorty.resize(N);
		for(uint i=0; i<N; i++)
		{
			in_pts_sortx[i] = in_pts[i];
			in_pts_sorty[i] = in_pts[i];
		}
		std::sort(in_pts_sortx.begin(), in_pts_sortx.end(), sortBitsetX_2D2D);
		std::sort(in_pts_sorty.begin(), in_pts_sorty.end(), sortBitsetY_2D2D);
		
		std::bitset<SamplerBNLDS_BITSETSIZE> xpermuts[2];
		xpermuts[0] = permuts[0];
		xpermuts[1] = permuts[1];
		
		Pointset< 2, std::bitset<SamplerBNLDS_BITSETSIZE>, Point< 2, std::bitset<SamplerBNLDS_BITSETSIZE> > > pts_indices;
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
		
		int ind;
		
		uint NBITS = logkd;
		uint PERMUTS_NBITS = tilesize-1;
		
		for(uint ipt=0; ipt<tilesize; ipt++) {
		std::bitset<SamplerBNLDS_BITSETSIZE> xresdigits[2];
		std::bitset<SamplerBNLDS_BITSETSIZE> thisSobolPt[2];
		for(uint d=0; d<2; d++)
			thisSobolPt[d] = pts_indices[ipt].pos()[d];

		for(uint ilevel=0; ilevel<NBITS; ilevel++) {
			
			for(uint d=0; d<2; d++)
			{
				ind = (pts_indices[ipt].pos()[d].to_ulong() >> (NBITS-ilevel)) + (pow(2,ilevel)-1);
				xresdigits[d].set(NBITS-ilevel-1, (xpermuts[d][PERMUTS_NBITS-ind-1] ^ thisSobolPt[d][NBITS-ilevel-1]) );
			}
			
		}
		
		for(uint d=0; d<2; d++)
			pts_indices[ipt].pos()[d] = xresdigits[d].to_ulong();
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
		nb_pts_total <<= (logKd*level_max);
		
		for(uint l=0; l<level_max; l++)
		{	
			uint nb_strata = 1;
			nb_strata <<= (logKd*l);
			
			uint nb_strata_axis = 1;
			nb_strata_axis <<= (logK*l);
			
			strata[l].resize( nb_strata );
			for(uint i=0; i<nb_strata; i++)
				strata[l][i].resize(tilesize);
			
			std::vector< uint > cpt_strata;
			cpt_strata.resize(nb_strata);
			
			unsigned long long int nbpts = 1;
			nbpts <<= (logKd*(l+1));
			for(unsigned long long int i=0; i<nbpts; i++)
			{
				double x = input_pts[i].pos()[0];
				double y = input_pts[i].pos()[1];
				x /= nb_pts_total;
				y /= nb_pts_total;
				unsigned long long int stratum_x = x * nb_strata_axis;
				unsigned long long int stratum_y = y * nb_strata_axis;
				
				uint& n = cpt_strata[stratum_x*nb_strata_axis + stratum_y];
				if(n >= tilesize)
				{
					ERROR("SamplerBNLDS::initializeStrata non dyadic point repartition");
					return false;
				}
				strata[l][stratum_x*nb_strata_axis + stratum_y][n] = i;
				n++;
			}
			
			for(unsigned long long int i=0; i<nb_strata; i++)
				if( cpt_strata[i] != tilesize )
				{
					ERROR("SamplerBNLDS::initializeStrata non dyadic point repartition");
					return false;
				}
		}
			
		return true;
	}
	
	uint subdiv_factor_K;
	uint tilesize;
	std::string lookuptable_infile;
	std::string lookuptable_outfile;
	std::vector<uint> sobol_ids;
	
	SamplerSobolIndices sobol;
	std::vector< std::vector< std::vector<uint> > > strata;
	SamplerBNLDS_LUT<2> lookuptable;
};

} //end namespace utk

#endif
