/*
*  _____ _____ ________  _
* /  __//  __//  __/\  \//
* | |  _|  \  |  \   \  / 
* | |_//|  /_ |  /_  /  \ 
* \____\\____\\____\/__/\\
*
* Graphics Environment for EXperimentations.
*  Copyright (C) 2006 INRIA - Project ALICE
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*  If you modify this software, you should include a notice giving the
*  name of the person performing the modification, the date of modification,
*  and the reason for such modification.
*
*  Contact: 
*
*     ALICE Project - INRIA
*     INRIA Lorraine, 
*     Campus Scientifique, BP 239
*     54506 VANDOEUVRE LES NANCY CEDEX 
*     FRANCE
*
*  Note that the GNU General Public License does not permit incorporating
*  the Software into proprietary programs. 
*/

#ifndef __GEEX_AABB_TREE_CGAL__
#define __GEEX_AABB_TREE_CGAL__

#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>
#include <Geex/CVT/topo_poly_mesh.h>
#include <Geex/combinatorics/map.h>

typedef CGAL::Simple_cartesian<double> K;

typedef K::FT FT;
typedef K::Ray_3 Ray;
typedef K::Line_3 Line;
typedef K::Point_3 Point;
typedef K::Vector_3 Vector;
typedef K::Triangle_3 Triangle;

typedef std::vector<Triangle>::iterator Iterator;
typedef CGAL::AABB_triangle_primitive<K,Iterator> Primitive;
typedef CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
typedef CGAL::AABB_tree<AABB_triangle_traits> Tree;


namespace Geex {

    class AABBTree : public Tree {
	public:
		typedef Tree baseclass ;
    public:
		AABBTree() { 
		}

		AABBTree(Map* map) { 
			insert(map) ;
		}

		AABBTree(TopoPolyMesh* boundary)  {
			insert(boundary) ;
		}

		virtual ~AABBTree() { }

		void insert(Map* map) {
			triangles_.clear() ;
			FOR_EACH_FACET(Map, map, f){
				Point v[3] ;
				Map::Halfedge* h = f->halfedge() ;
				for(int j=0; j<3; ++j) {					
					v[j] = to_cgal(h->vertex()->point()) ;
					h = h->next() ;
				}
				//				tris.push_back(Triangle(v[0], v[1], v[2])) ;
				triangles_.push_back(Triangle(v[0], v[1], v[2])) ;
			}
			Tree::insert(triangles_.begin(), triangles_.end()) ;
		}

		void insert(TopoPolyMesh* boundary) {
//			std::vector<Triangle> tris ;
			for(int i=0; i<boundary->nb_facets(); ++i) {
				Point v[3] ;
				for(int j=0; j<3; ++j) {
					v[j] = to_cgal(boundary->vertex(boundary->facet_begin(i)+j)) ;
				}
//				tris.push_back(Triangle(v[0], v[1], v[2])) ;
				triangles_.push_back(Triangle(v[0], v[1], v[2])) ;
			}
			Tree::insert(triangles_.begin(), triangles_.end()) ;
		}

		void insert(std::vector<std::vector<vec3> >& tris_in) {
//			std::vector<Triangle> tris ;
			for(int i=0; i<tris_in.size(); ++i) {
				Point v[3] ;
				for(int j=0; j<3; ++j) {
					v[j] = to_cgal(tris_in[i][j]) ;
				}
//				tris.push_back(Triangle(v[0], v[1], v[2])) ;
				triangles_.push_back(Triangle(v[0], v[1], v[2])) ;
			}
			Tree::insert(triangles_.begin(), triangles_.end()) ;
		}

		vec3 closest_point(const vec3& p) {
			return to_geex(baseclass::closest_point(to_cgal(p))) ;
		}

		void closest_point(const vec3& p, vec3& cp, int& fid) {
			baseclass::Point_and_primitive_id ppid = closest_point_and_primitive(to_cgal(p)) ;
			cp  = to_geex(ppid.first) ;
			if(ppid.second!=triangles_.end()) {
				fid = ppid.second - triangles_.begin() ;
			//	fid = (int)(ppid.second - triangles_.begin()) ;
				//int size = triangles_.end() - triangles_.begin() ;
				// std::list<Triangle>::iterator it = triangles_.begin() + ppid.second - triangles_.begin() ;
				// triangles_.end() - triangles_.begin() ;
			}
			else fid = -1 ;
		}

		bool closest_point(const vec3& o, const vec3& dir, vec3& cp, int& fid) {
			std::vector<baseclass::Object_and_primitive_id> ops ;
			Vector v = to_cgal(dir)-CGAL::ORIGIN ;
			Point  p = to_cgal(o) ;
			Ray ray_query(p, v);
			//number_of_intersected_primitives(ray_query) ;
			all_intersections(ray_query, std::back_inserter(ops)) ;
			////cp = to_geex(ppid.first) ;
			double dist = 1e10 ;
			bool   suc = false ;
			for(int i=0; i<ops.size(); ++i) {
				CGAL::Object obj = ops[i].first ;
				Point pt ;
				if(CGAL::assign(pt, obj)) {
					double d2 = distance2(o, to_geex(pt)) ;
					if(d2<dist) {
						dist = d2 ;
						cp = to_geex(pt) ;
						suc = true ;
						fid = ops[i].second - triangles_.begin() ;
					}
				}
			}
			return suc ;
		}

	protected:
		vec3 to_geex(const Point& p) {
			return vec3(p.x(), p.y(), p.z()) ;
		}

		Point to_cgal(const vec3& p) {
			return Point(p.x, p.y, p.z) ;
		}
	protected:
		std::vector<Triangle> triangles_ ;
    } ;

} 

#endif

