#ifndef _UTK_INTEGRATION_DISK_ND_
#define _UTK_INTEGRATION_DISK_ND_

#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"
#include <cmath>
#include <array>

namespace utk
{
  
class IntegrationNSphere
{
public:
  IntegrationNSphere() { m_radius = 0.25; }
  
  void setRadius(double arg_radius) { m_radius = arg_radius; }
  
  template<unsigned int D, typename T, typename P>
  bool compute(const Pointset<D, T, P>& arg_pointset, double& integration_value, double& analytic_value)
  {
    if(m_radius <= 0 && m_radius >= 0.5)
    {
      ERROR("IntegrationNSphere::compute radius must be in ]0, 0.5[");
      return false;
    }
    
    std::array<double, D> nd_domain_extent;
    for(unsigned int d=0; d<D; d++)
    {
      nd_domain_extent[d] = (arg_pointset.domain.pMax.pos() - arg_pointset.domain.pMin.pos())[d];
    
      if(nd_domain_extent[d] == 0)
      {
	WARNING("IntegrationNSphere::compute domain extent is 0 on at least one dimension, scaling might fail");
      }
      if(nd_domain_extent[d] < 0)
      {
	ERROR("IntegrationNSphere::compute domain extent is negative on at least one dimension");
	return false;
      }
    }
    
    analytic_value = analyticIntegrand(D);

    integration_value = 0;
    for(uint i=0; i<arg_pointset.size(); i++)
    {
      Point<D, double> pt;
      for(unsigned int d=0; d<D; d++)
		pt.pos()[d] = -0.5 + (arg_pointset[i].pos() - arg_pointset.domain.pMin.pos())[d] / nd_domain_extent[d];
      
	   integration_value += sampleIntegrand<D>(pt);
    }
    integration_value /= (double)arg_pointset.size();
    
    return true;
  }

  double analyticIntegrand(uint dim)
  {
    double num = pow(M_PI, (double)dim/2.0);
    double denom = tgamma(((double)dim/2.0) + 1);
    return (num/denom)*pow(m_radius, dim);
  }
  
  template<uint D>
  double sampleIntegrand(Point<D, double>& pt)
  {
	  double l = pt.pos().length();
      if(l < m_radius)
		return 1;
	  return 0;
  }
  
  double m_radius;
  
};

}//end namespace

#endif
