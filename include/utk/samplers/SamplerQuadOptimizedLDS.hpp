/*
 * Coded Bastien Doignies bastien.doignies@liris.cnrs.fr 
 * and David Coeurjolly David.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2025 CNRS Université de Lyon
 * All rights reserved.
 * 
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
 * either expressed or implied, of the UTK project.
 */

#pragma once

#include <utk/utils/Pointset.hpp>

#include <iostream>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <vector>
#include <random>
#include <cmath>
#include <array>

namespace QuadOptimizedLDS {
    typedef int32_t int32;
    typedef uint8_t uchar;

    #define SOBOLGFNSEQLENGTH	10	// 3^10 = 59049 		3^20 = 3486784401
    #define N_SOBOL_INIT_TAB_ENTRIES    48 	// IN initIrreducibleGF3.dat
    #define MATRIX_SZ 20
						//

    constexpr inline unsigned int pow3tab[21] = {1,3,9,27,81,243,729,2187,6561,19683,59049,177147,531441,1594323,4782969,14348907,43046721,129140163,387420489,1162261467,3486784401};
    constexpr inline int32 convertToGF3[3] = {0,2,1};

    inline std::vector<int32> IntegerDigits(int32 val, const int base, const int len) {
      std::vector<int32> digits;
      for (int i = 0; i < len; i++) {
	digits.push_back(val % base);
	val = (val / base);
      }
      return digits;
    }

    inline int32 FromDigits(const std::vector<int32>& digits, const int base, const int len) {
      int32 pow = 1, res = 0;
      for (int i = 0; i < len; i++) {
	res += pow * digits[i];
	pow = pow*base;
      }
      return res;
    }

    inline int32 multiplyByFactorInGFN(const int32 x, const int32 factor, const int base, const int len) {
      std::vector<int32>  digits = IntegerDigits(x,base,len);
      for (int i = 0; i < len; i++)
	digits[i] = (digits[i] * factor) % base;
      return FromDigits(digits, base, len);
    }

    inline int32 BitXorGFN(const int base, const std::vector<int32>& lst, const int len, const int polynomialDegree) {
      int32 digits[SOBOLGFNSEQLENGTH][SOBOLGFNSEQLENGTH];
      for (int i = 0; i < SOBOLGFNSEQLENGTH; i++)
	for (int j = 0; j < SOBOLGFNSEQLENGTH; j++)
	  digits[i][j] = 0;
      for (int i = 0; i <= polynomialDegree; i++) {
	std::vector<int32>  d = IntegerDigits(lst[i],base,len);
	for(int j = 0; j < len; j++)  digits[i][j] = d[j];
      }
      std::vector<int32> final_digits(SOBOLGFNSEQLENGTH+1,0);
      for (int i = 0; i < len; i++) {
	final_digits[i] = 0;
	for (int j = 0; j <= polynomialDegree; j++) {
	  final_digits[i] += digits[j][i];
	}
	final_digits[i] = final_digits[i] % base;
      }
      return FromDigits(final_digits, base, len);
    }       //BitXorGFN

    inline
    void generate_mkGF3(const int32 ipolynomial, const int32 polynomialDegree, int32* msobol, const int base) {	// msobol is suppossed to be pre-filled
      std::vector<int32> polynomial = IntegerDigits(ipolynomial,base,polynomialDegree+1);
      for (int i = polynomialDegree+1; i <= SOBOLGFNSEQLENGTH; i++) {
	std::vector<int32> lst;
	lst.push_back(msobol[i-polynomialDegree-1]);
	for (int j = 1; j < polynomialDegree+1; j++) {
	  lst.push_back(pow3tab[j] * multiplyByFactorInGFN(msobol[i-j-1], convertToGF3[polynomial[polynomialDegree-j]], base, SOBOLGFNSEQLENGTH));
	}
	msobol[i-1] = BitXorGFN(base, lst, i, polynomialDegree);
      }
    }

