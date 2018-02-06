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

#include <Geex/CVT/regular_CGAL.h>
#include <Geex/CVT/delaunay_skel_CGAL.h>
#include <Geex/CVT/CVT_spatial_sort.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#include <CGAL/Regular_triangulation_filtered_traits_3.h> 
#include <CGAL/Regular_triangulation_3.h>

#include <algorithm>
#include <fstream>

namespace Geex {

	namespace MyCGALStuff_Regular {

		// ------------- My stuff to extend CGAL --------------------------

		// Used to mark each individual halfedge of a tetrahedron.
		// Each halfedge is identified by its two vertices's local
		// indices (in the range 0..3). Halfedge flags are packed
		// in a 16 bits integer (there are 12 halfedges in a tet).
		class HalfedgeFlags {
		public:
			HalfedgeFlags() : flags_(0) { }
			void mark(unsigned int i, unsigned int j) {
				gx_debug_assert(i < 4 && j < 4) ;
				flags_ |= mask_[i][j] ;
			}
			bool is_marked(unsigned int i, unsigned int j) const {
				gx_debug_assert(i < 4 && j < 4) ;                
				return ((flags_ & mask_[i][j]) != 0) ;
			}
			void clear() { flags_ = 0 ; }
		private:
			unsigned short flags_ ; // 16 bits
			static unsigned short mask_[4][4] ;
		} ;

		// Pre-computed masks for marking and testing 
		// an individual halfedge flag.
		unsigned short HalfedgeFlags::mask_[4][4] = {
			{0,      1,    2,   4},
			{8,      0,   16,  32},
			{64,   128,    0, 256},
			{512, 1024, 2048,   0}
		} ;

		// ------------- CGAL stuff ---------------------------------        

		// ----------------------- A CGAL::Vertex with decoration ------------------
		template < class Gt, class Vb = CGAL::Triangulation_vertex_base_3<Gt> >
		class Vertex : public  Vb {
			typedef Vb superclass;
		public:
			typedef typename Vb::Vertex_handle      Vertex_handle;
			typedef typename Vb::Cell_handle        Cell_handle;
			typedef typename Vb::Point              Point;

			template < typename TDS2 >
			struct Rebind_TDS {
				typedef typename Vb::template Rebind_TDS<TDS2>::Other Vb2;
				typedef Vertex<Gt,Vb2> Other;
			} ;

		public:
			Vertex() : superclass(), id(-1) {}
			Vertex(const Point & p) : superclass(p), id(-1) {}
			Vertex(const Point & p, Cell_handle f) : superclass(f,p), id(-1) {}
			Vertex(Cell_handle f) : superclass(f), id(-1) {}

			int id ;
		} ;


		// ----------------------- A CGAL::Cell with decoration ------------------

		template < class Gt, class Cb = CGAL::Triangulation_cell_base_3<Gt> >
		class Cell : public Cb {
			typedef Cb superclass;
		public:
			typedef typename Cb::Vertex_handle      Vertex_handle;
			typedef typename Cb::Cell_handle        Cell_handle;
			template < typename TDS2 >
			struct Rebind_TDS {
				typedef typename Cb::template Rebind_TDS<TDS2>::Other Cb2;
				typedef Cell<Gt,Cb2> Other;
			} ;


			Cell() : superclass() { clear_cicl(); }
			Cell(
				Vertex_handle v0, Vertex_handle v1, Vertex_handle v2, Vertex_handle v3
				) : superclass(v0,v1,v2,v3) { clear_cicl(); }

			Cell(
				Vertex_handle v0, Vertex_handle v1, Vertex_handle v2, Vertex_handle v3,
				Cell_handle n0, Cell_handle n1, Cell_handle n2, Cell_handle n3
				) : superclass(v0,v1,v2,v3,n0,n1,n2,n3) { clear_cicl(); }

			// Clears Circular Incident Cell List
			void clear_cicl() {
				next_around_vertex_[0] = 0 ;
				next_around_vertex_[1] = 0 ;
				next_around_vertex_[2] = 0 ;
				next_around_vertex_[3] = 0 ;
				cc_computed_ = false;
			}

			// next_around_vertex stores a circular list
			// of cells incident to a given vertex.
			// It can be traversed as follows:
			// Cell_handle c = v->cell() ;
			// do {
			//   do something with c
			//   c = c->next_around_vertex(c->index(v)) ;
			// } while(c != v->cell()) ;

