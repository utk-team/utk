/**********************************************
 code from STK https://github.com/stk-team/stk
 * Adrien PILLEBOUE © 2012
 *
 * TODO:
 * - Reintroduce the influence of samples' weights 
 * - rework zoom and resolution parameters to better fit what we need
 *********************************************/

#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../pointsets/Pointset.hpp"
#include "../../io/fileIO.hpp"

#include "../../../externals/CImg-1.6.9/CImg.h"
using namespace cimg_library;

class ZonePlate
{
	private:
		CImg<double> m_data;
		CImg<double> m_dataCount;
		
		int m_res;
		double m_zoom;
		double m_density;
	
	private:	
		double zoneplate_func(const utk::Vector<2, double>& i_pos)
		{
			//Adrien Pilleboue's formula
			double zoom = 60./512.;
			return 0.5 + 0.5 * cos((i_pos[0]*zoom * i_pos[0]*zoom + i_pos[1]*zoom * i_pos[1]*zoom));
			//return 0.5 + 0.5 * sin((i_pos[0]*zoom * i_pos[0]*zoom + i_pos[1]*zoom * i_pos[1]*zoom));
			
			//my own factor
			/*double zoom = 42./512.;
			return 0.5 + 0.5 * sin((i_pos.x()*zoom * i_pos.x()*zoom + i_pos.y()*zoom * i_pos.y()*zoom));*/
			
			//the function used by abdallah in konstanz
			/*double zoom = 1./512.;
			double factor = 512*3.14159;
			return 0.5 + 0.5 * sin(factor*(i_pos.x()*zoom * i_pos.x()*zoom + i_pos.y()*zoom * i_pos.y()*zoom));*/
			
			
		}

		double mitchell_filter_1D(const double i_x)
		{
			const double x = fabs(i_x);

			if (x < 0.5)
				return (21.0 * x - 18.0) * x * x + 2.0;
			else if (x < 1.0)
				return ((-7.0 * x + 18.0) * x - 15.0) * x + 4.0;
			else
				return 0.0;
		}

		double mitchell_filter(const utk::Vector<2, double>& i_pt)
		{
			return mitchell_filter_1D(i_pt[0]) * mitchell_filter_1D(i_pt[1]);
		}
		
		double gaussian_filter(const utk::Vector<2, double>& i_pt)
		{
			double sigma = 0.2;
			double a = sigma*sqrt(2*M_PI);
			a = 1.0/a;
			
			double x_factor = (i_pt[0]*i_pt[0]) / (2*sigma*sigma);
			double y_factor = (i_pt[1]*i_pt[1]) / (2*sigma*sigma);
			
			double f_x = a*exp(-(x_factor+y_factor));
			
			return f_x;
		}

		void apply_mitchell(const utk::Vector<2, double>& i_pt)
		{
			//std::cout << i_pt[0] << "," << i_pt[1] << std::endl;
			const int mRadius = 2;
			int ix, iy;
			double mFact, val;
			utk::Vector<2, double> mDist;

			for(int j=-mRadius; j<=mRadius; j++)
			{
				for(int i=-mRadius; i<=mRadius; i++)
				{
					ix = (int)round(i_pt[0])+i;
					iy = (int)round(i_pt[1])+j;
					mDist = (utk::Vector<2, double>(0.5 + ix, 0.5 + iy) - i_pt) / mRadius;
					//std::cout << mDist[0] << "," << mDist[1] << std::endl;
					if(ix >= 0 && ix < m_res && iy >= 0 && iy < m_res)
					{
						mFact = mitchell_filter(mDist);
						val = zoneplate_func(i_pt*m_zoom)*mFact;
						
						//if(ix == 0 && iy == 0)
						//	std::cout << ix << "-" << iy << "   " << mFact << "," << val << std::endl;
						//exit(-1);
						
						m_data.atXY(ix, iy) += val;
						m_dataCount.atXY(ix, iy) += mFact;
					}
				}
			}
		}
		
		void apply_box(const utk::Vector<2, double>& i_pt)
		{
			const int mRadius = 0;
			int ix, iy;
			double val;
			utk::Vector<2, double> mDist;

			for(int j=-mRadius; j<=mRadius; j++)
			{
				for(int i=-mRadius; i<=mRadius; i++)
				{
					ix = (int)round(i_pt[0])+i;
					iy = (int)round(i_pt[1])+j;
					mDist = (utk::Vector<2, double>(0.5 + ix, 0.5 + iy) - i_pt) / mRadius;
					if(ix >= 0 && ix < m_res && iy >= 0 && iy < m_res)
					{
						//mFact = mitchell_filter(mDist);
						val = zoneplate_func(i_pt*m_zoom);
						
						m_data.atXY(ix, iy) += val;
						m_dataCount.atXY(ix, iy) += 1;
					}
				}
			}
		}
		
		void apply_gaussian(const utk::Vector<2, double>& i_pt)
		{
			const int mRadius = 3;
			int ix, iy;
			double mFact, val;
			utk::Vector<2, double> mDist;

			for(int j=-mRadius; j<=mRadius; j++)
			{
				for(int i=-mRadius; i<=mRadius; i++)
				{
					ix = (int)round(i_pt[0])+i;
					iy = (int)round(i_pt[1])+j;
					mDist = (utk::Vector<2, double>(0.5 + ix, 0.5 + iy) - i_pt) / mRadius;
					if(ix >= 0 && ix < m_res && iy >= 0 && iy < m_res)
					{
						mFact = gaussian_filter(mDist);
						val = zoneplate_func(i_pt*m_zoom)*mFact;
						
						m_data.atXY(ix, iy) += val;
						m_dataCount.atXY(ix, iy) += mFact;
					}
				}
			}
		}

