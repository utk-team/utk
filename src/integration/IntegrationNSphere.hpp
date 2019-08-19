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
 * either expressed or implied, of the UTK project.
 */
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
