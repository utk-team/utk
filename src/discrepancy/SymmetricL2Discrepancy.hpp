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
#ifndef _UTK_SYMMETRIC_L2_DISCREPANCY_
#define _UTK_SYMMETRIC_L2_DISCREPANCY_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"

namespace utk
{
/*
 * Reference
 * http://www.ams.org/journals/mcom/1998-67-221/S0025-5718-98-00894-1/S0025-5718-98-00894-1.pdf
 * section 5.3
 * Hickernell, F. (1998). A generalized discrepancy and quadrature error bound. Mathematics of Computation of the American Mathematical Society, 67(221), 299-322.
*/
	
class SymmetricL2Discrepancy
{
public:
	
	template<unsigned int D, typename T, typename P>
	bool compute(Pointset<D, T, P>& arg_pts, double& symmetricL2discrepancy)
	{
		if(isIntegerType<T>())
		{
			ERROR("CenteredL2Discrepancy::compute Point coordinates can't be integers, Pointset domain must be the unit hypercube");
			return false;
		}
		for(uint d=0; d<D; d++)
			if(arg_pts.domain.pMin.pos()[d] != 0 || arg_pts.domain.pMax.pos()[d] != 1)
			{
				ERROR("CenteredL2Discrepancy::compute Pointset domain must be the unit hypercube");
				return false;
			}
			
		uint N = arg_pts.size();
			
		if(N==0)
		{
			ERROR("CenteredL2Discrepancy::compute Pointset is empty");
			return false;
		}
			
		double a = pow((4.0/3.0), D);
		double factor_b = 2.0/(double)N;
		double factor_c = pow(2,D)/(double)(N*N);
		
		double sumb = 0;
		for(uint i=0; i<N; i++)
		{
			double prodb = 1;
			for(uint j=0; j<D; j++)
			{
				double uij = arg_pts[i].pos()[j];
				prodb *= (1 + 2*uij - 2*uij*uij);
			}
			sumb += prodb;
		}
		
		double sumc = 0;
		for(uint i=0; i<N; i++)
		for(uint iprime=0; iprime<N; iprime++)
		{
			double prodc = 1;
			for(uint j=0; j<D; j++)
			{
				double uij = arg_pts[i].pos()[j];
				double uiprimej = arg_pts[iprime].pos()[j];
				//double absuijuijprime = (uij - uiprimej) > 0 ? (uij - uiprimej) : -1*(uij - uiprimej);
				prodc *= (1 - fabs(uij - uiprimej));
			}
			sumc += prodc;
		}
		
		symmetricL2discrepancy = sqrt(a - factor_b*sumb + factor_c*sumc);
		
		return true;
	}
	
protected:

};

}//end namespace utk

#endif
