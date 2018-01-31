#ifndef _UTK_SAMPLER_GRADIENTDESCENT_
#define _UTK_SAMPLER_GRADIENTDESCENT_

#include "../io/fileIO.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"

#include "../../externals/dCpp/dCpp.h"

using namespace dCpp;

namespace utk
{

class SamplerGradientDescent
{
protected:
public:
	SamplerGradientDescent() { m_fileinit=""; }
	
	void setInputPointset(std::string arg_input) { m_fileinit = arg_input; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerGradientDescent::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerGradientDescent::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;
		
		if(m_fileinit.empty())
		{
			ERROR("The initial pointset to optimise is empty");
			return false;
		}
		
		PointsetReader<2, double, Point<2, double> > reader;
		Pointset<2, double, Point<2, double> > initpts;
		reader.open(m_fileinit);
		reader.readPointset(initpts);
		reader.close();
		
		uint arg_points = initpts.size();
		
		std::vector<var> px;
		std::vector<var> py;
		for(int i=0; i<arg_points; i++)
		{
			px.push_back(initpts[i].pos()[0]);
			py.push_back(initpts[i].pos()[1]);
		}
		alpha = 600;
		
		
		dCpp::initSpace(1);
		for(auto i=0; i< px.size(); ++i)
		{
			dCpp::init(px[i]);
			dCpp::init(py[i]);
		}
		
		double norm = 1.0;
		var e = generalizedL2NonSmooth(px,py);
		double init_disc = e.id;
		double lambda = 1;
		uint iter = 0;
		double norm_bound = 1e-5;
		uint last_energy_equal = 0;
		double last_energy = 1;
		while(norm > norm_bound && last_energy_equal < 6)
		{
			norm = 0.0;
			for(auto i=0; i<px.size(); ++i)
			{
				double mvx = lambda*e.d(&px[i]).id;
				double mvy = lambda*e.d(&py[i]).id;
				px[i] += -mvx;
				py[i] += -mvy;
				norm += sqrt(mvx*mvx + mvy*mvy);
			}
			e = generalizedL2NonSmooth(px,py);
			
			std::cout << "(lambda:" << lambda << ")(iter:" << iter << ")(norm:" << norm << "/" << norm_bound << ") L2Discrepancy= " << e.id << std::endl;
			iter++;
			lambda *= 0.9;
			if(lambda < 0.1) lambda = 0.1;
			if(e.id == last_energy)
				last_energy_equal++;
			else
				last_energy_equal=0;
			last_energy = e.id;
		}
		double end_disc = e.id;
		
		std::cout << "Discrepancy reduction from " << init_disc << " to " << end_disc << " (" << (int)(init_disc/end_disc) << "x)" << std::endl;
		std::cout << "Iter:" << iter << std::endl;
	
		arg_pts.resize(arg_points);
		for(uint i=0; i<arg_points; i++)
		{
				arg_pts[i].pos()[0] = px[i].id;
				arg_pts[i].pos()[1] = py[i].id;
		}

		return true;
	};

protected:
	std::string m_fileinit;
	
	std::vector<double> initx_d;
	std::vector<double> inity_d;

	std::vector<var> initx;
	std::vector<var> inity;
	
	double alpha;

	//approx of the max function
	var varmax(const var &a, const var &b)
	{
		double power=32.0;
		return ( (b^power) + (a^power) )^(1.0/power);
	}
	double varmax(const double &a, const double &b)
	{
		double power=32.0;
		return pow( pow(b,power) + pow(a,power), 1.0/power);
	}

	var smoothabs(var x)
	{
		double k = 10;
		return (2.0/k)*log(10, 1 + exp(k*x))-x-(2.0/k)*log(10, 2);
	}

	var smoothmax(const var& x, const var& y)
	{
		//double alpha = 300;
		return ( x*exp(alpha*x)+y*exp(alpha*y) ) / ( exp(alpha*x)+exp(alpha*y) );
	}

