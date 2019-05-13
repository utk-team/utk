#ifndef _UTK_IMAGEIO_TIKZ_HPP_
#define _UTK_IMAGEIO_TIKZ_HPP_

#include <fstream>
#include <cassert>
#include <sstream>

#include "imageIO_model.hpp"
#include "../pointsets/Pointset.hpp"

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetIllustrator_TIKZ : public PointsetIllustrator_Model<D, T, P>
{
public:
	virtual ~PointsetIllustrator_TIKZ() {}

	virtual bool open(const char* arg_filename)
	{
		this->m_file.open(arg_filename, std::ios::out);
		this->m_file << "\\documentclass{article}" << std::endl;
		this->m_file << "\\usepackage[margin=0cm, paperheight=20cm, paperwidth=20cm]{geometry}" << std::endl;
		this->m_file << "\\usepackage[utf8]{inputenc}" << std::endl; 
		this->m_file << "\\usepackage{tikz} \\usepackage{pgfplots}" << std::endl;
		this->m_file << "\\usepackage{nopageno} \\begin{document}" << std::endl;
		this->m_file << "\\noindent \\begin{figure} \\begin{tikzpicture}[scale=0.0345\\textwidth]" << std::endl;
		this->m_file << "\\draw[line width= 1, draw=white](0,0) rectangle (1,1);" << std::endl;
		return true;
	}

	virtual bool drawGrid(int arg_grid_resolution, float linewidth, bool dashed, std::string color)
	{
		return true;
	}
	
	virtual bool drawRectangle(float x0, float y0, float sx, float sy, float linewidth)
	{
		double xcoord0 = x0;
		double ycoord0 = y0;
		double sxscale = sx;

		this->m_file << "\\draw[line width=" << linewidth*((this->m_boundingbox_x+this->m_boundingbox_y)/8) << ", draw=" << getColorStr() << "]";
		this->m_file << " (" << xcoord0 << "," << ycoord0 << ") rectangle (" << xcoord0+sxscale << "," << ycoord0+sxscale << ");" << std::endl;
		
		return true;
	}
	
	virtual bool drawCircle(float x0, float y0, float r0, float linewidth)
	{
        this->m_file << "\\draw[draw=red" << getColorStr() << ", line width=" << linewidth*((this->m_boundingbox_x+this->m_boundingbox_y)/4) << "] (" << x0 << "," << y0 << ") circle (" << r0*30 << "pt);" << std::endl;
		return true;
	}

	void drawNumber(const Point<D, double>& arg_disk, unsigned int number)
	{
		if(this->m_tiled)
			this->m_file << "\\node at (" << scale(arg_disk.pos()[this->m_dimension_0]*third+third+2*this->m_radius) << "," << scale(arg_disk.pos()[this->m_dimension_1]*third+third+2*this->m_radius) << ") { \\textcolor{red}{" << number << "}};" << std::endl;
		else
			this->m_file << "\\node at (" << scale(arg_disk.pos()[this->m_dimension_0]+2*this->m_radius) << "," << scale(arg_disk.pos()[this->m_dimension_1]+2*this->m_radius) << ") { \\textcolor{red}{" << number << "}};" << std::endl;
		
	}

	virtual bool drawPoint(const P& arg_disk)
	{
		this->m_file << "\\fill[fill=" << getColorStr() << "] (" << scale(arg_disk.pos()[this->m_dimension_0]) << "," << scale(arg_disk.pos()[this->m_dimension_1]) << ") circle (" << this->m_radius*30 << "pt);" << std::endl;
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
			this->m_radius /= 3;
			this->m_color[0] = 0;
			this->m_color[1] = 0;
			this->m_color[2] = 0;
			drawRectangle(third, third, third, third, 0.0025);
			
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
					Point<D, double> pt;
					pt.pos()[this->m_dimension_0] = (double)arg_pointset[i].pos()[this->m_dimension_0] * third;
					pt.pos()[this->m_dimension_1] = (double)arg_pointset[i].pos()[this->m_dimension_1] * third;
					pt.pos()[this->m_dimension_0] += (ix+1)*third;
					pt.pos()[this->m_dimension_1] += (iy+1)*third;
					drawPoint(pt);
				}
			}
		}
		else
		{
			for(int i=0; i<arg_pointset.size(); i++)
			{
				drawPoint(arg_pointset[i]);
			}
		}

		if(this->m_numbered)
		{
			if(this->m_tiled)
				this->m_file << "\\fontsize{" << this->m_fontsize*0.5 << "}{" << this->m_fontsize*0.5 << "}" << std::endl;
			else
				this->m_file << "\\fontsize{" << this->m_fontsize << "}{" << this->m_fontsize << "}" << std::endl;
			for(int i=0; i<arg_pointset.size(); i++)
			    drawNumber(arg_pointset[i], i);
		}
		return true;
	}

	virtual void close()
	{
		this->m_file << "\\end{tikzpicture} \\end{figure} \\end{document}";
		this->m_file.close();
	}

protected:
	
	double third = 0.3333333333;
	
	double scale(double coord)
	{
		return coord*(1-2*this->m_bordersize)+this->m_bordersize;
	}
	
	std::string getColorStr()
	{
		std::stringstream ss;
		ss << "{rgb,255:red," << this->m_color[0]*255 << "; green," << this->m_color[1]*255 << "; blue," << this->m_color[2]*255 << "}";
		return ss.str();
	}

};

};//end namespace utk

#endif
