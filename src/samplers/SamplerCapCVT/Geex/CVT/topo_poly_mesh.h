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

#ifndef __GEEX_CVT_TOPO_POLY_MESH__
#define __GEEX_CVT_TOPO_POLY_MESH__

#include <Geex/CVT/geometry.h>
#include <Geex/CVT/symbolic_vertex.h>
#include <Geex/basics/types.h>
#include <vector>
#include <iostream>

namespace Geex {


    typedef unsigned int TopoFlags ;
    typedef unsigned int TopoFlag ;


    // Timings: pegaso, 10 iterations, 1 seed per facet
    //  RVD clipping + weights computation:    13.5 s
    //  with additional member SymbolicVertex: 14.5 s
    //  Current version : 17 s (tests for current mode loose lot of time, replace with template args)    

    /**
     * Vertex of a polygon + information
     * attached to the edge starting from
     * this vertex.
     */
    class TopoPolyVertexEdge : public vec3Sym {
    public:
        enum {ORIGINAL = 1, INTERSECT = 2} ;

        TopoPolyVertexEdge(
            const TopoPolyVertexEdge& rhs
        ) : vec3Sym(rhs, rhs.sym), flags(rhs.flags), f(rhs.f), w(rhs.w) {
        }

        TopoPolyVertexEdge(
            const vec3Sym& rhs
        ) : vec3Sym(rhs), flags(0), f(-1), w(1.0) {
        }

        TopoPolyVertexEdge(
            const vec3& p, double w_in = 1.0
        ) : vec3Sym(p), flags(0), f(-1), w(w_in) {
        }

        TopoPolyVertexEdge() { }

        void set_point(const vec3& p) {
            vec3::operator=(p) ;
        }

        void set_edge(const TopoPolyVertexEdge& rhs) {
            flags = rhs.flags ;
            f = rhs.f ;
        }

        TopoPolyVertexEdge& operator=(const TopoPolyVertexEdge& rhs) {
            vec3::operator=(rhs) ;
            set_edge(rhs) ;
            w = rhs.w ;
            sym = rhs.sym ;
            return *this ;
        }

        void clear() { flags = 0 ; f = -1 ; w = 1.0 ; }
        void set_flag(TopoFlag f) { flags |= f ; }
        void unset_flag(TopoFlag f) { flags &= ~f ; }
        bool check_flag(TopoFlag f) const { return ((flags & f) != 0) ; }

        TopoFlags flags ;    // Indicates the type of edge 
                             // (virtual, original or intersection).
        int f ;              // The index of the other facet incident 
                             //  to this edge (or -1 on boundary).
        double w ;           // Vertex weight (e.g., curvature). 
    } ;

    /**
     * Information attached to a facet. 
     * flags are used to mark/unmark facets during traversal.
     */
    class TopoPolyFacetInfo {
    public:
        TopoPolyFacetInfo() : flags(0), id(~0) { }
        TopoFlags flags ;
        unsigned int id ;
    } ;

    /**
     * TopoPolyMesh stores facet connectivity of a polygon mesh. 
     * Vertices are duplicated. Facets store pointers to adjacent
     * facets.
     * Facets are stored using the CRS (compressed row storage) 
     * format, i.e. the vertices of the f-th facet are traversed
     * using:
     * for(unsigned int i=M.facet_begin(f); i<M.facet_end(f); i++) {
     *    do something with M.vertex(i)
     * }
     *
     * In addition, TopoPolyMesh can be used as a polygon stack
     * (this is used by the RVD computation algorithm).
     */
    class GEEX_API TopoPolyMesh {
    public:
        TopoPolyMesh(
        ) : in_facet_(false){
            facet_ptr_.push_back(0) ; 
        }

        TopoPolyMesh(
            const TopoPolyMesh& rhs
        ) : in_facet_(false){
            gx_assert(rhs.nb_vertices() == 0) ;
            facet_ptr_.push_back(0) ; 
        }

        void clear() {
            gx_assert(!in_facet_) ; 
            // Note: we use resize(0) instead of clear()
            // since this is guaranteed to keep the reserved
            // memory (this is what we want for our "ping-pong"
            // buffers).
            vertex_.resize(0) ; 
            facet_ptr_.resize(0) ; 
            facet_info_.resize(0) ; 
            facet_ptr_.push_back(0) ; 
            original_vertices_.resize(0) ;
            vertex_index_.resize(0) ;
        }

        //==----------------------- Construction

