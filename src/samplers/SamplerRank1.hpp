#ifndef _UTK_SAMPLER_RANK1_
#define _UTK_SAMPLER_RANK1_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <getopt.h>
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
		int d1 = extended_eucliden(d, gy, b3, gamma);

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
		int mu = long(floor(t + 0.5));
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
	for (uint i = 0; i < rasters.size(); i++){
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

		SamplerRank1() { exhaustSearch=false; }
		
		template<unsigned int D, typename T, typename P>
		bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
		{
			if (!arg_pts.empty())
			{
				WARNING("SamplerRank1::generateSamples the pointset to fill is not empty, clearing it ...");
				arg_pts.clear();
			}
			
			if (D != 2)
			{
				WARNING("SamplerRank1::generateSamples is only a 2D sampler ...");
				return false;
			}

			if (!isFloatingType<T>())
			{
				ERROR("SamplerRank1::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
				return false;
			}
			
			for(int d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			arg_pts.toricity = 1;
			
			std::vector<nsrank1::Point> pts;
			pts = nsrank1::general_rank1_lattices(arg_points, exhaustSearch);
			
			arg_pts.resize(pts.size());
			for(int i=0; i<pts.size(); i++)
			{
				arg_pts[i].pos()[0] = pts[i].x;
				arg_pts[i].pos()[1] = pts[i].y;
			}
			
			return true;
		};

	protected:
	bool exhaustSearch;
};

} //end namespace utk


#endif

