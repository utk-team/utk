#ifndef _UTK_GENERALIZED_L2_DISCREPANCY_
#define _UTK_GENERALIZED_L2_DISCREPANCY_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"
#include <math.h>

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
		//std::cout << "sum =" << sumb << std::endl;
		
		long double sumc = 0.0;
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
		//std::cout << "sum2 =" << sumc << std::endl;
		
		/*std::cout << std::setprecision(11) << "a=" << a << std::endl;
		std::cout << "factor_b=" << factor_b << std::endl;
		std::cout << "sum_b=" << sumb << std::endl;
		std::cout << "factor_c=" << factor_c << std::endl;
		std::cout << "sum_c=" << sumc << std::endl;*/
		
		long double tmp0 = factor_b*sumb;
		long double tmp1 = factor_c*sumc;
		
		/*std::cout << "factor_b*sum_b=" << tmp0 << std::endl;
		std::cout << "factor_c*sum_c=" << tmp1 << std::endl;
		
		std::cout << "all=" << a -tmp0 + tmp1 << std::endl;*/
		
		//std::cout << "total=" << a -tmp0 + tmp1 << std::endl;
		generalizedL2discrepancy = sqrtl(a -tmp0 + tmp1);
	
		return true;
	}
	
protected:
	
	bool m_logValue;

};

}//end namespace utk

#endif
