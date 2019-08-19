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
#ifndef _TRIANGLE_ARRANGEMENT_H_
#define _TRIANGLE_ARRANGEMENT_H_

#include <random>
#include <CGAL/Exact_rational.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Env_triangle_traits_3.h>
#include <CGAL/Env_surface_data_traits_3.h>
#include <CGAL/envelope_3.h>
#include <CGAL/Arr_naive_point_location.h>
#include <CGAL/Arr_walk_along_line_point_location.h>
#include <CGAL/Arr_landmarks_point_location.h>
#include <boost/variant/get.hpp>
#include <CGAL/Triangle_3.h>
#include "DrawTriangleArrangement.hpp"
#include "../../io/messageStream.hpp"

#ifdef CIMG_ON
	#include "../../externals/CImg-1.6.9/CImg.h"
	using namespace cimg_library;
#endif

typedef CGAL::Exact_rational                             Number_type;
typedef CGAL::Cartesian<Number_type>                     Kernel;
typedef Kernel::FT                                       FT;
typedef CGAL::Env_triangle_traits_3<Kernel>              Traits_3;
typedef Traits_3::Point_3                                  Point_3;
typedef Traits_3::Surface_3                              Triangle_3;
typedef CGAL::Env_surface_data_traits_3<Traits_3, float>  Data_traits_3;
typedef Data_traits_3::Surface_3                         Data_triangle_3;
typedef CGAL::Envelope_diagram_2<Data_traits_3>          Envelope_diagram_2;
typedef Envelope_diagram_2::Point_2                      Point_2;
typedef CGAL::Arr_naive_point_location<Envelope_diagram_2> Naive_pl;
typedef CGAL::Arr_walk_along_line_point_location<Envelope_diagram_2> Walk_pl;

std::vector<std::pair<float, float>> luminance_histogram;
std::vector<std::pair<float, float>> orientation_histogram;


//MAXSIZE is 2^16
const int TRIANGLE_MAXSIZE = 65536;

namespace utk
{
	
class TriangleArrangement
{

public:
	TriangleArrangement() {};
	~TriangleArrangement() {};
	
	virtual void generateTriangles(const unsigned int seed, std::string histogram_lumi="") = 0;

	void computeEnveloppe() { CGAL::upper_envelope_3 (triangles.begin(), triangles.end(), diagram); }
	
	double exact_integration();
	double integration(std::list<Point_2> &pointset);
	void rasterizerPS(const unsigned int Width,
                const char *filename,
                const bool displayProgressBar=true);
	double sample(const Point_2& p);
	
protected:
	float generateTag();
	float generateAngle();

	void load_histogram(std::string luminancehistogram_file="");
	void load_orientations(float min, float max, float mean, float stddev, float step=0.01);
	
