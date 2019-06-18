#ifndef _UTK_INTEGRATION_DISK_ND_
#define _UTK_INTEGRATION_DISK_ND_

#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"
#include <cmath>
#include <array>

namespace utk
{
  
class IntegrationCorneredGaussian2D
{
public:
  IntegrationCorneredGaussian2D() { m_mean_x=0; m_stddev_x=0.20; m_mean_y=0; m_stddev_y=0.20; }
  
  template<unsigned int D, typename T, typename P>
  bool compute(const Pointset<D, T, P>& arg_pointset, double& integration_value, double& analytic_value)
  {
    std::array<double, D> nd_domain_extent;
    for(unsigned int d=0; d<D; d++)
    {
      nd_domain_extent[d] = (arg_pointset.domain.pMax.pos() - arg_pointset.domain.pMin.pos())[d];
    
      if(nd_domain_extent[d] == 0)
      {
			WARNING("IntegrationGaussian::compute domain extent is 0 on at least one dimension, scaling might fail");
      }
      if(nd_domain_extent[d] < 0)
      {
			ERROR("IntegrationGaussian::compute domain extent is negative on at least one dimension");
			return false;
	  }
    }
    
    analytic_value = analyticIntegrand();

    integration_value = 0;
    for(uint i=0; i<arg_pointset.size(); i++)
    {
      Point<D, double> pt;
      for(unsigned int d=0; d<D; d++)
		pt.pos()[d] = (arg_pointset[i].pos() - arg_pointset.domain.pMin.pos())[d] / nd_domain_extent[d];
      
	  integration_value += sampleIntegrand(pt);
    }
    integration_value /= (double)arg_pointset.size();
    
    return true;
  }

  double analyticIntegrand()
  {
	return 0.2499997133485104;
  }
  
  double sampleIntegrand(Point<2, double>& pt)
  {
	 double deltax = pt.pos()[0] - m_mean_x;
	 double deltay = pt.pos()[1] - m_mean_y;
	 double a = (deltax*deltax)/(2.0*m_stddev_x*m_stddev_x);
	 double b = (deltay*deltay)/(2.0*m_stddev_y*m_stddev_y);
	 return 1.0/(2.0*M_PI*m_stddev_x*m_stddev_y) * exp(-a-b);
  }
  
  double m_stddev_x;
  double m_mean_x;
  double m_stddev_y;
  double m_mean_y;
  
};

}//end namespace

#endif
