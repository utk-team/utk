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
 *  You should have received a copy of the GNU General Public License<
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

#include "delaunay.h"
#include <Geex/basics/stopwatch.h>
#include <Geex/basics/file_system.h>
#include <Geex/combinatorics/map.h>
#include <glut_viewer/glut_viewer.h>
#include <fstream>
#include <queue>

#define printf

#define SQR(x) ((x)*(x))

namespace Geex {

    void SegmentDelaunay::insert(int id, const vec2& p1, const vec2& p2, double step_length) {
        vec2 V = p2 - p1 ;
        double l = V.length() ;
        int count = int(l / step_length) ;
        count = gx_max(count, 2) ;
        V = normalize(V) ;
        double delta = l / double(count) ;
        for(int i=0; i<=count; i++) {
            insert(id, p1 + (delta * double(i)) * V) ;
        }
    } 

    int SegmentDelaunay::locate(const vec2& p) {
        return nearest_vertex(to_cgal(p))->index ;
    }

    void SegmentDelaunay::insert(int id, const vec2& p) {
        baseclass::insert(to_cgal(p))->index = id ;
    }

//____________________________________________________________________________________

	const vec2 Delaunay::v_offset_[4] = {vec2(-1,-1), vec2(1,-1), vec2(1, 1), vec2(-1, 1) } ;
	const vec2 Delaunay::e_offset_[4] = {vec2( 0,-1), vec2(1, 0), vec2(0, 1), vec2(-1, 0) } ;
	//            7 | 6 | 5
	//           -----------
	//            8 | 0 | 4 
	//           -----------
	//            1 | 2 | 3
	const vec2 Delaunay::domain_offset_[9] = {
		vec2(0, 0), vec2(-1, -1), vec2(0, -1), vec2(1, -1), vec2(1, 0), vec2(1, 1), vec2(0, 1), vec2(-1, 1), vec2(-1, 0)
	} ;

    Delaunay::Delaunay() {
        non_convex_mode_ = false ;
        cached_bbox_ = false ;
        //load_boundary(FileSystem::get_project_root() + "/data/square.line") ;
        opened_ = false ;
        insert_boundary_ = false ;

		min_angle_ = 30.0 ;
		max_angle_ = 90 ;
    }

	Delaunay::~Delaunay() {
	} 

    void Delaunay::save(const std::string& filename, int nb) {
		if(nb == 0)
		{
			std::ofstream out(filename.c_str()) ;
			out.precision(30) ;
			
			out << "mpts2 2 n unit-tor" << std::endl;
			for(Vertex_iterator it = vertices_begin(); it != vertices_end() ; it++) {
				if(is_primary(it)) { // save vertex
					vec2 point = to_geex(it->point());
					out << point.x << "\t" << point.y << std::endl;
				}
			}
			out.close() ;
		}
		else
		{
			std::ofstream out(filename.c_str(), std::ios::app) ;
		
			out << "#patch-separator" << std::endl;
			out.precision(30) ;
			for(Vertex_iterator it = vertices_begin(); it != vertices_end() ; it++) {
				if(is_primary(it)) { // save vertex
					vec2 point = to_geex(it->point());
					out << point.x << "\t" << point.y << std::endl;
				}
			}
			out.close() ;
		}
    }
    
	void Delaunay::save_eps(const std::string& filename, double radius) {                  // Plot EPS file of points
		std::ofstream epsfile(filename.c_str());
		epsfile.precision(30);
		epsfile << "%%!PS-Adobe-3.0 EPSF-3.0"  << std::endl;
		epsfile << "%%%%BoundingBox: 0 0 500 500" << std::endl;
		epsfile << "20 20 translate" << std::endl;
		epsfile << "500 dup scale" << std::endl;
		epsfile << "/r " << radius / sqrt(nb_vertices()) << " def" << std::endl;
		epsfile << "/p {r 0 360 arc fill} def" << std::endl;

		for (Vertex_iterator it = vertices_begin(); it != vertices_end(); it++) {
			if (is_primary(it)) { // save vertex
				vec2 point = to_geex(it->point());
				epsfile << point.x << "\t" << point.y << " p" << std::endl;
			}
		}
		epsfile << "showpage" << std::endl;
		epsfile.close();	                        
	};
	