			Cell_handle next_around_vertex(unsigned int i) const {
				gx_debug_assert(i < 4) ;
				return next_around_vertex_[i] ;
			}

			void set_next_around_vertex(unsigned int i, Cell_handle c) {
				gx_debug_assert(i < 4) ;
				next_around_vertex_[i] = c ;
			}

			HalfedgeFlags& halfedge_flags() { return halfedge_flags_ ; }

			const bool & is_circumcenter_computed() const {return cc_computed_; }
			bool & is_circumcenter_computed() {return cc_computed_; }
			const vec3 & self_circumcenter() const { return circumcenter_; }
			vec3 & self_circumcenter() { return circumcenter_; }

			const int & cell_id() const { return id_; }
			int & cell_id() { return id_; }

		private:
			Cell_handle next_around_vertex_[4] ;
			HalfedgeFlags halfedge_flags_ ;

			vec3 circumcenter_ ;
			bool cc_computed_;
			int id_;

		} ;

		typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
		typedef CGAL::Regular_triangulation_filtered_traits_3<K>    Traits;
		typedef CGAL::Regular_triangulation_cell_base_3<Traits, Cell<Traits> > Cb;
		typedef Vertex<Traits> Vb ;
		typedef CGAL::Triangulation_data_structure_3<Vb,Cb> TDS;
		typedef CGAL::Regular_triangulation_3<Traits, TDS>          TRI; 
		typedef Traits::RT                                          Weight;
		typedef Traits::Bare_point                                  Point;
		typedef Traits::Weighted_point                              Weighted_point;

		inline vec3 to_geex(const Point& p) {
			return vec3(
				p.cartesian(0), p.cartesian(1), p.cartesian(2)
				) ;
		}

		// Used by spatial sort.
		struct PointWithId {
			Weighted_point pt ;
			int id ;
		} ;

		//      class DelaunayBase : public CGAL::Triangulation_hierarchy_3<TRI> {
		class DelaunayBase : public TRI {
		public:
			// Members for spatial sorting.
			std::vector<PointWithId> sorted_points_ ;
			std::vector<Vertex_handle> vertices_ ; 

			// Initialize circular incident cell lists.
			void init_cicl() {
				for(Vertex_iterator it = vertices_begin(); it != vertices_end(); it++) {
					Cell_handle c = it->cell() ;
					c->set_next_around_vertex(c->index(it), c) ;
				}
				for(Cell_iterator it = cells_begin(); it != cells_end(); it++) {
					for(unsigned int i=0; i<4; i++) {
						Vertex_handle v = it->vertex(i) ;
						if(v->cell() != it) {
							Cell_handle c1 = v->cell() ;
							unsigned int iv1 = c1->index(v) ;
							Cell_handle c2 = c1->next_around_vertex(iv1) ;
							c1->set_next_around_vertex(iv1,it) ;
							it->set_next_around_vertex(i,c2) ;
						}
					}
				}
			}

			bool edge_is_marked(const Edge& e) const {
				return e.first->halfedge_flags().is_marked(e.second, e.third) ;
			}

			// Marks all the halfedges in the tets incident to edge e.
			//   Note1: orientation is taken into account, i.e.
			// (v1,v2) is different from (v2,v1) and can be marked independently.
			//   Note2: we use our next_around_halfedge static table rather than
			// CGAL's circulator since doing so enables us to keep track of local
			// indices and saves some computations (see timings below).
			void mark_edge(const Edge& e) {
				Cell_handle c = e.first ; 
				unsigned int iv1 = e.second ;
				unsigned int iv2 = e.third ;
				Vertex_handle v1 = c->vertex(iv1) ;
				Vertex_handle v2 = c->vertex(iv2) ;
				Cell_handle cir = c ;
				do {
					iv1 = cir->index(v1) ;
					iv2 = cir->index(v2) ;
					cir->halfedge_flags().mark(iv1, iv2) ;
					cir = cir->neighbor(next_around_halfedge[iv1][iv2]) ;
				} while(cir != c) ;
			}

		public:
			static unsigned int next_around_halfedge[4][4] ;
		} ;

