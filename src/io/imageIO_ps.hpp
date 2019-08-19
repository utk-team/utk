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
#ifndef _UTK_IMAGEIO_PS_HPP_
#define _UTK_IMAGEIO_PS_HPP_

#include <fstream>
#include <cassert>
#include <sstream>

#include "imageIO_model.hpp"
#include "../pointsets/Pointset.hpp"

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetIllustrator_PS : public PointsetIllustrator_Model<D, T, P>
{
public:
	virtual ~PointsetIllustrator_PS() {}
	
	Point<D, double> scale(const P& arg_pt, Domain<P> arg_domain)
	{
	  double min_domain_x = arg_domain.pMin.pos()[this->m_dimension_0];
	  double max_domain_x = arg_domain.pMax.pos()[this->m_dimension_0];
	  double min_domain_y = arg_domain.pMin.pos()[this->m_dimension_1];
	  double max_domain_y = arg_domain.pMax.pos()[this->m_dimension_1];
	  Point<D, double> scalept;
	  scalept.pos()[this->m_dimension_0] = arg_pt.pos()[this->m_dimension_0];
	  scalept.pos()[this->m_dimension_1] = arg_pt.pos()[this->m_dimension_1];
	  scalept.pos()[this->m_dimension_0] -= min_domain_x;
	  scalept.pos()[this->m_dimension_1] -= min_domain_y;
	  scalept.pos()[this->m_dimension_0] /= (max_domain_x - min_domain_x);
	  scalept.pos()[this->m_dimension_1] /= (max_domain_y - min_domain_y);
	  scalept.pos()[this->m_dimension_0] *= this->m_boundingbox_x-(2*(this->m_bordersize*this->m_boundingbox_x));
	  scalept.pos()[this->m_dimension_1] *= this->m_boundingbox_y-(2*(this->m_bordersize*this->m_boundingbox_x));
	  scalept.pos()[this->m_dimension_0] += (this->m_bordersize*this->m_boundingbox_x);
	  scalept.pos()[this->m_dimension_1] += (this->m_bordersize*this->m_boundingbox_x);
	  return scalept;
	}

	virtual bool open(const char* arg_filename)
	{
		this->m_file.open(arg_filename, std::ios::out);
		this->m_file << "%!PS-Adobe-3.0 " << std::endl;
		this->m_file << "%%Creator: [UTK]" << std::endl;
		this->m_file << "%%Origin: [eg: 0 0]" << std::endl;
		if(this->m_tiled)
		{
			this->m_file << "%%BoundingBox: " << -this->m_boundingbox_x << " " << -this->m_boundingbox_y << " " << 2*this->m_boundingbox_x << " " << 2*this->m_boundingbox_y << std::endl;
			this->m_file << "newpath" << std::endl;
			this->m_file << -this->m_boundingbox_x << " " << -this->m_boundingbox_y << " moveto" << std::endl;
			this->m_file << 0 << " " << 3*this->m_boundingbox_y << " rlineto" << std::endl;
			this->m_file << 3*this->m_boundingbox_x << " " << 0 << " rlineto" << std::endl;
			this->m_file << 0 << " " << -3 * (int)this->m_boundingbox_y << " rlineto" << std::endl;
			this->m_file << -3 * (int)this->m_boundingbox_x << " " << 0 << " rlineto" << std::endl;
			this->m_file << "closepath" << std::endl;
			this->m_file << "1 1 1 setrgbcolor" << std::endl;
			this->m_file << "gsave\n\
			fill\n\
			grestore\n\
			closepath\n\
			stroke\n";

			this->m_file << "newpath" << std::endl;
			this->m_file << this->m_boundingbox_x << " " << this->m_boundingbox_y << " moveto" << std::endl;
			this->m_file << 0 << " " << -this->m_boundingbox_y << " rlineto" << std::endl;
			this->m_file << -this->m_boundingbox_x << " " << 0 << " rlineto" << std::endl;
			this->m_file << 0 << " " << this->m_boundingbox_y << " rlineto" << std::endl;
			this->m_file << this->m_boundingbox_x << " " << 0 << " rlineto" << std::endl;
			this->m_file << "closepath" << std::endl;
			this->m_file << "0 0 0 setrgbcolor" << std::endl;
			this->m_file << 0.001*this->m_boundingbox_x << " setlinewidth" << std::endl;
			this->m_file << "gsave\n\
			grestore\n\
			closepath\n\
			stroke\n";

		}
		else
		{
			this->m_file << "%%BoundingBox: " << 0 << " " << 0 << " " << this->m_boundingbox_x << " " << this->m_boundingbox_y << std::endl;
			this->m_file << "newpath" << std::endl;
			this->m_file << "0 0 moveto" << std::endl;
			this->m_file << "0 " << this->m_boundingbox_y << " rlineto" << std::endl;
			this->m_file << this->m_boundingbox_x << " 0 rlineto" << std::endl;
			this->m_file << "0 " << -1 * (int)this->m_boundingbox_y << " rlineto" << std::endl;
			this->m_file << -1 * (int)this->m_boundingbox_x << " 0 rlineto" << std::endl;
			this->m_file << "closepath" << std::endl;
			this->m_file << "1 1 1 setrgbcolor" << std::endl;
			this->m_file << "gsave\n\
			fill\n\
			grestore\n\
			closepath\n\
			stroke\n";
		}


		return true;
	}

	virtual bool drawGrid(int arg_grid_resolution, float linewidth, bool dashed, std::string color)
	{
		for(int i=1; i<arg_grid_resolution; i++)
		{
			double xcoord = i*(float)((float)this->m_boundingbox_x/(float)arg_grid_resolution);
			double ycoord = i*(float)((float)this->m_boundingbox_y/(float)arg_grid_resolution);

			//std::cout << i << "*(" << this->m_boundingbox_x << "/" << arg_grid_resolution << ") = " << xcoord << std::endl;

			this->m_file << "newpath" << std::endl;
			this->m_file << xcoord << " 0 moveto" << std::endl;
			this->m_file << 0 << " " << this->m_boundingbox_y << " rlineto" << std::endl;
			this->m_file << "closepath" << std::endl;
			this->m_file << color << " setrgbcolor" << std::endl;
			this->m_file << linewidth << " setlinewidth" << std::endl;
			if(dashed)
				this->m_file << "[3 3] 0 setdash" << std::endl;
			else
				this->m_file << "[3 0] 0 setdash" << std::endl;
			this->m_file << "gsave\n\
			grestore\n\
			closepath\n\
			stroke\n";

			this->m_file << "newpath" << std::endl;
			this->m_file << "0 " << ycoord << " moveto" << std::endl;
			this->m_file << this->m_boundingbox_x << " " << 0 << " rlineto" << std::endl;
			this->m_file << "closepath" << std::endl;
			this->m_file << color << " setrgbcolor" << std::endl;
			this->m_file << linewidth << " setlinewidth" << std::endl;
			if(dashed)
				this->m_file << "[3 3] 0 setdash" << std::endl;
			else
				this->m_file << "[3 0] 0 setdash" << std::endl;
			this->m_file << "gsave\n\
			grestore\n\
			closepath\n\
			stroke\n";
		}
		return true;
	}
	
	virtual bool drawRectangle(float x0, float y0, float sx, float sy, float linewidth)
	{
		double xcoord0 = x0*this->m_boundingbox_x;
		double ycoord0 = y0*this->m_boundingbox_y;
		double sxscale = sx*this->m_boundingbox_x;
		double syscale = sy*this->m_boundingbox_y;

		this->m_file << "newpath" << std::endl;
		this->m_file << linewidth*((this->m_boundingbox_x+this->m_boundingbox_y)/2) << " setlinewidth" << std::endl;
		this->m_file << this->m_color[0] << " " << this->m_color[1] << " " << this->m_color[2] << " setrgbcolor" << std::endl;
		this->m_file << xcoord0 << " " << ycoord0 << " moveto" << std::endl;
		this->m_file << "0 " << sxscale << " rlineto" << std::endl;
		this->m_file << syscale << " 0 rlineto" << std::endl;
		this->m_file << "0 " << -sxscale << " rlineto" << std::endl;
		this->m_file << -syscale << " 0 rlineto" << std::endl;
		this->m_file << "closepath" << std::endl;
		this->m_file << "stroke" << std::endl;
		
		/*this->m_file << alpha << " setopacityalpha" << std::endl;
		this->m_file << "fill" << std::endl;
		this->m_file << 1 << " setopacityalpha" << std::endl;*/
		
		return true;
	}
	
	virtual bool drawCircle(float x0, float y0, float r0, float linewidth)
	{
		double xcoord0 = x0*this->m_boundingbox_x;
		double ycoord0 = y0*this->m_boundingbox_y;
		double rscale = r0*this->m_boundingbox_x;
			
		this->m_file << "newpath" << std::endl;
		this->m_file << linewidth*((this->m_boundingbox_x+this->m_boundingbox_y)/2) << " setlinewidth" << std::endl;
		this->m_file << this->m_color[0] << " " << this->m_color[1] << " " << this->m_color[2] << " setrgbcolor" << std::endl;
		this->m_file << xcoord0 << " " << ycoord0 << " " << rscale << " 0 360 arc\n";
		this->m_file << "closepath" << std::endl;
		this->m_file << "stroke" << std::endl;
        
        return true;
	}

	void drawNumber(const Point<D, double>& arg_disk, unsigned int number)
	{
		this->m_file << "1 0 0 setrgbcolor" << std::endl;
		this->m_file << "/Times-Roman findfont" << std::endl;
		this->m_file << this->m_fontsize*2 << " scalefont" << std::endl;
		this->m_file << "setfont" << std::endl;
		this->m_file << "newpath" << std::endl;
		this->m_file << (arg_disk.pos()[this->m_dimension_0]+this->m_radius*this->m_boundingbox_x) << " " << (arg_disk.pos()[this->m_dimension_1]+this->m_radius*this->m_boundingbox_x) << " moveto" << std::endl;
		this->m_file << "(" << number << ") show" << std::endl;
	}

	virtual bool drawPoint(const P& arg_disk)
	{
		std::string it;

		std::ostringstream oss;
		it += "newpath\n";
		it += "[3 0] 0 setdash\n";
		it += "1 setlinewidth\n";

		oss << this->m_color[0];
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");
		oss << this->m_color[1];
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");
		oss << this->m_color[2];
		it += oss.str();
		it += " setrgbcolor\n";
		oss.clear();
		oss.str("");

		/**Move to the first point of the disk*/
		oss << std::fixed << std::setprecision(10) << arg_disk.pos()[this->m_dimension_0];
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " ";
		oss << arg_disk.pos()[this->m_dimension_1];
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " moveto\n";

		oss << arg_disk.pos()[this->m_dimension_0];
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		oss << arg_disk.pos()[this->m_dimension_1];
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		oss << (this->m_radius)*(this->m_boundingbox_x);
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		it += "0 360 arc\n";

		it += "gsave\n\
		fill\n\
		stroke\n";

		this->m_file << it;

		return true;
	}

	virtual bool drawPointset(const Pointset<D, T, P>& arg_pointset)
	{
		if(this->m_dimension_0 >= D || this->m_dimension_1 >= D)
		{
		  std::stringstream sdim;
		  sdim << D;
		  
		  std::stringstream sproj0;
		  sproj0 << this->m_dimension_0;
		  
		  std::stringstream sproj1;
		  sproj1 << this->m_dimension_1;
		  
		  ERROR("PointsetIllustrator_Model::setProjection Can't draw the wanted projection (" + sproj0.str() + "," + sproj1.str() + ") as the pointset is only " + sdim.str() + "D");
		  return false;
		}
	  
		if( ((arg_pointset.domain.pMax.pos() - arg_pointset.domain.pMin.pos())[this->m_dimension_0] == 0) ||
		    ((arg_pointset.domain.pMax.pos() - arg_pointset.domain.pMin.pos())[this->m_dimension_1] == 0) )
			  WARNING("PointsetIllustrator_PS::drawPointset pointset domain extend is 0 on at least one dimension. Scaling to drawing domain might fail");
		
			  
		std::array<float, 3> bu_color = this->m_color;
		if(this->m_tiled)
		{
			for(int ix=-1; ix<=1; ix++)
			for(int iy=-1; iy<=1; iy++)
			{
				if(ix == iy && ix == 0)
					this->m_color = bu_color;
				else if(ix == -1 && iy == -1)
				{
					//TODO passer au systeme HSV pour eclaircir les points
					this->m_color[0] = 0.5;
					this->m_color[1] = 0.75;
					this->m_color[2] = 0.75;
				}
				else if(ix == -1 && iy == 0)
				{
					//TODO passer au systeme HSV pour eclaircir les points
					this->m_color[0] = 0.75;
					this->m_color[1] = 0.5;
					this->m_color[2] = 0.75;
				}
				else if(ix == -1 && iy == 1)
				{
					//TODO passer au systeme HSV pour eclaircir les points
					this->m_color[0] = 0.75;
					this->m_color[1] = 0.75;
					this->m_color[2] = 0.5;
				}
				else if(ix == 0 && iy == -1)
				{
					//TODO passer au systeme HSV pour eclaircir les points
					this->m_color[0] = 0.5;
					this->m_color[1] = 0.5;
					this->m_color[2] = 0.75;
				}
				else if(ix == 0 && iy == 1)
				{
					//TODO passer au systeme HSV pour eclaircir les points
					this->m_color[0] = 0.5;
					this->m_color[1] = 0.75;
					this->m_color[2] = 0.5;
				}
				else if(ix == 1 && iy == -1)
				{
					//TODO passer au systeme HSV pour eclaircir les points
					this->m_color[0] = 0.75;
					this->m_color[1] = 0.5;
					this->m_color[2] = 0.5;
				}
				else if(ix == 1 && iy == 0)
				{
					//TODO passer au systeme HSV pour eclaircir les points
					this->m_color[0] = 0.5;
					this->m_color[1] = 0.5;
					this->m_color[2] = 0.5;
				}
				else if(ix == 1 && iy == 1)
				{
					//TODO passer au systeme HSV pour eclaircir les points
					this->m_color[0] = 0.75;
					this->m_color[1] = 0.75;
					this->m_color[2] = 0.75;
				}
				
				
				for(int i=0; i<arg_pointset.size(); i++)
				{
					Point<D, double> pt = scale(arg_pointset[i], arg_pointset.domain);
					pt.pos()[this->m_dimension_0] += ix*this->m_boundingbox_x;
					pt.pos()[this->m_dimension_1] += iy*this->m_boundingbox_y;
					drawScaledPoint(pt);
				}
			}
		}
		else
		{
			for(int i=0; i<arg_pointset.size(); i++)
			{
				Point<D, double> pt = scale(arg_pointset[i], arg_pointset.domain);
				drawScaledPoint(pt);
			}
		}

		if(this->m_numbered)
		{
			//std::cout << "Tutu" << std::endl;
			for(int i=0; i<arg_pointset.size(); i++)
			{
			    Point<D, double> pt = scale(arg_pointset[i], arg_pointset.domain);
			    drawNumber(pt, i);
			}
		}

		return true;
	}

	virtual void close()
	{
		this->m_file << "%EOF"  << std::endl;
		this->m_file.close();
	}

protected:
	
	bool drawScaledPoint(const Point<D, double>& arg_disk)
	{
		std::string it;

		std::ostringstream oss;
		it += "newpath\n";
		it += "[3 0] 0 setdash\n";
		it += "1 setlinewidth\n";

		oss << this->m_color[0];
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");
		oss << this->m_color[1];
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");
		oss << this->m_color[2];
		it += oss.str();
		it += " setrgbcolor\n";
		oss.clear();
		oss.str("");

		/**Move to the first point of the disk*/
		oss << std::fixed << std::setprecision(10) << arg_disk.pos()[this->m_dimension_0];
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " ";
		oss << arg_disk.pos()[this->m_dimension_1];
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " moveto\n";

		oss << arg_disk.pos()[this->m_dimension_0];
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		oss << arg_disk.pos()[this->m_dimension_1];
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		oss << (this->m_radius)*(this->m_boundingbox_x);
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		it += "0 360 arc\n";

		it += "gsave\n\
		fill\n\
		stroke\n";

		this->m_file << it;

		return true;
	}

};

};//end namespace utk

#endif