    inline int32 load_mk(const std::string& filename, const bool dbg_flag, int32* sobol_aj, int32* sobol_sj, int32* sobol_dj, int32 (*sobol_mk)[32]) {
      int dim_from = 1;
      if (dbg_flag) std::cout << "Loading J&K file " << filename << " ... dim_from=" << dim_from << std::endl;
      std::ifstream file(filename);
      if (!file.is_open()) {
	return 0;
      };
      char c = file.get();
      if( c == 'd' ) {
	file.ignore(256, '\n');
      } else {
	file.putback(c);
      }
      c = file.get();
      if( c == 'd' ) {
	file.ignore(256, '\n');
      } else {
	file.putback(c);
      }
      int32 index = dim_from;
      while (file.good() && index < N_SOBOL_INIT_TAB_ENTRIES) {
	int32 d, sj, aj;
	file >> d >> sj >> aj;
	sobol_aj[index] = aj;
	sobol_sj[index] = sj;
	sobol_dj[index] = d;
	if (dbg_flag) std::cout << index << " : " << d << " " << sj << " "  << aj << " \t " << std::flush;
	for (int i = 0; i < sj; ++i) {
	  file >> sobol_mk[index][i];
	  if (dbg_flag) std::cout << sobol_mk[index][i] << " " << std::flush ;
	}
	if (dbg_flag) std::cout << std::endl;
	
	index++;
      }
      return index-2;
    }	// load_mk
    
    inline void fill_mx(int sobol_mk_index, std::vector<std::vector<int>>& mx, const int32 (*sobol_mk)[32])
    {
      for (int i = 0; i < MATRIX_SZ; i++)
      {
	int val = sobol_mk[sobol_mk_index][i];
	int len = i+1;
	
	std::vector<int> digits = IntegerDigits(val,3,len);
	for (int j = 0; j < len; j++)
	  mx[len-j-1][i] = digits[j];
      }
    }

    typedef std::vector< std::vector<int> > sobol3_matrix;

    // integer digits / base 3
    struct integer3
    {
      std::array<int8_t, 10> digits;
      static constexpr unsigned pow3_tab[]= { 1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049 };
      
      integer3( ) : digits() {}
      
      integer3( unsigned x ) : digits()
      {
	for(unsigned i= 0; i < digits.size(); i++)
	  digits[i]= (x / pow3_tab[i]) % 3;
      }
      
      unsigned value( const unsigned m= 10 ) const
      {
	unsigned x= 0;
	for(unsigned i= 0; i < m; i++)
	  x+= pow3_tab[i] * digits[i];
	
	return x;
      }
      
      double value_double( const unsigned m ) const
      {
	return double(value(m)) / double(pow3_tab[m]);
      }
      
      operator unsigned( ) const
      {
	return value();
      }
      
      // x % 3
      static int8_t mod( const int x )
      {
	static constexpr int8_t tab_mod3[]= { 0, 1, 2, 0, 1, 2 };
	assert(x >= 0);
	assert(x < 6 );
	return tab_mod3[x];
      }
      
      // ( a + (b*c)%3 )%3
      static int8_t fma( const int a, const int b, const int c )
      {
	assert(a >= 0);
	assert(a < 3);
	assert(b >= 0);
	assert(b < 3);
	assert(c >= 0);
	assert(c < 3);
	static constexpr int8_t tab_fma4[]= { 0, 0, 0, 0, 0, 1, 2, 0, 0, 2, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 2, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 0, 1, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	return tab_fma4[a*4*4 + b*4 + c];
      }
    };

    // build the Gray code of a base-3 integer
    // cf https://en.wikipedia.org/wiki/Gray_code#n-ary_Gray_code
    inline integer3 graycode( const integer3 n )
    {
      integer3 gray;
      
      int shift= 0;
      for(int i= n.digits.size() -1; i >= 0; i--)
      {
	// The Gray digit gets shifted down by the sum of the higher digits.
	gray.digits[i]= integer3::mod(n.digits[i] + shift);
	shift= integer3::mod(shift + 3 - gray.digits[i]);
      }
      
      return gray;
    }


