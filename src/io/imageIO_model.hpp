#ifndef _UTK_IMAGEIO_MODEL_HPP_
#define _UTK_IMAGEIO_MODEL_HPP_

#include <fstream>
#include <cassert>
#include <array>
#include <sstream>

#include "../pointsets/Pointset.hpp"

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetIllustrator_Model
{
	
public:
	virtual ~PointsetIllustrator_Model() {}
	
	virtual bool open(const char* arg_filename) { return false; }
	
	virtual bool drawPointset(const Pointset<D, T, P>& arg_pointset) { return false; }
	virtual bool drawPoint(const P& arg_point) { return false; }
	virtual bool drawGrid(int arg_grid_resolution, float linewidth, bool dashed, std::string color) { return false; }
	virtual bool drawRectangle(float x0, float y0, float sx, float sy, float linewidth) { return false; }
	virtual bool drawCircle(float x0, float y0, float r0, float linewidth) { return false; }
	
	virtual void close() {}
	
	
	void setColor(float arg_r, float arg_g, float arg_b) 
	{
		m_color[0] = arg_r;
		m_color[1] = arg_g;
		m_color[2] = arg_b;
	}
	void setPointRadius(float arg_radius) { m_radius = arg_radius; }
	void setFontSize(int arg_fontsize) { m_fontsize = arg_fontsize; }
	void setBorderSize(double arg_bordersize) { m_bordersize = arg_bordersize; }
	void setNumbered(bool arg_numbered) { m_numbered = arg_numbered; }
	void setBoundingBox(int arg_boundingbox) { m_boundingbox_x = arg_boundingbox; m_boundingbox_y = arg_boundingbox; }
	void setBoundingBoxX(int arg_boundingbox) { m_boundingbox_x = arg_boundingbox; }
	void setBoundingBoxY(int arg_boundingbox) { m_boundingbox_y = arg_boundingbox; }
	void setTiled(int arg_tiled) { m_tiled = arg_tiled; }
	void setProjection(unsigned int arg_dimension_0, unsigned int arg_dimension_1) { 
	  m_dimension_0 = arg_dimension_0; m_dimension_1 = arg_dimension_1; 
	}
	
protected:
	
	std::ofstream m_file;
	
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

};//end namespace utk

#endif