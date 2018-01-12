#ifndef _UTK_SAMPLER_SOBOL_
#define _UTK_SAMPLER_SOBOL_

// Frances Y. Kuo
//
// Email: <f.kuo@unsw.edu.au>
// School of Mathematics and Statistics
// University of New South Wales
// Sydney NSW 2052, Australia
// 
// Last updated: 21 October 2008
//
//   You may incorporate this source code into your own program 
//   provided that you
//   1) acknowledge the copyright owner in your program and publication
//   2) notify the copyright owner by email
//   3) offer feedback regarding your experience with different direction numbers
//
//
// -----------------------------------------------------------------------------
// Licence pertaining to sobol.cc and the accompanying sets of direction numbers
// -----------------------------------------------------------------------------
// Copyright (c) 2008, Frances Y. Kuo and Stephen Joe
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the names of the copyright holders nor the names of the
//       University of New South Wales and the University of Waikato
//       and its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// -----------------------------------------------------------------------------

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <fstream>

namespace utk
{
	
class SamplerSobol
{
public:
	
	SamplerSobol() :
		m_filename_direction_numbers ("new-joe-kuo-6.21201.txt") {}
		
	void setFilenameDirectionNumbers(std::string arg_filename) { m_filename_direction_numbers = arg_filename; }
	
	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerSobol::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		unsigned long long int N = arg_points;
		arg_pts.resize(arg_points);
		
		arg_pts.toricity = 1;
		
		//load the direction numbers
		std::ifstream infile(m_filename_direction_numbers,std::ios::in);
		if (!infile) {
			ERROR("SamplerSobol::generateSamples Input file containing direction numbers cannot be found!");
			return false;
		}
		
		char buffer[1000];
		infile.getline(buffer,1000,'\n');
		
		// L = max number of bits needed 
		unsigned L = (unsigned)ceil(log((double)N)/log(2.0)); 

		// C[i] = index from the right of the first zero bit of i
		unsigned *C = new unsigned [N];
		C[0] = 1;
		for (unsigned i=1;i<=N-1;i++) {
			C[i] = 1;
			unsigned value = i;
			while (value & 1) {
			value >>= 1;
			C[i]++;
			}
		}
		
		// POINTS[i][j] = the jth component of the ith point
		//                with i indexed from 0 to N-1 and j indexed from 0 to D-1
		double **POINTS = new double * [N];
		for (unsigned i=0;i<=N-1;i++) POINTS[i] = new double [D];
		for (unsigned j=0;j<=D-1;j++) POINTS[0][j] = 0; 

		// ----- Compute the first dimension -----
		
		// Compute direction numbers V[1] to V[L], scaled by pow(2,32)
		unsigned *V = new unsigned [L+1]; 
		for (unsigned i=1;i<=L;i++) V[i] = 1 << (32-i); // all m's = 1

		// Evalulate X[0] to X[N-1], scaled by pow(2,32)
		unsigned *X = new unsigned [N];
		X[0] = 0;
		for (unsigned i=1;i<=N-1;i++) {
			X[i] = X[i-1] ^ V[C[i-1]];
			POINTS[i][0] = (double)X[i]/pow(2.0,32); // *** the actual points
			//        ^ 0 for first dimension
		}
		
		// Clean up
		delete [] V;
		delete [] X;
		
		
		// ----- Compute the remaining dimensions -----
		for (unsigned j=1;j<=D-1;j++) {
			
			// Read in parameters from file 
			unsigned d, s;
			unsigned a;
			infile >> d >> s >> a;
			unsigned *m = new unsigned [s+1];
			for (unsigned i=1;i<=s;i++) infile >> m[i];
			
			// Compute direction numbers V[1] to V[L], scaled by pow(2,32)
			unsigned *V = new unsigned [L+1];
			if (L <= s) {
			for (unsigned i=1;i<=L;i++) V[i] = m[i] << (32-i); 
			}
			else {
			for (unsigned i=1;i<=s;i++) V[i] = m[i] << (32-i); 
			for (unsigned i=s+1;i<=L;i++) {
			V[i] = V[i-s] ^ (V[i-s] >> s); 
			for (unsigned k=1;k<=s-1;k++) 
			V[i] ^= (((a >> (s-1-k)) & 1) * V[i-k]); 
			}
			}
			
			// Evalulate X[0] to X[N-1], scaled by pow(2,32)
			unsigned *X = new unsigned [N];
			X[0] = 0;
			for (unsigned i=1;i<=N-1;i++) {
			X[i] = X[i-1] ^ V[C[i-1]];
			POINTS[i][j] = (double)X[i]/pow(2.0,32); // *** the actual points
			//        ^ j for dimension (j+1)
		}
			
			// Clean up
			delete [] m;
			delete [] V;
			delete [] X;
		}
		delete [] C;
		
		
		if(isIntegerType<T>())
		{
			int smax = ceil(log2(arg_points));
			smax = pow(2, smax);
			
			for(uint d=0; d<D; d++){
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = smax;
			}
			
			for(uint i=0; i<arg_points; i++)
			{
				for(uint d=0; d<D; d++)
				{
					double p = POINTS[i][d];
					if (p > 1)
						p -= 1;
					arg_pts[i].pos()[d] = p*smax;
				}
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
				{
					arg_pts[i].pos()[d] = POINTS[i][d];
					if (arg_pts[i].pos()[d] > 1)
						arg_pts[i].pos()[d] -= 1;
				}
			}
		}
		else
		{
			ERROR("SamplerSobol::generateSamples The samples corrdinates must be of numeric value");
			return false;
		}

		delete [] POINTS;
		
		return true;
	}
	
protected:
	std::string m_filename_direction_numbers;
};	
	
} //end namespace utk

#endif
