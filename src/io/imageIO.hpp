#ifndef _UTK_IMAGEIO_HPP_
#define _UTK_IMAGEIO_HPP_

#include "imageIO_ps.hpp"
#include "imageIO_tikz.hpp"
#include "../pointsets/Histogram.hpp"

#include <iostream>
#include <cassert>

namespace utk
{
	
static inline std::string getExtImageIO(std::string f, std::string default_value)
{
	size_t pos_ext = f.find_last_of('.');
	if (pos_ext == std::string::npos)
		return default_value;
	
	return f.substr(pos_ext);
}

template <unsigned int D, typename T, typename P>
class PointsetIllustrator
{
public:
	PointsetIllustrator() 
	{ 
		m_drawer = NULL; 
		m_color[0] = 0;
		m_color[1] = 0;
		m_color[2] = 0;
		m_radius = 0.0025;
		m_boundingbox_x = 1024;
		m_boundingbox_y = 1024;
		m_numbered = 0;
		m_tiled = 0;
		m_dimension_0 = 0;
		m_dimension_1 = 1;
		m_fontsize = 10;
		m_bordersize=0;
	}
	~PointsetIllustrator() { if(m_drawer!=NULL) delete m_drawer; }

	bool open(const std::string arg_filename)
	{
	    if(D == 1)
		{
		  ERROR("PointsetIllustrator::open Can only draw 2D projections of pointsets. Current pointset is only 1D");
		  return false;
		}
		  
		assert(!m_drawer);
		std::string ext = getExtImageIO(arg_filename, ".ps");
		
		if(ext == ".ps")
			m_drawer = new PointsetIllustrator_PS<D, T, P>;
		else if(ext == ".tex")
			m_drawer = new PointsetIllustrator_TIKZ<D, T, P>;
		else
		{
			ERROR("PointsetIllustrator::open unknown file format "+ext);
			return false;
		}
		
		m_drawer->setColor(m_color[0], m_color[1], m_color[2]);
		m_drawer->setPointRadius(m_radius);
		m_drawer->setBorderSize(m_bordersize);
		m_drawer->setFontSize(m_fontsize);
		m_drawer->setNumbered(m_numbered);
		m_drawer->setBoundingBoxX(m_boundingbox_x);
		m_drawer->setBoundingBoxY(m_boundingbox_y);
		m_drawer->setTiled(m_tiled);
		m_drawer->setProjection(m_dimension_0, m_dimension_1);
		
		return m_drawer->open(arg_filename.c_str());
	}
	
	void setColor(float arg_r, float arg_g, float arg_b) 
	{
		m_color[0] = arg_r;
		m_color[1] = arg_g;
		m_color[2] = arg_b;
		if(m_drawer)
			m_drawer->setColor(m_color[0], m_color[1], m_color[2]);
	}
	void setPointRadius(float arg_radius) { m_radius = arg_radius; if(m_drawer) m_drawer->setPointRadius(m_radius); }
	void setBorderSize(double arg_bordersize) { m_bordersize = arg_bordersize; if(m_drawer) m_drawer->setBorderSize(m_bordersize); }
	void setFontSize(int arg_fontsize) { m_fontsize = arg_fontsize; if(m_drawer) m_drawer->setFontSize(m_fontsize); }
	void setNumbered(bool arg_numbered) { m_numbered = arg_numbered; if(m_drawer) m_drawer->setNumbered(m_numbered); }
	void setBoundingBox(int arg_boundingbox) {
		m_boundingbox_x = arg_boundingbox; if(m_drawer) m_drawer->setBoundingBoxX(arg_boundingbox); 
		m_boundingbox_y = arg_boundingbox; if(m_drawer) m_drawer->setBoundingBoxY(arg_boundingbox);
	}
	void setBoundingBoxX(int arg_boundingbox) { m_boundingbox_x = arg_boundingbox; if(m_drawer) m_drawer->setBoundingBoxX(arg_boundingbox); }
	void setBoundingBoxY(int arg_boundingbox) { m_boundingbox_y = arg_boundingbox; if(m_drawer) m_drawer->setBoundingBoxY(arg_boundingbox); }
	void setTiled(bool arg_tiled) { m_tiled = arg_tiled; if(m_drawer) m_drawer->setTiled(arg_tiled); }
	void setProjection(unsigned int arg_dimension_0, unsigned int arg_dimension_1) { 
	  m_dimension_0 = arg_dimension_0; 
	  m_dimension_1 = arg_dimension_1;
	  if(m_drawer) m_drawer->setProjection(arg_dimension_0, arg_dimension_1);
	}
	
	virtual bool drawPointset(Pointset<D, T, P>& arg_pointset) { return m_drawer->drawPointset(arg_pointset); }
	virtual bool drawPoint(const P& arg_point) { return m_drawer->drawPoint(arg_point); }
	virtual bool drawGrid(int arg_grid_resolution, float linewidth=1, bool dashed=0, std::string color="0.75 0.75 0.75") { return m_drawer->drawGrid(arg_grid_resolution, linewidth, dashed, color); }
	virtual bool drawRectangle(float x0, float y0, float sx, float sy, float linewidth) { return m_drawer->drawRectangle(x0, y0, sx, sy, linewidth); }
	virtual bool drawCircle(float x0, float y0, float r0, float linewidth) { return m_drawer->drawCircle(x0, y0, r0, linewidth); }
	
	void close()
	{
		assert(m_drawer);
		m_drawer->close();
		delete m_drawer;
		m_drawer=NULL;
	}
	
private:
	PointsetIllustrator_Model<D, T, P>* m_drawer;
	
	std::array<float, 3> m_color;
	float m_radius;
	int m_boundingbox_x;
	int m_boundingbox_y;
	int m_fontsize;
	double m_bordersize;
	bool m_numbered;
	bool m_tiled;
	
	unsigned int m_dimension_0;
	unsigned int m_dimension_1;
};

}//end namespace utk

#endif
