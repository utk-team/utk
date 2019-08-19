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
#ifndef _UTK_INTEGRATION_BOXARR_
#define _UTK_INTEGRATION_BOXARR_

#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"
#include "arrangements/BoxesAligned.hpp"
#include <cmath>
#include <array>

namespace utk
{
  
class IntegrationBoxArrangement2D
{
public:
  IntegrationBoxArrangement2D() { rasterize_arrangement=false; raster_file=""; 
	  std::cout << "Generate arrangement ..." << std::endl;
	  arrangement.generateTriangles(1234, (std::string(UTK_DATA_PATH)+"/luminance_hdr/hdr002.dat").c_str());
	  std::cout << "Compute Enveloppe ..." << std::endl;
	  arrangement.computeEnveloppe();
	  std::cout << "Compute analytical integration ..." << std::endl;
	  analytic_value_precompute = arrangement.exact_integration();
  }
  
  void setRasterizationTrue(std::string arg_rasterfile) { rasterize_arrangement=true; raster_file = arg_rasterfile; }
  void setRasterizationFalse() { rasterize_arrangement = false; }
  
  template<unsigned int D, typename T, typename P>
  bool compute(const Pointset<D, T, P>& arg_pointset, double& integration_value, double& analytic_value)
  {
    if(D != 2)
    {
      ERROR("IntegrationBoxArrangement2D::compute only in dimension 2");
      return false;
    }
    
    std::array<double, D> nd_domain_extent;
    for(unsigned int d=0; d<D; d++)
    {
      nd_domain_extent[d] = (arg_pointset.domain.pMax.pos() - arg_pointset.domain.pMin.pos())[d];
    
      if(nd_domain_extent[d] == 0)
      {
		WARNING("IntegrationBoxArrangement2D::compute domain extent is 0 on at least one dimension, scaling might fail");
      }
      if(nd_domain_extent[d] < 0)
      {
		ERROR("IntegrationBoxArrangement2D::compute domain extent is negative on at least one dimension");
		return false;
      }
    }
    
    analytic_value = analyticIntegrand();
	
	std::list<Point_2> pointset;
    for(uint i=0; i<arg_pointset.size(); i++)
	//for(uint i=0; i<1; i++)
    {
		double x = (arg_pointset[i].pos() - arg_pointset.domain.pMin.pos())[0] / nd_domain_extent[0];
		double y = (arg_pointset[i].pos() - arg_pointset.domain.pMin.pos())[1] / nd_domain_extent[1];
		pointset.push_back(Point_2(x*TRIANGLE_MAXSIZE, y*TRIANGLE_MAXSIZE)); 
	}
	
	integration_value = arrangement.integration(pointset);
	
	if(rasterize_arrangement)
	{
		std::cout << "Raster ...." << std::flush;
		arrangement.rasterizerPS(1024, raster_file.c_str(), true);
		std::cout << "Done" << std::endl;
	}

    /*integration_value = 0;
    for(uint i=0; i<arg_pointset.size(); i++)
    {
      Point<D, double> pt;
      for(unsigned int d=0; d<D; d++)
		pt.pos()[d] = (arg_pointset[i].pos() - arg_pointset.domain.pMin.pos())[d] / nd_domain_extent[d]; //-0.5 + (arg_pointset[i].pos() - arg_pointset.domain.pMin.pos())[d] / nd_domain_extent[d];
      
      double l = pt.pos().length();
      if(l < m_radius)
		integration_value += getSample(pt);
    }
    integration_value /= (double)arg_pointset.size();*/
    
    return true;
  }
  
	double analyticIntegrand()
	{
		return analytic_value_precompute;
	}
	
	double sampleIntegrand(Point<2, double>& pt)
	{
		Point_2 p( pt.pos()[0], pt.pos()[1] );
		return arrangement.sample(p);
	}
  
private:
	
  utk::BoxesAligned arrangement;
  bool rasterize_arrangement;
  std::string raster_file;
  double analytic_value_precompute;
  
};

}//end namespace

#endif
