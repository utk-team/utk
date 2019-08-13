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
#ifndef _UTK_SAMPLER_SOBOL_INDICES_
#define _UTK_SAMPLER_SOBOL_INDICES_

#include "../pointsets/Pointset.hpp"
#include <fstream>

#define DMAX 32
#define a058947_LENGTH 32

#include "../utils.hpp"

namespace utk
{
	
class SamplerSobolIndices
{
protected:
public:
	
	SamplerSobolIndices() {
		
		for(int i=0; i<32; i++)
			m_indices[i] = i+1;
		m_dimensions = 32;
	}
	
	void setIndices(int* arg_indices, uint arg_dimension)
	{
		if(arg_dimension > DMAX)
			WARNING("SamplerSobolIndices::setIndices Can only generate Sobol Samples in dimensions < 32, only taking the 32 firsts indices\n");
		
		for(uint i=0; i<arg_dimension; i++)
			m_indices[i] = arg_indices[i];
		
		m_dimensions = arg_dimension;
	}
	
	/*template<uint D, typename T, typename P>
	bool generateSamplesIn(Pointset<D, T, P>& arg_pts, uint arg_points, double* x, uint lvl)
	{
		if(D > DMAX)
		{
			ERROR("SamplerSobolIndices::generateSamples Can only generate Sobol Samples in dimensions < 10\n");
			exit(1);
		}
		if (!arg_pts.empty())
		{
			WARNING("SamplerSobolIndices::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		arg_pts.resize(arg_points);
		arg_pts.toricity = 1;
		
		if(isIntegerType<T>())
		{
			ERROR("SamplerSobolIndices::generateSamples No le he hecho ya\n");
		}
		else if (isFloatingType<T>())
		{
			for(int d=0; d<D; d++){
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			
			std::vector<uint> id = getIndicesIn<D, T, P>(x, D, lvl, arg_points);
			for(int i=0; i<arg_points; i++)
			{
				//std::cout << id[i] << std::endl;
				arg_pts[i] = generateIthPoint<D, T, P>(id[i]);
			}
		}
		else
		{
			ERROR("SamplerSobolIndices::generateSamples The samples coordinates must be of numeric value");
			return false;
		}

		return true;
	}*/
	
	//Grünschloß, L., Raab, M., & Keller, A. (2012). Enumerating quasi-monte carlo point sequences in elementary intervals. 
	//Monte Carlo and Quasi-Monte Carlo. Retrieved from http://link.springer.com/chapter/10.1007/978-3-642-27440-4_21
	/*template<uint D>
	bool getIndicesInElementaryInterval(std::vector<uint>& arg_indices, uint pj[D], uint dj[D])
	{
		uint sumDj = 0;
		for(uint i=0;i<D; i++)
			sumDj += dj[i];
		
		uint** A = (uint**)malloc(sizeof(uint*)*sumDj);
		for(uint i=0;i<sumDj; i++)
			A[i] = (uint*)malloc(sizeof(uint)*sumDj);
		
		uint currentrow = 0;
		for(uint d=0;d<D;d++)
		{
			for(uint j=0;j<sumDj; j++)
			for(uint i=0;i<dj[d];i++)
				A[currentrow+i][j] = getValueFromMatrix(i, j, d);
			currentrow+=dj[d];
		}
		
		for(uint i=0;i<sumDj; i++)
		{
			for(uint j=0;j<sumDj; j++)
				std::cout << A[i][j] << " ";
			std::cout << std::endl;
		}
		
		for(uint i=0;i<sumDj; i++)
			free(A[i]);
		free(A);
		
		return true;
	}*/
	
	template<uint D, typename T, typename P>
	uint getIndexFromSample(P arg_sample)
	{
		return getIndexFromSampleAndDimension<D, T, P>(arg_sample, 0);
	}
	
	template<uint D, typename T, typename P>
	uint getIndexFromSampleGrayCode(P arg_sample)
	{
		uint g_i = getIndexFromSampleAndDimension<D, T, P>(arg_sample, 0);
		return Ginv(g_i);
	}
	