    // generate a sobol point in Gray code order.
    inline integer3 point3_graycode( const sobol3_matrix& matrix, const unsigned m, const integer3& ig3, integer3& pg3, integer3& x3 )
    {
      // find modified digit : previous graycode(i-1) and graycode(i)
      unsigned g= 0;
      for(; g < m; g++)
	if(ig3.digits[g] != pg3.digits[g])
	  break;
      
      int d= int(ig3.digits[g]) - int(pg3.digits[g]);
      d= integer3::mod(d + 3);
      assert(d == 1);
      
      // update previous point
      for(unsigned j= 0; j < m; j++)
	x3.digits[j]= integer3::mod(x3.digits[j] + matrix[m-1 - j][g]);
	
      // update previous graycode index
      pg3= ig3;
      
      return x3;
    }


    // generate a sobol point by incrementally modifying the previous point.
    inline integer3 point3_digits( const sobol3_matrix& matrix, const unsigned m, const integer3& i3, integer3& p3, integer3& x3 )
    {
      for(unsigned k= 0; k < m; k++)
      {
	// find modified digits : previous index (i-1) and index (i)
	if(p3.digits[k] != i3.digits[k])
	{
	  int d= int(i3.digits[k]) - int(p3.digits[k]);
	  d= integer3::mod(d + 3);
	  
	  // update previous point
	  for(unsigned j= 0; j < m; j++)
	    x3.digits[j]= integer3::fma(x3.digits[j], d, matrix[m-1 - j][k]);
	}
      }
      
      // update previous index
      p3= i3;
      return x3;
    }


    // counter based rng
    // all small crush tests ok
    struct FCRNG
    {
      unsigned n;
      unsigned key;
      
      FCRNG( ) : n(), key() { seed(123451); }
      FCRNG( const unsigned s ) : n(), key() { seed(s); }
      void seed( const unsigned s ) { n= 0; key= (s << 1) | 1u; }
      
      FCRNG& index( const unsigned i ) { n= i; return *this;}
      unsigned sample( ) { return hash(++n * key); }
      
      unsigned sample_range( const unsigned range )
      {
	// Efficiently Generating a Number in a Range
	// cf http://www.pcg-random.org/posts/bounded-rands.html
	unsigned divisor= ((-range) / range) + 1; // (2^32) / range
	if(divisor == 0) return 0;
	
	while(true)
	{
	  unsigned x= sample() / divisor;
	  if(x < range) return x;
	}
      }
      
      // c++ interface
      unsigned operator() ( ) { return sample(); }
      static constexpr unsigned min( ) { return 0; }
      static constexpr unsigned max( ) { return ~unsigned(0); }
      typedef unsigned result_type;
      
      inline unsigned hash( unsigned x )
      {
	x ^= x >> 16;
	x *= 0x21f0aaad;
	x ^= x >> 15;
	x *= 0xd35a2d97;
	x ^= x >> 15;
	return x;
      }
      // cf "hash prospector" https://github.com/skeeto/hash-prospector/blob/master/README.md
    };


    // nested uniform scrambling / owen srambling
    inline integer3 scramble_base3( const integer3& a3, const unsigned seed, const unsigned ndigits )
    {
      // all random permutations of base-3 digits
      static constexpr int8_t scramble[6][3]=
      {
	{0, 1, 2},
	{0, 2, 1},
	{1, 0, 2},
	{1, 2, 0},
	{2, 0, 1},
	{2, 1, 0},
      };
      
      // counter-based random number generator
      FCRNG rng(seed);
      
      integer3 b3;
      unsigned node_index= 0;                                   // start at the root node
      for(unsigned i= 0; i < ndigits; i++)
      {
	unsigned flip= rng.index(node_index).sample_range(6);   // get a random permutation using the node index
	unsigned digit= a3.digits[ndigits-1 - i];               // get a digit
	b3.digits[ndigits-1 - i]= scramble[flip][digit];        // store the permuted digit
	
	node_index= 3*node_index +1 + digit;                    // continue walking the permutation tree
	// heap layout, root i= 0, childs 3i+1, 3i.s+2, 3i+3
      }
      
      return b3;
    }
}

namespace utk
{
    class SamplerQuadOptimizedLDS 
    {
    public:
	SamplerQuadOptimizedLDS(
            uint32_t d,
            const std::string& fp = "") : D(d) 
        { 
            setDirectionFile(fp);
            setRandomSeed(); 
        }