		// Pre-computed table for turning around halfedges in
		// a tetrahedron. Given a cell c and an halfedge (v1, v2), 
		// c->neighbor(next_around_halfedge[v1][v2]) is the cell
		// adjacent to c on the left of (v1,v2).
		// Diagonal entries are not supposed to be accessed.
		unsigned int DelaunayBase::next_around_halfedge[4][4] = {
			{5, 3, 1, 2},
			{2, 5, 3, 0},
			{3, 0, 5, 1},
			{1, 2, 0, 5}
		} ;


	} // end namespace CGAL 


	class Regular_CGAL::Implementation : public ::Geex::MyCGALStuff_Regular::DelaunayBase {
	} ;

	Regular_CGAL::Regular_CGAL(bool use_spatial_sort) : use_spatial_sort_(use_spatial_sort) {
		impl_ = new Implementation ;
	}

	Regular_CGAL::~Regular_CGAL() {
		delete impl_ ;
	}

	void Regular_CGAL::set_vertices(unsigned int n, const double* xyz) {
		vertices_ = xyz ;
		nb_vertices_ = n ;
		impl_->clear() ;

		if(use_spatial_sort_) {

			bool first_time = false;

			if ( (int) impl_->sorted_points_.size() != n) {
				impl_->sorted_points_.resize(n) ;
				first_time = true;
				for(unsigned int i=0; i<n; i++) {
					impl_->sorted_points_[i].id = i ;
				}
			}

			if ( (int) impl_->vertices_.size() != n)
				impl_->vertices_.resize(n);

			int count = 0;
			for(unsigned int i = 0; i < n; i++) {
				count = 3*impl_->sorted_points_[i].id;
				impl_->sorted_points_[i].pt = MyCGALStuff_Regular::Weighted_point(
					MyCGALStuff_Regular::Point(
					xyz[count], xyz[count+1], xyz[count+2]), 
					weight_?weight_[impl_->sorted_points_[i].id]:0.0
				) ;
			}

			if (first_time) {
				std::random_shuffle(
					impl_->sorted_points_.begin(), impl_->sorted_points_.end()
					) ;
			}

			CGAL::CVT_spatial_sort_3(
				impl_->sorted_points_.begin(), impl_->sorted_points_.end(), MyCGALStuff_Regular::K()
				) ;

			Implementation::Cell_handle ch;
			Implementation::Vertex_handle v;

			for (unsigned int i=0; i<n; i++) {
				int id = impl_->sorted_points_[i].id ; 
				v = impl_->insert(impl_->sorted_points_[i].pt, ch);
				if(v == Implementation::Vertex_handle() || v->id != -1 ) {
					v = nil;
				}
				else
				{
					ch = v->cell();
					v->id = id ;
				}
				impl_->vertices_[id] = v;
			}

		} else {

			impl_->vertices_.resize(n);
			impl_->vertices_.assign(n,nil);

			Implementation::Vertex_handle v;
			for(unsigned int i=0; i<n; i++) {
				v = impl_->insert(
					MyCGALStuff_Regular::Weighted_point(
					MyCGALStuff_Regular::Point(xyz[3*i], xyz[3*i+1], xyz[3*i+2]), 
					weight_?weight_[i]:0.0) );
				//This test only finds the vertices out of the triangulation recognized as so
				//at the moment of the insertion, not those kicked out afterwards when other 
				//points are inserted
				//if(v == Implementation::Vertex_handle() || v->id != -1 ) {
				//	v = nil;
				//}
				if(v != Implementation::Vertex_handle() && v->id == -1 ) {
				  v->id = i ;
				}
			}
			Implementation::Finite_vertices_iterator vi ;
			for(vi = impl_->finite_vertices_begin(); vi != impl_->finite_vertices_end(); ++vi) {
				Implementation::Vertex_handle v(vi) ;
				impl_->vertices_[v->id] = v ;
			}
		}

		skeleton_dirty_ = true ;
		cell_id_tag_ = false;
	}