	std::vector<double> Delaunay::get_samples()
	{
		std::vector<double> samples;
		for(Vertex_iterator it = vertices_begin(); it != vertices_end() ; it++) {
			if(is_primary(it)) { // save vertex
				vec2 point = to_geex(it->point());
				samples.push_back(point.x);
				samples.push_back(point.y);
			}
		}
		return samples;
	}

    void Delaunay::load(const std::string& filename) {
        std::cerr << "loading " << filename << std::endl ;
        clear() ;
        std::ifstream in(filename.c_str()) ;
        if(!in) {
            std::cerr << "could not open file" << std::endl ;
            return ;
        }
		std::vector<vec2> points;
        vec2 p ;
        while(in) {
            in >> p ;
            if(in) {  // we need to do this additional check else we got the last point twice !
				if(in_boundary(p)) {
					points.push_back(p);
				}
            }
        } ;
		set_vertices(points) ;
		in.close() ;
    }

    void Delaunay::load_boundary(const std::string& filename) {
        cached_bbox_ = false ;
        boundary_.clear() ;
        boundary_convex_.clear() ;
        boundary_.load(filename) ;
//        boundary_.normalize() ;
        if(!non_convex_mode_) {
            for(unsigned int i=0; i<boundary_.size(); i++) {
                boundary_convex_.push_back(boundary_[i].line()) ;
            }
        }

        double x_min, y_min, z_min ;
        double x_max, y_max, z_max ;
        get_bbox(x_min, y_min, z_min, x_max, y_max, z_max) ;
        double dx = x_max - x_min ;
        double dy = y_max - y_min ;
        double diag = sqrt(dx*dx + dy*dy) ;

        segments_.clear() ;
        for(unsigned int i=0; i<boundary_.size(); i++) {
            segments_.insert(int(i), boundary_[i].vertex[0], boundary_[i].vertex[1], 0.01 * diag) ;
        }
    }


    void Delaunay::get_bbox(
        real& x_min, real& y_min, real& z_min,
        real& x_max, real& y_max, real& z_max
    ) {
        z_min = 0.0 ; z_max = 1.0 ;
        if(!cached_bbox_) {
            x_min_ = y_min_ =  1e30 ;
            x_max_ = y_max_ = -1e30 ;
            if(boundary_.size() == 0) {
                x_min_ = 0.0 ; y_min_ = 0.0 ;
                x_max_ = 1.0 ; y_max_ = 1.0 ;
            }
            for(unsigned int i=0; i<boundary_.size(); i++) {
                for(unsigned int j=0; j<2; j++) {
                    x_min_ = gx_min(x_min_, boundary_[i].vertex[j].x) ;
                    y_min_ = gx_min(y_min_, boundary_[i].vertex[j].y) ;
                    x_max_ = gx_max(x_max_, boundary_[i].vertex[j].x) ;
                    y_max_ = gx_max(y_max_, boundary_[i].vertex[j].y) ;
                }
            }
            cached_bbox_ = true ;
        }
        x_min = x_min_ ; y_min = y_min_ ;
        x_max = x_max_ ; y_max = y_max_ ;
    }

    inline vec2 random_v() {
        return vec2( 
            Numeric::random_float64(),
            Numeric::random_float64()
        ) ;
    }

	void Delaunay::set_vertices(std::vector<vec2>& points) {
		SystemStopwatch timer ;
		double t0 = timer.now() ;

		clear() ;
		begin_insert() ;
		for(unsigned int i=0; i<points.size(); ++i) {
			insert(points[i]) ;
		}
		end_insert() ;
		double t1 = timer.now() ;
		std::cout << "Delaunay triangulation of " << this->number_of_vertices() << " vertices, " 
			<< t1-t0 << " second. " << std::endl ;

		//insert_copies(true, true) ;
		//double t2 = timer.now() ;
		//std::cout << "Insertion  mirror vertices, " << t2-t1 << " second. " << std::endl ;
	}

    // ------------------------------------ Delaunay 

    void Delaunay::clear() {
        baseclass::clear() ;
        all_vertices_.clear() ;
		mirrors_.clear() ;
    }

    Delaunay::Vertex_handle Delaunay::insert(const vec2& p) {
        gx_assert(opened_) ;
        if(Numeric::is_nan(p.x) || Numeric::is_nan(p.y)) {
            std::cerr << "Nan !" << std::endl ;
            return 0 ;
        }
        Vertex_handle result = baseclass::insert(to_cgal(p)) ;
        //result->theta = ::Geex::Numeric::random_float64() * M_PI / 2.0 ;
        //result->energy = 0.0 ;
        //result->rho = 1.0 ; 
        //result->locked = false ;
        return result ;
    }