	float getId(Walk_pl& pl, const Point_2 &p);
	FT measureTriangle(const Envelope_diagram_2::Ccb_halfedge_const_circulator &a,
                            const Envelope_diagram_2::Ccb_halfedge_const_circulator &b,
                            const Envelope_diagram_2::Ccb_halfedge_const_circulator &c);
	FT measureFace(const Envelope_diagram_2::Face_const_iterator &fit);
	
protected:
	Envelope_diagram_2 diagram;
	std::list<Data_triangle_3> triangles;
};

/*******************************************/
/** Statistics over the image to generate **/
/*******************************************/

/*Load the luminance histogram*/
void TriangleArrangement::load_histogram(std::string luminancehistogram_file)
{
  if (!luminancehistogram_file.empty())
  {
	std::ifstream file;
	file.open(luminancehistogram_file.c_str(), std::ifstream::in);
	do
	{
		float val;
		float cdf;
		file >> val;
		file >> cdf;
		luminance_histogram.push_back(std::pair<float, float>(val, cdf));
	}
	while(!file.eof());
	luminance_histogram.resize(luminance_histogram.size()-1);
	
	file.close();
  }
  else
  {
	for(int i=1; i<=TRIANGLE_MAXSIZE; i++)
		luminance_histogram.push_back(std::pair<float, float>((double)i/(double)TRIANGLE_MAXSIZE, (double)i/(double)TRIANGLE_MAXSIZE));
  }
}

/*returns a random tag value following the distribution in luminance_histogram*/
float TriangleArrangement::generateTag()
{
	double a = drand48();
	if (luminance_histogram[0].second > a)
		return luminance_histogram[0].first;
	
	for(unsigned int i=0; i<luminance_histogram.size(); i++)
	{
		if (luminance_histogram[i].second < a && luminance_histogram[i+1].second > a)
			return luminance_histogram[i].first;
	}
	
	return luminance_histogram[luminance_histogram.size()-1].first;
}

/*Load the orientations histogram*/
float gaussianCDF(float x, float mean, float stddev)
{
	float num = x - mean;
	float denom = stddev*sqrt(2);
	
	float erf_val = std::erf( num/denom );
	
	return 0.5*(1 + erf_val);
}
void TriangleArrangement::load_orientations(float min, float max, float mean, float stddev, float step)
{
	for(float i=min; i<=max; i+=step)
		orientation_histogram.push_back(std::pair<float,float>(i, gaussianCDF(i, mean, stddev)) );
}

/*returns a random angle in radians following the distribution in orientation_histogram*/
float TriangleArrangement::generateAngle()
{
	double a = drand48();
	if (orientation_histogram[0].second > a)
		return orientation_histogram[0].first;
	
	for(unsigned int i=0; i<orientation_histogram.size(); i++)
	{
		if (orientation_histogram[i].second < a && orientation_histogram[i+1].second > a)
			return orientation_histogram[i].first;
	}
	
	return orientation_histogram[orientation_histogram.size()-1].first;
}

/*******************************************/
/******** Integration over the soup ********/
/*******************************************/


//Return the triangle Id  associated to the arrangement cell containing a given point*
//The walker defines the point location strategy
//
// BE CAREFULL: the returned Id  0 corresponds to the infinite face (not a triangle).
//
float TriangleArrangement::getId(Walk_pl& pl, const Point_2 &p)
{
  typedef typename Envelope_diagram_2::Vertex_const_handle   Vertex_const_handle;
  typedef typename Envelope_diagram_2::Halfedge_const_handle Halfedge_const_handle;
  typedef typename Envelope_diagram_2::Face_const_handle     Face_const_handle;
  const Vertex_const_handle*   v;
  const Halfedge_const_handle* e;
  const Face_const_handle*     f;
  
  typename CGAL::Arr_point_location_result<Envelope_diagram_2>::Type obj = pl.locate( p );
  if ((f = boost::get<Face_const_handle>(&obj))) //point in a face
  {
    if (!((*f)->is_unbounded())) //not in unbounded
    {
      Envelope_diagram_2::Face_const_iterator it(*f);
      if (it->surfaces_begin() == it->surfaces_end() )
      {
        return 0; //should be a hole or a face with no data
      }
      else
        return it->surfaces_begin()->data();
     }
    else
      return 0;  // unbounded face
  }
  
  if ((e = boost::get<Halfedge_const_handle>(&obj))) // located on an edge
  {
    Envelope_diagram_2::Halfedge_const_iterator it(*e);
    return it->surfaces_begin()->data();
  }
  
  if ((v = boost::get<Vertex_const_handle>(&obj))) // located on an edge
  {
    Envelope_diagram_2::Vertex_const_iterator it(*v);
    return it->surfaces_begin()->data();
  }
  std::cerr << "Oops something goes wrong while locating a point " << p << std::endl;
  exit(2);
  return 0;
}

//Measure of a signed triangle
FT TriangleArrangement::measureTriangle(const Envelope_diagram_2::Ccb_halfedge_const_circulator &a,
                            const Envelope_diagram_2::Ccb_halfedge_const_circulator &b,
                            const Envelope_diagram_2::Ccb_halfedge_const_circulator &c)
{
  assert(a->target()->point().x().to_double() > 0 && a->target()->point().x().to_double() < TRIANGLE_MAXSIZE);
  assert(b->target()->point().x().to_double() > 0 && b->target()->point().x().to_double() < TRIANGLE_MAXSIZE);
  assert(c->target()->point().x().to_double() > 0 && c->target()->point().x().to_double() < TRIANGLE_MAXSIZE);

  assert(a->target()->point().y().to_double() > 0 && a->target()->point().y().to_double() < TRIANGLE_MAXSIZE);
  assert(b->target()->point().y().to_double() > 0 && b->target()->point().y().to_double() < TRIANGLE_MAXSIZE);
  assert(c->target()->point().y().to_double() > 0 && c->target()->point().y().to_double() < TRIANGLE_MAXSIZE);
  
  FT area = CGAL::area( a->target()->point(), b->target()->point(), c->target()->point());
  return area;
}

//Measure of a face
// Area of the face x the tagId of the face
// (i.e. constant function with value tagId on the face)
FT TriangleArrangement::measureFace(const Envelope_diagram_2::Face_const_iterator &fit)
{
  Envelope_diagram_2::Ccb_halfedge_const_circulator  ccb,nextccb,origin;
  
  //Face without any data
  if (fit->surfaces_begin() == fit->surfaces_end())
    return 0;
 
  const float faceId = fit->surfaces_begin()->data();
  
  // Print the vertices along the outer boundary of the face.
  ccb = fit->outer_ccb();
  origin = ccb;
  ++ccb;
  nextccb = ccb;
  ++nextccb;
  FT measure = 0;
  do
  {
    measure += measureTriangle( origin, ccb, nextccb );
    ++ccb;
    ++nextccb;
  } while (nextccb != origin);

  return measure * (faceId);
}

// Exact Integration
// normalized to stay in [0,1]
double TriangleArrangement::exact_integration()
{
  FT integral = 0;
  for(Envelope_diagram_2::Face_const_iterator faceit = diagram.faces_begin(), faceend = diagram.faces_end();
      faceit != faceend; ++faceit)
  {
    if (! (faceit->is_unbounded()))
      integral += measureFace( faceit );
  }
  FT scale = TRIANGLE_MAXSIZE;
  integral = integral / scale  / scale;
  return CGAL::to_double(integral);
}

// MonteCarlo Integration
double TriangleArrangement::integration(std::list<Point_2> &pointset)
{
  double sum = 0.0;
  Walk_pl pl(diagram);
  const double N = static_cast<double>(pointset.size());
  
  for(auto it = pointset.begin(), itend = pointset.end() ; it != itend; ++it)
  {
	float faceId =  getId(pl, *it);
	double quantity = (faceId);
    sum += quantity;
  }
  
  return 1.0/N * sum;
}

double TriangleArrangement::sample(const Point_2 &pt)
{
	Walk_pl pl(diagram);
	return getId(pl, pt);
}


/*******************************************/
/************* Rasterization ***************/
/*******************************************/


void TriangleArrangement::rasterizerPS(const unsigned int Width,
                const char *filename,
                const bool displayProgressBar)
{
	utk::Drawer_Postscript d(filename, Width);
	double normalisation_factor = 1.0/TRIANGLE_MAXSIZE;
	
	for(Envelope_diagram_2::Face_const_iterator faceit = diagram.faces_begin(), faceend = diagram.faces_end();
      faceit != faceend; ++faceit)
	{
		Envelope_diagram_2::Ccb_halfedge_const_circulator  ccb,nextccb,origin;
		//Face without any data
		if (faceit->surfaces_begin() == faceit->surfaces_end())
			continue;
		
		std::vector< utk::Vector<2, double> > points;
		const float color  = faceit->surfaces_begin()->data() / luminance_histogram[luminance_histogram.size()-1].first;
		
		ccb = faceit->outer_ccb();
		origin = ccb;
		++ccb;
		nextccb = ccb;
		++nextccb;
		
		double pt[2];
    pt[0] = CGAL::to_double(origin->target()->point().x())*normalisation_factor;
		pt[1] = CGAL::to_double(origin->target()->point().y())*normalisation_factor;
		points.push_back(utk::Vector<2, double>(pt));
		
		pt[0] = CGAL::to_double(ccb->target()->point().x())*normalisation_factor;
		pt[1] = CGAL::to_double(ccb->target()->point().y())*normalisation_factor;
		points.push_back(utk::Vector<2, double>(pt));
		do
		{
			pt[0] = CGAL::to_double(nextccb->target()->point().x())*normalisation_factor;
			pt[1] = CGAL::to_double(nextccb->target()->point().y())*normalisation_factor;
			points.push_back(utk::Vector<2, double>(pt));
			++nextccb;
		} while (nextccb != origin);
		
		d.drawPolygon(points, color);
	}
}

/*******************************************/
/***************** Utils *******************/
/*******************************************/

bool backToBounds(double& x, double& y)
{
	bool hasmoved = false;
	if (x <= 0) {
		hasmoved = true;
		x = drand48()*1000+1;
	}
	if (y <= 0){
		hasmoved = true;
		y = drand48()*1000+1;
	}
	if (x >= TRIANGLE_MAXSIZE){
		hasmoved = true;
		x = TRIANGLE_MAXSIZE-(drand48()*1000)-1;
	}
	if (y >= TRIANGLE_MAXSIZE){
		hasmoved = true;
		y = TRIANGLE_MAXSIZE-(drand48()*1000)-1;
	}
	return hasmoved;
}

} //end namespace utk

#endif
