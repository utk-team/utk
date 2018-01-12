//
// Source code for the paper, edited to fit the utk framework
// 
// D. Heck and T. Schloemer and O. Deussen, "Blue Noise Sampling with
// Controlled Aliasing", ACM Trans. Graph., 2013, in press
//
// Copyright (C) 2012,2013 Daniel Heck and Thomas Schloemer
//

#include "SamplerStep/common.hh"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <cfloat>
#include <signal.h>
#include <sys/time.h>
#include <random>
#include <chrono>

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace heck
{

int nbins = -1;                 // default: number of points
bool randomize_force = false;
int maxattempts = 5;
float Tmax = 1, Tmin = 1e-3, Tfactor = 0.9;

// A flag set on SIGKILL to stop optimization
bool abort_flag = false;

float CalcGradients(const std::vector<heck_Point> &pts, 
        const Curve &rdf, const Curve &target,
        std::vector<heck_Point> &gradient) {
    Curve force(nbins, 0, 0.5f);

    for (int j=0; j<force.size(); j++)
        force[j] = force.dx * (rdf[j] - target[j]);
    for (int j=1; j<force.size(); j++)
        force[j] += force[j-1];
    for (int j=1; j<force.size(); j++) {
        float x = force.ToX(j);
        force[j] /= (pts.size() * x * x);
        // if (force[j] < 0) force[j] = 0;
    }
    force.Write("force.dat");

    float maxforce = 0.0f;
    gradient.resize(pts.size());
    
#ifdef _OPENMP
#pragma omp parallel
#endif
{
#ifdef _OPENMP
#pragma omp for schedule(static)
#endif
    for (unsigned int i=0; i<pts.size(); i++) {
        heck_Point grad;
        for (unsigned int j = 0; j < pts.size(); j++) {
            if (i == j)
                continue;

            float dx = pts[j].x - pts[i].x;
            float dy = pts[j].y - pts[i].y;
            dx += (dx < -0.5f) - (dx > 0.5f);
            dy += (dy < -0.5f) - (dy > 0.5f);
            float dist2 = dx * dx + dy * dy;

            // RDF is only reliable up to 0.5, if we go higher, the periodic
            // boundary conditions cause anisotropies in the final spectrum
            if (dist2 > 0.49f * 0.49f || dist2 == 0) continue;

            float dist = sqrtf(dist2);
            float f = force.At(dist);// / dist;
            grad.x -= dx * f;
            grad.y -= dy * f;
        }
        float ff = grad.x*grad.x + grad.y*grad.y;
        if (ff > maxforce)
            maxforce = ff;
        gradient[i] = grad;
    }
}
    return sqrtf(maxforce);
}

std::vector<heck_Point> MovePoints(const std::vector<heck_Point> &pts,
        const std::vector<heck_Point> &gradient, float stepsize)
{
    std::vector<heck_Point> newpts(pts.size());
    for (unsigned i=0; i<pts.size(); i++) {
        heck_Point force = gradient[i];
        if (randomize_force) {
            force.x *= drand48();
            force.y *= drand48();
        }
        float x = pts[i].x + stepsize * force.x;
        float y = pts[i].y + stepsize * force.y;

        while (x >= 1) x -= 1;
        while (y >= 1) y -= 1;
        while (x < 0) x += 1;
        while (y < 0) y += 1;

        newpts[i].x = x;
        newpts[i].y = y;
    }
    return newpts;
}


float CalcEnergy(const Curve &a, const Curve &b) {
    Curve c(a);
    for (int i=0; i<c.size(); i++) 
        c[i] = powf(c[i] - b[i], 2.0f);
    return sqrtf(Integrate(c));
//    fprintf(stderr, "e = %g\n", x);
//    return x;
}

float MainOptimization(const std::vector<heck_Point> &pts,
        Curve &target,
        float smoothing,
        std::vector<heck_Point> &output)
{

    std::vector<heck_Point> current = pts, best = pts;
    std::vector<heck_Point> gradient;
    Curve rdf = CalcRDF(nbins, current.size(), &current[0].x, smoothing);
    float bestenergy = CalcEnergy(rdf, target);
    float T = Tmax;             // temperature
    int attempts = 0;
    while (!abort_flag && T >= Tmin) {
        // Calculate gradients and move points
        float maxgrad = CalcGradients(current, rdf, target, gradient);
        float stepsize = T / (sqrt(current.size()) * maxgrad);
        current = MovePoints(current, gradient, stepsize);

        // Perform iteration control
        rdf = CalcRDF(nbins, current.size(), &current[0].x, smoothing);
        float energy = CalcEnergy(rdf, target);
        attempts++;
        if (energy < bestenergy) {
            attempts = 0;
            best = current;
            bestenergy = energy;
        } else if (energy > bestenergy * 1.2) {
            attempts = maxattempts;
            current = best;
        }
        /*printf("%1.5g %1.5g\t%5.5g\t%g\n", 
                T, stepsize, energy, bestenergy);*/
        if (attempts >= maxattempts) {
            attempts = 0;
            T *= Tfactor;
        }
    }
    output = best;
    return bestenergy;
}


void FunctionStep(float critx, Curve &c, int npts) {
    float x0 = critx * sqrt(2/(sqrt(3)*npts));
    for (int i=0; i<c.size(); i++) {
        c[i] = (c.ToX(i) >= x0) ? 1 : 0;
    }
}

void FunctionJinc(float critx, Curve &c, int npts) {
    float x0 = critx / sqrt(2/(sqrt(3)*npts));
    for (int i=0; i<c.size(); i++) {
        float xx0 = c.ToX(i);
    
        float a = 0.0;
        const int N=20;
        for (int j=0; j<N; j++) {
            float x = xx0 + j*c.dx/N;
            if (x < 1e-5) a += 1-M_PI*x0*x0/npts;
            else {
                float xx = 2*M_PI*x0*x;
                a += 1 - 2*M_PI*x0*x0/npts * jn(1, xx) / xx;
            }
        }
        c[i] = a / N;
    }
}

void FunctionPeak(float critx, float peaky, Curve &c, int npts) {
    for (int i=0; i<c.size(); i++) {
        c[i] = (c.ToX(i) > critx) ? 1 : 0;
        c[i] = ((int) c.ToX(i) == (int) critx) ? peaky : c[i];
    }
}

}

