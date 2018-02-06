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

#ifndef __GEEX_CUSTOM_RVD__
#define __GEEX_CUSTOM_RVD__

#include <Geex/CVT/topo_poly_mesh.h>
#include <Geex/CVT/delaunay_skel.h>
#include <Geex/CVT/delaunay.h>
#include <Geex/CVT/RVD_predicates.h>
#include <stack>
#include <queue>
#include <set>

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

	// dmyan: for detecting gap triangles
	// Used by RVD internally to implement for_each_voronoi_vertex()
	template <class T> class VoronoiVertexAction {
	public:
		VoronoiVertexAction(const T& do_it_in) : do_it(do_it_in) { }
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
										do_it(iv1, iv2, iv3, ve) ;
										found = true ;
									} else if(ve2.sym.has_bisector(iv3) && ve3.sym.has_bisector(iv2)) {
										do_it(iv1, iv3, iv2, ve) ;
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


	struct TopoPolyStackItem {
		TopoPolyStackItem(
			unsigned int f_in, unsigned int cell_in
			) : f(f_in), cell(cell_in) { }
		unsigned int f ;
		unsigned int cell ;

		//bool operator< (TopoPolyStackItem& rhs) {
		//  return f < rhs.f ;
		//}
	} ;

	typedef std::deque< TopoPolyStackItem > TopoPolyStack ;

	/*
	class TopoPolyStack {
	public:
	bool empty() const {
	return stack_.empty() && queue_.empty() ;
	}

	TopoPolyStackItem& front() {
	if(stack_.empty()) {
	return queue_.front() ;
	} else {
	return stack_.top() ;
	}
	}

	void pop_front() {
	if(stack_.empty()) {
	queue_.pop() ;
	} else {
	stack_.pop() ;
	}
	}

	void push_back(const TopoPolyStackItem& x) {
	queue_.push(x) ;
	}

	void push_front(const TopoPolyStackItem& x) {
	stack_.push(x) ;
	}

	private:
	std::stack<TopoPolyStackItem> stack_ ;
	std::queue<TopoPolyStackItem> queue_ ;
	} ;
	*/


	/*
	* Computes the restricted voronoi diagram using propagation.
	* This version uses simple cell clipping.
	*/
	class RestrictedVoronoiDiagram_poly {
		typedef RestrictedVoronoiDiagram_poly thisclass ;

	protected:
		template<class Del>
		void init_visited(TopoPolyMesh* M, Del* delaunay) {
			m = M ;
			visited_size_ = delaunay->nb_vertices() ;
			seed_visited_.assign(visited_size_,-1) ;
			size_ = triangle_id_ ;
			triangle_id_ = 0 ;
		}


	public:
		RestrictedVoronoiDiagram_poly() : 
		  seed_visited_(nil), visited_size_(0), m(nil), delaunay_(nil), symbolic_(false), exact_(false){ 
		  }

		  RestrictedVoronoiDiagram_poly(Delaunay* delaunay_in, TopoPolyMesh* m_in) : 
		  seed_visited_(nil), visited_size_(0), m(m_in), delaunay_(delaunay_in), 
			  symbolic_(false), exact_(false) { 
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
				  init_visited(M,delaunay) ;
				  for(unsigned int i=0; i<M->nb_facets(); i++) {
					  if(!facet_visited(i)) {
						  unsigned int cell = find_seed_near_facet(delaunay, M, i) ;
						  push_back(i,cell) ;
						  while(!S.empty()) {
							  TopoPolyMesh* ping = &M1 ;
							  TopoPolyMesh* pong = &M2 ;
							  unsigned int f = S.front().f ;
							  unsigned int cell = S.front().cell ;

							  current_facet_ = f ;

							  S.pop_front() ;
							  ping->clear() ;
							  M->copy_facet(f, *ping) ;
							  visit(f,cell) ;

							  // Compute triangle clipped by cell and
							  // send neighbors to stack
							  clip_by_cell(skel, cell, ping, pong) ;
							  current_mesh_ = ping ;
							  // Apply action to triangles intersected by cell.
							  //if(ping->nb_vertices() == 0)
							  //  std::cout << "warning, empty couple facet/seed: " << f << " " << cell << std::endl ;
							  //else
							  triangle_id_++ ;
							  action(cell, ping) ;

							  for(unsigned int i=0; i<ping->nb_vertices(); i++) {
								  int neigh_f = ping->vertex(i).f ;
								  if(neigh_f >= 0 && neigh_f != f) push_back(neigh_f, cell) ;
							  }
						  }
					  }
				  }

				  // Final cleanup: unmark all triangles
				  for(unsigned int f=0; f<M->nb_facets(); f++) {
					  M->unmark_facet(f) ;
				  }
				  current_mesh_ = nil ;
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

		  // ACTION needs to implement:
		  //      operator()(unsigned int i, unsigned j, unsigned int k, TopoPolyVertexEdge& v) const
		  //   where i,j,k denote the three indices of the Delaunay vertices, that define the primal triangle.
		  //   v denotes the corresponding Restricted Voronoi vertex on surface
		  //   
		  template <class PRIMTRIACTION> inline void for_each_voronoi_vertex(
			  const PRIMTRIACTION& action
			  ) {
				  if(exact_) {
					  RVD_predicates::begin_stats() ;
				  }
				  // PrimalTriangleAction needs symbolic mode
				  bool sym_backup = symbolic_ ;
				  symbolic_ = true ;
				  this->template compute<Delaunay, VoronoiVertexAction<PRIMTRIACTION> >(
					  m, delaunay_, delaunay_->skeleton(), VoronoiVertexAction<PRIMTRIACTION>(action)
					  ) ;
				  symbolic_ = sym_backup ;
				  if(exact_) {
					  RVD_predicates::end_stats() ;
				  }
		  }

		  unsigned int current_facet() const { return current_facet_ ; }
		  TopoPolyMesh* current_mesh() const { return current_mesh_ ; }

		  //****************************JwGuo: extend to handle features and acclerate speed***********************
		  // ACTION needs to implement:
		  //      operator()(unsigned int c, const TopoPolyVertexEdge& v1, v2, v3) const
		  //   where c denotes the index of the current Voronoi cell (or Delaunay vertex).
		  /*template <class TRIACTION> inline void for_one_triangle(unsigned int& f, const TRIACTION& action) {
			  if(exact_) {
				  RVD_predicates::begin_stats() ;
			  }
			  this->template compute2<Delaunay, myTriangleAction<TRIACTION> >(
				  f, m, delaunay_, delaunay_->skeleton(), myTriangleAction<TRIACTION>(action)
				  ) ;
			  if(exact_) {
				  RVD_predicates::end_stats() ;
			  }
		  }*/
		  //****************************End***********************************************

	protected:

		void clip_by_cell(
			const DelaunaySkeleton* skel, 
			unsigned int v, 
			TopoPolyMesh*& ping, TopoPolyMesh*& pong,
			bool do_push = true
			) {
				//reset the bisectors array
				edge_bisectors1_.resize(0) ;
				edge_bisectors2_.resize(0) ;

				//-1 means an edge of the triangle and not a bisector
				edge_bisectors1_.assign(ping->nb_vertices(),-1) ;

				for(unsigned int i=skel->star_begin(v); i<skel->star_end(v); i++) {
					unsigned int w = skel->neighbor(i) ;
					const Plane<real>& P = skel->bisector(i) ;
					pong->clear() ;
					clip_by_plane(ping, pong, P, v, w) ;
					gx_swap(ping, pong) ;
					edge_bisectors1_.swap(edge_bisectors2_) ;
					edge_bisectors2_.resize(0) ;
				}

				if(do_push && ping->nb_vertices() != 0) {
					unsigned int facet = ping->facet_info(0).id ;

					//for each bisector in the edge_bisectors array, propagate
					for(unsigned int i=0; i<edge_bisectors1_.size(); i++) {
						unsigned int bisector = edge_bisectors1_[i] ;
						if((bisector+1) != 0)
							push_front(facet, bisector) ;
					}
				}
				//if(ping->nb_vertices() == 0)
					//std::cout << "Warning: empty intersection of mesh with cell" << std::endl ;
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
			unsigned int cell_in, unsigned int cell_out,
			bool do_push = true
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
									if(symbolic_ || exact_) {
										ping->intersect(I, *last_v, *v, P, cell_out) ;
									} else {
										ping->intersect(I, *last_v, *v, P) ;
									}
									if(!pong->in_facet()) { pong->begin_facet() ; }
									pong->add_vertex(I) ;
									if(status > 0) {
										// Here the edge crosses the bisector from out to in
										pong->top_vertex().set_edge(*last_v) ;
										// The bisector is added to the edge_bisectors
										if(do_push)
											edge_bisectors2_.push_back(cell_out) ;
									} else {
										// Here the edge crosses the bisector from in to out
										pong->top_vertex().set_flag(
											TopoPolyVertexEdge::INTERSECT
											) ;
										// The bisector corresponding to the edge is added
										if(do_push)
											edge_bisectors2_.push_back(edge_bisectors1_[i]) ;
									}
								}

								if(status > 0) {
									if(!pong->in_facet()) { pong->begin_facet() ; }
									pong->add_vertex(*v) ;
									if(do_push)
										//just copy the previous edge bisectors array
										edge_bisectors2_.push_back(edge_bisectors1_[i]) ;
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

		void visit(unsigned int f, unsigned int cell) {
			m->mark_facet(f) ;
			seed_visited_[cell] = f ;
		}

		bool has_visited(unsigned int f, unsigned int cell) {
			return m->facet_is_marked(f) || seed_visited_[cell] == f ;
		}

		bool facet_visited(unsigned int f) {
			return m->facet_is_marked(f) ;
		}

		bool seed_visited(unsigned int f, unsigned int cell) {
			return seed_visited_[cell] == f ;
		}

		void push_back(
			unsigned int f, 
			unsigned int cell
			) {
				if(facet_visited(f)) { return ; }
				m->mark_facet(f) ;
				S.push_back(TopoPolyStackItem(f, cell)) ;
		}

		void push_front(
			unsigned int f, 
			unsigned int cell
			) {
				if(seed_visited(f, cell)) { return ; }
				seed_visited_[cell] = f ;
				S.push_front(TopoPolyStackItem(f, cell)) ;
		}

	private:
		TopoPolyStack S ;
		TopoPolyMesh M1, M2 ;
		std::vector<unsigned int> seed_visited_ ;
		std::vector<unsigned int> edge_bisectors1_ ;
		std::vector<unsigned int> edge_bisectors2_ ;
		unsigned int visited_size_ ;
		TopoPolyMesh* m ;
		Delaunay* delaunay_ ;
		bool symbolic_ ;
		bool exact_ ;
		unsigned int current_facet_ ;
		TopoPolyMesh* current_mesh_ ;

		unsigned int triangle_id_ ;
		unsigned int size_ ;

	private:
		RestrictedVoronoiDiagram_poly(const thisclass& rhs) ;
		thisclass& operator=(const thisclass& rhs) ;        
	} ;
}

#endif
