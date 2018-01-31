#ifndef _UTK_SAMPLER_NEBRASKA_GRID_
#define _UTK_SAMPLER_NEBRASKA_GRID_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include "../imageIO.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>

namespace utk
{

	class SamplerNebraska
	{
	protected:

	public:

		SamplerNebraska() { srand(time(NULL)); }

		template<unsigned int D, typename T, typename P>
		bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
		{
			if (!arg_pts.empty())
			{
				WARNING("SamplerNebraska::generateSamples the pointset to fill is not empty, clearing it ...");
				arg_pts.clear();
			}

			if(!isFloatingType<T>())
			{
				ERROR("SamplerNebraska::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
				return false;
			}
			
			if(D != 2)
			{
				ERROR("SamplerNebraska::generateSamples the Nebraska sampler is only defined in 2D");
				return false;
			}

			//load domain & toricity
			for(int d = 0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			arg_pts.toricity = 0;

			//arg_pts.resize(arg_points);
			
			double domain = sqrt(2);
			double nbdisk = sqrt(2*arg_points);
			double step = domain/nbdisk;

			double off = 0.5;
			
			double x0_orig = -off;
			double y0_orig = -off;
			
			double x1_orig = -off;
			double y1_orig = 1+off;
			
			/*PointsetIllustrator<D, T, P> illu;
			illu.setPointRadius(0.002);
			illu.open("toto.ps");*/
			
			double diag0 = sqrt(x0_orig*x0_orig+y0_orig*y0_orig);
			double diag1 = sqrt(x1_orig*x1_orig+(1-y1_orig)*(1-y1_orig));

			int r0i=0;
			int r1i=0;
			for(double r0_orig=diag0+(step/2); r0_orig<=sqrt(2)+diag0; r0_orig+=step)
			{
				double x0 = x0_orig;
				double y0 = y0_orig;
				double r0 = r0_orig;
				r1i=0;
				r0i++;
				/*if(r0i%3==2)
					r0_orig += 0.04;*/
				
				for(double r1_orig=diag1+(step/2); r1_orig<=sqrt(2)+diag1; r1_orig+=step)
				{
					double x1 = x1_orig;
					double y1 = y1_orig;
					double r1 = r1_orig;
					/*r1i++;
					if(r1i%3==1)
						r1_orig += 0.04;*/
					
					double R02 = r0*r0;
					double R12 = r1*r1;
					
					double x02 = x0*x0;
					double x12 = x1*x1;
					
					double y02 = y0*y0;
					double y12 = y1*y1;
					
					//illu.drawCircle(x0, y0, r0, 0.002);
					//illu.drawCircle(x1, y1, r1, 0.002);
						
					if(y0 == y1)
					{
						Point<2, double> pt0, pt1;
						double x = (R12 - R02 - x12 + x02) / 2*(x0-x1);
						
						double A = 1;
						double B = -2*y1;
						double C = x12 + x*x - 2*x1*x + y12 - R12;
						
						double D2 = sqrt(B*B - 4*A*C);
						
						if(D2 > 0)
						{
							pt0.pos()[1] = (-B + D2) / (2.0*A);
							pt1.pos()[1] = (-B - D2) / (2.0*A);
							
							pt0.pos()[0] = x;
							pt1.pos()[0] = x;
							
							if(pt0.pos()[0] >= 0 && pt0.pos()[0] < 1)
							if(pt0.pos()[1] >= 0 && pt0.pos()[1] < 1)
								arg_pts.push_back(pt0);
							
							if(pt1.pos()[0] >= 0 && pt1.pos()[0] < 1)
							if(pt1.pos()[1] >= 0 && pt1.pos()[1] < 1)
								arg_pts.push_back(pt1);
						}
						else if (D2 == 0)
						{
							pt0.pos()[1] = (-B + D2) / (2.0*A);
							pt0.pos()[0] = x;
							
							if(pt0.pos()[0] >= 0 && pt0.pos()[0] < 1)
							if(pt0.pos()[1] >= 0 && pt0.pos()[1] < 1)
								arg_pts.push_back(pt0);
						}
					}
					else
					{
						double d = (x0 - x1) / (y0 - y1);
						double A = d*d + 1;
						
						double N = R12 - R02 - x12 + x02 - y12 + y02;
						N /= (2 * (y0 - y1));
						
						double B = 2*y0*d - 2*N*d - 2*x0;
						double C = x02 + y02 + N*N - R02 - 2*y0*N;
						
						double D2 = sqrt(B*B - 4*A*C);
						
						Point<2, double> pt0, pt1;
						
						if(D2 > 0)
						{
							pt0.pos()[0] = (-B + D2) / (2.0*A);
							pt1.pos()[0] = (-B - D2) / (2.0*A);
							
							pt0.pos()[1] = N - pt0.pos()[0]*d;
							pt1.pos()[1] = N - pt1.pos()[0]*d;
							
							if(pt0.pos()[0] >= 0 && pt0.pos()[0] < 1)
							if(pt0.pos()[1] >= 0 && pt0.pos()[1] < 1)
								arg_pts.push_back(pt0);
							
							if(pt1.pos()[0] >= 0 && pt1.pos()[0] < 1)
							if(pt1.pos()[1] >= 0 && pt1.pos()[1] < 1)
								arg_pts.push_back(pt1);
						}
						else if (D2 == 0)
						{
							pt0.pos()[0] = (-B + D2) / (2.0*A);
							pt0.pos()[1] = N - pt0.pos()[0]*d;
							
							if(pt0.pos()[0] >= 0 && pt0.pos()[0] < 1)
							if(pt0.pos()[1] >= 0 && pt0.pos()[1] < 1)
								arg_pts.push_back(pt0);
						}
					}
				}
				/*std::cout << "step=" << step << " N=" << arg_pts.size() << std::endl;

				if(arg_pts.size() < arg_points)
				{
					arg_pts.clear();
					step /= 1.1;
				}
				else*/
				//illu.close();
				return true;
			}
		};
	};

} //end namespace utk


#endif
