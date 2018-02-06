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

#ifndef __GEEX_CC_RVD__
#define __GEEX_CC_RVD__

#include <Geex/CVT/topo_poly_mesh.h>
#include <Geex/CVT/delaunay_skel.h>
#include <Geex/CVT/delaunay.h>
#include <Geex/CVT/RVD_predicates.h>
#include <stack>
#include <queue>
#include <set>

/*
For help about stack
  push(x)
  top()
  pop()
*/
/*
For help about set
  empty()
  begin()
  find() -> iterator
  erase(iterator)
*/
/*
For help about queue
  empty()
  front()
  back()
  push(x)
  pop()
*/

namespace Geex {

    /*
     * Returns a seed near a given facet. Does not
     * need to be exactly the nearest seed.
     * This version is used when computing RVD directly on CGAL
     * triangulations.
     */
    template <class DEL> inline unsigned int find_seed_near_facet(
        const DEL* delaunay, const TopoPolyMesh* M, unsigned int f
    ){
        typedef typename DEL::Point Point ;
        vec3 p = M->facet_center(f) ;
        return delaunay->nearest_vertex(Point(p.x, p.y, p.z))->id ;
    }

    /*
     * Returns a seed near a given facet. Does not
     * need to be exactly the nearest seed.
     * This version (specialization) is used when using the Delaunay
     * abstract interface.
     */
    template <> inline unsigned int find_seed_near_facet<Delaunay>(
        const Delaunay* delaunay, const TopoPolyMesh* M, unsigned int f
    ) {
        vec3 p = M->facet_center(f) ;
        return delaunay->nearest_vertex_id(p) ;
    }

    //--------------------------------------------------------------------------

    // Used by RVD internally to implement for_each_triangle()
    template <class T> class TriangleAction {
    public:
        TriangleAction(const T& do_it_in) : do_it(do_it_in) { }
        void operator()(unsigned int v, TopoPolyMesh* M) const {
            for(unsigned int f=0; f<M->nb_facets(); f++) {
                unsigned int i0 = M->facet_begin(f) ;
                for(unsigned int i = M->facet_begin(f)+1; i+1<M->facet_end(f); i++) {
                    do_it(v, M->vertex(i0), M->vertex(i), M->vertex(i+1)) ;
                }
            }
        }
    private:
        const T& do_it ;
    } ;

    // Used by RVD internally to implement for_each_halfedge()
    template <class T> class HalfedgeAction {
    public:
        HalfedgeAction(const T& do_it_in) : do_it(do_it_in) { }
        void operator()(unsigned int v, TopoPolyMesh* M) const {
            for(unsigned int f=0; f<M->nb_facets(); f++) {
                for(unsigned int i = M->facet_begin(f); i<M->facet_end(f); i++) {
                    if(M->vertex(i).check_flag(TopoPolyVertexEdge::INTERSECT)) {
                        do_it(v, M->vertex(i), M->vertex(M->next_around_facet(f,i))) ;
                    }
                }
            }
        }
    private:
        const T& do_it ;
    } ;