	Delaunay::Vertex_handle Delaunay::insert(const vec2& p, int index) {
		Vertex_handle v = insert(p) ;
		v->index = index ;
		return v ;
	}

    Delaunay::Vertex_handle Delaunay::nearest(const vec2& p) {
		Delaunay::Vertex_handle result ;
//		result = this->nearest_vertex(to_cgal(p)) ;
		if(dimension() == 1) {
			result = this->nearest_vertex(to_cgal(p)) ;
		}
		else {
			Delaunay::Face_handle f = locate(to_cgal(p)) ;
			double dist = 1e10 ;
			for(unsigned int i=0; i<3; i++) {
				double cur_dist = (to_geex(f->vertex(i)->point()) - p).length2() ;
				if(cur_dist < dist && !is_infinite(f->vertex(i))) {
					dist = cur_dist ;
					result = f->vertex(i) ;
				}
			}
		}
        return result ;
    }

    void Delaunay::remove(const vec2& p) {
        if(number_of_vertices() <= 3) { return ; }
        gx_assert(opened_) ;
        Face_handle f = locate(to_cgal(p)) ;


        double min_d = 1e30 ;
        Vertex_handle v = 0 ;
        for(unsigned int i=0; i<3; i++) {
            if(!is_infinite(f->vertex(i))) {
                double cur_d = (to_geex(f->vertex(i)->point()) - p).length2() ;
                if(cur_d < min_d) {
                    min_d = cur_d ;
                    v = f->vertex(i) ;
                }
            }
        }
        baseclass::remove(v) ;
    }

    void Delaunay::begin_insert() { opened_ = true ; }

    void Delaunay::end_insert(bool redraw) {
        all_vertices_.clear() ;

        for(All_vertices_iterator it = all_vertices_begin() ; it != all_vertices_end() ; it++) {
            it->dual_intersects_boundary = false ;
			it->dual_infinite = false ;
            it->index = -1 ;
			it->domain = -1 ;
        }

		for(All_faces_iterator it = all_faces_begin() ; it != all_faces_end() ; it++) {
            if(is_infinite(it)) {
//                it->infinite = true ;
                it->dual = vec2(0.0, 0.0) ;
                it->dual_outside = true ;
				it->vertex(0)->dual_infinite = true ;
				it->vertex(1)->dual_infinite = true ;
				it->vertex(2)->dual_infinite = true ;
            } else {
//                it->infinite = false ;
                it->dual = to_geex(baseclass::dual(it)) ;
                it->dual_outside = !in_boundary(it->dual) ;
            }
            if(it->dual_outside) {
				if(in_boundary(to_geex(it->vertex(0)->point())))
					it->vertex(0)->dual_intersects_boundary = true ;
					it->vertex(1)->dual_intersects_boundary = true ;
					it->vertex(2)->dual_intersects_boundary = true ;
			} 
        }        
       
        all_vertices_.clear() ;
        int cur_index = 0 ;
        for(Vertex_iterator it = vertices_begin(); it != vertices_end() ; it++) {
            all_vertices_.push_back(it) ;
            it->index = cur_index ;
			it->domain = 0 ; 
            cur_index++ ;
        }

		/*
		** for the case that Voronoi cell intersects the domain but
		** the Voronoi vertices are outside
		*/
		for(unsigned int i=0; i<boundary_.size(); ++i) {
			PolygonEdge& e = boundary_[i] ;
			// this returns the infinite vertex in release mode
			Vertex_handle vh = nearest(e.vertex[1]) ;
			all_vertices_[vh->index]->dual_intersects_boundary = true ;
		}

		opened_ = false ;
        if(redraw) {
            glut_viewer_redraw() ;            
        }
    }
	
    void Delaunay::insert_random_vertex() {
        double x_min, y_min, z_min, x_max, y_max, z_max ;
        get_bbox(x_min, y_min, z_min, x_max, y_max, z_max) ;
        Geex::vec2 p = random_v() ;
        p.x = x_min + (x_max - x_min) * p.x ;
        p.y = y_min + (y_max - y_min) * p.y ;
        int nb_tries = 0 ;
        while(!in_boundary(p)) {
            nb_tries++ ;
            if(!non_convex_mode_ && nb_tries > 1000) {
                std::cerr << "Could not insert point, probably missing +non_convex flag" << std::endl ;
                exit(-1) ;
            }
            p = random_v() ;
            p.x = x_min + (x_max - x_min) * p.x ;
            p.y = y_min + (y_max - y_min) * p.y ;
        }
        insert(p) ;
    }

