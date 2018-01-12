#ifndef _UTK_SAMPLER_PBN_
#define _UTK_SAMPLER_PBN_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>
#include <cmath>
#include <flann/flann.hpp>

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
			
			double distDT_1D = 2.0 * 1.0/(2.0*arg_points);
			double distDT_2D = 2.0 * sqrt(1.0/(2.0*sqrt(3)*arg_points));
			double distDT_3D = 2.0 * pow(1.0/(4.0*sqrt(2)*arg_points), 1.0/3.0);
			double distDT_4D = 2.0 * pow(1.0/(8.0*arg_points), 1.0/4.0);
			
			std::cout << "Dists " << std::endl;
			std::cout << "1D " << distDT_1D << std::endl;
			std::cout << "2D " << distDT_2D << std::endl;
			std::cout << "3D " << distDT_3D << std::endl;
			std::cout << "4D " << distDT_4D << std::endl;
			
			//First sample
			P p;
			for(uint d=0; d<D; d++)
				p.pos()[d] = getRandom01();
			arg_pts.push_back(p);

			/*int nn = 1;
			flann::Matrix<int> indices(new int[nn], 1, nn);
			flann::Matrix<double> distances(new double[nn], 1, nn);*/
			
			std::vector< std::vector<int> > indices_;
			std::vector< std::vector<double> > distances_;
			
			//One neighbour structure for each possible projection
			
			std::vector < flann::Matrix<T> > pts;
			
			//1D
			T pt0 = p.pos()[0];
			flann::Matrix<T> pts0(&pt0, 1, 1);
			pts.push_back(pts0);
			flann::Index< flann::L2<T> > index0(pts[0], flann::KDTreeIndexParams(4));
			index0.buildIndex();
			
			/*flann::Matrix<T> pts1(new T[1], 1, 1);
			flann::Index< flann::L2<T> > index1(pts1, flann::KDTreeIndexParams(4));
			index1.buildIndex();
			
			flann::Matrix<T> pts2(new T[1], 1, 1);
			flann::Index< flann::L2<T> > index2(pts2, flann::KDTreeIndexParams(4));
			index2.buildIndex();
			
			flann::Matrix<T> pts3(new T[1], 1, 1);
			flann::Index< flann::L2<T> > index3(pts3, flann::KDTreeIndexParams(4));
			index3.buildIndex();
			
			//2D
			flann::Matrix<T> pts01(new T[2], 1, 2);
			flann::Index< flann::L2<T> > index01(pts01, flann::KDTreeIndexParams(4));
			index01.buildIndex();*/
			
			
			
			for(uint i=index0.size(); i<arg_points; ++i)
			{
				double ratio = 1;
			
				std::cout << i << "/" << arg_points << std::endl;
				
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
					
					/***************************************
					 * 1D Proj
					 **************************************/
					double current_dist = distDT_1D; // / dist_full;
					current_dist *= ratio;
					
					//proj 0
					T pt = p.pos()[0];
					flann::Matrix<T> pmat(&pt, 1, 1);
					int n = index0.radiusSearch(pmat, indices_, distances_, current_dist, flann::SearchParams(128));
					
					std::cout << pt << " vs {";
					for(int itmp=0; itmp<index0.size(); itmp++)
						std::cout << *(index0.getPoint(itmp)) << ",";
					std::cout << "}" << std::endl;
					/*std::cout << n << "NN, Distances " << std::endl;
					for(int itmp=0; itmp<n; itmp++)
					{
						std::cout << indices_[0][itmp] << ", " << distances_[0][itmp] << " ? " << current_dist << std::endl;
						if(distances_[0][itmp] < current_dist)
							accept = false;
					}*/
					
					/*//proj1
					if(D >= 2)
					{
						T pt = p.pos()[1];
						flann::Matrix<T> pmat(&pt, 1, 1);
						index1.knnSearch(pmat, indices, distances, nn, flann::SearchParams(128));
						
						for(int itmp=0; itmp<nn; itmp++)
							if(distances[itmp][0] < current_dist )
								accept = false;
					}
					
					//proj2
					if(D >= 3)
					{
						T pt = p.pos()[2];
						flann::Matrix<T> pmat(&pt, 1, 1);
						index2.knnSearch(pmat, indices, distances, nn, flann::SearchParams(128));
						
						for(int itmp=0; itmp<nn; itmp++)
							if(distances[itmp][0] < current_dist )
								accept = false;
					}
					
					//proj3
					if(D >= 4)
					{
						T pt = p.pos()[3];
						flann::Matrix<T> pmat(&pt, 1, 1);
						index3.knnSearch(pmat, indices, distances, nn, flann::SearchParams(128));
						
						for(int itmp=0; itmp<nn; itmp++)
							if(distances[itmp][0] < current_dist )
								accept = false;
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
						
						T pt[2]; pt[0] = p.pos()[0]; pt[1] = p.pos()[1];
						flann::Matrix<T> pmat(pt, 1, 2);
						index01.knnSearch(pmat, indices, distances, nn, flann::SearchParams(128));
						
						for(int itmp=0; itmp<nn; itmp++)
							if(distances[itmp][0] < current_dist )
								accept = false;
					}
					if(D>=3)
					{
						for(int d=0; d<3; d++)
						{
							double current_dist = distDT_2D / dist_full;
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
									double diff = std::abs((p.pos()[(d+1)%D]-arg_pts[j].pos()[(d+1)%D]));
									double toric_diff = std::min( diff, 1-diff );
									dist += toric_diff*toric_diff;
								}
								
								dist = sqrt(dist);
								
								if ( dist < current_dist )
									accept = false;
							}
						}
					}
					if(D==4)
					{
						for(int d=0; d<3; d++)
						{
							double current_dist = distDT_2D / dist_full;
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
						for(int d=0; d<D; d++)
						{
							double current_dist = distDT_3D / dist_full;
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
					}
					if(D==4)
					{
						for(int d=0; d<3; d++)
						{
							double current_dist = distDT_3D / dist_full;
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
									double diff = std::abs((p.pos()[(d+1)%3]-arg_pts[j].pos()[(d+1)%3]));
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
						double current_dist = distDT_4D / dist_full;
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
					}*/
				}
				
				arg_pts.push_back(p);
			
				//1D
				T pt = p.pos()[0];
				
				std::cout << "Pts " << std::endl;
				for(int itmp=0; itmp<pts.size(); itmp++)
					std::cout << pts[itmp][0] << std::endl;
				
				flann::Matrix<T> pt0(&pt, 1, 1);
				pts.push_back(pt0);
				index0.addPoints(pts[pts.size()-1]);
				index0.buildIndex();
				/*if(D>=2){T pt = p.pos()[1];
				flann::Matrix<T> pmat(&pt, 1, 1);
				index1.addPoints(pmat);}
				if(D>=3){T pt = p.pos()[2];
				flann::Matrix<T> pmat(&pt, 1, 1);
				index2.addPoints(pmat);}
				if(D>=4){T pt = p.pos()[3];
				flann::Matrix<T> pmat(&pt, 1, 1);
				index3.addPoints(pmat);}
				
				if(D>=2){T pt[2];
				pt[0] = p.pos()[0];
				pt[1] = p.pos()[1];
				flann::Matrix<T> pmat(pt, 1, 2);
				index01.addPoints(pmat);}*/
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
