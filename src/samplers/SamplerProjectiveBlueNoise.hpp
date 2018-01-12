#ifndef _UTK_SAMPLER_PBN_
#define _UTK_SAMPLER_PBN_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>
#include <cmath>

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
			
			double dist_full;
			switch(D)
			{
				case 1: dist_full = distDT_1D; break;
				case 2: dist_full = distDT_2D; break;
				case 3: dist_full = distDT_3D; break;
				case 4: dist_full = distDT_4D; break;
			}
			
			P p;
			for(uint i=0; i<arg_points; ++i)
			{
				double ratio = 2;
			
				if(i%10 == 0)
					std::cout << i << "/" << arg_points << "\r" << std::flush;
				
				bool accept=false;
				int iter = 1;
				while( !accept )
				{	
					iter++;
					if((iter % 10000) == 0)
					{
						//std::cout << "Current Dist " << ratio*(distDT_2D / dist_full) << std::endl;
						ratio -= 0.01;
					}
					
					accept=true;
					
					for(uint d=0; d<D; d++)
						p.pos()[d] = getRandom01();
					
					//test 1D
					double current_dist = distDT_1D; // / dist_full;
					current_dist *= ratio;
						
					for(int d=0; d<D; d++)
					{
						for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
						{
							double dist = 0;
							double diff = std::abs((p.pos()[d]-arg_pts[j].pos()[d]));
							double toric_diff = std::min( diff, 1-diff );
							dist = toric_diff;
							
							if ( dist < current_dist )
								accept = false;
						}
					}

					if(D==1)
						continue;
					
					//test 2D, projections 
					//Id D=2, 01
					//If D=3, 01, 12, 20
					//If D=4, 01, 12, 20, 03, 13, 23
					if(D>=2)
					{
						double current_dist = distDT_2D; // / dist_full;
						current_dist *= ratio;
						
						for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
						{
							double dist = 0;

							{
								double diff = std::abs((p.pos()[0]-arg_pts[j].pos()[0]));
								double toric_diff = std::min( diff, 1-diff );
								dist += toric_diff*toric_diff;
							}
							{
								double diff = std::abs((p.pos()[1]-arg_pts[j].pos()[1]));
								double toric_diff = std::min( diff, 1-diff );
								dist += toric_diff*toric_diff;
							}
							
							dist = sqrt(dist);
							
							if ( dist < current_dist )
								accept = false;
						}
					}
					if(D>=3)
					{
						double current_dist = distDT_2D;
						current_dist *= ratio;
						
						for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
						{
							double dist = 0;

							{
								double diff = std::abs((p.pos()[1]-arg_pts[j].pos()[1]));
								double toric_diff = std::min( diff, 1-diff );
								dist += toric_diff*toric_diff;
							}
							{
								double diff = std::abs((p.pos()[2]-arg_pts[j].pos()[2]));
								double toric_diff = std::min( diff, 1-diff );
								dist += toric_diff*toric_diff;
							}
							
							dist = sqrt(dist);
							
							if ( dist < current_dist )
								accept = false;
						}
						for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
						{
							double dist = 0;

							{
								double diff = std::abs((p.pos()[2]-arg_pts[j].pos()[2]));
								double toric_diff = std::min( diff, 1-diff );
								dist += toric_diff*toric_diff;
							}
							{
								double diff = std::abs((p.pos()[0]-arg_pts[j].pos()[0]));
								double toric_diff = std::min( diff, 1-diff );
								dist += toric_diff*toric_diff;
							}
							
							dist = sqrt(dist);
							
							if ( dist < current_dist )
								accept = false;
						}
						
					}
					if(D==4)
					{
						for(int d=0; d<3; d++)
						{
							double current_dist = distDT_2D;
							current_dist *= ratio;
							
							for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
							{
								double dist = 0;

								{
									double diff = std::abs((p.pos()[d]-arg_pts[j].pos()[d]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								{
									double diff = std::abs((p.pos()[3]-arg_pts[j].pos()[3]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								
								dist = sqrt(dist);
								
								if ( dist < current_dist )
									accept = false;
							}
						}
					}
					
					if(D==2)
						continue;
					
					//test 3D projections
					//If D=3: 012
					//If D=4: 012, 023, 013, 123
					
					if(D>=3)
					{
						double current_dist = distDT_3D;
						current_dist *= ratio;
						
						for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
						{
							double dist = 0;
							for(uint dtmp=0; dtmp<3; dtmp++)
							{
								double diff = std::abs((p.pos()[dtmp]-arg_pts[j].pos()[dtmp]));
								double toric_diff = std::min( diff, 1-diff );
								dist += toric_diff*toric_diff;
							}
							dist = sqrt(dist);
							
							if ( dist < current_dist )
								accept = false;
						}
					}
					if(D==4)
					{
						for(int d=0; d<3; d++)
						{
							double current_dist = distDT_3D;
							current_dist *= ratio;
							
							for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
							{
								double dist = 0;
								{
									double diff = std::abs((p.pos()[0]-arg_pts[j].pos()[0]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								{
									double diff = std::abs((p.pos()[1]-arg_pts[j].pos()[1]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								{
									double diff = std::abs((p.pos()[3]-arg_pts[j].pos()[3]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								dist = sqrt(dist);
								
								if ( dist < current_dist )
									accept = false;
							}
							for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
							{
								double dist = 0;
								{
									double diff = std::abs((p.pos()[0]-arg_pts[j].pos()[0]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								{
									double diff = std::abs((p.pos()[2]-arg_pts[j].pos()[2]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								{
									double diff = std::abs((p.pos()[3]-arg_pts[j].pos()[3]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								dist = sqrt(dist);
								
								if ( dist < current_dist )
									accept = false;
							}
							for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
							{
								double dist = 0;
								{
									double diff = std::abs((p.pos()[2]-arg_pts[j].pos()[2]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								{
									double diff = std::abs((p.pos()[1]-arg_pts[j].pos()[1]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								{
									double diff = std::abs((p.pos()[3]-arg_pts[j].pos()[3]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								dist = sqrt(dist);
								
								if ( dist < current_dist )
									accept = false;
							}
						}
	
					}
					
					if(D==3)
						continue;

					//test 4D projection
					//If D=4: 0123
					if(D==4)
					{
						double current_dist = distDT_4D;
						current_dist *= ratio;

						for(uint j=0; (j<arg_pts.size()) & (accept); ++j)
						{
							double dist = 0;
							for(uint dtmp=0; dtmp<D; dtmp++)
							{
								double diff = std::abs((p.pos()[dtmp]-arg_pts[j].pos()[dtmp]));
								double toric_diff = std::min( diff, 1-diff );
								dist += toric_diff*toric_diff;
							}
							dist = sqrt(dist);
							
							if ( dist < current_dist )
								accept = false;
						}
					}
				}
				
				arg_pts.push_back(p);
				/*std::cout << "######################################### Insert" << std::endl;
				std::cout << "Pointset" << std::endl;
				for(uint j=0; j<arg_pts.size(); ++j)
					std::cout << arg_pts[j] << std::endl;
				std::cout << "######################################### Insert" << std::endl;*/
			}

			return true;
		};

	protected:
		std::mt19937 m_mersenneTwister;
		double getRandom01()
		{
			return (double)m_mersenneTwister() / (double)m_mersenneTwister.max();
		}

	};

} //end namespace utk


#endif