    void Delaunay::insert_random_vertices(int nb) {
        begin_insert() ;
        for(unsigned int i=0; i<nb; i++) {
            insert_random_vertex() ;
            //std::cerr << (i+1) << '/' << nb << std::endl ;
        }
        if(insert_boundary_ ) {
            for(unsigned int i=0; i<boundary_.size(); i++) {
                Vertex_handle v = insert(boundary_[i].vertex[0]) ;
        //        v->locked = true ;
            }
        }
        end_insert(false) ;
    }


    Polygon2* Delaunay::dual_convex_clip(Vertex_handle v, bool close) {
        Polygon2* from = &boundary_ ;
        Polygon2* to = &pong_ ;

        std::vector<Edge> edges ;
        Edge_circulator it = incident_edges(v) ;
        do {
			if(!is_infinite(it))
                edges.push_back(*it) ;
            it++ ;
        } while(it != incident_edges(v)) ;

        // No need to have special cases for infinite vertices: they
        // do not yield any clipping plane !
        for(unsigned int i=0; i<edges.size(); i++) {
            Edge e = edges[i] ;
			int ne = edges.size() ;
            if(is_infinite(e)) { continue ; }
            Geex::Line<real> L = get_dual_line(e) ;
            int E = e.first->vertex(ccw(e.second))->index ;
			Vertex_handle v1 = e.first->vertex(ccw(e.second)) ;
			if(dimension()==1) { // degenerate cases
				int id0 = e.first->vertex(0)->index ;
				int id1 = e.first->vertex(1)->index ;
				if(id0 != v->index) {
					L.inverse() ;
				}
				E = e.first->vertex(0)->index==v->index ? e.first->vertex(1)->index : e.first->vertex(0)->index ; 
			}
			gx_assert(E != v->index || (E==v->index && v1->domain!=v->domain)) ;
			from->convex_clip(*to, L, E, close) ;
			if(from == &boundary_) {
				from = &pong_ ;
				to = &ping_ ;
			} else {
				gx_swap(from, to) ;
			}
        }

		//if(from->size()==0) {
		//	std::cerr << "vertex " << v->index << " has non-clipped cell" << std::endl ;
		//}
        return from ;
		//return dual_convex_clip(boundary_, v, close) ;
    }

	Polygon2* Delaunay::dual_convex_clip(Polygon2& poly, Vertex_handle v, bool close) {
		Polygon2* from = &poly ;
		Polygon2* to = &pong_ ;

		std::vector<Edge> edges ;
		Edge_circulator it = incident_edges(v) ;
		do {
			edges.push_back(*it) ;
			it++ ;
		} while(it != incident_edges(v)) ;

		// No need to have special cases for infinite vertices: they
		// do not yield any clipping plane !
		for(unsigned int i=0; i<edges.size(); i++) {
			Edge e = edges[i] ;
			if(is_infinite(e)) { continue ; }
			Geex::Line<real> L = get_dual_line(e) ;
			int E = e.first->vertex(ccw(e.second))->index ;
			gx_assert(E != v->index) ;
			from->convex_clip(*to, L, E, close) ;
			if(from == &poly) {
				from = &pong_ ;
				to = &ping_ ;
			} else {
				gx_swap(from, to) ;
			}
		}
		return from ;
	}

    int Delaunay::dual_facet_degree(Vertex_handle v, bool period) {
        int result = 0 ;
        if(dual_cell_intersects_boundary(v) && !period) { 
            Polygon2* P = dual_convex_clip(v) ;
            result = P->size() ;
        } else {
            Face_circulator it = incident_faces(v) ;
			if(dimension()>1) {
				do {
					result++ ;
					it++ ;
				} while(it != incident_faces(v)) ;
			}
        } 
        return result ;
    }

