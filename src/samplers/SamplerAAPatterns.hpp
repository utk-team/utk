/* Generate a distribution based on an AA pattern and a loadable displacement map
 * Values which depend on pattern parameters are read from a header file so
 *  that different parameters can be used with this code.
 * We could also load these dynamically, but hard-coding them gives substantial speedup
 * Created by Abdalla Ahmed
 * 2014-11-11
 * Last revision 2015-09-10
 */

#ifndef _UTK_SAMPLER_AAPATTERNS_
#define _UTK_SAMPLER_AAPATTERNS_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include <algorithm>
#include "SamplerAAPatterns/t191861r110771.h"

namespace nsaapattern
{
	
const unsigned long aa_q = aa_t + aa_r;                                         // m is assumed 1.
const unsigned long aa_ONE = 2 * aa_q;                                          // We call 2q "ONE" coz it's the denominator of indexes in a unit torus
const unsigned long aa_p = aa_q + aa_t;                                         // Please refer to JGT paper by Ahmed for the meaning of all these parameters.
const unsigned long aa_2t = 2 * aa_t;                                           // We fix these to avoid repeated calculations in the inner loop
const unsigned long aa_ONE_minus_2t = aa_ONE - aa_2t;                           // ~
const unsigned long aa_minus_2t = aa_2t - aa_ONE;                               // ~

struct TPoint {
    double x, y;
};

TPoint *map;
double maxDisplacement;                                                         // A supremum for the shift in x or y directions.

int aa(TPoint *list, int nxn) {                                                 // This function generates n x n points in the unit domain
    double n = sqrt(nxn);                                                       // Average number of points in a single row
    double w = (n / aa_t) * aa_ONE;                                             // The width to scan: aa_ONE produces aa_t points per row
    double scanRange = w + 2 * maxDisplacement + 2;                                  // Add a margin for displacement of points; and a margin for th
    double scale = 1.0 / w;                                                     // Multiplication is faster than division
    int dX0 = rand() % aa_t;                                                    // Randomly choose a slicing position in the pattern. Only affects spatial domain,
    int dY0 = rand() % aa_t;                                                    // but it doesn't make much difference in statistics or spectrum
    int dX, dY;                                                                 // Indexes
    unsigned long count = 0;                                                    // Counter for generated points
    for (int y = 0; y < scanRange; y++) {                                       // Loop through rows
        dY = (aa_p*y + dY0) % aa_ONE;                                           // dY alternates between only two values in a row (for even/odd values of x)
        if ((dY % aa_q) >= aa_t) continue;                                      // If both values do not fulfill the pattern condition skip this row
        int xStart = 0;                                                         // Assume dY would work for even values of x
        dX = (aa_q*y + dX0) % aa_ONE;                                           // Find dX at beginning of row. NB this can also be rewritten to use addition only
        if ((unsigned long)dY >= aa_q) {                                        // If dY actually satisfy the condition for odd values of x
            xStart++;                                                           // Advance one step to odd x's; start at x = 1
            dY -= aa_q;                                                         // Adjust dY accordingly; this remains fixed for all odd values of x
            dX = (dX + aa_p) % aa_ONE;                                          // And adjust dX.
        }
        TPoint *row = &map[bsearch(dY) * MAP_WIDTH];                            // Since dY is fixed we can point at the beginning of concerned row in map
        for (int x = xStart; x < scanRange; x += 2) {                           // Step only through even or odd values of x (for which dY fulfills condition)
            if ((unsigned long)dX < aa_t) {                                                    // Check pattern condition on dX; dY is already checked
                TPoint& shift = row[ bsearch(dX) ];                             // Locate the appropriate shift
                double xx = scale * (x + shift.x - maxDisplacement);            // Apply shift and scale to unit domain
                double yy = scale * (y + shift.y - maxDisplacement);
                if (xx >= 0 && xx < 1 && yy >= 0 && yy < 1)
                    list[count++] = {xx, yy};
            }
            if ((unsigned long)dX < aa_ONE_minus_2t) dX += aa_2t; else dX += aa_minus_2t;      // "dX = (dX + 2*aa_p) % aa_ONE", Avoiding division gives substantial speedup.
        }
    }
    return count;
}

void loadVectors(const char *fileName) {                                        // This function loads the vectors lookup table
    FILE *vectorsFile = fopen(fileName, "r");
    if (!vectorsFile) {
        fprintf(stderr, "Failed to open %s\n", fileName);
        exit(1);
    }
    int ng = MAP_WIDTH * MAP_WIDTH;                                             // MAP_WIDTH is supplied in the header file, ng is the number of groups
    map = new TPoint[ng];
    maxDisplacement = 0;
    for (int i = 0; i < ng; i++) {
        fscanf(vectorsFile, " %lf %lf", &map[i].x, &map[i].y);
        if (feof(vectorsFile)) {
            fprintf(stderr, "Failed to read all vectors from %s\n", fileName);
            exit(1);
        }
        maxDisplacement = std::max(maxDisplacement, (fabs(map[i].x)));
        maxDisplacement = std::max(maxDisplacement, (fabs(map[i].y)));
    }
    maxDisplacement = ceil(maxDisplacement);
}

} //end namespace nsaapattern

namespace utk
{

class SamplerAAPatterns
{
protected:

public:

	SamplerAAPatterns() { setRandomSeedTime(); }

	void setVectorFile( std::string arg_vectorfile) { m_vectorfile = arg_vectorfile; }
	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerAAPatterns::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerAAPatterns::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;
		
		nsaapattern::loadVectors(m_vectorfile.c_str());
		int n = arg_points;
		nsaapattern::TPoint *list = new nsaapattern::TPoint[(int)(1.1*n)];                                    // Add a 10% safety margin in buffer size. This is admittedly subjective
		int actual = aa(list, n);

		arg_pts.resize(actual);
		for(int i=0; i<actual; i++)
		{
			arg_pts[i].pos()[0] = list[i].x;
			arg_pts[i].pos()[1] = list[i].y;
		}
		
		return true;
	};

protected:
    std::mt19937 m_mersenneTwister;
    double getRandom01()
    {
        return (double)m_mersenneTwister()/(double)m_mersenneTwister.max();
    }
    
    std::string m_vectorfile;
};

} //end namespace utk


#endif