	void Regular_CGAL::insert_vertices(unsigned int pos, unsigned int n, const double* xyz) {
		vertices_ = xyz ;
		nb_vertices_ += n ;
		//impl_->vertices_.resize(nb_vertices_, nil) ;

		Implementation::Vertex_handle v;
		for(unsigned int i=pos; i<pos+n; i++) {
			v = impl_->insert(
				MyCGALStuff_Regular::Weighted_point(
				MyCGALStuff_Regular::Point(xyz[3*i], xyz[3*i+1], xyz[3*i+2]), 
				weight_?weight_[i]:0.0) );
			//This test only finds the vertices out of the triangulation recognized as so
			//at the moment of the insertion, not those kicked out afterwards when other 
			//points are inserted
			//if(v == Implementation::Vertex_handle() || v->id != -1 ) {
			//	v = nil;
			//}
			if(v != Implementation::Vertex_handle() && v->id == -1 ) {
				v->id = i ;
				gx_assert(v->id == impl_->vertices_.size()) ;
				impl_->vertices_.push_back(v) ;				
			}
		}
		//Implementation::Finite_vertices_iterator vi ;
		//for(vi = impl_->finite_vertices_begin(); vi != impl_->finite_vertices_end(); ++vi) {
		//	Implementation::Vertex_handle v(vi) ;
		//	impl_->vertices_[v->id] = v ;
		//}

		skeleton_dirty_ = true ;
		cell_id_tag_ = false;
	}

	void Regular_CGAL::remove_vertices(std::vector<vec3>& V, std::vector<double>& weight, std::set<int>& to_remove) {
		// it doesn't work here if use_spatial_sort_ is used
		gx_assert(!use_spatial_sort_) ;
		std::vector<vec3> newpoints ;
		std::vector<int>  vids ;
		std::vector<Implementation::Vertex_handle> new_vertices ;
		std::vector<bool> flags(nb_vertices_, false) ;

		std::vector<Implementation::Vertex_handle> dirty ;
		for(std::set<int>::iterator it=to_remove.begin(); it!=to_remove.end(); ++it) {
			dirty.push_back(impl_->vertices_[*it]) ;
			flags[*it] = true ;
		}

		//for(unsigned int i=0; i<dirty.size(); ++i) {
		//	impl_->remove(dirty[i]) ;
		//}

		int idx = 0 ;
		for(int i=0; i<nb_vertices_; ++i) {
			if(flags[i]) {
				impl_->remove(impl_->vertices_[i]) ;
			}
			else {
				new_vertices.push_back(impl_->vertices_[i]) ;
				impl_->vertices_[i]->id = idx ++ ;
			}
		}

		
		
		vertices_ = &(V[0][0]) ;
		weight_ = &(weight[0]) ;
		nb_vertices_-=to_remove.size() ;
		std::swap(impl_->vertices_, new_vertices) ;
		gx_assert(impl_->vertices_.size() == nb_vertices_)

		for(int i=0; i<nb_vertices_; ++i) {
			vec3 p = MyCGALStuff_Regular::to_geex(impl_->vertices_[i]->point()) ;
			gx_assert(distance2(p, V[i])<1e-10) ;
		}

		//impl_->vertices_.resize(nb_vertices_, nil) ;

		//int nv = impl_->number_of_vertices () ;
		//gx_assert(nv == nb_vertices_) ;

		//int i = 0 ;
		//Implementation::Finite_vertices_iterator vi ;
		//for(vi = impl_->finite_vertices_begin(); vi != impl_->finite_vertices_end(); ++vi) {
		//	Implementation::Vertex_handle v(vi) ;
		//	vids.push_back(v->id) ;
		//	v->id = i ;
		//	impl_->vertices_[i] = v ;

		//	// TODO: fix the problem. The points are not aligned well after removal
		//	vec3 p = MyCGALStuff_Regular::to_geex(v->point()) ;
		//	//V[i] = p ;
		//	gx_assert(weight[i] == v->point().weight()) ; //
		//	weight[i] = v->point().weight() ;
		//	newpoints.push_back(p) ;
		//	gx_assert(distance2(p, V[i])<1e-10) ;
		//	//double w = v->point().weight() ;
		//	//gx_assert(fabs(w-weight_[i])<1e-10) ;
		//	i++ ;
		//}		

		skeleton_dirty_ = true ;
		cell_id_tag_ = false;
	}

	void Regular_CGAL::set_cells_id()	{
		int cell_start = 0 ;
		Implementation::Finite_cells_iterator citer = impl_->finite_cells_begin();
		for (; citer != impl_->finite_cells_end(); citer++, cell_start++)
			citer->cell_id() = cell_start;
		cell_id_tag_ = true;
	}