	int Delaunay::dual_facet_degree_period(Vertex_handle v) {
		int pid = v->index ; //is_primary(v) ? v->index : v->domain ;
		Vertex_handle pv = all_vertices_[pid] ;
		std::set<Vertex_handle>& mset = mirrors_[pid] ;
		int result = 0 ;

		if(dual_cell_intersects_boundary(pv)) {
			Polygon2* P = dual_convex_clip(pv, true) ;
			for(unsigned int i=0; i<P->size(); ++i) {
				PolygonEdge& e = (*P)[i] ;
				if(e.vertex[0].bisectors.size()==2) {
					result ++ ;
				}
			}

			for(std::set<Vertex_handle>::iterator vi=mset.begin(); vi!=mset.end(); ++vi) {
				P = dual_convex_clip(*vi, true) ;
				for(unsigned int i=0; i<P->size(); ++i) {
					PolygonEdge& e = (*P)[i] ;
					if(e.vertex[0].bisectors.size()==2) {
						result ++ ;
					}
				}
			}
		}
		else {
			Face_circulator it = incident_faces(v) ;
			if(dimension()>1) {
				do {
					result++ ;
					it++ ;
				} while(it != incident_faces(v)) ;
			}
		}

		return result ;
	}

	// -------------------------------------- Periodic Delaunay: necessary copies -------------------------
	void Delaunay::insert_copies(bool full_copy, bool redraw) {
		
		clear_copies() ;

		
		if(full_copy) {
			insert_copies_full() ;
		} else {
//			compute_pvd() ;
			insert_copies_ring() ;
		}
	}

	void Delaunay::insert_copies_full(bool redraw) {
		int nv = nb_vertices() ;  // master vertices
//		int nb_copies = 8 ;
//		double shift[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} } ;

		begin_insert_copies() ;
		mirrors_.clear() ;
		for(int i=0; i<nv; ++i) {
			Vertex_handle mc = all_vertices_[i] ;
			for(int j=1; j<9; ++j) {
				vec2 newp = to_geex(mc->point())+domain_offset_[j] ; //vec2(shift[j][0], shift[j][1]) ;
				Vertex_handle sc = insert(newp) ;
				sc->domain = j ; //mc->index ; 
				sc->index = mc->index ;
				mirrors_[mc->index].insert(sc) ;
			}
		}
		end_insert_copies(redraw) ;
	}

	// the opposite domain the current domain
	// domain 0 maps to itself
	static int opposite_domain(int idx) {
		static int opp_domain[] = {0, 5, 6, 7, 8, 1, 2, 3, 4} ;
		gx_assert(idx>=0 && idx<=8) ;
		return opp_domain[idx] ;
	}

	void Delaunay::insert_copies_ring(bool redraw) {
		static int edge_domain[] = {6, 8, 2, 4} ;
		static int vert_domain[] = {5, 7, 1, 3} ;
		std::vector<std::pair<int, int> > to_insert ;
		unsigned int nv = all_vertices_.size() ;

		mirror_vertices_.clear() ;
		mirrors_.clear() ;

		for(unsigned int i=0; i<nv; ++i) {
			if(dual_cell_intersects_boundary(all_vertices_[i])) {
				Polygon2* P = dual_convex_clip(all_vertices_[i]) ;
				for(unsigned int j=0; j<P->size(); ++j) {
					int bidx = (*P)[j].boundary_index() ;

					if(bidx>=0) {
						to_insert.push_back(std::pair<int, int>(i, edge_domain[bidx])) ;
						if((*P)[j].vertex[0].on_boundary) {
							to_insert.push_back(std::pair<int, int>(i, vert_domain[bidx])) ;
						}
					}
				}
			}
		}

		if(to_insert.size()>0) {
			begin_insert_copies() ;
			for(unsigned int i=0; i<to_insert.size(); ++i) {
				vec2 newp = to_geex(all_vertices_[to_insert[i].first]->point()) + domain_offset_[to_insert[i].second] ;
				Vertex_handle newv = insert(newp) ;
				newv->index = to_insert[i].first ; 	 
				newv->domain = to_insert[i].second ; //to_insert[i].first ; 	 
				gx_assert(!is_primary(newv)) ;
				mirror_vertices_[newv->index].push_back(newv) ;
				mirrors_[to_insert[i].first].insert(newv) ; //
			}
			end_insert_copies(true) ;
		}

		// check copies
		to_insert.clear() ;

		for(unsigned int i=0; i<nv; ++i) {
			std::vector<Vertex_handle>& M = mirror_vertices_[i] ;
			for(unsigned int j=0; j<M.size(); ++j) {
				Vertex_handle vh = M[j] ;
				int opp = opposite_domain(M[j]->domain) ;
				int vid = M[j]->index ;
				gx_assert(vid==i) ;

				Vertex_circulator cir = incident_vertices(M[j]) ;
				Vertex_handle vpri = all_vertices_[vid] ;
				do {
					if(is_primary(cir)) {
						Vertex_circulator cir2 = incident_vertices(vpri) ;
						bool find = false ;
						do {
							if(cir2->index == cir->index) {
								find = true ;
							}
							++cir2 ;
						} while(cir2!=incident_vertices(vpri)) ;

						if(!find) {
							to_insert.push_back(std::pair<int, int>(cir->index, opp)) ;
						}
					}
					++cir ;
				} while(cir!=incident_vertices(M[j])) ;
			}
		}

		if(to_insert.size()>0) {
			begin_insert_copies() ;
			for(unsigned int i=0; i<to_insert.size(); ++i) {
				vec2 newp = to_geex(all_vertices_[to_insert[i].first]->point()) + domain_offset_[to_insert[i].second] ;
				Vertex_handle newv = insert(newp) ;
				newv->index = to_insert[i].first ; 	 
				newv->domain = to_insert[i].second ; //to_insert[i].first ; 	 
				gx_assert(!is_primary(newv)) ;
				mirror_vertices_[newv->index].push_back(newv) ;
				mirrors_[to_insert[i].first].insert(newv) ; //
			}
			end_insert_copies(true) ;
		}
	}

