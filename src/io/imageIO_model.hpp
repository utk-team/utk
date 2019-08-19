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