        void begin_facet() { 
            gx_assert(!in_facet_) ; 
            in_facet_ = true ; 
        }

        void add_vertex(const TopoPolyVertexEdge& ve) {
            vertex_.push_back(ve) ;
        }

        void end_facet() {
            gx_assert(in_facet_) ;
            if(*(facet_ptr_.rbegin()) == vertex_.size()) {
                std::cerr << "[W] Created empty facet" << std::endl ;
            }
            facet_ptr_.push_back((unsigned int)vertex_.size()) ;
            facet_info_.push_back(TopoPolyFacetInfo()) ;
            in_facet_ = false ;
        }

        bool in_facet() const { return in_facet_ ; }

        //==----------------------- Access

        unsigned int nb_facets() const { return (unsigned int)facet_ptr_.size() - 1 ; }
        unsigned int nb_vertices() const { return (unsigned int)vertex_.size() ; }
        unsigned int facet_begin(unsigned int f) const {
            gx_debug_assert(f < nb_facets()) ;
            return facet_ptr_[f] ;
        }

        unsigned int facet_end(unsigned int f) const {
            gx_debug_assert(f < nb_facets()) ;
            return facet_ptr_[f+1] ;
        }

        unsigned int facet_size(unsigned int f) const {
            return facet_end(f) - facet_begin(f) ;
        }

        TopoPolyVertexEdge& facet_vertex(unsigned int f, unsigned int i) {
            gx_debug_assert(i < facet_size(f)) ;
            return vertex(facet_begin(f)+i) ;
        }

        const TopoPolyVertexEdge& facet_vertex(unsigned int f, unsigned int i) const {
            gx_debug_assert(i < facet_size(f)) ;
            return vertex(facet_begin(f)+i) ;
        }

        unsigned int next_around_facet(unsigned int f, unsigned int i) const {
            gx_debug_assert(i >= facet_begin(f) && i < facet_end(f)) ;
            return (i+1 == facet_end(f) ? facet_begin(f) : i+1) ;
        }

        unsigned int prev_around_facet(unsigned int f, unsigned int i) const {
            gx_debug_assert(i >= facet_begin(f) && i < facet_end(f)) ;            
            return (i == facet_begin(f) ? facet_end(f)-1 : i-1) ;
        }

        /**
         * do_it is supposed to overload operator()(
         *   const TopoPolyVertexEdge& v1, 
         *   const TopoPolyVertexEdge& v2, 
         *   const TopoPolyVertexEdge& v3 )
         * Can be for instance an inline function.
         */
        template<class T> void for_each_triangle(unsigned int f, T& do_it) const {
            unsigned int i0 = facet_begin(f) ;
            for(unsigned int i = facet_begin(f)+1; i+1<facet_end(f); i++) {
                do_it(vertex(i0), vertex(i), vertex(i+1)) ;
            }
        }

        /**
         * do_it is supposed to overload operator()(
         *   const TopoPolyVertexEdge& v1, 
         *   const TopoPolyVertexEdge& v2, 
         *   const TopoPolyVertexEdge& v3 )
         * Can be for instance an inline function.
         */
        template<class T> void for_each_triangle(
            unsigned int f, const T& do_it
        ) const {
            unsigned int i0 = facet_begin(f) ;
            for(unsigned int i = facet_begin(f)+1; i+1<facet_end(f); i++) {
                do_it(vertex(i0), vertex(i), vertex(i+1)) ;
            }
        }

        /**
         * do_it is supposed to overload operator()(
         *   const TopoPolyVertexEdge& v1, 
         *   const TopoPolyVertexEdge& v2, 
         *   const TopoPolyVertexEdge& v3 )
         * Can be for instance an inline function.
         */
        template<class T> void for_each_triangle(T& do_it) const {
            for(unsigned int f=0; f<nb_facets(); f++) {
                unsigned int i0 = facet_begin(f) ;
                for(unsigned int i = facet_begin(f)+1; i+1<facet_end(f); i++) {
                    do_it(vertex(i0), vertex(i), vertex(i+1)) ;
                }
            }
        }

        /**
         * do_it is supposed to overload operator()(
         *   const TopoPolyVertexEdge& v1, 
         *   const TopoPolyVertexEdge& v2, 
         *   const TopoPolyVertexEdge& v3 )
         * Can be for instance an inline function.
         */
        template<class T> void for_each_triangle(const T& do_it) const {
            for(unsigned int f=0; f<nb_facets(); f++) {
                unsigned int i0 = facet_begin(f) ;
                for(unsigned int i = facet_begin(f)+1; i+1<facet_end(f); i++) {
                    do_it(vertex(i0), vertex(i), vertex(i+1)) ;
                }
            }
        }