	vec2 Delaunay::copy_point(vec2& p, PolygonEdge& e) {
		// find the transform vector. only for square
		vec2 dir = e.vertex[1] - e.vertex[0] ;
		dir /= dir.length() ;
		return p + vec2(-dir[1], dir[0]) ;
	}

	vec2 Delaunay::copy_point(vec2& p, PolygonVertex& e) {
		return p ;	
	}

	void Delaunay::clear_copies(bool redraw) {
		std::vector<vec2> points ;
		for(int i=0; i<all_vertices_.size(); ++i) {
			if(is_primary(all_vertices_[i]))
				points.push_back(to_geex(all_vertices_[i]->point())) ;
		}
		mirrors_.clear() ;
		mirror_vertices_.clear() ;

		clear() ;
		begin_insert() ;
		for(int i=0; i<points.size(); ++i)
			insert(points[i]) ;
		end_insert(redraw) ;
	}

	void Delaunay::begin_insert_copies() {	opened_ = true ; }

	void Delaunay::end_insert_copies(bool redraw) {
		for(All_vertices_iterator it = all_vertices_begin() ; it != all_vertices_end() ; it++) {
			it->dual_intersects_boundary = false ;
			it->dual_infinite = false ;
		//	it->index = -1 ;
		}

		for(All_faces_iterator it = all_faces_begin() ; it != all_faces_end() ; it++) {
			if(is_infinite(it)) {
//				it->infinite = true ;
				it->dual = vec2(0.0, 0.0) ;
				it->dual_outside = true ;
				it->vertex(0)->dual_infinite = true ;
				it->vertex(1)->dual_infinite = true ;
				it->vertex(2)->dual_infinite = true ;
			} else {
//				it->infinite = false ;
				it->dual = to_geex(baseclass::dual(it)) ;
				it->dual_outside = !in_boundary(it->dual) ;
			}
			if(it->dual_outside) {
				for(int i=0; i<3; ++i) {
					if(in_boundary(to_geex(it->vertex(i)->point())))
						it->vertex(i)->dual_intersects_boundary = true ;
				}
			} else{
				for(int i=0; i<3; ++i) {
					if(!in_boundary(to_geex(it->vertex(i)->point())))
						it->vertex(i)->dual_intersects_boundary = true ;
				}
			}
		}        

////		all_vertices_.clear() ;
//		int nb_pri = 0 ;
//		int cur_index = 0 ;
//		for(Vertex_iterator it = vertices_begin(); it != vertices_end() ; it++) {
//			all_vertices_.push_back(it) ;
//			//it->index = cur_index ;
//			cur_index++ ;
//			if(is_primary(it)) 
//				nb_pri ++ ;
//			std::cerr << "domain idx: " << it->domain << ", primary vertex index: " << it->index << std::endl ;
//		}
//
//		std::cerr << "number of primary vertices: " << nb_pri << std::endl ;

		/*
		** for the case that Voronoi cell intersects the domain but
		** the Voronoi vertices are outside
		*/
		for(unsigned int i=0; i<boundary_.size(); ++i) {
			PolygonEdge& e = boundary_[i] ;
			Vertex_handle vh = nearest(e.vertex[0]) ;
			all_vertices_[vh->index]->dual_intersects_boundary = true ;
		}

		opened_ = false ;
		if(redraw) {
			glut_viewer_redraw() ;            
		}
	}