	var generalizedL2(const std::vector<var> &px,
					const std::vector<var> &py)
	{
		unsigned int N = px.size();
		var a, factor_b, factor_c;

		a = 16.0/9.0;
		factor_b = 2.0/(double)N;
			
		factor_c = 1.0/(double)(N);
		factor_c *= factor_c;
		
		var sumb = 0.0;
		for(unsigned int i=0; i<N; i++)
		{
			var prodb = 1.0;
			
			var uij = px[i];
			prodb *= ((3.0 - uij*uij)/2.0);
			
			uij = py[i];
			prodb *= ((3.0 - uij*uij)/2.0);
			
			sumb += prodb;
		}
		//std::cout << "sum =" << sumb << std::endl;
		
		var sumc = 0.0;
		for(uint i=0; i<N; i++)
		for(unsigned int iprime=0; iprime<N; iprime++)
		{
			var prodc = 1.0;
			
			var uij = px[i];
			var uiprimej = px[iprime];
			//var m = uij > uiprimej ? uij : uiprimej;//std::max(uij, uiprimej);
			var m = smoothmax(uij, uiprimej);
			prodc *= (2.0 - m);
			
			uij = py[i];
			uiprimej = py[iprime];
			//m = uij > uiprimej ? uij : uiprimej;//std::max(uij, uiprimej);
			m = smoothmax(uij, uiprimej);
			prodc *= (2.0 - m);
			
			sumc += prodc;
		}
		
		var tmp0 = factor_b*sumb;
		var tmp1 = factor_c*sumc;
		return sqrt(a -tmp0 + tmp1);
	}
	
	var generalizedL2NonSmooth(const std::vector<var> &px,
					const std::vector<var> &py)
	{
		unsigned int N = px.size();
		var a, factor_b, factor_c;

		a = 16.0/9.0;
		factor_b = 2.0/(double)N;
			
		factor_c = 1.0/(double)(N);
		factor_c *= factor_c;
		
		var sumb = 0.0;
		for(unsigned int i=0; i<N; i++)
		{
			var prodb = 1.0;
			
			var uij = px[i];
			prodb *= ((3.0 - uij*uij)/2.0);
			
			uij = py[i];
			prodb *= ((3.0 - uij*uij)/2.0);
			
			sumb += prodb;
		}
		//std::cout << "sum =" << sumb << std::endl;
		
		var sumc = 0.0;
		for(uint i=0; i<N; i++)
		for(unsigned int iprime=0; iprime<N; iprime++)
		{
			var prodc = 1.0;
			
			var uij = px[i];
			var uiprimej = px[iprime];
			var m = uij > uiprimej ? uij : uiprimej;//std::max(uij, uiprimej);
			//var m = smoothmax(uij, uiprimej);
			prodc *= (2.0 - m);
			
			uij = py[i];
			uiprimej = py[iprime];
			m = uij > uiprimej ? uij : uiprimej;//std::max(uij, uiprimej);
			//m = smoothmax(uij, uiprimej);
			prodc *= (2.0 - m);
			
			sumc += prodc;
		}
		
		var tmp0 = factor_b*sumb;
		var tmp1 = factor_c*sumc;
		return sqrt(a -tmp0 + tmp1);
	}
	
	double generalizedL2(const std::vector<double> &px,
					const std::vector<double> &py, bool d=0)
	{
		unsigned int N = px.size();
		double a, factor_b, factor_c;

		a = 16.0/9.0;
		factor_b = 2.0/(double)N;
			
		factor_c = 1.0/(double)(N);
		factor_c *= factor_c;
		
		double sumb = 0.0;
		for(unsigned int i=0; i<N; i++)
		{
			double prodb = 1.0;
			
			double uij = px[i];
			prodb *= ((3.0 - uij*uij)/2.0);
			
			uij = py[i];
			prodb *= ((3.0 - uij*uij)/2.0);
			
			sumb += prodb;
		}
		//std::cout << "sum =" << sumb << std::endl;
		
		double sumc = 0.0;
		for(uint i=0; i<N; i++)
		for(unsigned int iprime=0; iprime<N; iprime++)
		{
			double prodc = 1.0;
			
			double uij = px[i];
			double uiprimej = px[iprime];
			double m = uij > uiprimej ? uij : uiprimej;//std::max(uij, uiprimej);
			prodc *= (2.0 - m);
			
			uij = py[i];
			uiprimej = py[iprime];
			m = uij > uiprimej ? uij : uiprimej;//std::max(uij, uiprimej);
			prodc *= (2.0 - m);
			
			sumc += prodc;
		}
		
		double tmp0 = factor_b*sumb;
		double tmp1 = factor_c*sumc;
		return sqrt(a -tmp0 + tmp1);
	}
};

} //end namespace utk


#endif
