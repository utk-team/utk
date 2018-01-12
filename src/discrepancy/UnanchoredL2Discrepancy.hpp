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