	vec2 Delaunay::mirror_vertex_point(vec2& p, int vidx) {
		gx_assert(vidx>=0 && vidx<4) ;
		return p-v_offset_[vidx] ;
	}

	vec2 Delaunay::mirror_edge_point(vec2& p, int eidx) {
		gx_assert(eidx>=0 && eidx<4) ;
		return p-e_offset_[eidx] ;	
	}

	int  Delaunay::domain_idx(vec2& p) {
		if(in_boundary(p)) 
			return 0 ;
		for(int i=0; i<4; ++i) {
			if(in_boundary(p-v_offset_[i]))
				return i*2+1 ;
			if(in_boundary(p-e_offset_[i]))
				return (i+1)*2 ;
		}

		return -1 ;
	}	

	vec2 Delaunay::translate(int domain_idx, vec2 p) {
		if(domain_idx%2==0) 
			return p-e_offset_[(domain_idx/2-1)];
		else
			return p-v_offset_[((domain_idx-1)/2)];
	}

	int  Delaunay::nb_primary() {
		int nb = 0 ;
		FOR_EACH_VERTEX_DT(Delaunay, this, it) {
			if(is_primary(it))
				nb ++ ;
		}
		return nb ;
	}

	bool Delaunay::neighbor_to_primary(Vertex_handle v) {
		Vertex_circulator cir = incident_vertices(v) ;
		do {
			if(is_primary(cir))
				return true ;
			++ cir ;
		} while(cir!=incident_vertices(v)) ;
		return false ;
	}

	void Delaunay::compute_pvd() {
		std::map<int, std::set<int> > vvmap ;
		bool suc = false ;
		mirrors_.clear() ;
		mirror_vertices_.clear() ;

		while(!suc) {
			std::vector<std::pair<int,int> > to_insert ;
			
			compute_rvd(false) ;

			// add mirror for domain vertices
			for(unsigned int i=0; i<boundary_.size(); ++i) {
				vec2 bv = boundary_[i].vertex[0] ;
				Vertex_handle v = baseclass::nearest_vertex(to_cgal(bv)) ;
				vec2 unicorn = to_geex(v->point());
				vec2 mp = mirror_vertex_point(unicorn, i) ;
				int  pid = v->index ;//is_primary(v) ? v->index : v->domain ;  // index of the master vertex of the nearest vertex
				int  vid = domain_idx(mp) ; // index of the salve vertex
				if(vid==0) continue ; // itself
				if(vvmap[pid].insert(vid).second) {
					to_insert.push_back(std::pair<int,int>(pid, vid)) ;
				}
			}

			// add mirror for boundary sites
			for(unsigned int i=0; i<rvd_.size(); ++i) {
				if(rvd_[i].size()>0) { // clipped by boundary
					Vertex_handle v = all_vertices_[i] ;
					for(unsigned int j=0; j<rvd_[i].size(); ++j) {
						vec2 pink_unicorn = to_geex(v->point());
						vec2 mp = mirror_edge_point(pink_unicorn, rvd_[i][j].boundary_index()) ;
						int  pid = v->index ;// is_primary(v) ? v->index : v->domain ;  // index of the master vertex of the nearest vertex
						int  vid = domain_idx(mp) ; // index of the salve vertex
						if(vid==0) continue ; // itself
						if(vvmap[pid].insert(vid).second) {
							to_insert.push_back(std::pair<int,int>(pid, vid)) ;
						}
					}
				}
			}

			if(to_insert.size()>0) {
				begin_insert_copies() ;
				for(unsigned int i=0; i<to_insert.size(); ++i) {
					vec2 newp = to_geex(all_vertices_[to_insert[i].first]->point()) + domain_offset_[to_insert[i].second] ;
					Vertex_handle newv = insert(newp) ;
					newv->index  = to_insert[i].first ; 	 
					newv->domain = to_insert[i].second ; 	 
					gx_assert(!is_primary(newv)) ;
					mirror_vertices_[newv->index].push_back(newv) ;
					mirrors_[to_insert[i].first].insert(newv) ; //
				}
				end_insert_copies(true) ;
			}
			else {
				suc = true ;
			}
		} 
	}