	static std::string GetDefaultTablePath()
        {
            return std::string(UTK_DATA_PATH) + "/Sobol/quad_optimized_lds_init_tab.dat";
        }

	void setDirectionFile(const std::string& file) 
        { 
            if (file.empty())
                directionFile = GetDefaultTablePath();
            else
                directionFile = file;
        }

        void setRandomSeed(uint64_t arg_seed) { seed = arg_seed; }
        void setRandomSeed() { seed = std::random_device()();}

	template <typename T>
        bool generateSamples(Pointset<T> &arg_pts, uint64_t N)
	{
	    const uint32_t m = std::round(std::log(N) / std::log(3));
	    if (std::pow(3, m) != N)
	    {
		UTK_ERROR("SamplerQuadOptimizedLDS: N({}) should be a power of 3", N);
		return false;
	    }

	    if (!QuadOptimizedLDS::load_mk(directionFile, false, sobol_aj, sobol_sj, sobol_dj, sobol_mk)) 
	    {
	        UTK_ERROR("SamplerQuadOptimizedLDS: error loading {}", directionFile);
                return false;
	    }

	    std::vector<QuadOptimizedLDS::sobol3_matrix> matrices;
	    for (size_t d = 1; d <= D; ++d) 
	    {
		QuadOptimizedLDS::generate_mkGF3(sobol_aj[d], sobol_sj[d], sobol_mk[d], 3);
		
		QuadOptimizedLDS::sobol3_matrix mat(MATRIX_SZ, std::vector<int>(MATRIX_SZ));
		QuadOptimizedLDS::fill_mx(d, mat, sobol_mk);
		matrices.push_back(std::move(mat));
	    }

            arg_pts.Resize(N, D);
	    std::default_random_engine gen(seed);
	    std::vector<unsigned int> seeds;
	    for (size_t d = 0; d < D; ++d)
		seeds.push_back(gen());
	    seed = gen();

	    std::vector<QuadOptimizedLDS::integer3> x3(D);
	    std::vector<QuadOptimizedLDS::integer3> p3(D);
	    for (size_t d = 0; d < D; ++d) 
	    {
		arg_pts[0][d] = QuadOptimizedLDS::scramble_base3(x3[d], seeds[d], m).value_double(m);
	    }

	    for (size_t i = 1; i < N; ++i) 
	    {
		QuadOptimizedLDS::integer3 i3 = i;
		for (size_t d = 0; d < D; ++d) 
		{
		    QuadOptimizedLDS::integer3 x = QuadOptimizedLDS::point3_digits(matrices[d], m, i3, p3[d], x3[d]);
		    arg_pts[i][d] = QuadOptimizedLDS::scramble_base3(x, seeds[d], m).value_double(m);
		}
	    }

	    return true;
	}


    private:
	const uint32_t D;
	std::string directionFile;
	
	uint64_t seed;

	QuadOptimizedLDS::int32 sobol_aj[N_SOBOL_INIT_TAB_ENTRIES];
	QuadOptimizedLDS::int32 sobol_sj[N_SOBOL_INIT_TAB_ENTRIES];
	QuadOptimizedLDS::int32 sobol_dj[N_SOBOL_INIT_TAB_ENTRIES];
	QuadOptimizedLDS::int32 sobol_mk[N_SOBOL_INIT_TAB_ENTRIES][32];
    };
}