	unsigned int Regular_CGAL::get_number_of_finite_vertices() const{return (unsigned int)impl_->number_of_vertices(); }
	unsigned int Regular_CGAL::get_number_of_finite_cells() const{return (unsigned int)impl_->number_of_finite_cells(); }


	unsigned int Regular_CGAL::nearest_vertex_id(double x, double y, double z) const {
		//the following line does not return the nearest vertex according to euclidean space
		return impl_->nearest_power_vertex(MyCGALStuff_Regular::Point(x,y,z))->id ;
	}

	template <class WPOINT> inline Plane<real> gx_regular_bisector(
		const WPOINT& p1, const WPOINT& p2
		) {
			const vec3 gx_p1(p1.point().cartesian(0), p1.point().cartesian(1), p1.point().cartesian(2));
			const vec3 gx_p2(p2.point().cartesian(0), p2.point().cartesian(1), p2.point().cartesian(2));
            const vec3 N = gx_p1 - gx_p2;
            const real d = 0.5 * ( gx_p2.length2() - gx_p1.length2() + p1.weight() -p2.weight());
            return Geex::Plane<real>(N.x, N.y, N.z, d) ;
	}

	// Skeleton timings: 10 Lloyd iterations on pegaso (one vertex per facet)
	// Version 1: 9.12 (uses stack + set to traverse tets, and checks vrtx presence in skel)
	// Version 2: 5.75 (uses incident tet lists and checks vrtx presence in skel)
	// Version 3: 5.00 (uses incident tet lists stored in cells and checks vrtx presence in skel)
	// Version 4: 5.48 (uses incident tet lists stored in cells and marks halfedges)
	// Version 5: 4.82 (same as v.4 but uses next_around_halfedge() instead of CGAL's circulator)

	void Regular_CGAL::update_skeleton() const {
		skeleton_dirty_ = false ;
		//if(!use_spatial_sort_) {
		//    copy_CGAL_Delaunay_to_skel(impl_, &skeleton_) ;
		//    return ;
		//}

		// Version 1, commented-out
		// copy_CGAL_Delaunay_to_skel(impl_, &skeleton_, impl_->vertices_) ;
		// return ;

		// This is Version 5 (2x faster than Version 1 :-)

		// Compute Circular Incident Cell Lists
		impl_->init_cicl() ;

		skeleton_.clear() ;

		// clear halfedge flags, used for updating skeleton after insertion new vertices
		for(Implementation::Finite_vertices_iterator vi = impl_->finite_vertices_begin(); vi != impl_->finite_vertices_end(); ++vi) {
			Implementation::Cell_handle c = vi->cell() ;
			do {
				c->halfedge_flags().clear() ;
				c = c->next_around_vertex(c->index(vi)) ;
			} while(c != vi->cell()) ;
		}

		for(unsigned int ivg=0; ivg < impl_->vertices_.size(); ivg++) {
			Implementation::Vertex_handle it = impl_->vertices_[ivg] ;


			// in regular triangulation, some vertices are not in the triangulation
			if(it == nil ) {
				continue ; 
			}

			//   Skip nil vertices by inserting empty stars in the
			// Delaunay skeleton (to take into account skipped duplicated
			// vertices, see above).
			while((int)skeleton_.nb_vertices() < it->id) {
				skeleton_.begin_star() ;
				// No need to bark, we already said something when we inserted the vertex.
				// std::cerr << "Delaunay Skel Warning: Inserted empty star" << std::endl ;
				skeleton_.end_star() ;
			}

			// Sanity check
			//if(skeleton_.nb_vertices() != it->id)
			//  std::cout << "I'll probably crash. Current id is " << it->id 
			//	    << " for " << skeleton_.nb_vertices() 
			//	    << " vertices in the skel" << std::endl ;
			gx_debug_assert(skeleton_.nb_vertices() == it->id) ;

			skeleton_.begin_star() ;
			// Traverse incident cell list
			Implementation::Cell_handle c = it->cell() ;
			do {
				unsigned int ivit = c->index(it) ;

				// In the current cell, test all edges incident to current vertex 'it'
				for(unsigned int iv=0; iv<4; iv++) {
					Implementation::Vertex_handle neigh = c->vertex(iv) ;

					// Skip (it,it) edge and infinite edges
					if(ivit != iv && !impl_->is_infinite(neigh)) {
						Implementation::Edge e(c, ivit, iv) ;

						// Skip edges that are already marked
						if(!impl_->edge_is_marked(e)) {

							// Mark current edge
							impl_->mark_edge(e) ;

							// Add current neighbor and bisector to current star.
							skeleton_.add_to_star(
								neigh->id,
								gx_regular_bisector(it->point(), neigh->point())
								) ;
						}
					}
				}
				c = c->next_around_vertex(c->index(it)) ;
			} while(c != it->cell()) ;
			skeleton_.end_star() ;
		}
	}