    // Used by RVD internally to implement for_each_primal_triangle()
    template <class T> class PrimalTriangleAction {
    public:
        PrimalTriangleAction(const T& do_it_in) : do_it(do_it_in) { }
        void operator()(unsigned int iv1, TopoPolyMesh* M) const {
            for(unsigned int f=0; f<M->nb_facets(); f++) {
                for(unsigned int i = M->facet_begin(f); i<M->facet_end(f); i++) {
                    const TopoPolyVertexEdge& ve = M->vertex(i) ;
                    // Primal triangles correspond to vertices of
                    // the RVD that are on two bisectors.  
                    unsigned int nb_bisect = ve.sym.nb_bisectors() ;
                    if(nb_bisect >= 2) {
                        bool found = false ;
                        for(unsigned int k1=0; k1<nb_bisect-1 && !found; k1++) {
                            for(unsigned int k2=k1+1; k2<nb_bisect && !found; k2++) {
                                unsigned int iv2 = ve.sym.bisector(k1) ; 
                                unsigned int iv3 = ve.sym.bisector(k2) ;
                                // Make sure each triangle is generated once only
                                // (skip the triangle if iv1 is not the smallest index)
                                // I still fully not understand what's going on, when
                                // we got degeneracies, the test skips some triangles (holes)
                                if(iv1 < iv2 && iv1 < iv3) {
                                    // Check whether the orientation of the triangle is
                                    // consistent with the orientation of the facet. If
                                    // its not the case, swap iv2 and iv3
                                    unsigned int j1 = M->prev_around_facet(f,i) ;
                                    unsigned int j2 = M->next_around_facet(f,i) ;

                                    const TopoPolyVertexEdge& ve2 = M->vertex(j1) ; 
                                    const TopoPolyVertexEdge& ve3 = M->vertex(j2) ;

                                    if(ve2.sym.has_bisector(iv2) && ve3.sym.has_bisector(iv3)) {
                                        do_it(iv1, iv2, iv3) ;
                                        found = true ;
                                    } else if(ve2.sym.has_bisector(iv3) && ve3.sym.has_bisector(iv2)) {
                                        do_it(iv1, iv3, iv2) ;
                                        found = true ;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    private:
        const T& do_it ;
    } ;

    //--------------------------------------------------------------------------


//    struct TopoPolyStackItem {
//        TopoPolyStackItem(
//            unsigned int f_in, unsigned int cell_in
//        ) : f(f_in), cell(cell_in) { }
//        unsigned int f ;
//        unsigned int cell ;
//
//	//bool operator< (TopoPolyStackItem& rhs) {
//	//  return f < rhs.f ;
//	//}
//    } ;
//
//    typedef std::deque< TopoPolyStackItem > TopoPolyStack ;

    class CellCorner {
      //The information about a 2 bisector point. Contains more data than a
      //symbolic vertex since the inner seed is also stored.
      public:

      CellCorner(
	unsigned int f_in,
	unsigned int c1_in,
	unsigned int c2_in,
	unsigned int c3_in
      ) : f(f_in), c1(c1_in), c2(c2_in), c3(c3_in) {
	normalize() ;
      }

      //A seed corner is contained in a facet, at the intersection of two of
      //the bisectors between c1, c2 and c3 (the three are crossing along the
      //same line anyway)
      unsigned int f ;
      unsigned int c1 ;
      unsigned int c2 ;
      unsigned int c3 ;

      void normalize() {
	//sort the three seeds
	unsigned int min = gx_min(c1,c2) ;
	min = gx_min(min, c3) ;

	unsigned int max = gx_max(c1,c2) ;
	max = gx_max(max,c3) ;

	unsigned int mid = c1+c2+c3-min-max ;

	c1 = min ;
	c2 = mid ;
	c3 = max ;
      }

      bool operator==(const CellCorner& rhs) const{
	return (f == rhs.f) && (c1 == rhs.c1) && (c2 == rhs.c2) && (c3 == rhs.c3) ;
      }

      bool operator<(const CellCorner& rhs) const{
	//lexicographic order
	if(f == rhs.f) {
	  if(c1 == rhs.c1) {
	    if(c2 == rhs.c2) {
	      return c3 < rhs.c3 ;
	    } else {
	      return c2 < rhs.c2 ;
	    }
	  } else {
	    return c1 < rhs.c1 ;
	  }
	} else {
	  return f < rhs.f ;
	}
      }

      bool operator>(const CellCorner& rhs) const{
	//lexicographic order
	if(f == rhs.f) {
	  if(c1 == rhs.c1) {
	    if(c2 == rhs.c2) {
	      return c3 > rhs.c3 ;
	    } else {
	      return c2 > rhs.c2 ;
	    }
	  } else {
	    return c1 > rhs.c1 ;
	  }
	} else {
	  return f > rhs.f ;
	}
      }
    } ;

    inline std::ostream& operator<<( std::ostream& out, const Geex::CellCorner& c) {
      return out  << "{" << c.f << "|" << c.c1 << "," << c.c2 << "," << c.c3 << "}" ;
    }

    class PathEnd {
      //A path endpoint
      public:

      PathEnd(
	unsigned int f1_in,
	unsigned int f2_in,
	unsigned int s_in
      ) : f1(f1_in), f2(f2_in), s(s_in) {
	normalize() ;
      }

      PathEnd( 
	const TopoPolyVertexEdge& v
      ) : f1(-v.sym[0]-1), f2(-v.sym[1]-1), s(v.sym[2]-1) {
	//std::cout << "created path end with facets " << f1 << " and " << f2 << " and seed " << s << std::endl ;
	normalize() ;
      }

      //The endpoints are only 1 bisector vertices, since the pieces are
      //discovered facet by facet. The two facets are stored in the point. The
      //seed is used to be able to propagate events to a neighbouring seed in
      //case the path crosses no 2 bisector vertex.
      unsigned int f1 ;
      unsigned int f2 ;
      unsigned int s ;

      void normalize() {
	//sort the facets
	unsigned int min = gx_min(f1,f2) ;
	unsigned int max = gx_max(f1,f2) ;
	f1 = min ;
	f2 = max ;
      }

      bool operator==(const PathEnd& rhs) const {
	return (f1 == rhs.f1) && (f2 == rhs.f2) && (s == rhs.s);
      }
    } ;

    inline std::ostream& operator<<( std::ostream& out, const Geex::PathEnd& p) {
      return out  << "{" << p.s << "|" << p.f1 << "," << p.f2 << "}" ;
    }

    class UFPath {
      //Union find paths
      public:

      UFPath(
	PathEnd start_in,
	PathEnd end_in,
	bool multiple_seed_in
      ) : start(start_in), end(end_in), multiple_seed(multiple_seed_in) {
	resp = this ;
	min_facet = gx_min(start.f1,end.f1) ;
      }

      //endpoints
      PathEnd start ;
      PathEnd end ;
      //true if the path goes through a 2 bisector vertex
      bool multiple_seed ;
      //the smallest indexed facet the path goes through
      unsigned int min_facet ;
      //the responsible path of this path's class
      mutable UFPath* resp ;

      bool is_resp() const {
	//tests whether this path is responsible for its class
	return resp == this ;
      }

      UFPath* get_resp() const {
	//provide the path responsible for the class, and sets
	//this path at depth 1 in the class bottom-up tree
	if(!is_resp()) {
	  UFPath* new_resp = resp->get_resp() ;
	  resp = new_resp ;
	}
	return resp ;
      }

      bool connect(UFPath* rhs) {
	//merge the class of the rhs path with this path's class if the
	//endpoints are matching, therefore updating the endpoints of the
	//responsible path
	UFPath* lhs = get_resp() ;
	rhs = rhs->get_resp() ;
        bool connected = false ;
	if(lhs->start == rhs->end) {
	  //this path starts where the rhs path ends
	  //update endpoint
	  lhs->start = rhs->start ;
	  //merge single seed and min facet properties
	  lhs->multiple_seed = lhs->multiple_seed || rhs->multiple_seed ;
	  lhs->min_facet = gx_min(lhs->min_facet,rhs->min_facet) ;
	  //set the class of rhs as part of this class
	  rhs->resp = lhs ;
          //set return value
          connected = true ;
          //std::cout << "connected path into " << "{" << resp->start << "---"  << resp->end << " | " << resp->multiple_seed << "," << resp->min_facet << "}"  << std::endl ;
	} else if (lhs->end == rhs->start) {
	  //this path ends where the rhs path starts
	  //update endpoint
	  lhs->end = rhs->end ;
	  //set the class of rhs as part of this class
	  lhs->multiple_seed = lhs->multiple_seed || rhs->multiple_seed ;
	  lhs->min_facet = gx_min(lhs->min_facet,rhs->min_facet) ;
	  rhs->resp = lhs ;
          connected = true ;
          //std::cout << "connected path into " << "{" << resp->start << "---"  << resp->end << " | " << resp->multiple_seed << "," << resp->min_facet << "}"  << std::endl ;
	}
        return connected ;
      }
    } ;

    inline std::ostream& operator<<( std::ostream& out, const Geex::UFPath& p) {
      return out  << "{" << p.start << " --- " << p.end << " | " << p.multiple_seed << "," << p.min_facet << "}" ;
    }


    /*
     * Computes the restricted voronoi diagram using propagation.
     * This version uses simple cell clipping.
     */
    class RestrictedVoronoiDiagram_poly {
        typedef RestrictedVoronoiDiagram_poly thisclass ;

    protected:
        void init_algo(TopoPolyMesh* M, Delaunay* delaunay) {
            m = M ;
	    size_ = triangle_id_ ;
	    triangle_id_ = 0 ;
            seed_size_ = delaunay->nb_vertices() ;
	    seed_stacked_.assign(seed_size_,false) ;
	    seed_corners_.assign(seed_size_,(std::set<CellCorner>*) nil) ;
	    facet_seed_.assign(m->nb_facets(),-1) ;
            //TODO this can be insufficient for non triangular facets
	    facet_paths_.assign(3*m->nb_facets(),(UFPath*) nil) ;
        }


    public:
        RestrictedVoronoiDiagram_poly() : 
            m(nil), delaunay_(nil), symbolic_(false), exact_(false){ 
        }
        
        RestrictedVoronoiDiagram_poly(Delaunay* delaunay_in, TopoPolyMesh* m_in) : 
            m(m_in), delaunay_(delaunay_in), symbolic_(false), exact_(false) { 
	  }
        
        // In symbolic mode, each generated vertex knows the ID's of the planes
        // it belongs to.
        void set_symbolic(bool x) { symbolic_ = x ; }

        bool symbolic() const { return symbolic_ ; }

        // In exact mode, all combinatorial decisions (predicates) use exact
        // arithmetics.
        void set_exact(bool x) { exact_ = x ; }

        bool exact() const { return exact_ ; }

	unsigned int triangle_id() { return triangle_id_ ; }

	unsigned int size() { return size_ ; }

        const TopoPolyMesh* mesh() const { return m ; }
        TopoPolyMesh* mesh() { return m ; }

        const Delaunay* delaunay() const { return delaunay_ ; }
        Delaunay* delaunay() { return delaunay_ ; }

        // Low-level API. Client code may use for_each_facet(), for_each_triangle() or
        // for_each_primal_triangle() instead.
        template <class DEL, class ACTION> inline void 
        compute(
            TopoPolyMesh* M, DEL* delaunay, const DelaunaySkeleton* skel, const ACTION& action
        ) {
            current_mesh_ = nil ;
	    init_algo(M,delaunay) ;
            for(unsigned int i=0; i<M->nb_facets(); i++) {
              if(!facet_visited(i)) {
		//initialize by taking the first non visited facet and its
		//nearest seed
                unsigned int cell = find_seed_near_facet(delaunay, M, i) ;
		//the propagation is done using cell corners stored in sets for
		//each vertex. We initialize by storing a fake cell corner
		//which won't bother the rest of the algorithm
	        CellCorner fakecorner(i,0,0,0) ;
	        push_seed(cell, fakecorner) ;
		//iteration on the propagated seeds
	        while(!seedStack.empty()) {
                  TopoPolyMesh* ping = &M1 ;
                  TopoPolyMesh* pong = &M2 ;

		  //unstack a seed
	          unsigned int c = seedStack.front() ;
		  current_seed_ = c ;
		  //std::cout << "======== new seed " << c << " ========" << std::endl ;
	          seedStack.pop() ;
		  //iteration on the seed corners
	          while(!seed_corners_[c]->empty()) {
		    //start a propagation from the facet of a corner
	            push_facet(seed_corners_[c]->begin()->f, c) ;

		    //std::cout << "treating corner " << seed_corners_[c]->begin()->f << " " << seed_corners_[c]->begin()->c1 << " " << seed_corners_[c]->begin()->c2 << " " << seed_corners_[c]->begin()->c3 << " for cell " << c << std::endl ;
		    //propagation
	            while(!facetStack.empty()) {
		      //unstack a facet
	              unsigned int f = facetStack.top() ;
		      current_facet_ = f ;
	              facetStack.pop() ;

	              ping->clear() ;
	              M->copy_facet(f, *ping) ;

		      //clip the facet by the seed
	              clip_by_cell(skel, c, ping, pong) ;

		      //apply the treatment on the intersection
	              action(c, ping) ;
		      triangle_id_++ ;

		      //corner propagation
                      //std::cout << "bisectors: " ;
                      for(unsigned int i = 0; i<ping->nb_vertices(); ++i) {
                        //std::cout << "{" << ping->vertex(i).sym.nb_bisectors() << "," << ping->vertex(i).f << "} " ;
                        //test if a corner was found
                        if(ping->vertex(i).sym.nb_bisectors() == 2) {
                          //build the corner
                          int c1 = ping->vertex(i).sym[1]-1 ;
                          int c2 = ping->vertex(i).sym[2]-1 ;
                          CellCorner corner(f, c, c1,c2) ;
                          if(seed_corners_[c]->count(corner)==0) {
                            //the corner was unknown, propagate it to the two
                            //neighbouring seeds for seed propagation
                            //std::cout << "pushing corner " << corner << std::endl ;
                            push_seed(c1,corner) ;
                            push_seed(c2,corner) ;
                          } else {
                            //the corner was known, don't propagate it, and
                            //delete it from the corners of the current seed to
                            //avoid computing twice the same connected
                            //component for this seed's restricted voronoi cell
                            //std::cout << "erasing corner " << corner << std::endl ;
                            seed_corners_[c]->erase(corner) ;
                          }
                        } else {
                          //propagate the connected component computation to
                          //the neighbouring facets on the boundary of the
                          //intersection
                          int neigh_f = ping->vertex(i).f ;
                          if(neigh_f >= 0 && neigh_f != f) push_facet(neigh_f, c) ;
                        }
                      }
                      //std::cout << std::endl ;

		      //extraction of the boundary path of the cell
		      extract_paths(f, ping) ;

	            }
		    //propagate the boundary paths which do not go through any
		    //corner vertex, and therefore could not be propagated
		    //using the above corner propagation.
		    propagate_paths(c) ;

		    //erase fake corner if any, since it was not discovered as
		    //a corner of the aforecmoputed connected component, and
		    //therefore would make this loop infinite
	            seed_corners_[c]->erase(fakecorner) ;
	          }
		  clean_seed_data(c) ;
	        }
              }
            }

            // Final cleanup: unmark all triangles
            for(unsigned int f=0; f<M->nb_facets(); f++) {
                M->unmark_facet(f) ;
            }
            current_mesh_ = nil ;
        }

	void extract_paths(unsigned int facet, TopoPolyMesh* polygon) {
	  //explores the boundary of the intersection of a facet with a cell,
	  //finds the parts corresponding to the restricted cell boundary,
	  //combines the paths together, and stacks the facet for further
	  //extraction of the restricted cell boundary.

	  //The contour alternates between paths on the boundary of the facet,
	  //and paths on the bounrady of the restricted cell. The switch
	  //between the two cases occurs on a 1 bisector type vertex. Let's
	  //find one

      	  unsigned int start_index = 0 ;
      	  bool intersect_edge = false ;
      	  while(start_index<polygon->nb_vertices()) {
      	    if(polygon->vertex(start_index).sym.nb_bisectors() == 1) {
      	      intersect_edge = true ;
      	      break ;
      	    }
      	    ++start_index ;
      	  }

	  //If no starting vertex was found, the restricted cell is completely
	  //within the triangle, or the triangle is completely in the voronoi
	  //cell. In the first case, for convexity reasons, the cell has more
	  //than one neighbouring cell, and is a topological disk. The
	  //propagation will therefore have no problems. In the second case,
	  //the facet is not useful for the computation of the boundary.
	  //Therefore no further treatment is required

      	  if(intersect_edge) {
	    save_facet(facet) ;

	    //Once such a vertex found, we must determine whether the following
	    //path is on the triangle boundary or on the cell boundary. The
	    //next vertex is therefore checked to see whether it is a triangle
	    //vertex, or belongs to the same edge.  In such case the path is a
	    //triangle boundary path, otherwise it is on the restricted cell
	    //boundary.

      	    bool out = false ;
      	    TopoPolyVertexEdge& start = polygon->vertex(start_index) ;
      	    TopoPolyVertexEdge& next = polygon->vertex(start_index+1) ;

      	    out = (
      	      next.sym.nb_bisectors() == 0 || (
      	        next.sym.nb_bisectors() == 1 &&
      	        next.sym[0] == start.sym[0] &&
      	        next.sym[1] == start.sym[1]
      	      )
      	    ) ;

	    //iteration on the path vertices
	    int current_index = start_index ;
	    //true if a 2 bisector vertex was encountered in the path
	    bool found_corner = false ;
	    //information about the endpoint beginning the path
	    PathEnd last_end(polygon->vertex(start_index)) ;
	    unsigned int last_f ;
	    last_f = last_end.f1 == facet ? last_end.f2 : last_end.f1 ;
	    do{
	      //get the next vertex of the polygon
	      current_index = (current_index+1) % polygon->nb_vertices() ;
	      TopoPolyVertexEdge& current = polygon->vertex(current_index) ;

	      if(current.sym.nb_bisectors() == 1) { //a path ends
		//Build the path endpoint
		PathEnd next_end(current) ;
		unsigned int next_f ;
		next_f = next_end.f1 == facet ? next_end.f2 : next_end.f1 ;
		if(!out) { //the path is a cell boundary one
		  //build the path object
		  UFPath* path = new UFPath(last_end, next_end, found_corner) ;
		  //std::cout << "built a new path " << *path << " for facet " << facet << std::endl ;
		  //Add the path to the facet
		  add_path_to_facet(facet,path) ;
		  //merge it to the neighbouring facets
		  if(last_f<m->nb_facets())
		    merge_path_to_facet(last_f,path) ;
		  if(next_f<m->nb_facets())
		    merge_path_to_facet(next_f,path) ;
		}
		//reset the corner check variable
		found_corner = false ;
		//update the last end
		last_end = next_end ;
		last_f = next_f ;
		//switch from boundary path to triangle path and vice versa
		out = !out ;
	      } else if (current.sym.nb_bisectors() == 2) {
		found_corner = true ;
	      }
	    } while(current_index != start_index) ;
      	        
      	  }
	}

	void merge_path_to_facet(unsigned int facet, UFPath* path) {
	  //merges a given path with those of a facet
	  //iteration over all the paths of the facet
	  for(int i=0; i<3 && facet_paths_[3*facet+i] != nil; ++i) {
	    //std::cout << "looking for path at " << facet_paths_[3*facet+i] << " for facet " << facet << std::endl ;
	    facet_paths_[3*facet+i]->connect(path) ;
	  }
	}

	void add_path_to_facet(unsigned int facet, UFPath* path) {
	  //find an empty position to store the path for this facet
	  int offset = 0 ;
	  while(facet_paths_[3*facet+offset] != nil) {
	    ++offset ;
	  }
	  //theoretically, no facet contains more than 3 paths
	  gx_assert(offset<3) ;
	  //add the path to the facet
	  facet_paths_[3*facet+offset] = path ;
	}

	void propagate_paths(unsigned int cell) {
	  //TODO this step could be fastened since this kind of propagation
	  //only occurs sparsely. A good way would be to store the number of
	  //independent paths for this cell, and if a corner was encountered.
	  //This way, in the far most usual case when a single path with
	  //corners occurs, a simple test could avoid this step.

	  //propate the distinct responsible paths if they have no corners
	  for(int i=0; i<recordFacet.size(); ++i) {
	    unsigned int facet = recordFacet[i] ;
	    for(int j=0; j<3 && facet_paths_[3*facet+j] != nil; ++j) {
	      UFPath* path = facet_paths_[3*facet+j] ;
	      if(path->is_resp() && !path->multiple_seed) {
		//the path is responsible for its class, and crosses no 2
		//bisector vertices
		unsigned int opposite_cell = path->start.s ;
		CellCorner fakecorner(path->min_facet, cell, opposite_cell, 0) ;
		if(seed_corners_[cell]->count(fakecorner) == 0) {
		  //propagation was not done previously from the opposite cell
		  //std::cout << "propagating fake corner " << fakecorner << " to cell " << opposite_cell << " from facet " << facet << " from path at " << path << std::endl ;
		  push_seed(opposite_cell,fakecorner) ;
		} else {
		  //std::cout << "deleting fake corner " << fakecorner << std::endl ;
		  //propagation already done, suppress the corner
		  seed_corners_[cell]->erase(fakecorner) ;
		}
	      }
	    }
	  }

	  //std::cout << "cleaning paths" << std::endl ;
	  //clean the paths and the record facet structure
	  for(int i=0; i<recordFacet.size(); ++i) {
	    unsigned int facet = recordFacet[i] ;
	    //std::cout << "cleaning paths of facet " << facet << std::endl ;
	    for(int j=0; j<3 && facet_paths_[3*facet+j] != nil; ++j) {
	      //std::cout << "deleting path at " << facet_paths_[3*facet+j] << " for facet " << facet << std::endl ;
	      delete facet_paths_[3*facet+j] ;
	      facet_paths_[3*facet+j] = (UFPath*) nil ;
	    }
	  }
	  //drop the recorded facets
	  recordFacet.resize(0) ;
	}


        // ACTION needs to implement 
        //      operator()(unsigned int c, TopoPolyMesh* M) const
        //   where c denotes the index of the current Voronoi cell (or Delaunay vertex).
        template <class ACTION> inline void for_each_facet(const ACTION& action) {
            if(exact_) {
                RVD_predicates::begin_stats() ;
            }
            this->template compute<Delaunay, ACTION>(m, delaunay_, delaunay_->skeleton(), action) ;
            if(exact_) {
                RVD_predicates::end_stats() ;
            }
        }

        // ACTION needs to implement:
        //      operator()(unsigned int c, const TopoPolyVertexEdge& v1, v2, v3) const
        //   where c denotes the index of the current Voronoi cell (or Delaunay vertex).
        template <class TRIACTION> inline void for_each_triangle(const TRIACTION& action) {
            if(exact_) {
                RVD_predicates::begin_stats() ;
            }
            this->template compute<Delaunay, TriangleAction<TRIACTION> >(
                m, delaunay_, delaunay_->skeleton(), TriangleAction<TRIACTION>(action)
            ) ;
            if(exact_) {
                RVD_predicates::end_stats() ;
            }
        }

        // ACTION needs to implement:
        //      operator()(unsigned int c, const TopoPolyVertexEdge& v1, v2) const
        //   where c denotes the index of the current Voronoi cell (or Delaunay vertex).
        template <class HEACTION> inline void for_each_halfedge(const HEACTION& action) {
            if(exact_) {
                RVD_predicates::begin_stats() ;
            }
            this->template compute<Delaunay, HalfedgeAction<HEACTION> >(
                m, delaunay_, delaunay_->skeleton(), HalfedgeAction<HEACTION>(action)
            ) ;
            if(exact_) {
                RVD_predicates::end_stats() ;
            }
        }

        // ACTION needs to implement:
        //      operator()(unsigned int i, unsigned j, unsigned int k) const
        //   where i,j,k denote the three indices of the Delaunay vertices 
        //   that define the primal triangle.
        template <class PRIMTRIACTION> inline void for_each_primal_triangle(
            const PRIMTRIACTION& action
        ) {
            if(exact_) {
                RVD_predicates::begin_stats() ;
            }
            // PrimalTriangleAction needs symbolic mode
            bool sym_backup = symbolic_ ;
            symbolic_ = true ;
            this->template compute<Delaunay, PrimalTriangleAction<PRIMTRIACTION> >(
                m, delaunay_, delaunay_->skeleton(), PrimalTriangleAction<PRIMTRIACTION>(action)
            ) ;
            symbolic_ = sym_backup ;
            if(exact_) {
                RVD_predicates::end_stats() ;
            }
        }

        unsigned int current_facet() const { return current_facet_ ; }
        unsigned int current_seed() const { return current_seed_ ; }
        TopoPolyMesh* current_mesh() const { return current_mesh_ ; }

    protected:

        void clip_by_cell(
            const DelaunaySkeleton* skel, 
            unsigned int v, 
            TopoPolyMesh*& ping, TopoPolyMesh*& pong
        ) {
	    ////reset the bisectors array
	    //edge_bisectors1_.resize(0) ;
	    //edge_bisectors2_.resize(0) ;

	    ////-1 means an edge of the triangle and not a bisector
	    //edge_bisectors1_.assign(ping->nb_vertices(),-1) ;

            for(unsigned int i=skel->star_begin(v); i<skel->star_end(v); i++) {
                unsigned int w = skel->neighbor(i) ;
                const Plane<real>& P = skel->bisector(i) ;
                pong->clear() ;
                clip_by_plane(ping, pong, P, v, w) ;
                gx_swap(ping, pong) ;
		//edge_bisectors1_.swap(edge_bisectors2_) ;
		//edge_bisectors2_.resize(0) ;
            }

	    //if(do_push) {
	    //  unsigned int facet = ping->facet_info(0).id ;

	    //  //for each bisector in the edge_bisectors array, propagate
	    //  for(unsigned int i=0; i<edge_bisectors1_.size(); i++) {
	    //      unsigned int bisector = edge_bisectors1_[i] ;
	    //      if((bisector+1) != 0)
	    //        push_front(facet, bisector) ;
	    //  }
	    //}
        }

        Sign side(
            const Geex::Plane<real>& P, 
            const TopoPolyVertexEdge& v,
            unsigned int cell_in, unsigned int cell_out
        ) const {
            //Sign result ;
            if(exact_) {
                vec3 p1 = delaunay_->vertex(cell_in) ;
                vec3 p2 = delaunay_->vertex(cell_out) ;
                return RVD_predicates::side(p1, p2, v, this) ; 
            } 
            return (P.side(v) > 0.0 ? POSITIVE : NEGATIVE) ; 
        }

        void clip_by_plane(
            TopoPolyMesh* ping, TopoPolyMesh* pong, 
            const Geex::Plane<real>& P, 
            unsigned int cell_in, unsigned int cell_out
        ) {
            for(unsigned int f=0; f<ping->nb_facets(); f++) {

                bool crosses = false ;

                unsigned int last_i = ping->facet_end(f) - 1 ;
                const TopoPolyVertexEdge* last_v = &(ping->vertex(last_i)) ;
                Sign last_status = side(P, *last_v, cell_in, cell_out) ;

                for(
                    unsigned int i=ping->facet_begin(f); 
                    i<ping->facet_end(f); i++
                ) {
                    const TopoPolyVertexEdge* v = &(ping->vertex(i)) ;
                    Sign status = side(P, *v, cell_in, cell_out) ; 
                    
                    if(status == 0) { // Can only occur in exact mode
                        crosses = true ;
                        if(!pong->in_facet()) { pong->begin_facet() ; }
                        pong->add_vertex(*v) ;
                        // Commented-out for the moment, check 
                        // that sym size is 10 in symbolic_vertex.h
                        // before reactivating. 
                        // pong->top_vertex().sym.add_bisector(cell_out) ; 
                    } else {

                        if(status != last_status) {
                            TopoPolyVertexEdge I ;
                            ping->intersect(I, *last_v, *v, P, cell_out) ;
                            if(!pong->in_facet()) { pong->begin_facet() ; }
                            pong->add_vertex(I) ;
                            if(status > 0) {
				// Here the edge crosses the bisector from out to in
                                pong->top_vertex().set_edge(*last_v) ;
				//// The bisector is added to the edge_bisectors
				//if(do_push)
				//  edge_bisectors2_.push_back(cell_out) ;
                            } else {
				// Here the edge crosses the bisector from in to out
                                pong->top_vertex().set_flag(
                                    TopoPolyVertexEdge::INTERSECT
                                ) ;
				//// The bisector corresponding to the edge is added
				//if(do_push)
				//  edge_bisectors2_.push_back(edge_bisectors1_[i]) ;
                            }
                        }

                        if(status > 0) {
                            if(!pong->in_facet()) { pong->begin_facet() ; }
                            pong->add_vertex(*v) ;
			    //if(do_push)
			    //  //just copy the previous edge bisectors array
			    //  edge_bisectors2_.push_back(edge_bisectors1_[i]) ;
                        } else {
                            crosses = true ;
                        }
                    }
                    last_v = v ; last_status = status ; last_i = i ; 
                }
                if(pong->in_facet())  { 
                    pong->end_facet() ;  
                    pong->facet_info(pong->top()).id = ping->facet_info(f).id ;
                } 
            }
        }

        void visit(unsigned int f) {
	    //mark the facet as being in facetStack
            m->mark_facet(f) ;
        }

        bool facet_visited(unsigned int f) {
	  //test whether the facet was previously handled. This is used to
	  //restart the propagation when a connected component of the initial
	  //mesh is finished, and one has to find a facet on an other connected
	  //component.
	  return m->facet_is_marked(f) ;
        }

        bool seed_stacked(unsigned int cell) {
	  //test whether the seed is already in seedStack
	  return seed_stacked_[cell] ;
        }

	void clean_seed_data(unsigned int cell) {
	  //clear the seed's corner structure to free memory, and mark this
	  //seed as stackable again if any other connected component is
	  //discovered.
	  delete seed_corners_[cell] ;
	  seed_corners_[cell] = nil ;
	  seed_stacked_[cell] = false ;
	}

	void push_seed(unsigned int cell, CellCorner corner) {
	  //propagate a corner to a seed
	  if(seed_corners_[cell] == nil) {
	    //if no corner structure built, initialize it
	    seed_corners_[cell] = new std::set<CellCorner> ;
	  }
	  //add the corner to the seed's structure
	  seed_corners_[cell]->insert(corner) ;
	  if(!seed_stacked_[cell]) {
	    //if the seed was not previously stacked stack it. 
	    //TODO Theoretically since a seed in the stacked has an initializes
	    //corner structure, which could serve as a test and avoid the
	    //seed_stacked vector
	    seedStack.push(cell) ;
	    seed_stacked_[cell] = true ;
	  }
	}

	void push_facet(unsigned int f, unsigned int cell) {
	  //stack a facet in facetStack for a seed's connected component's
	  //propagation
	  visit(f) ;
	  if(facet_seed_[f] != cell) {
	    //if the facet's seed is already the current seed, it is already in
	    //the stack, otherwise update the facet's current seed, and push it
	    facetStack.push(f) ;
	    facet_seed_[f] = cell ;
	  }
	}

	void save_facet(unsigned int f) {
	  //save a facet for further computations
	  recordFacet.push_back(f) ;
	}

    private:
	//propagate the seed on the surface in a breadth first manner
        std::queue<unsigned int> seedStack ;
	//propagate the facets of a seed's connected component
        std::stack<unsigned int> facetStack ;
	//record the facets intersecting the boundary of a cell
	std::vector<unsigned int> recordFacet ;
	//the meshes used to compute the intersections of facets and cells
        TopoPolyMesh M1, M2 ;
	//structure to reconstruct the path of the boundary of a voronoi cell
	//A map would be less memory consuming but slower for facet paths
	std::vector<UFPath*> facet_paths_ ;
	//structures to avoid stacking twice the same seed or facet
        std::vector<bool> seed_stacked_ ;
	std::vector<int> facet_seed_ ;
	//A map would be less memory consuming but slower for seed corners
	std::vector<std::set<CellCorner>*> seed_corners_ ;
	int seed_size_ ;
        TopoPolyMesh* m ;
        Delaunay* delaunay_ ;
	unsigned int triangle_id_ ;
	unsigned int size_ ;
        bool symbolic_ ;
        bool exact_ ;
        unsigned int current_facet_ ;
        unsigned int current_seed_ ;
        TopoPolyMesh* current_mesh_ ;

    private:
        RestrictedVoronoiDiagram_poly(const thisclass& rhs) ;
        thisclass& operator=(const thisclass& rhs) ;        
    } ;
}

#endif 