        // Note: this is not the centroid, but this is 
        // OK for what we do with it.
        vec3 facet_center(unsigned int f) const {
            vec3 result(0.0, 0.0, 0.0) ;
            double n(0.0) ;
            for(unsigned int i=facet_begin(f); i<facet_end(f); i++) {
                result += vertex(i) ;
                n++ ;
            }
            return (1.0/n)*result ;
        }

        double facet_area(unsigned int f) const {
            double result = 0.0 ;
            unsigned int i0 = facet_begin(f) ;
            for(unsigned int i = facet_begin(f)+1; i+1<facet_end(f); i++) {
                vec3 v1 = vertex(i) - vertex(i0) ;
                vec3 v2 = vertex(i+1) - vertex(i0) ;
                result += cross(v1,v2).length() ;
            }
            result /= 2.0 ;
            return result ;
        }

		void facet_centroid_and_mass(unsigned int f, vec3& g, double& V) {
			g = vec3(0, 0, 0) ;
			V = 0 ;
			unsigned int i0 = facet_begin(f) ;
			for(unsigned int i = facet_begin(f)+1; i+1<facet_end(f); i++) {
				vec3 v1 = vertex(i) - vertex(i0) ;
				vec3 v2 = vertex(i+1) - vertex(i0) ;
				double area = 0.5*cross(v1,v2).length() ; 
				V += area ;
				g += (area/3.0)*(vertex(i)+vertex(i+1)+vertex(i0)) ;
			}
		}
        
        vec3 facet_normal(unsigned int f) const {
            vec3 result = cross(
                facet_vertex(f,1) - facet_vertex(f,0), facet_vertex(f,2) - facet_vertex(f,0)
            ) ; 
            int d = facet_size(f) ;
            // Check for degeneracies
            if(d > 3) {
                for(unsigned int i=1; i<facet_size(f); i++) {
                    int i1 = i ;
                    int i2 = (i1+1)%d ;
                    int i3 = (i2+1)%d ;
                    result += cross(
                        facet_vertex(f,i2) - facet_vertex(f,i1), facet_vertex(f,i3) - facet_vertex(f,i1)
                    ) ;                    
                }
            }
            return result ;
        }

        Plane<real> facet_plane(unsigned int f) const { 
            return Plane<real>(facet_vertex(f,0), facet_normal(f)) ; 
        }

        TopoPolyVertexEdge& vertex(unsigned int v) {
            gx_debug_assert(v < nb_vertices()) ;
            return vertex_[v] ;
        }

        const TopoPolyVertexEdge& vertex(unsigned int v) const {
            gx_debug_assert(v < nb_vertices()) ;
            return vertex_[v] ;
        }

        //==----------------------- Marking

        void mark_facet(unsigned int f) { 
            gx_debug_assert(f < nb_facets()) ;
            facet_info_[f].flags = 1 ;
        }
        void unmark_facet(unsigned int f) { 
            gx_debug_assert(f < nb_facets()) ;
            facet_info_[f].flags = 0 ;
        }
        bool facet_is_marked(unsigned int f) const { 
            gx_debug_assert(f < nb_facets()) ;
            return (facet_info_[f].flags != 0) ;
        }
        const TopoPolyFacetInfo& facet_info(unsigned int f) const {
            gx_debug_assert(f < nb_facets()) ;
            return facet_info_[f] ;
        }
        TopoPolyFacetInfo& facet_info(unsigned int f) {
            gx_debug_assert(f < nb_facets()) ;
            return facet_info_[f] ;
        }

        //==----------------------- Stack behavior

        unsigned int top() const { 
            gx_debug_assert(nb_facets() > 0) ;
            return nb_facets() - 1 ; 
        }
        
        TopoPolyVertexEdge& top_vertex() {
            return *(vertex_.rbegin()) ;
        }

        const TopoPolyVertexEdge& top_vertex() const {
            return *(vertex_.rbegin()) ;
        }

        bool empty() { return nb_facets() == 0 ; }

        // copies facet to target.
        void copy_facet(unsigned int f, TopoPolyMesh& target) {
            target.begin_facet() ;
            for(unsigned int v = facet_begin(f) ; v < facet_end(f) ; v++) {
                target.add_vertex(vertex(v)) ;
            }
            target.end_facet() ;
            target.facet_info(target.top()) = facet_info(f) ;
        }