	void Regular_CGAL::get_tetras(std::vector<int>& tetras, bool finite_only) {
		tetras.resize(0) ;

		if(finite_only) {
			tetras.reserve(4*impl_->number_of_finite_cells());
			for(
				Implementation::Finite_cells_iterator it = impl_->finite_cells_begin() ; 
				it != impl_->finite_cells_end(); it++
				) {
					for(unsigned int i=0; i<4; i++) {
						tetras.push_back(it->vertex(i)->id) ;
					}
			}
		} else {
			tetras.reserve(4*impl_->number_of_cells());
			for(
				Implementation::Cell_iterator it = impl_->cells_begin() ; 
				it != impl_->cells_end(); it++
				) {
					for(unsigned int i=0; i<4; i++) {
						tetras.push_back(
							impl_->is_infinite(it->vertex(i)) ? -1 : it->vertex(i)->id 
							) ;
					}
			}
		}
	}

	void Regular_CGAL::get_tetras(std::vector<int>& tetras, std::vector<int>& neighbors, std::vector<vec3>& duals) {
		tetras.resize(0) ;
		neighbors.resize(0) ;
		duals.resize(0) ;
		neighbors.reserve(4*impl_->number_of_finite_cells()) ;
		tetras.reserve(4*impl_->number_of_finite_cells());
		duals.reserve(impl_->number_of_finite_cells());
		
		set_cells_id() ;

		for(
			Implementation::Finite_cells_iterator it = impl_->finite_cells_begin() ; 
			it != impl_->finite_cells_end(); it++
			) {
				for(unsigned int i=0; i<4; i++) {
					tetras.push_back(it->vertex(i)->id) ;
					if(!impl_->is_infinite(it->neighbor(i))) {
						int nbor_id = it->neighbor(i)->cell_id() ;
						neighbors.push_back(it->neighbor(i)->cell_id()) ;
					} else {
						neighbors.push_back(-1) ;
					}
				}
				if(!it->is_circumcenter_computed()) {
					it->is_circumcenter_computed() = true ;
					it->self_circumcenter() = MyCGALStuff_Regular::to_geex(impl_->dual(it)) ;
				}
				duals.push_back(it->self_circumcenter()) ;
		}
	}

