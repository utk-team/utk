/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * and Bastien Doignies bastien.doignies@liris.cnrs.fr 
 * and David Coeurjolly David.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2023 CNRS Université de Lyon
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
 * either expressed or implied, of the UTK project.
 */
#pragma once

#include <Lloyd/Delaunay_psm.h>
#include <utk/utils/FastPRNG.hpp>
#include <utk/utils/Pointset.hpp>

#include <cstdlib>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace utk
{

class SamplerLloyd
{
protected:
public:

	SamplerLloyd(uint32_t nbSteps_ = 100, bool periodic = false) : 
		nbSteps(nbSteps_),
		periodic_(periodic) 
	{ 
		GEO::initialize();
		setRandomSeed(); 
	}

    uint32_t GetDimension() const { return 3; }

	void setNbSteps(uint32_t nb) { nbSteps = nb; }

	void setRandomSeed( uint64_t arg_seed ) { gen.seed(arg_seed); }
	void setRandomSeed() { gen.seed(std::random_device{}()); }

	void setToricity(bool isToric)
	{
		periodic_ = isToric;
	}

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		if constexpr (std::is_same_v<T, double>)
		{
			generate(arg_pts, N);
		}
		else
		{
			arg_pts.Resize(N, 3);
			
			Pointset<double> tmp;
			tmp.Resize(N, 3);

			generate(tmp, N);
			for (uint32_t i = 0; i < N * 3; i++)
			{
				arg_pts.Data()[i] = static_cast<T>(tmp.Data()[i]);
			}
		}
		return true;
	};

	~SamplerLloyd()
	{
		if (delaunay_ != nullptr)
			delete delaunay_;
	}

protected:
    utk::PCG32 gen;
	uint32_t nbSteps;
	bool periodic_;
    
	GEO::PeriodicDelaunay3d::IncidentTetrahedra W_;
    GEO::PeriodicDelaunay3d *delaunay_ = nullptr;

	void generate(Pointset<double>& arg_pts, uint32_t N)
    {
		if (delaunay_ != nullptr)
			delete delaunay_;

		delaunay_ = new GEO::PeriodicDelaunay3d(periodic_, 1.0);
		if(!periodic_)
			delaunay_->set_keeps_infinite(true);
		
		Pointset<double> new_pts(N, 3);
		arg_pts.Resize(N, 3);

		init_random_points(arg_pts);

		//Main loop
		for(uint32_t i = 0; i < nbSteps ; ++i)
			Lloyd_step(arg_pts, new_pts);
    }
	
	void init_random_points(Pointset<double>& arg_pts)
    {
		std::uniform_real_distribution<double> dist(0, 1);
		GEO::index_t nb_points = arg_pts.Npts();

		for(GEO::index_t i = 0; i < 3 * nb_points; ++i)
		{
			arg_pts.Data()[i] = dist(gen);
		}
		delaunay_->set_vertices(nb_points, arg_pts.Data());
		delaunay_->compute();
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

	void Lloyd_step(Pointset<double>& arg_pts, Pointset<double>& new_pts)
    {
		GEO::ConvexCell C;
		for(GEO::index_t v = 0; v < arg_pts.Npts(); ++v)
		{
			get_cell(v, C);
			GEO::vec3 g = C.barycenter();
			new_pts[v][0] = g.x;
			new_pts[v][1] = g.y;
			new_pts[v][2] = g.z;
		}
		// In periodic mode, points may escape out of
		// the domain. Relocate them in [0,1]^3

		for(GEO::index_t i = 0; i < 3 * arg_pts.Npts(); ++i)
		{
			if(new_pts.Data()[i] < 0.0)
				new_pts.Data()[i] += 1.0;
		
			if(new_pts.Data()[i] > 1.0)
				new_pts.Data()[i] -= 1.0;
		}

		arg_pts.swap(new_pts);
		delaunay_->set_vertices(arg_pts.Npts(), arg_pts.Data());
		delaunay_->compute();
    }
};

}
