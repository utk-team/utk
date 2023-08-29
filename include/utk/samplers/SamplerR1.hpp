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

#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>
#include <cstring>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
// #include <getopt.h>
#include <string>
#include <fstream>
#include <vector>
#include <limits.h>
#include <iomanip>


namespace nsrank1
{
	
struct Point
{
	double x;
	double y;
	Point() { x = 0; y = 0; }
	Point(double pt[2]) { x = pt[0]; y = pt[1]; }
	Point(double p, double q) { x = p; y = q; }
};

struct Pair{
	long long x;
	long long y;
	Pair() { x = 0; y = 0; }
	Pair(long long pt[2]) { x = pt[0]; y = pt[1]; }
	Pair(long long a, long long b) { x = a; y = b; }

	inline long long length2() const { return x*x + y*y; }
	inline double length() const { return sqrt(x*x + y*y); }
};

inline bool isInRect(const Point &p, const Point &BL, const Point &TR) {        // Check if a point is inside a given rectangle
	return BL.x <= p.x && BL.y <= p.y &&
		p.x < TR.x && p.y < TR.y;
}

inline Point wrap_unit_torus(const Point &p) {
	double x = p.x < 0 ? p.x + 1.0 : (p.x >= 1.0 ? p.x - 1.0 : p.x);
	double y = p.y < 0 ? p.y + 1.0 : (p.y >= 1.0 ? p.y - 1.0 : p.y);
	return Point(x, y);
}

//Generate Fibonacci number, given the index
inline int fibonacci(int n, int& before)
{
	int a = 0;
	int b = 1;
	while (n-- > 1) {
		int t = a;
		a = b;
		b += t;
	}
	before = a;
	return b;
}

// Euclidean Algorithm for computing greatest common divisor (gcd)
inline int compute_GCD(int a, int b)
{
	if (b == 0)
		return a;
	return compute_GCD(b, a % b);
}

// Extended Euclidean Algorithm,
// OUTPUT: d = gcd(a, b) and integers x and y satifying ax + by = d.
inline int extended_eucliden(int a, int b, int &x, int &y){
	if (b == 0)
	{
		x = 1;
		y = 0;
		return a;
	}
	int d = extended_eucliden(b, a % b, x, y);
	int t = x;
	x = y;
	y = t - a / b * y;

	return d;
}

long long gaussian_basisReduction(int nb, int gx, int gy, Pair& b1, Pair& b2){
	//initialize basis
	if (gx == 1){
		b1.x = gx; b1.y = gy;
		b2.x = 0; b2.y = nb;
	}
	else if (gy == 1){
		b1.x = gx; b1.y = gy;
		b2.x = nb; b2.y = 0;
	}
	else{
		int a1, a2, b3, gamma;
		int d = extended_eucliden(nb, gx, a2, a1);
		/*int d1 = */extended_eucliden(d, gy, b3, gamma);

		long long b11 = (nb / d) * gamma;
		long long b22 = (-gx / d) * gamma;

		b1.x = a1 * gx + a2 * nb;
		b1.y = a1 * gy;
		b2.x = b11 * gx + b22 * nb;
		b2.y = b11 * gy + b3 * nb;
	}
	// perform reduction
	long long lb1 = b1.x * b1.x + b1.y * b1.y;
	long long lb2 = b2.x * b2.x + b2.y * b2.y;
	if ((double(lb1)*1.0) > LLONG_MAX || (double(lb2)*1.0) > LLONG_MAX){
		return 0;
	}
	if ((double(lb1)*1.0) < LLONG_MIN || (double(lb2)*1.0) < LLONG_MIN){
		return 0;
	}

	while (1){
		if (lb1 > lb2){
			Pair tmp = b1;
			b1 = b2;
			b2 = tmp;
		}
		double t = double(b1.x*b2.x + b1.y*b2.y) / double(b1.x*b1.x + b1.y*b1.y);
		long mu = long(floor(t + 0.5));
		b2.x -= (mu*b1.x);
		b2.y -= (mu*b1.y);

		lb1 = b1.x * b1.x + b1.y * b1.y;
		lb2 = b2.x * b2.x + b2.y * b2.y;
		if ((double(lb1)*1.0) > LLONG_MAX || (double(lb2)*1.0) > LLONG_MAX){
			return 0;
		}
		if ((double(lb1)*1.0) < LLONG_MIN || (double(lb2)*1.0) < LLONG_MIN){
			return 0;
		}

		if (lb1 <= lb2){
			break;
		}
	}
	return b1.length2();
}

std::vector<Pair> search_MMD_R1Lattice_exhaustive(int nb, long long& mmd){
	long long max_min_dist = 0;
	long long nb2 = long(nb) * 10000;

	std::vector<Pair> generators;
	for (int gx = 1; gx < nb; gx++){
		for (int gy = 1; gy < (nb / 2 + 1); gy++){
			if (compute_GCD(compute_GCD(gx, gy), nb) != 1){
				continue;
			}
			Pair b1(0, 0), b2(0, 0);
			long long min_dist = gaussian_basisReduction(nb, gx, gy, b1, b2);

			if (min_dist > nb2) continue;

			if (min_dist == max_min_dist){
				generators.push_back(Pair(gx, gy));
			}
			else if (min_dist > max_min_dist){
				max_min_dist = min_dist;
				generators.clear();
				generators.push_back(Pair(gx, gy));
			}
		}
	}
	mmd = max_min_dist;
	return generators;
}

std::vector<Pair> rasterize_circle(double radius){
	//long size = long(ceilf(radius));
	double cd = 1.25 - radius;
	double cy = 0.0, cx = radius;

	long gx = long(floorf(cx));
	long gy = long(floorf(cy));
	std::vector<Pair> rasters;
	rasters.push_back(Pair(gx, gy));
	rasters.push_back(Pair(gx + 1, gy));
	if ((gx - 1) >= 0){
		rasters.push_back(Pair(gx - 1, gy));
	}
	rasters.push_back(Pair(gx + 2, gy));
	if ((gx - 2) >= 0){
		rasters.push_back(Pair(gx - 2, gy));
	}
	rasters.push_back(Pair(gx + 3, gy));
	if ((gx - 3) >= 0){
		rasters.push_back(Pair(gx - 3, gy));
	}

	while (cx > cy){
		if (cd < 0.0){
			cd += (2.0*cy + 3.0);
		}
		else{
			cd += (2.0*cy - 2.0*cx + 5.0);
			cx -= 1.0;
		}
		cy += 1.0;

		gx = long(floorf(cx));
		gy = long(floorf(cy));
		rasters.push_back(Pair(gx, gy));
		rasters.push_back(Pair(gx + 1, gy));
		if ((gx - 1) >= 0){
			rasters.push_back(Pair(gx - 1, gy));
		}
		rasters.push_back(Pair(gx + 2, gy));
		if ((gx - 2) >= 0){
			rasters.push_back(Pair(gx - 2, gy));
		}
		rasters.push_back(Pair(gx + 3, gy));
		if ((gx - 3) >= 0){
			rasters.push_back(Pair(gx - 3, gy));
		}
	}
	return rasters;
}

std::vector<Pair> search_MMD_R1Lattice_approxGR(int nb, long long& mmd){
	double ini = sqrt(sqrt(4.0 / 3.0)*(double)nb);
	long long max_min_dist = 0;
	long long nb2 = long(nb) * 10000;

	std::vector<Pair> generators;
	std::vector<Pair> rasters = rasterize_circle(ini);
	for (uint32_t i = 0; i < rasters.size(); i++){
		Pair e = rasters[i];
		long long a = e.x, b = e.y;
		if (compute_GCD(compute_GCD(nb, a), b) != 1){
			continue;
		}
		Pair b1(0, 0), b2(0, 0);
		long long min_dist = gaussian_basisReduction(nb, a, b, b1, b2);

		if (min_dist > nb2) continue;

		if (min_dist == max_min_dist){
			generators.push_back(Pair(a, b));
		}
		else if (min_dist > max_min_dist){
			max_min_dist = min_dist;
			generators.clear();
			generators.push_back(Pair(a, b));
		}
	}
	mmd = max_min_dist;
	return generators;
}

std::vector<Point> fibonacci_rank1_lattices(int nf){

	int nb_before;
	int nb = fibonacci(nf, nb_before);
	//std::cout << "fibonacci (index, num): ( " << nf << ", " << nb << " )" << std::endl;

	std::vector<Point> pts;
	pts.resize(nb);
	for (int i = 0; i<nb; i++) {
		double x = ((double)i / nb)*1.0;
		double y = ((double)i / nb)*nb_before;
		x -= (int)x;
		y -= (int)y;
		Point p(x, y);
		while (!isInRect(p, Point(0.0,0.0), Point(1.0,1.0))) {
			p = wrap_unit_torus(p);
		}
		pts[i] = p;
	}
	return pts;
}

std::vector<Point> general_rank1_lattices(int nb, bool exhaustSearch){

	long long mmd;
	std::vector<Pair> generators;
	if (!exhaustSearch){
		generators = search_MMD_R1Lattice_approxGR(nb, mmd);
	}
	else{
		generators = search_MMD_R1Lattice_exhaustive(nb, mmd);
	}

	Pair cur_generator = generators[0];
	//std::cout << generators.size() << " generators" << std::endl;
	/*for (int j = 0; j < generators.size(); j++){
	std::cout << generators[j].x << "  " << generators[j].y << std::endl;
	}*/

	std::vector<Point> pts;
	pts.resize(nb);
	for (int i = 0; i<nb; i++) {
		double x = ((double)i / nb)*cur_generator.x;
		double y = ((double)i / nb)*cur_generator.y;
		x -= (int)x;
		y -= (int)y;
		Point p(x,y);
		while (!isInRect(p, Point(0.0, 0.0), Point(1.0, 1.0))) {
			p = wrap_unit_torus(p);
		}
		pts[i] = p;
	}
	return pts;
}

} // end namespace nsrank1

