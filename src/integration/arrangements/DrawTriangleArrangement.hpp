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
#ifndef _UTK_GEOMETRYIO_POSTSCRIPT_
#define _UTK_GEOMETRYIO_POSTSCRIPT_

#include <fstream>
#include <iomanip>
#include <sstream>
#include "../../pointsets/Vector.hpp"

namespace utk
{
	
struct ArcPostscript
{
		double angle_start;
		double angle_end;
		double radius;
		double center_x;
		double center_y;
		bool concave;
		
		ArcPostscript() {
			angle_start = 0;
			angle_end = 0;
			radius = 0;
			center_x = 0;
			center_y = 0;
			concave=0;
		}
		ArcPostscript(double arg_center_x, double arg_center_y, double arg_radius, double arg_angle_start, double arg_angle_end, bool arg_concave){
			angle_start = arg_angle_start;
			angle_end = arg_angle_end;
			radius = arg_radius;
			center_x = arg_center_x;
			center_y = arg_center_y;
			concave=arg_concave;
		}
};
	
class Drawer_Postscript
{
public:
	Drawer_Postscript(int arg_size=1024) { m_size=arg_size; }
	Drawer_Postscript(std::string arg_file, int arg_size = 1024) { m_size=arg_size; open(arg_file); }
	~Drawer_Postscript() { close(); }
	
	void drawPolygon(const std::vector< Vector<2, double> >& arg_points, double arg_grayscale)
	{		
		if (arg_points.size() < 3)
		{
			std::cerr << "Cannot draw a polygon with only " << arg_points.size() << " < 3 points" << std::endl;
			return;
		}
		
		std::string it;
		std::ostringstream oss;
		it += "newpath\n";

		/*Set color*/
		oss << arg_grayscale;
		it += oss.str();
		it += " setgray\n";
		oss.clear();
		oss.str("");
	
		
		/**Move to the first point of the polygon*/
		oss << std::fixed << std::setprecision(10) << arg_points[0][0]*m_size;
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " ";
		oss << arg_points[0][1]*m_size;
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " moveto\n";
		
		/*Line to each point of the polygon*/
		for(unsigned int i=1; i<arg_points.size(); i++)
		{
			oss << std::fixed << std::setprecision(10) << arg_points[i][0]*m_size;
			it += oss.str();
			oss.clear();
			oss.str("");
			it += " ";
			oss << arg_points[i][1]*m_size;
			it += oss.str();
			oss.clear();
			oss.str("");
			it += " lineto\n";
		}
		
		/*Back to the first point to close the shape*/
		oss << std::fixed << std::setprecision(10) << arg_points[0][0]*m_size;
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " ";
		oss << arg_points[0][1]*m_size;
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " lineto\n";
		
		it += "gsave\n\
fill\n\
grestore\n\
closepath\n\
stroke\n";

		m_file << it;
	}
	
	void drawCurvePolygon(const std::vector< ArcPostscript >& arg_arcs, double arg_grayscale)
	{
		std::string it;
		
		std::ostringstream oss;
		oss << std::setprecision(15);
		it += "newpath\n";
		
		oss << arg_grayscale;
		it += oss.str();
		it += " setgray\n";
		oss.clear();
		oss.str("");
		
		for(unsigned int i=0; i<arg_arcs.size(); i++)
		{
			oss << arg_arcs[i].center_x*m_size;
			it += oss.str();
			it += " ";
			oss.clear();
			oss.str("");
			
			oss << arg_arcs[i].center_y*m_size;
			it += oss.str();
			it += " ";
			oss.clear();
			oss.str("");

			oss << arg_arcs[i].radius*m_size;
			it += oss.str();
			it += " ";
			oss.clear();
			oss.str("");

			oss << arg_arcs[i].angle_start;
			it += oss.str();
			it += " ";
			oss.clear();
			oss.str("");
			
			oss << arg_arcs[i].angle_end;
			it += oss.str();
			it += " ";
			oss.clear();
			oss.str("");
			
			if(arg_arcs[i].concave)
				it +=" arcn\n";
			else
				it +=" arc\n";
		}
		
		it += "gsave\n\
fill\n\
grestore\n\
closepath\n\
stroke\n";

		m_file << it;
	}
	
	void drawDisk(const Vector<2, double>& arg_disk, double arg_radius, double arg_grayscale)
	{
		std::string it;
		
		std::ostringstream oss;
		it += "newpath\n";
		
		oss << arg_grayscale;
		it += oss.str();
		it += " setgray\n";
		oss.clear();
		oss.str("");
		
		/**Move to the first point of the disk*/
		oss << std::fixed << std::setprecision(10) << arg_disk[0]*m_size;
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " ";
		oss << arg_disk[1]*m_size;
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " moveto\n";
		
		oss << arg_disk[0]*m_size;
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");
		
		oss << arg_disk[1]*m_size;
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		oss << arg_radius*m_size;
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		it += "0 360 arc\n";
		
		it += "gsave\n\
fill\n\
grestore\n\
closepath\n\
stroke\n";

		m_file << it;
	}
	
	void drawArc(ArcPostscript& arg_arc, double arg_grayscale)
	{
		double p[2]; p[0] = arg_arc.center_x; p[1] = arg_arc.center_y;
		drawArc(p, arg_arc.radius, arg_arc.angle_start, arg_arc.angle_end, arg_grayscale);
	}
	void drawArc(const Vector<2, double>& arg_position, double arg_radius, double arg_angle_start, double arg_angle_end, double arg_grayscale)
	{
		std::string it;
		
		std::ostringstream oss;
		it += "newpath\n";
		
		oss << arg_grayscale;
		it += oss.str();
		it += " setgray\n";
		oss.clear();
		oss.str("");
		
		/**Move to the first point of the disk*/
		/*oss << std::fixed << std::setprecision(10) << arg_position[0]*m_size;
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " ";
		oss << arg_position[1]*m_size;
		it += oss.str();
		oss.clear();
		oss.str("");
		it += " moveto\n";*/
		
		oss << arg_position[0]*m_size;
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");
		
		oss << arg_position[1]*m_size;
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		oss << arg_radius*m_size;
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");

		oss << arg_angle_start;
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");
		
		oss << arg_angle_end;
		it += oss.str();
		it += " ";
		oss.clear();
		oss.str("");
		
		it +=" arc\n";
		
		it += "gsave\n\
grestore\n\
stroke\n";

		m_file << it;
    }
	 
	void open(std::string arg_file)
	{
		m_filename = arg_file;
        m_file.open(arg_file.c_str(), std::ios::out);
		m_file << "%!PS-Adobe-3.0 " << std::endl;
		m_file << "%%Creator: [UTK]" << std::endl;
		m_file << "%%Origin: [eg: 0 0]" << std::endl;
		m_file << "%%BoundingBox: " << 0 << " " << 0 << " " << m_size << " " << m_size << std::endl;
		m_file << "0.1 setlinewidth" << std::endl;
	}
	
	void close()
	{
		m_file << "%EOF"  << std::endl;
		m_file.close();
	}
private:
	std::ofstream m_file;
	std::string m_filename;
	int m_size;
};

} //end namespace utk

#endif