        void copy_top_facet(TopoPolyMesh& target) {
            gx_debug_assert(nb_facets() > 0) ;
            copy_facet(top(), target) ;
        }

        // removes last facet
        void pop_facet() {
            facet_ptr_.pop_back() ;
            facet_info_.pop_back() ;
            unsigned int nb_v = *(facet_ptr_.rbegin()) ;
            while(vertex_.size() > nb_v) {
                vertex_.pop_back() ;
            }
        }


        // copies last facet to target and
        // removes last facet.
        void pop_facet(TopoPolyMesh& target) {
            copy_top_facet(target) ;
            pop_facet() ;
        }

        //==----------------------- IO

        // Gamma is the exageration factor to be used for density 
        // if density is present. Returns the number of borders.
        unsigned int load(const std::string& filename, double gamma = 1.0) ;

		unsigned int load(const std::vector<vec3>& surf_vertices, const std::vector<int>& surf_facets) ;
        // loads the boundary of a tetrahedral mesh (and returns the indices of the vertices
        // of all the tetrahedra in tetrahedra_indice).
        void load_tet(const std::string& filename, std::vector<int>& tetrahedra_indice);
        // loads the combinatorics of a tetrahedral mesh.
        void load_tet(const std::vector<vec3>& tet_vertices, const std::vector<int>& cell_indice);

        // Note: does not save adjacencies, just geometry.
        void save(const std::string& filename) ;

        void normalize() ;
        void normalize(const vec3& g, double R, bool disable_normalize_ = false) ;

        void get_bbox(
            real& x_min, real& y_min, real& z_min,
            real& x_max, real& y_max, real& z_max
        ) ;

        //==----------------------- Clipping

        void intersect(
            TopoPolyVertexEdge& I, 
            const TopoPolyVertexEdge& v1, const TopoPolyVertexEdge& v2, 
            const Plane<real>& P
        ) const {
            double h = fabs( v1.x * P.a + v1.y * P.b + v1.z * P.c + P.d );
            double l = fabs( v2.x * P.a + v2.y * P.b + v2.z * P.c + P.d );
            double hl = h + l;
            //if hl is very small, hl > 0 will be false
            if (hl > 0) {
                double t_h = h / hl;
                double t_l = l / hl;
                I = t_l * v1 + t_h * v2;
                I.w = t_l * v1.w + t_h * v2.w;
            } else {
                I = 0.5 * (v1 + v2);
                I.w = 0.5 * (v1.w + v2.w);
            }
        }

        void intersect(
            TopoPolyVertexEdge& I, 
            const TopoPolyVertexEdge& v1, const TopoPolyVertexEdge& v2, 
            const Plane<real>& P,
            unsigned int E
        ) const {
            double h = fabs( v1.x * P.a + v1.y * P.b + v1.z * P.c + P.d );
            double l = fabs( v2.x * P.a + v2.y * P.b + v2.z * P.c + P.d );
            double hl = h + l;
            //if hl is very small, hl > 0 will be false
            if (hl > 0) {
                double t_h = h / hl;
                double t_l = l / hl;
                I = t_l * v1 + t_h * v2;
                I.w = t_l * v1.w + t_h * v2.w;
            } else {
                I = 0.5 * (v1 + v2);
                I.w = 0.5 * (v1.w + v2.w);
            }
            sets_intersect(v1.sym, v2.sym, I.sym) ;
            I.sym.add_bisector(E) ;
//            std::cerr << v1.sym << " /\\ " << v2.sym << " \\/ " << E 
//                      << " = " << I.sym << std::endl ;
        }

