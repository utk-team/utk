/*
 * Mayur Patel  mayur.patel2@rci.rogers.com
 *
 * Copyright (c) 2021 Rogers Communications
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
 * either expressed or implied, of the Halton project.
 */
#ifndef _UTK_DIAPHONY_
#define _UTK_DIAPHONY_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"

namespace utk
{
//Formula from Zinterhof 1976

class Diaphony
{
public:

	template<unsigned int D, typename T, typename P>
	bool compute(Pointset<D, T, P>& arg_pts, double& diaphony)
	{
		if(isIntegerType<T>())
		{
			ERROR("Diaphony::compute Point coordinates can't be integers, Pointset domain must be the unit hypercube");
			return false;
		}
		for(uint d=0; d<D; d++)
			if(arg_pts.domain.pMin.pos()[d] != 0 || arg_pts.domain.pMax.pos()[d] != 1)
			{
				ERROR("Diaphony::compute Pointset domain must be the unit hypercube");
				return false;
			}

		const unsigned n = arg_pts.size();

		if(n==0)
		{
			ERROR("Diaphony::compute Pointset is empty");
			return false;
		}

		double sum_over_i = 0;
		for(unsigned i=0; i<n; i++)
		{
			double sum_over_j = 0;
			for(unsigned j=0; j<n; j++)
			{
				double product_over_k = 1;
				for(unsigned k=0; k<D; k++)
				{
					const T uik = arg_pts[i].pos()[k];
					const T ujk = arg_pts[j].pos()[k];

					const double delta = uik - ujk;
					const double mod1_delta = delta - floor(delta);

					const double B2 = mod1_delta * mod1_delta - mod1_delta + (1.0/6.0); // second Bernoulli polynomial
					product_over_k *= 1.0 + 2.0 * M_PI * M_PI * B2;
				}
				sum_over_j += product_over_k - 1.0;
			}
			sum_over_i += sum_over_j;
		}

		const double bot = double(n*n) * ( pow ( 1.0 + M_PI * M_PI / 3.0, D ) - 1.0 );
		diaphony = sqrt(sum_over_i / bot);

		return true;
	}

protected:

};

}//end namespace utk

#endif
