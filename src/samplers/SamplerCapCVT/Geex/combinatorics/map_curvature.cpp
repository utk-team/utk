/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000-2003 Bruno Levy
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will bein useful,
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
 *  Contact: Bruno Levy
 *
 *     levy@loria.fr
 *
 *     ISA Project
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */

#include <Geex/combinatorics/map_curvature.h>
#include <Geex/combinatorics/map_geometry.h>
#include <Geex/mathematics/normal_cycle.h>
//#include <Geex/basics/processor.h>
//#include <OGF/cells/map/geometry.h>
//#include <OGF/math/geometry/normal_cycle.h>
//#include <OGF/basic/debug/progress.h>

#include <set>
#include <stack>

namespace Geex {

    MapCurvature::MapCurvature(Map* map) : map_(map), radius_(0.001) { 
        anisotropic_ = false ;
        nb_anisotropic_iters_ = 3 ;
        anisotropic_factor_ = 1.5 ;
    }    

    void MapCurvature::compute_curvature_tensor() {
        double a = 1.0 / (M_PI * gx_sqr(radius_)) ;
        //ProgressLogger progress(map_->size_of_vertices()) ;
        FOR_EACH_VERTEX(Map, map_, it) {
            //if(progress.is_canceled()) {
            //    return ;
            //}
            //progress.next() ;
            NormalCycle ncycle ;
            if(radius_ > 0) {
                if(anisotropic_) {
                    compute_curvature_tensor_anisotropic(
                        it,radius_,nb_anisotropic_iters_,anisotropic_factor_,ncycle) ; 
                } else {
                    compute_curvature_tensor(it,radius_,ncycle) ;
                }
            } else {
                std::cerr << "one ring" << std::endl ;
                compute_curvature_tensor_one_ring(it, ncycle) ;
            }

            if(Kmin_.is_bound()) {
                Kmin_[it] = a* ncycle.kmin() * ncycle.Kmin() ;
            }

            if(Kmax_.is_bound()) {
                Kmax_[it] = a * ncycle.kmax() * ncycle.Kmax() ;
            }

            if(N_.is_bound()) {
                N_[it] =  a * ncycle.n() * ncycle.N() ;
            }

            if(kmin_.is_bound()) {
                kmin_[it] = ncycle.kmin() ;
            }

            if(kmax_.is_bound()) {
                kmax_[it] = ncycle.kmax() ;
            }
            
            if(n_.is_bound()) {
                n_[it] = ncycle.n() ;
            }
        }
    }

    void MapCurvature::compute_curvature_tensor_on_facets() {
        double a = (radius_ > 0.0) ? (1.0 / (M_PI * gx_sqr(radius_))) : 1.0 ;
        MapFacetProperty<vec3> Kmin(map_, "K1") ;
        MapFacetProperty<vec3> Kmax(map_, "K2") ;
		MapFacetProperty<double> kmin(map_, "k1") ;
        MapFacetProperty<double> kmax(map_, "k2") ;
//        ProgressLogger progress(map_->size_of_facets()) ;
        FOR_EACH_FACET(Map, map_, it) {
            //if(progress.is_canceled()) {
            //    break ;
            //}
            NormalCycle ncycle ;
            compute_curvature_tensor(it,radius_,ncycle) ;
            Kmin[it] = a * ncycle.kmin() * ncycle.Kmin() ;
            Kmax[it] = a * ncycle.kmax() * ncycle.Kmax() ;
			kmin[it] = ncycle.kmin();
		    kmax[it] = ncycle.kmax();
           // progress.next() ;
        }
    }

    static inline double angle(Map::Halfedge* h) {
        vec3 e  = Geom::vector(h) ;
        vec3 n1 = Geom::facet_normal(h->facet()) ;
        vec3 n2 = Geom::facet_normal(h->opposite()->facet()) ;
        double sine = dot(cross(n1, n2), e) / length(e) ;
        if(sine >= 1.0) {
            return M_PI / 2.0 ;
        }
        if(sine <= -1.0) {
            return -M_PI / 2.0 ;
        }
        return ::asin(sine) ;
    }

    void MapCurvature::compute_curvature_tensor(
        Map::Facet* start, double radius, NormalCycle& nc
    ) {
        nc.begin() ;
        std::set<Map::Vertex*> vertices ;
        vec3 O = Geom::facet_barycenter(start) ;
        std::stack<Map::Vertex*> S ;

        Map::Halfedge* h = start->halfedge() ;
        do {
            if(!h->is_border_edge()) {
                double beta = angle(h) ;
                vec3 V = Geom::vector(h->opposite()) ;
                nc.accumulate_dihedral_angle(V, beta) ;
            }
            S.push(h->vertex()) ;
            vertices.insert(h->vertex()) ;
            h = h->next() ;
        } while(h != start->halfedge()) ;

        if(radius > 0.0) {
            SphereNeighborhood neighborhood(O, radius) ;
            while(!S.empty()) {
                Map::Vertex* v = S.top() ;
                S.pop() ;
                const vec3& P = v->point() ;
                Map::Halfedge* h = v->halfedge() ;
                do {
                    vec3 V = Geom::vector(h->opposite()) ;
                    {
                        bool isect = neighborhood.clip_vector(P, V) ;
                        if(!h->is_border_edge()) {
                            double beta = angle(h) ;
                            nc.accumulate_dihedral_angle(V, beta) ;
                        }
                        if(!isect) {
                            Map::Vertex* w = h->opposite()->vertex() ;
                            if(vertices.find(w) == vertices.end()) {
                                vertices.insert(w) ;
                                S.push(w) ;
                            }
                        }
                    }
                    h = h->next_around_vertex() ;
                } while(h != v->halfedge()) ;
            }
        }
        nc.end() ;
    }