namespace utk
{

class SamplerRank1
{
	protected:

	public:

		SamplerRank1(bool exhaustSearch = false, bool fibonnacci = false) :
            m_exhaustSearch(exhaustSearch), m_fibonnacci(fibonnacci)
        { }
		
		uint32_t GetDimension() const { return 2; }

		void setFibonnacci(bool arg_fibonnacci) { m_fibonnacci = arg_fibonnacci; }
		void setExhaustSearch(bool arg_exhauseSearch) { m_exhaustSearch = arg_exhauseSearch; }

		template<typename T>
		bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
		{		
			std::vector<nsrank1::Point> pts;
			
			if(!m_fibonnacci)
				pts = nsrank1::general_rank1_lattices(N, m_exhaustSearch);
			else
			{
				uint32_t fib_lvl0 = 0;
				uint32_t fib_lvl1 = 1;
				uint32_t fib_lvl2 = 1;
				uint32_t i = 2;
				while(fib_lvl2 < N)
				{
					fib_lvl0 = fib_lvl1;
					fib_lvl1 = fib_lvl2;
					fib_lvl2 = fib_lvl0 + fib_lvl1;
					i++;
				}
				if(std::abs((long)fib_lvl2 - (long)N) < std::abs((long)fib_lvl1 - (long)N))
					pts = nsrank1::fibonacci_rank1_lattices(i);
				else
					pts = nsrank1::fibonacci_rank1_lattices(i-1);
			}
			
			arg_pts.Resize(N, 2);
			// @TODO : memcpy (if structure packing is guarenteed)
			// std::memcpy()
			// if constexpr (std::is_same_v<T, double>)
			// {
			// 		std::memcpy(pts.data(), arg_pts.Data(), N * 2);
			// }
			// else
			// {
			for(uint32_t i = 0; i < pts.size(); i++)
			{
				arg_pts[i][0] = pts[i].x;
				arg_pts[i][1] = pts[i].y;
			}
			// }
			
			
			return true;
		};

	protected:
        bool m_exhaustSearch;
        bool m_fibonnacci;
};

} //end namespace utk
