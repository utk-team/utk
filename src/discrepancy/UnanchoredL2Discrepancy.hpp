/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
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
#ifndef _UTK_UNANCHORED_L2_DISCREPANCY_
#define _UTK_UNANCHORED_L2_DISCREPANCY_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"

namespace utk
{
//Formula from Lemieux
//p183 - 5.21
	
class UnanchoredL2Discrepancy
{
public:
	
	template<unsigned int D, typename T, typename P>
	bool compute(Pointset<D, T, P>& arg_pts, double& unanchoredL2discrepancy)
	{
		if(isIntegerType<T>())
		{
			ERROR("UnanchoredL2Discrepancy::compute Point coordinates can't be integers, Pointset domain must be the unit hypercube");
			return false;
		}
		for(uint d=0; d<D; d++)
			if(arg_pts.domain.pMin.pos()[d] != 0 || arg_pts.domain.pMax.pos()[d] != 1)
			{
				ERROR("UnanchoredL2Discrepancy::compute Pointset domain must be the unit hypercube");
				return false;
			}
			
		uint N = arg_pts.size();
		if(N==0)
		{
			ERROR("UnanchoredL2Discrepancy::compute Pointset is empty");
			return false;
		}

		double factor_a = 1.0/(double)(N*N);
		
		double sum_a=0;
		for(unsigned int i=0; i<N; i++)
		for(unsigned int j=0; j<N; j++)
		{
			double prod_a=1;
			for(unsigned int k=0; k<D; k++)
			{
				double uik = arg_pts[i].pos()[k];
				double ujk = arg_pts[j].pos()[k];
				double maxuijujk = uik > ujk ? uik : ujk;//std::max(uik, ujk);
				double minuijujk = uik < ujk ? uik : ujk;//std::min(uik, ujk);
				prod_a *= ((1 - maxuijujk)*minuijujk);
			}
			
			sum_a += prod_a;
		}
		
		double factor_b = pow(2, -((int)D)+1)/(double)N;
		
		double sum_b=0;
		for(unsigned int i=0; i<N; i++)
		{
			double prod_b=1;
			for(unsigned int k=0; k<D; k++)
			{
				double uik = arg_pts[i].pos()[k];
				prod_b *= (uik*(1-uik));
			}
			
			sum_b += prod_b;
		}
		
		unanchoredL2discrepancy = sqrt(factor_a*sum_a - factor_b*sum_b + pow(12, -((int)D)));

		return true;
	}
	
protected:

};

}//end namespace utk

#endif
