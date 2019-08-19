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
#ifndef _UTK_KLFUNC_HPP_
#define _UTK_KLFUNC_HPP_

#include "../pointsets/Pointset.hpp"
#include "../pointsets/Histogram.hpp"

namespace utk
{

class KLFunctions
{
	
public:
	template <uint D, typename T, typename P>
	bool computeK(Pointset<D, T, P> arg_pts, Histogram1dd& arg_K, double rmin=0.01, double rmax=0.15, uint nbbins=200, bool normalized_distance=true)
	{
		if(D != 2)
		{
			ERROR("KLFunctions::computeK can only be computed in 2D (other dimensions not implemented yet)");
			return false;
		}
		
		Pointset<D, double, Point<D, double> > pts01;
		arg_pts.normalizePositions(pts01);
			
		uint N = pts01.size();

		double ra = rmin;//*smoothing;
		double rb = rmax;
		
		//std::cout << "rmin " << rmin << " - rmax " << rmax << std::endl;

		arg_K.resize(nbbins);
		
		//int pcfid=0;
		//double step = (rb-ra)/(double)nbbins;
		
		for(uint k_id = 0; k_id < nbbins; ++k_id)
		{
			std::cout << 100.0 * (double(k_id)/double(nbbins)) << "%    \r" << std::flush;
			
			double r = ra + k_id*(rb - ra) / (double) nbbins;
			//std::cout << " r " << r << std::endl;

			double estimator = 0;
			//double n = 0;
			for(uint i=0; i<N; i++)
			{
				for(uint j=i+1; j<N; j++)
				{
					double val = distance<D, double, Point<D, double> >(pts01[i], pts01[j]);
				//double x = pts01[i].pos()[0] - pts01[j].pos()[0];
				//	double y = pts01[i].pos()[1] - pts01[j].pos()[1];
					if (val < r)
						estimator += 2.0;//2.0 / ((1.0 - fabs(x))*(1.0 - fabs(y)));//2*(val * 2*M_PI*r) / (M_PI*val*val);
					//if(i==j) continue;
						
					//for the pair (i,j)
					//double val = gaussianKernel(smoothing, r - distance<D, double, Point<D, double> >(pts01[i], pts01[j]));
					//estimator += val;

					//for the pair (j,i)
					//estimator += val;
				}
			}
 			//double cov = 1.0 - ((2.0*r)/M_PI)*2.0 + ((r*r)/M_PI);//2.0*rmax*rmax*acos(r/(2.0*rmax)) - (rmax/2.0)*(sqrt(4.0*rmax*rmax - r*r));
 			//double factor = 2.0*M_PI*r*cov;
 			//factor = 1.0/factor;
// 			
 			//estimator*=factor;
// 			
 			//estimator /= (N*(N-1));
 			
 			double intensity_estim_2 = N*N;

			arg_K[k_id].first[0] = r;
			arg_K[k_id].second = estimator / (intensity_estim_2);// / (double)(N*N);
		}
		
		if(normalized_distance)
		{
			double norm_dist = 0;
			if(D==2)
				norm_dist = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*N) );
			else if(D==3)
			{
				double maxpack = M_PI/(3.0*sqrt(2.0));
				norm_dist = 2.0 * cbrt( (maxpack/(double)N) * (tgamma((double)D/2.0 + 1)/pow(M_PI, (double)D/2.0)) );
			}
			for(uint i=0; i<arg_K.size(); i++)
				arg_K[i].first[0] /= norm_dist;
		}
		