	template<uint D, typename T, typename P>
	P generateIthPointGrayCode(uint i)
	{
		if(D > DMAX)
		{
			ERROR("SamplerSobolIndices::generateIthPoint Can only generate Sobol Samples in dimensions < 10\n");
			exit(1);
		}
		P pt;
		for(uint d=0; d<D; d++)
			pt.pos()[d] = sobol1dGrayCode(i, d);
		return pt;
	}
	
	template<uint D, typename T, typename P>
	P generateIthPoint(uint i)
	{
		if(D > DMAX)
		{
			ERROR("SamplerSobolIndices::generateIthPoint Can only generate Sobol Samples in dimensions < 10\n");
			exit(1);
		}
		P pt;
		for(uint d=0; d<D; d++)
		{
			pt.pos()[d] = sobol1d(i, d);
		}
		return pt;
	}
	
	template<uint D, typename T, typename P>
	bool generateSamplesGrayCode(Pointset<D, T, P>& arg_pts, uint arg_points)
	{
		if(!buildMSobol(m_indices, m_dimensions))
			return false;
		
		if(D > DMAX)
		{
			ERROR("SamplerSobolIndices::generateSamples Can only generate Sobol Samples in dimensions < 10\n");
			return false;
		}
		if (!arg_pts.empty())
		{
			WARNING("SamplerSobolIndices::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		arg_pts.resize(arg_points);
		arg_pts.toricity = 1;
		
		if(isIntegerType<T>())
		{
			uint smax = ceil(log2(arg_points));
			smax = pow(2, smax);
			
			for(uint d=0; d<D; d++){
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = smax;
			}
			
			for(uint i=0; i<arg_points; i++)
			{
				Point<D, double> pt = generateIthPointGrayCode< D, double, Point<D, double> >(i);
				for(int d=0; d<D; d++)
				{
					arg_pts[i].pos()[d] = pt.pos()[d]*smax;
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
				arg_pts[i] = generateIthPointGrayCode<D, T, P>(i);
			}
		}
		else
		{
			ERROR("SamplerSobolIndices::generateSamples The samples coordinates must be of numeric value");
			return false;
		}

		return true;
	}
	
	template<uint D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
	{
		if(!buildMSobol(m_indices, m_dimensions))
			return false;
		
		if(D > DMAX)
		{
			ERROR("SamplerSobolIndices::generateSamples Can only generate Sobol Samples in dimensions < 10\n");
			exit(1);
		}
		if (!arg_pts.empty())
		{
			WARNING("SamplerSobolIndices::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		arg_pts.resize(arg_points);
		arg_pts.toricity = 1;
		
		if(isIntegerType<T>())
		{
			//int smax = ceil(log2(arg_points));
			//smax = pow(2, smax);
			
			unsigned long long int smax = ceil(log2(arg_points));
			smax = pow(2, smax);
			
			for(uint d=0; d<D; d++){
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = smax;
			}
			
			for(unsigned long long int i=0; i<arg_points; i++)
			{
				Point<D, double> pt = generateIthPoint< D, double, Point<D, double> >(i);
				for(uint d=0; d<D; d++)
					arg_pts[i].pos()[d] = pt.pos()[d]*smax;
			}
		}
		else if (isFloatingType<T>())
		{
			for(uint d=0; d<D; d++){
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			
			for(unsigned long long int i=0; i<arg_points; i++)
			{
				arg_pts[i] = generateIthPoint<D, T, P>(i);
			}
		}
		else
		{
			ERROR("SamplerSobolIndices::generateSamples The samples coordinates must be of numeric value");
			return false;
		}

		return true;
	}
	
protected:
	std::string m_filename_direction_numbers;

	int m_indices[DMAX] = {0};
	int m_dimensions;
	
	uint m_polynomial[a058947_LENGTH] = {1,11,111,1011,1101,
	10011,11001,100101,101001,101111,
	110111,111011,111101,1000011,1011011,
	1100001,1100111,1101101,1110011,10000011,
	10001001,10001111,10010001,10011101,10100111,
	10101011,10111001,10111111,11000001,11001011,
	11010011,11010101};

	//columns of the sobol matrix from diagonal to top,
	//least significant bit on the diagonal and most significant bit on the upper row
	//if m_values is always 1, we have an identity matrix.
	uint m_mvalues[DMAX][a058947_LENGTH] = { {1} };
	
	uint getValueFromMatrix(uint row, uint column, uint dimension)
	{
		if(row > column)
			return 0;
		return ithBit(m_mvalues[dimension][column], column-row);
	}

	template<uint B>
	uint fromBase10toBase(int n)
	{
		uint res = 0;
		uint power = 0;
		while(n != 0)
		{
			uint tmp = n%10;
			tmp *= B;
			res += pow(tmp, power);
			n /= 10;
			power++;
		}
		return res;
	}
	uint ithBit(uint val, uint bitid)
	{
		return (val >> bitid) & 1u;
	}
	
	bool buildMSobol1D(int ID, uint d)
	{
		if(ID <= 0)
		{
			ERROR("SobolIndices::buildMSobol1D Indices <= 0");
			return false;
		}
		
		uint polynomial = fromBase10toBase<2>(m_polynomial[ID-1]);
		uint nbbits_polynomials = log2(polynomial)+1;

		for(uint i=0; i<a058947_LENGTH; i++)
			m_mvalues[d][i]=1;
		for(uint i=2; i<nbbits_polynomials; i++)
			m_mvalues[d][i-1] = (pow(2, (i-1)) - 1)*2 + 1;
		
		uint* val_xor_0 = (uint*) malloc(sizeof(uint)*nbbits_polynomials);
		for(uint i=nbbits_polynomials; i<=a058947_LENGTH; i++)
		{
			for(uint j=0; j<nbbits_polynomials-1; j++)
			{
				val_xor_0[j] = pow(2, (j+1)) * ithBit(polynomial,nbbits_polynomials-j-2) * m_mvalues[d][i-j-2];
			}
			
			uint val_xor_1 = m_mvalues[d][i-nbbits_polynomials];
			//std::cout << "xor1 "<< val_xor_1 << std::endl;
			
			m_mvalues[d][i-1] = val_xor_1;
			for(uint j=0; j<nbbits_polynomials-1; j++)
				m_mvalues[d][i-1] ^= val_xor_0[j];
		}
			
		free(val_xor_0);
		return true;
	}
	bool buildMSobol(int* IDs, int nb)
	{
		for(int i=0; i<nb; i++)
		{
			if(!buildMSobol1D(IDs[i], i))
				return false;
		}
		return true;
	}

	double sobol1d(uint n, int d)
	{
		uint nbbits_polynomials = log2(n)+1;
		if(n==0)
			nbbits_polynomials = 1;
		
		uint* val_xor = (uint*) malloc(sizeof(uint)*nbbits_polynomials);

		for(uint i=0; i<nbbits_polynomials; i++)
			val_xor[i] = ithBit(n, i) * m_mvalues[d][i] * pow(2,nbbits_polynomials-1-i);
		
		uint res = val_xor[0];
		for(uint i=1; i<nbbits_polynomials; i++)
			res ^= val_xor[i];
		
		free(val_xor);

		return (double)res/pow(2, nbbits_polynomials);
	}
	
	/*double invertBits(uint bits, uint size_bits) {
		uint res = 0;
		while(size_bits > 0)
		{
			uint tmp = bits & 1u;
			res = (res << 1) | tmp;
			bits = bits >> 1;
			size_bits--;
		}
		return res;
	}
	
	bool nbbitspairs(uint bits, uint v)
	{
		int nb = 0;
		int size = log2(bits)+1;
		if(bits == 0)
			size = 1;
		int tmp = bits;
		for(int s=0; s<size; s++)
		{
			if((tmp & 1) == v)
				nb++;
			tmp >>= 1;
		}
		return (nb%2==0);
	}
	
	void showbits(uint t)
	{
		int size = log2(t)+1;
		if(t==0)
			size = 1;
		int tmp = t;
		for(int s=0; s<size; s++)
		{
			std::cout << (tmp & 1);
			tmp >>= 1;
		}
	}*/
	
	template<uint D, typename T, typename P>
	uint getIndexFromSampleAndDimension(P arg_sample, int d)
	{
		return getIndexFromCodeAndDimension<D, T, P>(arg_sample.pos()[d], d);
	}
	
	template<uint D, typename T, typename P>
	uint getIndexFromCodeAndDimension(double x, int d)
	{
		const uint L = 20;
		uint code = x * pow(2, L);
		if (code == 0)
			return 0;

		uint g_i = 0;
		uint a[L];
		
		for(int i=L-1; i>=0; i--)
		{
			uint p_i = (code >> (L-1-i)) & 1;
			
			uint xor_with = 0;
			for(int b=L-1; b>i; b--)
			{
				double dv_b = m_mvalues[d][b] * pow(2, -b);
				uint v_b = dv_b * pow(2, a058947_LENGTH-1);
				
				uint a_b = a[b];
				uint v_bi = (v_b >> (a058947_LENGTH-i-1)) & 1;
				
				xor_with ^= (a_b * v_bi);
			}
			a[i] = p_i ^ xor_with;
			
			g_i <<= 1;
			g_i |= a[i];
		}
		
		return g_i;
	}
	
	//Tester juste le bit suivant suffit pas, faut un nombre pair de bits pour que ça marche
	//#intuite
	/*template<uint D, typename T, typename P>
	std::vector<uint> getIndicesIn(double* x, uint dim, uint lvl, int nb)
	{
		std::vector<uint> v(nb, 0);
		uint id_x;
		uint id_y;
		const uint L = 4;
		uint codex, codey;
		
		// 0 0
		P sample;
		for(int d=0; d<D; d++)
			sample.pos()[d] = x[d];
		
		id_x = getIndexFromSampleAndDimension<D, T, P>(sample, 0);
		id_y = getIndexFromSampleAndDimension<D, T, P>(sample, 1);
		
		std::cout << "id_x " << id_x << "   id_y " << id_y << std::endl;
		codex = sample.pos()[0] * pow(2, L); codex = invertBits(codex, L);
		codey = sample.pos()[1] * pow(2, L); codey = invertBits(codey, L);
		std::cout << "    code " << codex << "    "; showbits(codex); std::cout << std::endl;
		std::cout << "         " << codey << "    "; showbits(codey); std::cout << std::endl;
		
		if(id_x == id_y)
		{
			v[0] = id_x;
			return v;
		}
		
		// 1 1
		P sample1 = sample;
		sample1.pos()[0] = x[0] + 1.0/(double)lvl;
		
		id_x = getIndexFromSampleAndDimension<D, T, P>(sample1, 0);
		id_y = getIndexFromSampleAndDimension<D, T, P>(sample1, 1);
		
		std::cout << "id_x " << id_x << "   id_y " << id_y << std::endl;
		codex = sample1.pos()[0] * pow(2, L); codex = invertBits(codex, L);
		codey = sample1.pos()[1] * pow(2, L); codey = invertBits(codey, L);
		std::cout << "    code " << codex << "    "; showbits(codex); std::cout << std::endl;
		std::cout << "         " << codey << "    "; showbits(codey); std::cout << std::endl;
		
		if(id_x == id_y)
		{
			v[0] = id_x;
			return v;
		}
		
		// 1 1
		P sample2 = sample;
		sample2.pos()[1] = x[1] + 1.0/(double)lvl;
		
		id_x = getIndexFromSampleAndDimension<D, T, P>(sample2, 0);
		id_y = getIndexFromSampleAndDimension<D, T, P>(sample2, 1);
		
		std::cout << "id_x " << id_x << "   id_y " << id_y << std::endl;
		codex = sample2.pos()[0] * pow(2, L); codex = invertBits(codex, L);
		codey = sample2.pos()[1] * pow(2, L); codey = invertBits(codey, L);
		std::cout << "    code " << codex << "    "; showbits(codex); std::cout << std::endl;
		std::cout << "         " << codey << "    "; showbits(codey); std::cout << std::endl;
		
		if(id_x == id_y)
		{
			v[0] = id_x;
			return v;
		}
		
		
		// 1 1
		P sample3 = sample;
		for(int d=0; d<D; d++)
			sample3.pos()[d] = x[d] + 1.0/(double)lvl;
		
		id_x = getIndexFromSampleAndDimension<D, T, P>(sample3, 0);
		id_y = getIndexFromSampleAndDimension<D, T, P>(sample3, 1);
		
		std::cout << "id_x " << id_x << "   id_y " << id_y << std::endl;
		codex = sample3.pos()[0] * pow(2, L); codex = invertBits(codex, L);
		codey = sample3.pos()[1] * pow(2, L); codey = invertBits(codey, L);
		std::cout << "    code " << codex << "    "; showbits(codex); std::cout << std::endl;
		std::cout << "         " << codey << "    "; showbits(codey); std::cout << std::endl;
		
		if(id_x == id_y)
		{
			v[0] = id_x;
			return v;
		}
		
		return v;
	}
	
	std::vector<uint> getIndicesIn_1DOnly_IteratingOverAllIndexes(double x, uint lvl, int d, int nb)
	{
		std::vector<uint> ret_samples_indices;
		ret_samples_indices.resize(nb);
		
		std::vector<uint> masks;
		masks.resize(a058947_LENGTH);
		for(int j=0; j<a058947_LENGTH; j++)
			masks[j] = 0;
		
		for(int i=0; i<a058947_LENGTH; i++)
			for(int j=0; j<a058947_LENGTH; j++)
			{
				double dv_value = m_mvalues[d][j] * pow(2, -j);
				uint v_value = dv_value * pow(2, a058947_LENGTH-1);
				//std::cout << m_mvalues[d][j] << "   "; showbits(m_mvalues[d][j]); std::cout <<  std::endl;
				masks[a058947_LENGTH-i-1] <<= 1;
				masks[a058947_LENGTH-i-1] |= ((v_value >> (i)) & 1);
			}
		
		for(int i=0; i<masks.size(); i++)
		{
			masks[i] = invertBits(masks[i], a058947_LENGTH);
			//std::cout << "masks[" <<i << "]   " << masks[i] << "       "; showbits(masks[i]); std::cout << std::endl;
		}
		
		int L = pow(2,lvl);
		
		uint id = 0;
		for(uint i=0; i<nb; i++)
		{
			//std::cout << "id " << id << "    "; showbits(id); std::cout << std::endl;
			
			bool test_all_constraints = true;
			int pix = x*L;
			int ix = invertBits(pix, lvl);
			for(int f=0; f<lvl;f++)
			{
				uint masked_id = (id & masks[f]);
				test_all_constraints &= ( nbbitspairs(masked_id, 1) != ( (ix>>f) & 1) );
			}
			
			while( !test_all_constraints )
			{
				id++;
				//std::cout << "w id " << id << "    "; showbits(id); std::cout << std::endl;
				int pix = x*L;
				int ix = invertBits(pix, lvl);
				//std::cout << "pix " << pix << "  ix " << ix << "   L " << L << std::endl;
				test_all_constraints = true;
				for(int f=0; f<lvl;f++)
				{
					uint masked_id = (id & masks[f]);
					//std::cout << "mask " << f << " " << masks[f] << "    "; showbits(masks[f]); std::cout << std::endl;
					//std::cout << "masked_id " << f << " " << masked_id << "    "; showbits(masked_id); std::cout << std::endl;
					test_all_constraints &= ( nbbitspairs(masked_id, 1) != ( (ix>>f) & 1) );
					//std::cout << "nbbitspairs(masked_id, 1) " << nbbitspairs(masked_id, 1) << "    (ix>>f) & 1 " << ( (ix>>f) & 1 ) << std::endl;
				}
				
				//if(id > 4)
				//	exit(1);
			}
				
			//std::cout << "a id " << id << "    "; showbits(id); std::cout << std::endl;
			ret_samples_indices[i] = id;
			id++;
		}
		return ret_samples_indices;
	}
	
	std::vector<uint> getIndicesIn_IteratingOverAllIndexes(double* x, uint lvl, int dim, int nb)
	{
		std::vector<uint> ret_samples_indices;
		ret_samples_indices.resize(nb);
		
		std::vector<uint> masks[DMAX];
		for(int i=0; i<dim; i++)
		{
			masks[i].resize(a058947_LENGTH);
			for(int j=0; j<a058947_LENGTH; j++)
				masks[i][j] = 0;
		}
		
		for(int d=0; d<dim; d++)
			for(int i=0; i<a058947_LENGTH; i++)
				for(int j=0; j<a058947_LENGTH; j++)
				{
					double dv_value = m_mvalues[d][j] * pow(2, -j);
					uint v_value = dv_value * pow(2, a058947_LENGTH-1);
					//std::cout << m_mvalues[d][j] << "   "; showbits(m_mvalues[d][j]); std::cout <<  std::endl;
					masks[d][a058947_LENGTH-i-1] <<= 1;
					masks[d][a058947_LENGTH-i-1] |= ((v_value >> (i)) & 1);
				}
		
		for(int d=0; d<dim; d++)
		for(int i=0; i<masks[d].size(); i++)
		{
			masks[d][i] = invertBits(masks[d][i], a058947_LENGTH);
			//std::cout << "masks[" << d << "][" <<i << "]   " << masks[d][i] << "       "; showbits(masks[d][i]); std::cout << std::endl;
		}
		
		int L = pow(2,lvl);
		
		//std::cout << "L " << L << " pix " << pix << " ix " << ix << "    "; showbits(ix);  std::cout << std::endl;;
		
		uint id = 0;
		for(uint i=0; i<nb; i++)
		{
			//std::cout << "id " << id << "    "; showbits(id); std::cout << std::endl;
			
			bool test_all_constraints = true;
			for(int d=0; d<dim; d++)
			{
				int pix = x[d]*L;
				int ix = invertBits(pix, lvl);
				for(int f=0; f<lvl;f++)
				{
					uint masked_id = (id & masks[d][f]);
					test_all_constraints &= ( nbbitspairs(masked_id, 1) != ( (ix>>f) & 1) );
				}
			}
			while( !test_all_constraints )
			{
				id++;
				//std::cout << "w id " << id << "    "; showbits(id); std::cout << std::endl;
				test_all_constraints = true;
				for(int d=0; d<dim; d++)
				{
					int pix = x[d]*L;
					int ix = invertBits(pix, lvl);
					for(int f=0; f<lvl;f++)
					{
						uint masked_id = (id & masks[d][f]);
						test_all_constraints &= ( nbbitspairs(masked_id, 1) != ( (ix>>f) & 1) );
					}
				}
			}
				
			//std::cout << "id " << id << "    "; showbits(id); std::cout << std::endl;
			//std::cout << "masked id " << masked_id << "    "; showbits(masked_id); std::cout << std::endl;
			ret_samples_indices[i] = id;
			//std::cout << "ret_id " << ret_samples_indices[i] << "   " << sobol1d(ret_samples_indices[i], dim) << std::endl;
			id++;
		}
		return ret_samples_indices;
	}*/

	unsigned G(unsigned i)
	{
		return i ^ (i/2);
	}
	
	uint Ginv(uint G)
	{
		int i = 0;
		int L = log2(G)+1;
		if(G==0) L= 1;
		i |= (G >> (L-1));
		int last_bit = i;
		for(int b=L-1; b>0; b--)
		{
			int bit = (G >> (b-1)) & 1;
			i <<= 1;
			i |= bit^last_bit;
			last_bit = bit^last_bit;
		}
		return i;
	}

	double sobol1dGrayCode(int n, int d)
	{
		return sobol1d(G(n), d);
	}
};	
	
} //end namespace utk

#endif
