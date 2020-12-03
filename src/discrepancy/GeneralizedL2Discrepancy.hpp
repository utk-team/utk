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
#ifndef _UTK_GENERALIZED_L2_DISCREPANCY_
#define _UTK_GENERALIZED_L2_DISCREPANCY_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"
#include <math.h>
#include <omp.h>

namespace utk
{

//Formula from Lemieux
//p185
	
class GeneralizedL2Discrepancy
{
public:
	void setLogValue(bool logValue) { m_logValue = logValue; }
	
	template<unsigned int D, typename T, typename P>
	bool compute(Pointset<D, T, P>& arg_pts, double& generalizedL2discrepancy)
	{
		if(isIntegerType<T>())
		{
			ERROR("GeneralizedL2Discrepancy::compute Point coordinates can't be integers, Pointset domain must be the unit hypercube");
			return false;
		}
		for(uint d=0; d<D; d++)
			if(arg_pts.domain.pMin.pos()[d] != 0 || arg_pts.domain.pMax.pos()[d] != 1)
			{
				ERROR("GeneralizedL2Discrepancy::compute Pointset domain must be the unit hypercube");
				return false;
			}
			
		uint N = arg_pts.size();
			
		if(N==0)
		{
			ERROR("GeneralizedL2Discrepancy::compute Pointset is empty");
			return false;
		}
			
		long double a, factor_b, factor_c;

		a = pow((4.0/3.0), D);
		factor_b = 2.0/(double)N;
			
		factor_c = 1.0/(double)(N);
		factor_c *= factor_c;
		
		long double sumb = 0.0;
#pragma omp parallel for reduction(+:sumb)
		for(unsigned int i=0; i<N; i++)
		{
			long double prodb = 1.0;
			for(unsigned int j=0; j<D; j++)
			{
				double uij = arg_pts[i].pos()[j];
				prodb *= ((3.0 - uij*uij)/2.0);
			}
			sumb += prodb;
		}
		
		long double sumc = 0.0;
#pragma omp parallel for reduction(+:sumc)
		for(uint i=0; i<N; i++)
		for(unsigned int iprime=0; iprime<N; iprime++)
		{
			long double prodc = 1.0;
			for(unsigned int j=0; j<D; j++)
			{
				double uij = arg_pts[i].pos()[j];
				double uiprimej = arg_pts[iprime].pos()[j];
				double m = uij > uiprimej ? uij : uiprimej;//std::max(uij, uiprimej);
				prodc *= (2.0 - m);
			}
			sumc += prodc;
		}
		
		long double tmp0 = factor_b*sumb;
		long double tmp1 = factor_c*sumc;
		
	
		generalizedL2discrepancy = sqrtl(a -tmp0 + tmp1);
	
		return true;
	}
	
protected:
	
	bool m_logValue;

};

}//end namespace utk

#endif
