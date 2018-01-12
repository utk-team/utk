#ifndef _UTK_SAMPLER_FATTAL_
#define _UTK_SAMPLER_FATTAL_

#include "../pointsets/Pointset.hpp"
#include "SamplerFattal/pdist_code/main.hh"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"

namespace utk
{
	
/**
 * Code from
 * 
 * Fattal, R. (2011, August). 
 * Blue-noise point sampling using kernel density model. 
 * In ACM Transactions on Graphics (TOG) (Vol. 30, No. 4, p. 48). ACM.
 * http://www.cs.huji.ac.il/~raananf/projects/kdm/index.html
 * 
 */

class SamplerFattal
{
protected:
	
public:

	SamplerFattal() { 
		temperature = 0.5;
	}
	
	void setTemperature(double arg_temperature) { temperature = arg_temperature; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (D != 2)
		{
			ERROR("SamplerFattal::generateSamples handles only 2D samples");
			return false;
		}
		
		if (!arg_pts.empty())
		{
			WARNING("SamplerFattal::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerFattal::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;
		
		/* Global inits */
		int levels = 2;  // DEAL WITH THIS

		fattal::Exp_tab = new double[EXP_TAB_LEN] ;
		for(int i = 0 ; i < EXP_TAB_LEN ; i++) {
			fattal::Exp_tab[i] = KER_AMP * exp(-10.0 * ((double)i / EXP_TAB_LEN) / (RAD * RAD)) ;
		}

		fattal::exp_fac = (double)EXP_TAB_LEN / 10.0 ;

		fattal::Pow2_tab = new unsigned long[levels+2] ;
		fattal::Pow2_tab[0] = 1 ;
		for(int i = 1 ; i < levels+2 ; i++)
			fattal::Pow2_tab[i] = 2 * fattal::Pow2_tab[i-1] ;
		
		double *I, *osigma = NULL, *CA = NULL, *sigma = NULL, *sigma_x = NULL, *sigma_y = NULL ;
		double **rho ;
		fattal::Dot* dots ;
		int ndots ;

		int* nx = new int[levels] ;
		int* ny = new int[levels] ;
		rho = new double*[levels] ;

		int nbp = arg_points;
		int nres = int(sqrt(nbp)*pow(2,levels)+0.5);
		nx[0] = nres ;
		ny[0] = nres ;

		int len = nx[0] * ny[0] ;

		I = new double[len] ;

		fattal::SP.clear() ;

		rho[0] = new double[len] ;

		for(int x = 0 ; x < nx[0] ; x++)
			for(int y = 0 ; y < ny[0] ; y++) {
			int i = x + y * nx[0] ;
			I[i] = 0.43 ;
			}

		for(int i = 0 ; i < len ; i++)
			rho[0][i] = pow(I[i], 1.0) ;

		for(int l = 1 ; l < levels ; l++) {
			fattal::downsample(rho[l-1], nx[l-1], ny[l-1], rho[l], nx[l], ny[l]) ;
		}

		/* Coarsest level */
		int l = levels - 1 ;

		fattal::init_fields(rho[l], nx[l], ny[l], sigma, sigma_x, sigma_y, osigma) ;

		len = nx[l] * ny[l] ;

		CA = new double[len] ;

		for(int i = 0 ; i < len ; i++)
			CA[i] = 0 ;

		build_init_sample(osigma, rho[l], nx[l], ny[l], dots, ndots) ;

		reorder_dots(nx[l], ny[l], dots, ndots) ;

		fattal::relax(temperature, dots, ndots, CA, rho[l], osigma,
			sigma_x, sigma_y, nx[l], ny[l], fattal::iters, fattal::save_every) ;
			
		/* refining */
		//int ff = 0 ;

		for(int l = levels - 2 ; l >= 0 ; l--) {

			if(l==0)
			fattal::iters = fattal::fl_iters ;

			upsample_dots(dots, ndots, sigma, nx[l+1], ny[l+1]) ;

			fattal::upsample(CA, nx[l+1], ny[l+1], nx[l], ny[l]) ;

			fattal::init_fields(rho[l], nx[l], ny[l], sigma, sigma_x, sigma_y, osigma) ;

			render_points(CA, osigma, nx[l], ny[l], dots, ndots, INACT_POINTS) ;

			fattal::relax(temperature, dots, ndots, CA, rho[l], osigma,
				sigma_x, sigma_y, nx[l], ny[l], fattal::iters, fattal::save_every) ;
		}

		for(int i = 0 ; i < ndots ; i++)
			if(dots[i].density_log4 > 0)
			cout << "bad " << dots[i].density_log4 << endl ;

		arg_pts.resize(ndots);
		for(int i = 0 ; i < ndots ; i++)
		{
			double ax = dots[i].x / (double)(nx[0]);
			double by = dots[i].y / (double)(ny[0]);
			arg_pts[i].pos()[0] = ax;
			arg_pts[i].pos()[1] = by;
		}
		
		return true;
	}

protected:
	double temperature;
};

} //end namespace utk


#endif
