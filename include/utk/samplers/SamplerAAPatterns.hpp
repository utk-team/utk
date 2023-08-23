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


#include <utk/utils/FastPRNG.hpp>
#include <AAPatterns/t191861r110771.h>
#include <utk/utils/Pointset.hpp>
#include <random>
#include <tuple> // std::ignore
#include <cmath>
#include <map>


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

TPoint *map = nullptr /* modified*/;
double maxDisplacement;                                                         // A supremum for the shift in x or y directions.

unsigned long aa(TPoint *list, int nxn, 										 // This function generates n x n points in the unit domain
	/* added */ utk::PCG32& gen
) {                 
	/* added */ std::uniform_int_distribution<int> dist(0, aa_t - 1);
    double n = sqrt(nxn);                                                       // Average number of points in a single row
    double w = (n / aa_t) * aa_ONE;                                             // The width to scan: aa_ONE produces aa_t points per row
    double scanRange = w + 2 * maxDisplacement + 2;                                  // Add a margin for displacement of points; and a margin for th
    double scale = 1.0 / w;                                                     // Multiplication is faster than division
    /* Those two line are modified to use <random> api */
	int dX0 = dist(gen);                                                        // Randomly choose a slicing position in the pattern. Only affects spatial domain,
    int dY0 = dist(gen);                                                       // but it doesn't make much difference in statistics or spectrum
    int dX, dY;                                                                 // Indexes
    unsigned long count = 0;                                                    // Counter for generated points
    for (int y = 0; y < scanRange; y++) {                                       // Loop through rows
        dY = (aa_p*y + dY0) % aa_ONE;                                           // dY alternates between only two values in a row (for even/odd values of x)
        if ((dY % aa_q) >= aa_t) continue;                                      // If both values do not fulfill the pattern condition skip this row
        int xStart = 0;                                                         // Assume dY would work for even values of x
        dX = (aa_q*y + dX0) % aa_ONE;                                           // Find dX at beginning of row. NB this can also be rewritten to use addition only
        if ((unsigned long)dY >=   aa_q) {                                      // If dY actually satisfy the condition for odd values of x
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
    /* added */
	if (map != nullptr)
		delete[] map;

	map = new TPoint[ng];
    maxDisplacement = 0;
    for (int i = 0; i < ng; i++) {
        std::ignore = fscanf(vectorsFile, " %lf %lf", &map[i].x, &map[i].y);
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

	SamplerAAPatterns() { setRandomSeed(); m_vectorfile = GetDefaultFiles().begin()->second; }

    uint32_t GetDimension() const { return 2; }
	
	static std::map<std::string, std::string> GetDefaultFiles()
    {
		// TODO : use filesystem api to find them automatically ? 
		// (This would be very overkill tbh)
		static constexpr auto names = {
			"blue", "fpo-like", "green", "pink", "step"
		};
		std::map<std::string, std::string> files;

		for (const auto& name : names)
			files[name] = std::string(UTK_DATA_PATH) + "/aapatterns/" + std::string(name) + ".dat";
		return files;
    }

	void setRandomSeed( unsigned long long int arg_seed ) { gen.seed(arg_seed); }
	void setRandomSeed() { gen.seed(std::random_device{}()); }

	void setVectorFile(const std::string& file) { m_vectorfile = file; }

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		if (m_vectorfile.empty()) return false;
		
		nsaapattern::loadVectors(m_vectorfile.c_str());
		nsaapattern::TPoint* list = new nsaapattern::TPoint[(uint32_t)(1.1 * N)];   

		N = std::min(static_cast<uint32_t>(nsaapattern::aa(list, N, gen)), N);
		arg_pts.Resize(N, 2);
		for (uint32_t i = 0; i < N; i++)
		{
			arg_pts[i][0] = list[i].x;
			arg_pts[i][1] = list[i].y;
		}

        return true;
	};

protected:
	std::string m_vectorfile;
    utk::PCG32 gen;
};

}
