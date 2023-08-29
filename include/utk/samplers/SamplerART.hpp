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
#include <random>
#include <fstream>

#include "ART/self-similar.h"

namespace art
{

inline Point getSample(unsigned tileID, unsigned sampleNo, Matrix m = IDENTITY) {
    while (sampleNo > 0) {
        unsigned childNo = tiles[tileID].order[ sampleNo & mask ];
        tileID = tiles[tileID].children[childNo];
        m = m.concat( M[childNo] );
        sampleNo = (sampleNo >> shift);
    }
    return transform(tiles[tileID].p, m);
}

void generate(Points &samples) {
    uint32_t n = static_cast<uint32_t>(samples.size());
    int tileID = rand() % tileCount;
    for (decltype(n) i = 0; i < n; i++) {
        samples[i] = getSample(tileID, i);
    }
}

}

namespace utk
{

class SamplerART
{
protected:
public:
	SamplerART() : 
		tileFile(std::string(UTK_DATA_PATH) + "/ART/table.dat")
	{	}

    uint32_t GetDimension() const { return 2; }

	void setTileFile(const std::string& file) { tileFile = file; }

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
        std::fstream file(tileFile);
        if (!file.is_open())
			return false;

		art::loadTileSet(file);
		file.close();

		art::Points s(N);
		art::generate(s);
		
		arg_pts.Resize(N, 2);
		for (uint32_t i = 0; i < N; i++) 
		{
			arg_pts[i][0] = s[i].x;
			arg_pts[i][1] = s[i].y;
		}

		return true;
	};

protected:
	std::string tileFile;
};

}
