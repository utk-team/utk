/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * and Bastien Doignies bastien.doignies@liris.cnrs.fr 
 * and David Coeurjolly David.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2023 CNRS Université de Lyon
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
#include <array>
#include <random>
#include <cmath>

namespace utk
{

class SamplerLDBN
{
protected:
public:

	SamplerLDBN() { setRandomSeed(); setTileSize(128); }

    uint32_t GetDimension() const { return 2; }

	void setPermutFile(const std::string& file) { permutFile = file; }
	void setRandomSeed( uint64_t arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeed() { m_mersenneTwister.seed(std::random_device{}()); }

	void setTileSize(uint32_t tileSize)
	{
		tablesize = tileSize;
		tablebit  = std::log2(tablesize);
		maxval    = 1u << (tablebit+10);
		one = tablesize * tablesize;
		scl = 1.0 / (double)maxval;
	}

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		const uint32_t n  = (uint32_t) std::round(std::sqrt(N));
		if (n * n != N)
			return false;
		
		if (!setPermut())
			return false;
		
		N = n * n;
		s.resize(N);
		generate(n);

		arg_pts.Resize(N, 2);
		uint32_t pmax = std::ceil(std::log2(N));
		uint32_t smax = std::pow(2, pmax);

		if (std::is_integral_v<T>)
		{
			for (uint32_t i = 0; i < N; i++)
			{
				arg_pts[i][0] = static_cast<T>(s[i][0] * smax);
				arg_pts[i][1] = static_cast<T>(s[i][1] * smax);
			}
		}
		else
		{
			for (uint32_t i = 0; i < N; i++)
			{
				arg_pts[i][0] = static_cast<T>(s[i][0]);
				arg_pts[i][1] = static_cast<T>(s[i][1]);
			}	
		}

		return true;
	};

protected:
	bool setPermut()
	{
		if (permutFile.empty())
			return setRandomPermut();
		return setFilePermut();
	}

	bool setFilePermut()
	{
		std::ifstream permut_table(permutFile);
		if (!permut_table.is_open() || !permut_table.good())
			return false;
		
		O.resize(one);
		for (uint32_t i = 0; i < one; i++)
		{
			permut_table >> O[i][0] >> O[i][1];
		}

		tabulate();
		permut_table.close();
		return true;
	}

	bool setRandomPermut()
	{
		std::uniform_int_distribution<uint32_t> dist(0, 16 - 1);

		O.resize(one);

		//Permut Y
		for (uint32_t x = 0; x < 128; x++)
		for (uint32_t y = 0; y < 128; y+=16)
		{
			int tab[16];
			for(uint32_t n=0; n<16; n++) tab[n] = n;
			for(uint32_t n=0; n<16; n++) std::swap(tab[dist(m_mersenneTwister)], tab[dist(m_mersenneTwister)]);

			for(uint32_t n=0; n<16; n++)
			{
				uint32_t off = tab[n];
				
				uint32_t yn = y+n;
				uint32_t i; i_from_XY(i, x, yn);
				O[i][0] = radicalInverse_VdC(y+off);
			}
		}
		
		//Permut X
		for (uint32_t y = 0; y < 128; y++)
		for (uint32_t x = 0; x < 128; x += 16)
		{
			int tab[16];
			for(uint32_t n=0; n<16; n++) tab[n] = n;
			for(uint32_t n=0; n<16; n++) std::swap(tab[dist(m_mersenneTwister)], tab[dist(m_mersenneTwister)]);

			for(uint32_t n=0; n<16; n++)
			{
				uint32_t off = tab[n];
				
				uint32_t xn = x+n;
				uint32_t i; i_from_XY(i, xn, y);
				O[i][1] = radicalInverse_VdC(x+off);
			}
		}
		
		tabulate();
		return true;
	}

	void tabulate() 
	{
		for (unsigned i = 0; i < 512; i++) {
			mirror[i] = (i >> 9) + ((i >> 7) & 2) + ((i >> 5) & 4) + ((i >> 3) & 8)
			+ ((i >> 1) & 16) + ((i << 1) & 32) + ((i << 3) & 64)
			+ ((i << 5) & 128) + ((i << 7) & 256) + ((i << 9) & 512);
		}
		/*
		for (unsigned i = 0; i < 256; i++) {
			mirror[i] = (i >> 7) + ((i >> 5) & 2) + ((i >> 3) & 4) + ((i >> 1) & 8)
			+ ((i << 1) & 16) + ((i << 3) & 32) + ((i << 5) & 64) + ((i << 7) & 128);
		}
		*/
	}

	void generate(uint32_t n) 
	{
		double inv = 1.0 / n;
		int i = 0;
		/*for (unsigned Y = 0; Y < n; Y++) {
			double x = scl * mirror[Y >> 7];
			unsigned id0 = ((Y & 127) << 7);
			for (unsigned X = 0; X < n; X++) {
				double y = scl * mirror[X >> 7];
				unsigned id = id0 + (X & 127);
				s[i].x = inv * (X + O[id].x + x);
				s[i++].y = inv * (Y + O[id].y + y);
			}
		}*/
		for (uint32_t Y = 0; Y < n; Y++) {
			double x = scl * mirror[Y >> tablebit];
			unsigned id0 = ((Y & (tablesize-1)) << tablebit);
			//std::cout << "id0 " << id0 << std::endl;
			for (uint32_t X = 0; X < n; X++) {
				double y = scl * mirror[X >> tablebit];
				unsigned id = id0 + (X & (tablesize-1));
				//std::cout << "id " << id << std::endl;
				s[i][0] = inv * (X + O[id][0] + x);
				s[i++][1] = inv * (Y + O[id][1] + y);
			}
		}
	}
protected:
	std::vector<std::array<double, 2>> O, s;

	std::string permutFile;
	uint32_t tablesize;
	uint32_t tablebit;
	uint32_t maxval;
	uint32_t one;
	double scl;

	unsigned mirror[1024];  // 10 bits


    std::mt19937 m_mersenneTwister;

	static float radicalInverse_VdC(uint32_t bits) {
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) * 2.3283064365386963e-10; // / 0x100000000
	}

	static void XY_from_i(uint32_t i, uint32_t& x, uint32_t& y)
	{
		x = i%128;
		y = i/128;
	}

	static void i_from_XY(uint32_t&  i, uint32_t x, uint32_t y)
	{
		i = y*128 + x;
	}

};

}