namespace utk
{

class SamplerStep
{
protected:

public:

	SamplerStep() { setRandomSeedTime(); }

	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerStep::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerStep::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;

		arg_pts.resize(arg_points);
		
		
		std::vector<heck_Point> pts, result;
		
		int npts = arg_points;
		pts.resize(npts);
		for (int i=0; i<npts; i++) {
			pts[i].x = getRandom01();
			pts[i].y = getRandom01();
		}
		
		result.resize(pts.size());

		heck::nbins = pts.size();

		double critfreq = 0.606;
		Curve target(heck::nbins, 0, 0.5f);
		heck::FunctionJinc(critfreq, target, pts.size());
		Curve tmp(heck::nbins, 0, npts / 2);
		tmp = RDF2Power(npts, target, heck::nbins, 0, npts / 2);
		
		for (int i=0; i<npts; i++)
			result[i] = pts[i];
		
		float smoothing = 8;
		float bestd = heck::MainOptimization(pts, target, smoothing, result);
		
		for (int i=0; i<npts; i++)
		{
			arg_pts[i].pos()[0] = result[i].x;
			arg_pts[i].pos()[1] = result[i].y;
		}
		
		return true;
	}

protected:
    std::mt19937 m_mersenneTwister;
    double getRandom01()
    {
        return (double)m_mersenneTwister()/(double)m_mersenneTwister.max();
    }
};

} //end namespace utk