	public:
		ZonePlate(int i_res, double i_zoomFact, double i_density) :
			m_data(i_res, i_res, 1, 1),
			m_dataCount(i_res, i_res, 1, 1),
			m_res(i_res),
			m_density(i_density)
		{
			m_zoom = i_zoomFact;// * 60./512.;
		}
		
		~ZonePlate()
		{
			
		}
		
		void make(const utk::Pointset<2, double, utk::Point<2, double> >& i_pts)
		{
			m_data.fill(0.0);
			m_dataCount.fill(0.0);
			
			double scale = sqrt(i_pts.size())/m_density;
			int rep = ceil(m_res/scale);
			
			//std::cout << "scale " << scale << std::endl;
			//std::cout << "rep " << rep << std::endl;
			
			utk::Vector<2, double> pt, ptShift;
			for(uint i=0; i<i_pts.size(); i++)
			{
				pt = i_pts[i].pos();

				for(int ry=0; ry<rep; ry++)
				{
					for(int rx=0; rx<rep; rx++)
					{
						ptShift = pt + utk::Vector<2, double>(rx*scale, ry*scale);
						//std::cout << "pt " << pt[0] << ", " << pt[1] << std::endl;
						//std::cout << "ptShift " << ptShift[0] << ", " << ptShift[1] << std::endl;
						apply_mitchell(ptShift);
						//exit(-1);
						//apply_box(ptShift);
						//apply_gaussian(ptShift);
					}
				}
			}
			
			//std::cout << m_dataCount.getData(stk::Vector2i(0, 0)) << std::endl;
			
			double tmp0;
			double tmp1;
			for(int j=0; j<m_res/2; j++)
			{
				for(int i=0; i<m_res; i++)
				{
					int count0 = m_dataCount.atXY(i, j);
					int count1 = m_dataCount.atXY(i, m_res-j-1);
					
					//std::cout << count0 << "," << count1 << std::endl;
					//exit(-1);
					
					if(count0 && count1)
					{
						tmp0  = m_data.atXY(i, j);
						tmp0 /= count0;
						
						tmp1  = m_data.atXY(i, m_res-j-1);
						tmp1 /= count1;
						
						//std::cout << tmp0 << ":" << tmp1 << std::endl;
						//exit(-1);
						
						m_data.atXY(i, m_res-j-1) = tmp0;
						m_data.atXY(i, j) = tmp1;
					}
				}
			}
		}
		
		void writePng(std::string output) const
		{
			CImg<unsigned char> m_img(m_data.width(), m_data.height(), 1, 1);
			
			for(int j=0; j<m_res; j++)
			for(int i=0; i<m_res; i++)
			{
				double color = m_data.atXY(i, j);
				if(color < 0.) color = 0.;
				else if(color > 1.) color = 1.;
				
				m_img.atXY(i,j) = color*255;
				//m_img.atXYZC(i,j, 0, 1) = color*255;
				//m_img.atXYZC(i,j, 0, 2) = color*255;
				//if(m_img.atXY(i,j) > 255) m_img.atXY(i,j)=255;
				//if(m_img.atXY(i,j) < 0) m_img.atXY(i,j)=0;
			}
			
			m_img.save_png(output.c_str());
		}
};


int main(int argc, char** argv)
{
	srand48(time(NULL));
	
	/* ARG PARSER *****************************************************/
	std::string fn_input;
	std::string fn_output;
	std::string fn_weight;
	double density=4;
	double zoom=1;
	int res=512;
	bool help = false;
	
	utk::ParamParser_getopt parser;
	parser.addShortOption('i', &fn_input, 1, utk::assignString, utk::displayString, (char*)"\t The input pointset", (char*)"Input: ");
	parser.addShortOption('o', &fn_output, 1, utk::assignString, utk::displayString, (char*)"\t The output zoneplate", (char*)"Ouput: ");
	parser.addShortOption('r', &res, 1, utk::assignInt, utk::displayInt, (char*)"\t The image resolution", (char*)"Resolution: ");
	parser.addShortOption('h', &help, 0, utk::assignBoolTrue, NULL, (char*)"\t Displays this help message");
	parser.addShortOption('z', &zoom, 1, utk::assignDouble, utk::displayDouble, (char*)"\t Zoom factor", (char*)"Zoom: ");
	parser.addShortOption('p', &density, 1, utk::assignDouble, utk::displayDouble, (char*)"\t Nb samples per pixel", (char*)"Spp: ");
	parser.parse(argc, argv);
	
	if(fn_input.empty())
	{
		ERROR("Parameter -i mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	if(help)
	{
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}

	density = sqrt(density);
	
	/* INIT ***********************************************************/
	utk::Pointset<2, double, utk::Point<2, double> > pts;
	utk::PointsetReader<2, double, utk::Point<2, double> > pts_reader;
	pts_reader.open(fn_input);
	pts_reader.readPointset(pts);
	pts_reader.close();
	
	double scale = sqrt(pts.size())/density;
	std::cout << "Scaling " << scale << std::endl;
	//int rep = ceil(res/scale);
	
	for(uint i=0; i<pts.size(); i++)
	{
		pts[i].pos() *= scale;
	}
	
	/* TEST ***********************************************************/
	ZonePlate zp(res, 1/zoom, density);
	zp.make(pts);
	zp.writePng(fn_output);
	
	exit(EXIT_SUCCESS);
}
