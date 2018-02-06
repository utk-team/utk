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

#ifndef __GEEX_CVT_RVD_POLY_KDTREE__
#define __GEEX_CVT_RVD_POLY_KDTREE__

#include <Geex/CVT/topo_poly_mesh.h>
#include <Geex/CVT/delaunay_skel.h>
#include <Geex/CVT/delaunay.h>
#include <Geex/CVT/RVD_predicates.h>
#include <Geex/CVT/ann_kdtree.h>
#include <stack>
#include <set>
#include <queue>

namespace Geex {

    /*
     * Computes the restricted voronoi diagram for a single facet.
     * This version uses simple cell clipping.
     */
    class RestrictedVoronoiDiagram_poly_kdtree {
        typedef RestrictedVoronoiDiagram_poly_kdtree thisclass ;

    public:
        RestrictedVoronoiDiagram_poly_kdtree() : 
            m(nil), delaunay_(nil), symbolic_(false), exact_(false){ 
        }
        
        RestrictedVoronoiDiagram_poly_kdtree(Delaunay* delaunay_in, TopoPolyMesh* m_in) : 
            m(m_in), delaunay_(delaunay_in), 
            symbolic_(false), exact_(false) { 
        }
        
        ~RestrictedVoronoiDiagram_poly_kdtree() {}

        // In symbolic mode, each generated vertex knows the ID's of the planes
        // it belongs to.
        void set_symbolic(bool x) { symbolic_ = x ; }

        bool symbolic() const { return symbolic_ ; }

        // In exact mode, all combinatorial decisions (predicates) use exact
        // arithmetics.
        void set_exact(bool x) { exact_ = x ; }

        bool exact() const { return exact_ ; }

        const TopoPolyMesh* mesh() const { return m ; }
        TopoPolyMesh* mesh() { return m ; }

        const Delaunay* delaunay() const { return delaunay_ ; }
        Delaunay* delaunay() { return delaunay_ ; }

        // Low-level API. Client code may use for_each_facet(), for_each_triangle() or
        // for_each_primal_triangle() instead.
        template <class DEL, class ACTION> inline void 
        compute(
            TopoPolyMesh* M, DEL* delaunay, const DelaunaySkeleton* skel, ANNKdTree_3* kdtree, const ACTION& action
        ) {
 //           init_visited(M) ;
            for(unsigned int f=0; f<M->nb_facets(); f++) {
 //               if(!M->facet_is_marked(f)) {
                    const vec3& c = M->facet_center(f) ;
                    unsigned int cell = kdtree->queryNearestNeighbor(c[0], c[1], c[2]) ;
					visited_.clear() ;
					Q.push(cell) ;
//                    push(f,cell,false) ;
//                    while(!S.empty()) {
					while(!Q.empty()) {
                        TopoPolyMesh* ping = &M1 ;
                        TopoPolyMesh* pong = &M2 ;
//                        unsigned int f = S.top().f ;
//                        unsigned int cell = S.top().cell ;
						unsigned int cell = Q.front() ;
						Q.pop() ;
						if(!visited_.insert(cell).second) continue ;

//                        current_facet_ = f ;

//                        S.pop() ;
                        ping->clear() ;
                        M->copy_facet(f, *ping) ;
                        
                        // Compute triangle clipped by cell and
                        // send neighbors to stack
                        clip_by_cell(skel, cell, ping, pong) ;
                        
                        // Apply action to triangles intersected by cell.
                        action(cell, ping) ;

                        //for(unsigned int i=0; i<ping->nb_vertices(); i++) {
                        //    int neigh_f = ping->vertex(i).f ;
                        //    if(neigh_f >= 0 && !M->facet_is_marked(neigh_f)) {
                        //        push(neigh_f, cell) ;
                        //    }
                        //}
//                    }
                }
            }

            //// Final cleanup: unmark all triangles
            //for(unsigned int f=0; f<M->nb_facets(); f++) {
            //    M->unmark_facet(f) ;
            //}
        }

        //// ACTION needs to implement 
        ////      operator()(unsigned int c, TopoPolyMesh* M) const
        ////   where c denotes the index of the current Voronoi cell (or Delaunay vertex).
        //template <class ACTION> inline void for_each_facet(const ACTION& action) {
        //    if(exact_) {
        //        RVD_predicates::begin_stats() ;
        //    }
        //    this->template compute<Delaunay, ACTION>(m, delaunay_, delaunay_->skeleton(), action) ;
        //    if(exact_) {
        //        RVD_predicates::end_stats() ;
        //    }
        //}

        //// ACTION needs to implement:
        ////      operator()(unsigned int c, const TopoPolyVertexEdge& v1, v2, v3) const
        ////   where c denotes the index of the current Voronoi cell (or Delaunay vertex).
        //template <class TRIACTION> inline void for_each_triangle(const TRIACTION& action) {
        //    if(exact_) {
        //        RVD_predicates::begin_stats() ;
        //    }
        //    this->template compute<Delaunay, TriangleAction<TRIACTION> >(
        //        m, delaunay_, delaunay_->skeleton(), TriangleAction<TRIACTION>(action)
        //    ) ;
        //    if(exact_) {
        //        RVD_predicates::end_stats() ;
        //    }
        //}

