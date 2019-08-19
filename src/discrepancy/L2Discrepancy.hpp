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
#ifndef _UTK_L2_DISCREPANCY_
#define _UTK_L2_DISCREPANCY_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"

namespace utk
{
//Formula from Lemieux p 183
	
class L2Discrepancy
{
public:
	
	template<unsigned int D, typename T, typename P>
	bool compute(Pointset<D, T, P>& arg_pts, double& l2discrepancy)
	{
		if(isIntegerType<T>())
		{
			ERROR("L2Discrepancy::compute Point coordinates can't be integers, Pointset domain must be the unit hypercube");
			return false;
		}
		for(uint d=0; d<D; d++)
			if(arg_pts.domain.pMin.pos()[d] != 0 || arg_pts.domain.pMax.pos()[d] != 1)
			{
				ERROR("L2Discrepancy::compute Pointset domain must be the unit hypercube");
				return false;
			}
			
		double n = arg_pts.size();
			
		if(n==0)
		{
			ERROR("L2Discrepancy::compute Pointset is empty");
			return false;
		}
		
		double discrepancy_squared = 0;
		double s = D; //dimension
		
		double first_half_formula = 0;
		
		double sum_over_i = 0;
		for(int i=0; i<n; i++)
		{
			double sum_over_j = 0;
			for(int j=0; j<n; j++)
			{
				double product_over_k=1;
				for(int k=0; k<s; k++)
				{
					T uik = arg_pts[i].pos()[k];
					T ujk = arg_pts[j].pos()[k];
					T max_uik_ujk = uik > ujk ? uik : ujk; //std::max(arg_pts[i].pos()[k], arg_pts[j].pos()[k]);
					product_over_k*=(1-max_uik_ujk);
				}
				sum_over_j+=product_over_k;
			}
			sum_over_i += sum_over_j;
		}
		
		first_half_formula = (1.0 / (n*n))*sum_over_i;
		
		double second_half_formula = 0;
		
		double sum_over_i2 = 0;
		for(int i2=0; i2<n; i2++)
		{
			double product_over_k2=1;
			for(int k2=0; k2<s; k2++)
			{
				double uik2 = arg_pts[i2].pos()[k2];//*arg_pts[i2].pos()[k2];
				uik2*=uik2;
				product_over_k2*=(1-uik2);
			}
			sum_over_i2 += product_over_k2;
		}
		
		double a = (1.0 / pow(2, s-1));
		second_half_formula = (a/n)*sum_over_i2;
		
		double b = (1.0 / pow(3, s));
		discrepancy_squared = first_half_formula - second_half_formula + b;
		l2discrepancy = sqrt( fabs(discrepancy_squared) );
		
		return true;
	}
	
protected:

};

}//end namespace utk

#endif