	void Regular_CGAL::get_facets(std::vector<int>& facets, bool finite_only)
	{
		static int idlist[4][3] = {{1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {0, 2, 1}};


		facets.resize(0);
		if(finite_only) {
			facets.reserve(3*impl_->number_of_finite_facets());
			for(
				Implementation::Finite_facets_iterator it = impl_->finite_facets_begin() ; 
				it != impl_->finite_facets_end(); it++
				) {
					Implementation::Cell_handle ch = it->first;
					for(unsigned int i=0; i<3; i++) {
						facets.push_back(ch->vertex(idlist[it->second][i])->id) ;
					}
			}
		} else {
			facets.reserve(3*impl_->number_of_facets());
			for(
				Implementation::Facet_iterator it = impl_->facets_begin() ; 
				it != impl_->facets_end(); it++
				) {
					Implementation::Cell_handle ch = it->first;
					for(unsigned int i=0; i<3; i++) {
						Implementation::Vertex_handle vh = ch->vertex(idlist[it->second][i]);
						facets.push_back(
							impl_->is_infinite(vh) ? -1 : vh->id 
							) ;
					}
			}
		}
	}


	void Regular_CGAL::get_voronoi_cell(unsigned int iv, VoronoiCell& result, bool geometry, bool use_cell_id) {
		result.clear() ;

		// update_skeleton creates the circular incident cell lists that we use.
		if(skeleton_dirty_) { update_skeleton() ; }

		Implementation::Vertex_handle v = impl_->vertices_[iv] ;

		if (v == nil)
			return;
		//gx_assert(v != nil) ; // Did we have two vertices at same location ?
		// Rem: in this case, we are not likely to ask for the Voronoi cell of the 
		// second one, since it does not generate any intersection.

		// Step 1: clear edge flags 
		Implementation::Cell_handle c = v->cell() ;
		do {
			c->halfedge_flags().clear() ;
			c = c->next_around_vertex(c->index(v)) ;
		} while(c != v->cell()) ;

		// Step 2: traverse Delaunay edges
		c = v->cell() ;
		do {
			unsigned int ivit = c->index(v) ;
			// In the current cell, test all edges incident to current vertex 'it'
			for(unsigned int iv=0; iv<4; iv++) {
				Implementation::Vertex_handle neigh = c->vertex(iv) ;

				// Skip (it,it) edge and dual facet at infinity
				if(ivit != iv && !impl_->is_infinite(neigh)) {
					Implementation::Edge e(c, ivit, iv) ;

					// Skip edges that are already marked
					if(!impl_->edge_is_marked(e)) {

						result.begin_facet(neigh->id) ;
						{
							Implementation::Cell_handle c = e.first ; 
							unsigned int iv1 = e.second ;
							unsigned int iv2 = e.third ;
							Implementation::Vertex_handle v1 = c->vertex(iv1) ;
							Implementation::Vertex_handle v2 = c->vertex(iv2) ;
							Implementation::Cell_handle cir = c ;
							do {
								iv1 = cir->index(v1) ;
								iv2 = cir->index(v2) ;
								cir->halfedge_flags().mark(iv1, iv2) ;
								unsigned int f = Implementation::next_around_halfedge[iv1][iv2] ;
								unsigned int iv3 ;
								for(iv3 = 0; iv3 < 4; iv3++) {
									if(iv3 != iv1 && iv3 != iv2 && iv3 != f) {
										break ;
									}
								}
								gx_assert(iv3 != 4) ;
								Implementation::Vertex_handle e_v = cir->vertex(iv3) ;
								int e_bisector = impl_->is_infinite(e_v) ? -1 : e_v->id ;
								bool c_is_infinite = impl_->is_infinite(cir) ;
                                if (geometry) {
									if (!cir->is_circumcenter_computed()) {
										cir->is_circumcenter_computed() = true;
										if(c_is_infinite) {
											for(unsigned int iv=0; iv<4; iv++) {
												cir->self_circumcenter() = 10.0 * triangle_normal_with_area(
													MyCGALStuff_Regular::to_geex(cir->vertex((iv+1)%4)->point()),
													MyCGALStuff_Regular::to_geex(cir->vertex((iv+2)%4)->point()),
													MyCGALStuff_Regular::to_geex(cir->vertex((iv+3)%4)->point())
													) ;
												if(iv % 2 == 0) {
													cir->self_circumcenter() = -cir->self_circumcenter() ;
												}
												break ;
											}
										} else {
											cir->self_circumcenter() = MyCGALStuff_Regular::to_geex(impl_->dual(cir));;
										}
									}
									if (use_cell_id){
										if (!cell_id_tag_)
											set_cells_id();
										result.add_to_facet(
										e_bisector, cir->self_circumcenter(), c_is_infinite, cir->cell_id()
										) ;
									}else
										result.add_to_facet(
										e_bisector, cir->self_circumcenter(), c_is_infinite
										) ;
                                }else {
									if (use_cell_id) {
										if (!cell_id_tag_)
											set_cells_id();
										result.add_to_facet(
											e_bisector, c_is_infinite, cir->cell_id()
											) ;
									}else {
										result.add_to_facet(
											e_bisector, c_is_infinite
											) ;
									}
                                }
								cir = cir->neighbor(f) ;
							} while(cir != c) ;
						}
						result.end_facet() ;
					}
				}
			}
			c = c->next_around_vertex(c->index(v)) ;
		} while(c != v->cell()) ;
	}

	//***************Extend*******************
	void Regular_CGAL::insert_vertex( double* xyz,  double* weight, vec3 V, double weightv, unsigned int pos){
		/*impl_->vertices_.resize(n);
		impl_->vertices_.assign(n,nil);

		Implementation::Vertex_handle v;
		for(unsigned int i=0; i<n; i++) {
			v = impl_->insert(
				MyCGALStuff_Regular::Weighted_point(
				MyCGALStuff_Regular::Point(xyz[3*i], xyz[3*i+1], xyz[3*i+2]), 
				weight_?weight_[i]:0.0) );
			if(v != Implementation::Vertex_handle() && v->id == -1 ) {
				v->id = i ;
			}
		}
		Implementation::Finite_vertices_iterator vi ;
		for(vi = impl_->finite_vertices_begin(); vi != impl_->finite_vertices_end(); ++vi) {
			Implementation::Vertex_handle v(vi) ;
			impl_->vertices_[v->id] = v ;
		}

		for(int i=0;i<nb_vertices_; i++){
			if(i<pos){
				vertices_[3*i] = vertices_[3*i];
				vertices_[3*i+1] = vertices_[3*i+1];
				vertices_[3*i+2] = vertices_[3*i+2];
			}
			else if(i == pos){
				vertices_[3*i] = vertices_[3*i];
				vertices_[3*i+1] = vertices_[3*i+1];
				vertices_[3*i+2] = vertices_[3*i+2];
			}
			else{
				vertices_[3*i] = vertices_[3*i];
				vertices_[3*i+1] = vertices_[3*i+1];
				vertices_[3*i+2] = vertices_[3*i+2];
			}
		}
		vertices_.insert(vertices_.begin() + pos, V);
		power_weight_.insert(power_weight_.begin() + pos, weight(vert));
		nb_vertices_ += 1 ;*/

		vertices_ = xyz ;
		weight_ = weight ;
		nb_vertices_ += 1 ;

		Implementation::Vertex_handle v;
		v = impl_->insert(
			MyCGALStuff_Regular::Weighted_point(
			MyCGALStuff_Regular::Point(V.x, V.y, V.z), weightv) );

		if(v != Implementation::Vertex_handle() && v->id == -1 ) {
			v->id = pos ;
			//gx_assert(v->id == impl_->vertices_.size()) ;
			//impl_->vertices_.push_back(v) ;				
		}
		//std::vector<Implementation::Vertex_handle> new_vertices ;
		//new_vertices.assign(nb_vertices_,nil);
		impl_->vertices_.resize(nb_vertices_);
		impl_->vertices_.assign(nb_vertices_,nil);
		impl_->vertices_[pos] = v ;
	    impl_->vertices_[pos]->id = pos;
		Implementation::Finite_vertices_iterator vi ;
		for(vi = impl_->finite_vertices_begin(); vi != impl_->finite_vertices_end(); ++vi) {
			Implementation::Vertex_handle vv(vi) ;
			if(vv->id < pos){
				impl_->vertices_[vv->id] = vv ;
			}
			else{
				vv->id +=1;
				impl_->vertices_[vv->id] = vv ;
			}
		}

		/*for(int i=0;i < nb_vertices_; i++){
			if(i<pos){
				new_vertices[i] = impl_->vertices_[i];
				new_vertices[i]->id = impl_->vertices_[i]->id;
			}
			else if(i == pos){
				new_vertices[i] = v;
				v->id = pos;
			}
			else if(i > pos){
				new_vertices[i] = impl_->vertices_[i-1];
				new_vertices[i]->id = impl_->vertices_[i-1]->id +1;
			}
		}

		std::swap(impl_->vertices_, new_vertices) ;
		gx_assert(impl_->vertices_.size() == nb_vertices_);*/

		skeleton_dirty_ = true ;
		cell_id_tag_ = false;
	}
	void Regular_CGAL::remove_vertex(double* xyz, double* weight, unsigned int index){
		vertices_ = xyz ;
		weight_ = weight ;
		nb_vertices_ -= 1 ;
		impl_->remove(impl_->vertices_[index]) ;
		impl_->vertices_.erase(impl_->vertices_.begin() + index);
		for(int i=0;i < impl_->vertices_.size(); i++){
			impl_->vertices_[i]->id = i;
		}

		skeleton_dirty_ = true ;
		cell_id_tag_ = false;
	}
	//***************End******************
}
