
/*
 * David Coeurjolly <david.coeurjolly@liris.cnrs.fr>
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
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
#ifndef _UTK_SAMPLER_LDBN_
#define _UTK_SAMPLER_LDBN_

#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "../utils.hpp"
#include "SamplerLloyd/Delaunay_psm.h"

namespace utk
{

  class SamplerLloyd
  {
  protected:
  public:


    SamplerLloyd()
    {
      setRandomSeedTime();
      m_dis = std::uniform_real_distribution<>(0.0, 1.0);
      GEO::initialize();
      periodic_ = false;
    }
    
    ~SamplerLloyd()
    {
      if (!delaunay_) delete delaunay_;
    }
    
    void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
    void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

    void setToricity(const bool isToric)
    {
      periodic_ = isToric;
    }
    
    template<unsigned int D, typename T, typename P>
    bool generateSamples(Pointset<D, T, P>& arg_pts,
                         const unsigned long long int arg_points,
                         const unsigned int nbSteps)
    {
      if (!arg_pts.empty())
        {
          WARNING("SamplerLloyd::generateSamples the pointset to fill is not empty, clearing it ...");
          arg_pts.clear();
        }
      
      if (D != 3)
        {
          ERROR("SamplerLloyd::generateSamples is only defined in 3D, not "+std::to_string(D)+".");
          return false;
        }
		
      arg_pts.resize(arg_points);
      if (periodic_)
        arg_pts.toricity = 1;
      else
        arg_pts.toricity = 0;

      //Generate
      generate(arg_points,nbSteps);
      
      if (isFloatingType<T>())
        {
          //load domain
          for(uint d=0; d<D; d++)
            {
              arg_pts.domain.pMin.pos()[d] = 0;
              arg_pts.domain.pMax.pos()[d] = 1;
            }
          auto cpt=0;
          for(uint i=0; i<arg_points; i++)
            {
              for(uint d=0; d<D; d++)
                {
                  arg_pts[i].pos()[d] = points_[cpt];
                  cpt++;
                }
            }
        }
      else
        {
          ERROR("SamplerLloyd::generateSamples coordinates must be of real numeric value");
          return false;
        }

      return true;
    };

  protected:
    std::mt19937 m_mersenneTwister;
    std::vector<double> points_;
    std::vector<double> new_points_;
    GEO::PeriodicDelaunay3d *delaunay_;
    bool periodic_;
    unsigned int nb_points_;
    GEO::PeriodicDelaunay3d::IncidentTetrahedra W_;
    std::uniform_real_distribution<> m_dis;
    double getRandom01()
    {
      return m_dis(m_mersenneTwister);
    }

    
    void generate(const unsigned int n, const unsigned int nbSteps) 
    {
      delaunay_ = new GEO::PeriodicDelaunay3d(periodic_, 1.0);
      if(!periodic_)
        delaunay_->set_keeps_infinite(true);
      
      nb_points_ = n;
      new_points_.resize(3*nb_points_);

      init_random_points();

      //Main loop
      for(auto i=0u; i < nbSteps ;++i)
        Lloyd_step();
    }

    void get_cell(GEO::index_t v, GEO::ConvexCell& C)
    {
      delaunay_->copy_Laguerre_cell_from_Delaunay(v, C, W_);
      if(!periodic_)
        {
          C.clip_by_plane(GEO::vec4( 1.0, 0.0, 0.0, 0.0));
          C.clip_by_plane(GEO::vec4(-1.0, 0.0, 0.0, 1.0));
          C.clip_by_plane(GEO::vec4( 0.0, 1.0, 0.0, 0.0));
          C.clip_by_plane(GEO::vec4( 0.0,-1.0, 0.0, 1.0));
          C.clip_by_plane(GEO::vec4( 0.0, 0.0, 1.0, 0.0));
          C.clip_by_plane(GEO::vec4( 0.0, 0.0,-1.0, 1.0));
        }
      C.compute_geometry();
    }

    void init_random_points()
    {
      GEO::index_t nb_points = GEO::index_t(nb_points_);
      points_.resize(3*nb_points);
      for(GEO::index_t i=0; i<3*nb_points; ++i)
      {
        points_[i] = getRandom01();
      }
      delaunay_->set_vertices(nb_points, points_.data());
      delaunay_->compute();
    }
    
    void Lloyd_step()
    {
      GEO::ConvexCell C;
      for(GEO::index_t v=0; v < nb_points_; ++v)
        {
          get_cell(v, C);
          GEO::vec3 g = C.barycenter();
          new_points_[3*v]   = g.x;
          new_points_[3*v+1] = g.y;
          new_points_[3*v+2] = g.z;
        }
      // In periodic mode, points may escape out of
      // the domain. Relocate them in [0,1]^3
      for(GEO::index_t i=0; i<new_points_.size(); ++i)
        {
          if(new_points_[i] < 0.0)
            new_points_[i] += 1.0;
    
          if(new_points_[i] > 1.0)
            new_points_[i] -= 1.0;
        }
      points_.swap(new_points_);
      delaunay_->set_vertices(nb_points_, points_.data());
      delaunay_->compute();
    }
  };

} //end namespace utk


#endif