        //// ACTION needs to implement:
        ////      operator()(unsigned int c, const TopoPolyVertexEdge& v1, v2) const
        ////   where c denotes the index of the current Voronoi cell (or Delaunay vertex).
        //template <class HEACTION> inline void for_each_halfedge(const HEACTION& action) {
        //    if(exact_) {
        //        RVD_predicates::begin_stats() ;
        //    }
        //    this->template compute<Delaunay, HalfedgeAction<HEACTION> >(
        //        m, delaunay_, delaunay_->skeleton(), HalfedgeAction<HEACTION>(action)
        //    ) ;
        //    if(exact_) {
        //        RVD_predicates::end_stats() ;
        //    }
        //}


        //// ACTION needs to implement:
        ////      operator()(unsigned int i, unsigned j, unsigned int k) const
        ////   where i,j,k denote the three indices of the Delaunay vertices 
        ////   that define the primal triangle.
        //template <class PRIMTRIACTION> inline void for_each_primal_triangle(
        //    const PRIMTRIACTION& action
        //) {
        //    if(exact_) {
        //        RVD_predicates::begin_stats() ;
        //    }
        //    // PrimalTriangleAction needs symbolic mode
        //    bool sym_backup = symbolic_ ;
        //    symbolic_ = true ;
        //    this->template compute<Delaunay, PrimalTriangleAction<PRIMTRIACTION> >(
        //        m, delaunay_, delaunay_->skeleton(), PrimalTriangleAction<PRIMTRIACTION>(action)
        //    ) ;
        //    symbolic_ = sym_backup ;
        //    if(exact_) {
        //        RVD_predicates::end_stats() ;
        //    }
        //}

//        unsigned int current_facet() const { return current_facet_ ; }

    protected:
        
        void clip_by_cell(
            const DelaunaySkeleton* skel, 
            unsigned int v, 
            TopoPolyMesh*& ping, TopoPolyMesh*& pong
        ) {
            for(unsigned int i=skel->star_begin(v); i<skel->star_end(v); i++) {
                unsigned int w = skel->neighbor(i) ;
                const Plane<real>& P = skel->bisector(i) ;
                pong->clear() ;
                clip_by_plane(ping, pong, P, v, w) ;
                gx_swap(ping, pong) ;
            }
        }

        Sign side(
            const Geex::Plane<real>& P, 
            const TopoPolyVertexEdge& v,
            unsigned int cell_in, unsigned int cell_out
        ) const {
            ////Sign result ;
            //if(exact_) {
            //    vec3 p1 = delaunay_->vertex(cell_in) ;
            //    vec3 p2 = delaunay_->vertex(cell_out) ;
            //    return RVD_predicates::side(p1, p2, v, this) ; 
            //} 
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
                            if(symbolic_ || exact_) {
                                ping->intersect(I, *last_v, *v, P, cell_out) ;
                            } else {
                                ping->intersect(I, *last_v, *v, P) ;
                            }
                            if(!pong->in_facet()) { pong->begin_facet() ; }
                            pong->add_vertex(I) ;
                            if(status > 0) {
                                pong->top_vertex().set_edge(*last_v) ;
                            } else {
                                pong->top_vertex().set_flag(
                                    TopoPolyVertexEdge::INTERSECT
                                ) ;
                            }
                        }

                        if(status > 0) {
                            if(!pong->in_facet()) { pong->begin_facet() ; }
                            pong->add_vertex(*v) ;
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
                if(crosses) {
                    // push(ping->facet_info(f).id, cell_out) ;
					Q.push(cell_out) ;
                }
            }
        }

        //void push(
        //    unsigned int f, 
        //    unsigned int cell, bool check = true
        //) {
        //    if(check) {
        //        for(unsigned int i=0; i<visited_[f].size(); i++) {
        //            if(visited_[f][i] == cell) { return ; }
        //        }
        //    }
        //    visited_[f].push_back(cell) ;
        //    m->mark_facet(f) ;
        //    S.push(TopoPolyStackItem(f, cell)) ;
        //}

    private:
//        TopoPolyStack S ;
		std::queue<int>  Q ;
		std::set<int>    visited_ ; // incident cells for each facet
        TopoPolyMesh M1, M2 ;
        TopoPolyMesh* m ;
        Delaunay* delaunay_ ;
        bool symbolic_ ;
        bool exact_ ;
//        unsigned int current_facet_ ;
		ANNKdTree_3* kdtree_ ;

    private:
        RestrictedVoronoiDiagram_poly_kdtree(const thisclass& rhs) ;
        thisclass& operator=(const thisclass& rhs) ;        
    } ;
}

#endif
