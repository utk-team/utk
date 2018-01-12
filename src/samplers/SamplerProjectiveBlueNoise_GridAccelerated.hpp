#ifndef _UTK_SAMPLER_PBN_
#define _UTK_SAMPLER_PBN_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>
#include <cmath>
#include <boost/iterator/iterator_concepts.hpp>

namespace utk
{

	class SamplerProjectiveBlueNoise
	{
	protected:

	public:

		SamplerProjectiveBlueNoise() { setRandomSeedTime(); }

		void setRandomSeed(long unsigned int arg_seed) { m_mersenneTwister.seed(arg_seed); }
		void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }
		
		template<unsigned int D, typename T, typename P>
		bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
		{
			if (!arg_pts.empty())
			{
				WARNING("SamplerProjectiveBlueNoise::generateSamples the pointset to fill is not empty, clearing it ...");
				arg_pts.clear();
			}
			
			if(!isFloatingType<T>())
			{
				ERROR("SamplerProjectiveBlueNoise::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
				return false;
			}
			
			if(D < 1 || D > 4)
			{
				ERROR("SamplerProjectiveBlueNoise::generateSamples has only been defined between 1 and 4 dimensions");
				return false;
			}
			
			//load domain & toricity
			for(uint d = 0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			arg_pts.toricity = 1;

			//arg_pts.resize(arg_points);
			
			double distDT_1D = 1.0/(2.0*arg_points);
			double distDT_2D = sqrt(1.0/(2.0*sqrt(3)*arg_points));
			double distDT_3D = pow(1.0/(4.0*sqrt(2)*arg_points), 1.0/3.0);
			double distDT_4D = pow(1.0/(8.0*arg_points), 1.0/4.0);
			
			std::cout << "Dists " << std::endl;
			std::cout << "1D " << distDT_1D << std::endl;
			std::cout << "2D " << distDT_2D << std::endl;
			std::cout << "3D " << distDT_3D << std::endl;
			std::cout << "4D " << distDT_4D << std::endl;
			
			if(D==2)
				PBN2D(arg_pts,arg_points);
			
			return true;
		};

	protected:
		std::mt19937 m_mersenneTwister;
		double getRandom01()
		{
			return (double)m_mersenneTwister() / (double)m_mersenneTwister.max();
		}
		
		template<unsigned int D, typename T, typename P>
		bool PBN2D(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
		{
			double radius = 2.0 * sqrt(1.0/(2.0*sqrt(3)*arg_points));
			double res = radius / sqrt(2);
			int nb = 1.0/res + 1;
			
			std::vector< std::vector< Pointset<D, T, P> > > grid;
			//std::vector< std::vector <bool> > locked;
			grid.resize(nb);
			//locked.resize(nb);
			
			for(double i=0; i<nb; i++)
			{
				for(double j=0; j<nb; j++)
				{
					Pointset<D, T, P> pts;
					pts.resize(20);
					for(int n=0; n<pts.size(); n++)
					{
						P p;
						p.pos()[0] = (i+getRandom01())*res;
						p.pos()[1] = (j+getRandom01())*res;
						pts[n] = p;
					}
					grid[i].push_back(pts);
					//locked[i].push_back(0);
				}
			}
			
			for(int i=0; i<nb; i++)
				std::cout << nb << " " << grid.size() << "/" << grid[i].size() << std::endl;
			
			for(int i=0;i<nb; i++)
			for(int j=0;j<nb; j++)
			{
				//std::cout << i << "," << j << std::endl;
				P barycenter;
				double N = 1.0;
				for(int x=-1;x<=1; x++)
				for(int y=-1;y<=1; y++)
				{
					if(x==0 && y==0)
						continue;
					if(i+x < 0 || i+x >= nb)
						continue;
					if(j+y < 0 || j+y >= nb)
						continue;
					
					barycenter.pos()[0] += grid[i+x][j+y][0].pos()[0];
					barycenter.pos()[1] += grid[i+x][j+y][0].pos()[1];
					N++;
				}
				
				double dist_bary = 1;
				double id_bary = 0;
				for(int n=0; n<20; n++)
				{
					double x = grid[i][j][n].pos()[0] - barycenter.pos()[0];
					double y = grid[i][j][n].pos()[1] - barycenter.pos()[1];
					double dist = sqrt(x*x + y*y);
					if(dist < dist_bary)
					{
						dist_bary = dist;
						id_bary = n;
					}
				}
				grid[i][j][0] = grid[i][j][id_bary];
				
				arg_pts.push_back(grid[i][j][0]);
			}
			
			return true;
		}
	};

} //end namespace utk


#endif