    void MapCurvature::compute_curvature_tensor(
        Map::Vertex* start, double radius, NormalCycle& nc
    ) {
        nc.begin() ;
        std::set<Map::Vertex*> vertices ;
        const vec3& O = start->point() ;
        std::stack<Map::Vertex*> S ;
        S.push(start) ;
        vertices.insert(start) ;
        SphereNeighborhood neighborhood(O, radius) ;
        while(!S.empty()) {
            Map::Vertex* v = S.top() ;
            S.pop() ;
            const vec3& P = v->point() ;
            Map::Halfedge* h = v->halfedge() ;
            do {
                vec3 V = Geom::vector(h->opposite()) ;
                if((v == start) || dot(V, P - O) > 0.0) {
                    bool isect = neighborhood.clip_vector(P, V) ;
                    if(!h->is_border_edge()) {
                        double beta = angle(h) ;
                        nc.accumulate_dihedral_angle(V, beta) ;
                    }
                    if(!isect) {
                        Map::Vertex* w = h->opposite()->vertex() ;
                        if(vertices.find(w) == vertices.end()) {
                            vertices.insert(w) ;
                            S.push(w) ;
                        }
                    }
                }
                h = h->next_around_vertex() ;
            } while(h != v->halfedge()) ;
        }
        nc.end() ;
    }



    void MapCurvature::compute_curvature_tensor_anisotropic(
        Map::Vertex* start, 
        double initial_radius, int nb_iter, double expansion_factor,
        NormalCycle& nc,
        std::vector<Map::Vertex*>* vertices_in,
        std::vector<CylinderNeighborhood>* neighborhoods_in 
    ) {

        MapTexVertexNormal normal(map_) ;

        nc.begin() ;

        std::set<Map::Vertex*> vertices ;
        const vec3& O = start->point() ;
        std::stack<Map::Vertex*> S ;
        S.push(start) ;
        vertices.insert(start) ;
        SphereNeighborhood neighborhood(O, initial_radius) ;
        while(!S.empty()) {
            Map::Vertex* v = S.top() ;
            S.pop() ;
            const vec3& P = v->point() ;
            Map::Halfedge* h = v->halfedge() ;
            do {
                vec3 V = Geom::vector(h->opposite()) ;
                if((v == start) || dot(V, P - O) > 0.0) {
                    bool isect = neighborhood.clip_vector(P, V) ;
                    if(!h->is_border_edge()) {
                        double beta = angle(h) ;
                        nc.accumulate_dihedral_angle(V, beta) ;
                    }
                    Map::Vertex* w = h->opposite()->vertex() ;
                    if(vertices.find(w) == vertices.end()) {
                        vertices.insert(w) ;
                        if(!isect) {
                            S.push(w) ;
                        }
                    }
                }
                h = h->next_around_vertex() ;
            } while(h != v->halfedge()) ;
        }
        nc.end() ;


        double radius = initial_radius ;
        for(int iter=0; iter<nb_iter; iter++) {
            radius *= expansion_factor ;
//            vec3 N = normal[start->halfedge()->tex_vertex()] ;
            vec3 N = nc.N() ;
            double l = gx_max(::fabs(nc.kmin()), ::fabs(nc.kmax())) ;
            vec3 K1 = (radius * nc.kmax() / l ) * nc.Kmax() ;
            vec3 K2 = (radius * nc.kmin() / l ) * nc.Kmin() ;
            CylinderNeighborhood neighborhood(O, K1, K2, N) ;
            if(neighborhoods_in != nil) {
                neighborhoods_in->push_back(neighborhood) ;
            }

            vertices.clear() ;
            S.push(start) ;
            vertices.insert(start) ;

            nc.begin() ;

            while(!S.empty()) {
                Map::Vertex* v = S.top() ;
                if(vertices_in != nil && (iter == nb_iter - 1)) {
                    vertices_in->push_back(v) ;
                }
                S.pop() ;
                const vec3& P = v->point() ;
                Map::Halfedge* h = v->halfedge() ;
                do {
                    vec3 V = Geom::vector(h->opposite()) ;
                    if((v == start) || dot(V, P - O) > 0.0) {
                        bool isect = neighborhood.clip_vector(P,V) ;
                        if(!h->is_border_edge()) {
                            double beta = angle(h) ;
                            nc.accumulate_dihedral_angle(V, beta) ;
                        }
                        Map::Vertex* w = h->opposite()->vertex() ;
                        if(vertices.find(w) == vertices.end()) {
                            vertices.insert(w) ;
                            if(!isect) {
                                S.push(w) ;
                            }
                        }
                    }
                    h = h->next_around_vertex() ;
                } while(h != v->halfedge()) ;
            }
            nc.end() ;
        }
    }


    void MapCurvature::compute_curvature_tensor_one_ring(
        Map::Vertex* start, NormalCycle& nc
    ) {
        nc.begin() ;
        Map::Halfedge* h = start->halfedge() ;
        do {
            if(!h->is_border_edge()) {
                nc.accumulate_dihedral_angle(Geom::vector(h), angle(h)) ;
            }
            if(!h->prev()->is_border_edge()) {
                nc.accumulate_dihedral_angle(
                    Geom::vector(h->prev()), angle(h->prev())
                ) ;
            }
            h = h->next_around_vertex() ;
        } while(h != start->halfedge()) ;
        nc.end() ;
    }

}