	// new implementation
	void Delaunay::compute_pvd2() {
		std::map<int, std::set<int> > vvmap ;
		bool suc = false ;
		mirrors_.clear() ;

		while(!suc) {
			std::vector<std::pair<int,int> > to_insert ;

			for(unsigned int i=0; i<all_vertices_.size(); ++i) {
				Vertex_handle v = all_vertices_[i] ;
				int pid = v->index ; // is_primary(v) ? v->index : v->domain ;  // index of the master vertex of the nearest vertex
				int vid ;
				if(dual_cell_intersects_boundary(v) ) {
					Polygon2* P = dual_convex_clip(v, false) ;
					for(unsigned int j=0; j<P->size(); ++j) {
						PolygonEdge& e = (*P)[j] ;

						for(unsigned int k=0; k<2; ++k) {
							vec2 newp = to_geex(v->point()) ;
							if(e.vertex[k].boundary_edges.size()==1) {
								newp = newp - e_offset_[e.boundary_index()] ;
							}
							if(e.vertex[k].boundary_edges.size()==2) {
								std::set<int>& edges = e.vertex[k].boundary_edges ;
								for(std::set<int>::iterator it=edges.begin(); it!=edges.end(); ++it) {
									newp = newp - e_offset_[*it] ;
								}
							}
							vid = domain_idx(newp) ;
							if(vid==0) continue ; // primary site itself
							if(vvmap[pid].insert(vid).second) {
								to_insert.push_back(std::pair<int,int>(pid, vid)) ;
							}
						}
					}
				}
			}

			if(to_insert.size()>0) {
				begin_insert_copies() ;
				for(unsigned int i=0; i<to_insert.size(); ++i) {
					vec2 newp = to_geex(all_vertices_[to_insert[i].first]->point()) + domain_offset_[to_insert[i].second] ; 
					Vertex_handle newv = insert(newp) ;//to_insert[i].second) ;
					newv->index  = to_insert[i].first ; 	 
					newv->domain = to_insert[i].second ; 	 
					gx_assert(!is_primary(newv)) ;
					mirrors_[to_insert[i].first].insert(newv) ; //
				}
				end_insert_copies(false) ;
			}
			else {
				suc = true ;
			}
		} 
	}

	bool Delaunay::is_full_hex() {
		for(unsigned int i=0; i<all_vertices_.size(); ++i) {
			Vertex_handle v = all_vertices_[i] ;
			if(is_primary(v)) {
				if(dual_facet_degree_period(v)!=6)
					return false ;
			}
		}
		return true ;
	}

	void Delaunay::compute_rvd(bool closed) {
		std::set<int> boundary_cells ;
		std::vector<bool> visited ;
		std::queue<int> Q ;

		rvd_.clear() ;
		rvd_.resize(all_vertices_.size()) ;

		visited.resize(boundary_.size()) ;
		std::fill(visited.begin(), visited.end(), false) ;
		
		for(unsigned int j=0; j<boundary_.size(); ++j) {
			// start from the 1st unvisited boundary edge
			if(visited[j]) continue ;

			PolygonEdge e = boundary_[j] ;
			vec2 mp = 0.5*(e.vertex[0] + e.vertex[1]) ;
			Vertex_handle v = baseclass::nearest_vertex(to_cgal(mp)) ;
			boundary_cells.insert(v->index) ;
			Q.push(v->index) ;

			while(!Q.empty()) {
				int index = Q.front() ;
				Q.pop() ;
				Polygon2 *to ;
				to = dual_convex_clip(all_vertices_[index], closed) ;
				rvd_[index] = *to ;

				// find neighboring boundary cells
				for(int i=0; i<to->size(); ++i) {
					PolygonEdge e = (*to)[i] ;
					if(e.on_boundary()) {
						if(e.vertex[0].bisectors.size()==1) {
							int k = *(e.vertex[0].bisectors.begin()) ;
							if(boundary_cells.insert(k).second) {
								Q.push(k) ;
							}
						}
						if(e.vertex[1].bisectors.size()==1) {
							int k = *(e.vertex[1].bisectors.begin()) ;
							if(boundary_cells.insert(k).second) {
								Q.push(k) ;
							}
						}
						visited[e.boundary_index()] = true ;
					}
				}
			}
		}
	}

}