        // ------------------ Queries ---------------------------------

        
        // Returns in w1 and w2 the two extremities
        // of the edge shared by facets q1 and q2.
        inline void find_edge_extremities(
            unsigned int q1, unsigned int q2,
            vec3& w1, vec3& w2
        ) const {
            if(q1 > q2) { gx_swap(q1,q2) ; }
            const vec3* v1 = nil ; const vec3* v2 = nil ;
            if(q2 >= nb_facets()) {
                // q2 is a "Virtual facet" on the boundary, indicates the index of
                // the edge in the other facet q1.
                // Note: since w.sym is *ordered*, this will be always q2 that is
                // virtual and q1 that is a facet index.
                gx_debug_assert(q1 < nb_facets()) ;
                unsigned int iv1 = facet_begin(q1) + q2 - nb_facets() ;
                v1 = &vertex(iv1) ;
                v2 = &vertex(next_around_facet(q1, iv1)) ;
            } else {
                // Find the edge (v1, v2) of facet q1 adjacent to facet q2
                for(unsigned int i=facet_begin(q1); i<facet_end(q1); i++) {
                    if(vertex(i).f == q2) {
                        v1 = &vertex(i) ;
                        v2 = (i == facet_end(q1) - 1) ? &(vertex(facet_begin(q1))) : &(vertex(i+1)) ;
                        break ;
                    }
                }
            }
            gx_debug_assert(v1 != nil && v2 != nil) ;
            w1 = *v1 ; w2 = *v2 ;
        }

        // Returns the vertex shared by facets q1,q2 and q3

        inline const vec3& find_vertex(
            unsigned int q1, unsigned int q2, unsigned int q3
        ) const {

            gx_debug_assert(q1 < nb_facets()) ;
            gx_debug_assert(q2 < nb_facets()) ;
            gx_debug_assert(q3 < nb_facets()) ;

            for(unsigned int i=facet_begin(q1); i<facet_end(q1); i++) {
                unsigned int j =next_around_facet(q1,i) ;
                const TopoPolyVertexEdge& v1 = vertex(i) ;
                const TopoPolyVertexEdge& v2 = vertex(j) ;
                if(
                    v1.f == q2 && v2.f == q3 ||
                    v2.f == q2 && v1.f == q3
                ) {
                    return v2 ;
                }
            }

            for(unsigned int i=facet_begin(q2); i<facet_end(q2); i++) {
                unsigned int j =next_around_facet(q2,i) ;
                const TopoPolyVertexEdge& v1 = vertex(i) ;
                const TopoPolyVertexEdge& v2 = vertex(j) ;
                if(
                    v1.f == q3 && v2.f == q1 ||
                    v2.f == q3 && v1.f == q1
                ) {
                    return v2 ;
                }
            }

            for(unsigned int i=facet_begin(q3); i<facet_end(q3); i++) {
                unsigned int j =next_around_facet(q3,i) ;
                const TopoPolyVertexEdge& v1 = vertex(i) ;
                const TopoPolyVertexEdge& v2 = vertex(j) ;
                if(
                    v1.f == q1 && v2.f == q2 ||
                    v2.f == q1 && v1.f == q2
                ) {
                    return v2 ;
                }
            }
             
            gx_assert_not_reached;
            return *(vec3*)nil ;
        }

        // ------------------ METIS interface -------------------------

        void partition(
            unsigned int nb_parts, Array1d<TopoPolyMesh>& parts
        ) const ;
        // partition a tetrahedral mesh into nb_parts sub-tetrahedral mesh,
        // each boundary of the sub-tetrahedral mesh is saved in parts.
        //tetrahedra_indice is the indices of tetrahedral mesh and original_vertices stores
        //the vertices of the tetrahedral mesh. 
        //Note: load_tet function should be called before.
        void partition_volume(const std::vector<int>& tetrahedra_indice, 
            unsigned int nb_parts, Array1d<TopoPolyMesh>& parts
        ) ;
        // ------------------ Additional information

        const std::vector<vec3>& original_vertices() const {
            return original_vertices_;
        }

		std::vector<vec3>& original_vertices() {
            return original_vertices_;
        }

        //   Vertices are duplicated in the facets,
        // but they have a single vertex index (that
        // is valid for meshes loaded from a file).
        unsigned int vertex_index(unsigned int i) const {
            gx_debug_assert(i < vertex_index_.size()) ;
            return vertex_index_[i] ;
        }

        // ---------- Signed volume and orientation

        double area() const ; 
        double signed_volume() const ;
        bool orientation() const { return orientation_ ; }
        bool contains(const vec3& p) const ;

    protected:
        void init_symbolic_vertices() ;

    private:
        bool in_facet_ ;
        std::vector<TopoPolyVertexEdge> vertex_ ;
        std::vector<unsigned int> facet_ptr_ ;
        std::vector<TopoPolyFacetInfo> facet_info_ ;
        std::vector<vec3> original_vertices_ ;
        std::vector<unsigned int> vertex_index_ ;
        bool orientation_ ;

    //private:
    //    TopoPolyMesh& operator=(const TopoPolyMesh& rhs) ;
    } ;

}

#endif