		return true;
	}
	
	
	
	template <uint D, typename T, typename P>
	bool computeL(Pointset<D, T, P> arg_pts, Histogram1dd& arg_K, double rmin=0.01, double rmax=0.15, uint nbbins=200, bool normalized_distance=true)
	{
		if(D != 2)
		{
			ERROR("KLFunctions::computeK can only be computed in 2D (other dimensions not implemented yet)");
			return false;
		}
		
		Pointset<D, double, Point<D, double> > pts01;
		arg_pts.normalizePositions(pts01);
			
		uint N = pts01.size();

		double ra = rmin;//*smoothing;
		double rb = rmax;
		
		//std::cout << "rmin " << rmin << " - rmax " << rmax << std::endl;

		arg_K.resize(nbbins);
		
		//int pcfid=0;
		//double step = (rb-ra)/(double)nbbins;
		
		for(uint k_id = 0; k_id < nbbins; ++k_id)
		{
			std::cout << 100.0 * (double(k_id)/double(nbbins)) << "%    \r" << std::flush;
			
			double r = ra + k_id*(rb - ra) / (double) nbbins;
			//std::cout << " r " << r << std::endl;

			double estimator = 0;
			//double n = 0;
			for(uint i=0; i<N; i++)
			{
				for(uint j=i+1; j<N; j++)
				{
					double val = distance<D, double, Point<D, double> >(pts01[i], pts01[j]);
				//	double x = pts01[i].pos()[0] - pts01[j].pos()[0];
				//	double y = pts01[i].pos()[1] - pts01[j].pos()[1];
					if (val < r)
						estimator += 2.0;// / ((1.0 - fabs(x))*(1.0 - fabs(y)));//2*(val * 2*M_PI*r) / (M_PI*val*val);
					//if(i==j) continue;
						
					//for the pair (i,j)
					//double val = gaussianKernel(smoothing, r - distance<D, double, Point<D, double> >(pts01[i], pts01[j]));
					//estimator += val;

					//for the pair (j,i)
					//estimator += val;
				}
			}
 			//double cov = 1.0 - ((2.0*r)/M_PI)*2.0 + ((r*r)/M_PI);//2.0*rmax*rmax*acos(r/(2.0*rmax)) - (rmax/2.0)*(sqrt(4.0*rmax*rmax - r*r));
 			//double factor = 2.0*M_PI*r*cov;
 			//factor = 1.0/factor;
// 			
 			//estimator*=factor;
// 			
 			//estimator /= (N*(N-1));
 			
 			double intensity_estim_2 = N*N;
			estimator /= intensity_estim_2;

			estimator = sqrt(estimator / M_PI);
			
			arg_K[k_id].first[0] = r;
			arg_K[k_id].second = estimator;// / (double)(N*N);
		}
		
		if(normalized_distance)
		{
			double norm_dist = 0;
			if(D==2)
				norm_dist = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*N) );
			else if(D==3)
			{
				double maxpack = M_PI/(3.0*sqrt(2.0));
				norm_dist = 2.0 * cbrt( (maxpack/(double)N) * (tgamma((double)D/2.0 + 1)/pow(M_PI, (double)D/2.0)) );
			}
			for(uint i=0; i<arg_K.size(); i++)
				arg_K[i].first[0] /= norm_dist;
		}
		
		return true;
	}
	
private:
	
	//Fast toroidal distance
	template <uint D, typename T, typename P>
	double toricDistance(const P &a, const P &b)
	{
		double dist = 0;
		for(int d=0; d<D; d++)
		{
			double diff = std::min(std::abs(a.pos()[d] - b.pos()[d]), 1- std::abs(a.pos()[d] - b.pos()[d]));
			diff *= diff;
			dist += diff;
		}
		return sqrt(dist);
	}
	template <uint D, typename T, typename P>
	double distance(const P &a, const P &b)
	{
		double dist = 0;
		for(uint d=0; d<D; d++)
		{
			double diff = a.pos()[d] - b.pos()[d];
			diff *= diff;
			dist += diff;
		}
		return sqrt(dist);
	}

	//Gaussian Kernel 1D
	double gaussianKernel(const double sigma, const double x)
	{
		return ( 1.0/(sqrt(2*M_PI)*sigma) ) * exp( -0.5*(x*x)/(sigma*sigma) );
	}
	
	//Box Kernel 1D
	double boxKernel(const double h, const double x)
	{
		if(x >= -h && x < h)
			return 1.0/2.0*h;
		return 0;
	}
	
	// Scaling of the PCF such that it integrates to one
	void normalizeToOne(Histogram1dd& func)
	{
		double cdf = 0;
		//normalize & weight
		for(uint i=1; i<func.size(); i++)
			cdf += func[i].second * (func[i].first[0]-func[i-1].first[0]);
		
		if (cdf == 0) return;
		
		for(uint i=0; i<func.size(); i++)
			func[i].second /= cdf;
	}
};

}//end namespace utk

#endif
