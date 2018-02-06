/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000-2003 Bruno Levy
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
 
#ifndef __GEEX_COMBINATORICS_MAP_CURVATURE__
#define __GEEX_COMBINATORICS_MAP_CURVATURE__

#include <Geex/combinatorics/map.h> //<OGF/cells/map/map.h>
#include <Geex/combinatorics/map_properties.h> //<OGF/cells/map/map_attributes.h>

namespace Geex {

    class NormalCycle ;

    class GEEX_API MapCurvature {
    public:

        struct SphereNeighborhood {
            SphereNeighborhood(const vec3& p, double r) : center(p), radius(r) { }
            // precondition1: P is inside the sphere
            // precondition2: P,V points to the outside of 
            //   the sphere (i.e. OP.V > 0)
            inline bool clip_vector(const vec3& P, vec3& V) const ;
            vec3 center ;
            double radius ;
        } ;

        struct CylinderNeighborhood {
            CylinderNeighborhood(
                const vec3& p, 
                const vec3& x, const vec3& y, const vec3& z
            ) : center(p), Xx(x), Yy(y), Zz(z) { }

            inline bool contains(const vec3& P) const ;

            // precondition1: P is inside the cylinder
            // precondition2: P,V points to the outside of 
            //   the cylinder
            inline bool clip_vector(const vec3& P, vec3& V) const ;

            vec3 center ;
            vec3 Xx ;
            vec3 Yy ;
            vec3 Zz ;
        } ;

    public:
        MapCurvature(Map* map) ;
        void set_kmin_attribute(const std::string& name) {
            kmin_.bind(map_, name) ;
        }
        void set_kmax_attribute(const std::string& name) {
            kmax_.bind(map_, name) ;
        }
        void set_n_attribute(const std::string& name) {
            n_.bind(map_, name) ;
        }
        void set_Kmin_attribute(const std::string& name) {
            Kmin_.bind(map_, name) ;
        }
        void set_Kmax_attribute(const std::string& name) {
            Kmax_.bind(map_, name) ;
        }
        void set_N_attribute(const std::string& name) {
            N_.bind(map_, name) ;
        }
        void set_radius(double r) { radius_ = r ; }
        void set_anisotropic(bool b) { anisotropic_ = b ; }
        void set_nb_anisotropic_iters(int x) { nb_anisotropic_iters_ = x ; }
        void set_anisotropic_factor(double x) { anisotropic_factor_  = x ; }

        void compute_curvature_tensor() ;
        void compute_curvature_tensor_on_facets() ;

        void compute_curvature_tensor(
            Map::Vertex* start, double radius, NormalCycle& nc
        ) ;

        void compute_curvature_tensor(
            Map::Facet* start, double radius, NormalCycle& nc
        ) ;

        void compute_curvature_tensor_anisotropic(
            Map::Vertex* start, 
            double initial_radius, int nb_iter, double expansion_factor, 
            NormalCycle& nc,
            std::vector<Map::Vertex*>* vertices = nil,
            std::vector<CylinderNeighborhood>* neighborhoods = nil
        ) ;

        void compute_curvature_tensor_one_ring(
            Map::Vertex* start, NormalCycle& nc
        ) ;

    private:
        Map* map_ ;
        double radius_ ;
        bool anisotropic_ ;
        int nb_anisotropic_iters_ ;
        double anisotropic_factor_ ;
        MapVertexProperty<double> kmin_ ;
        MapVertexProperty<double> kmax_ ;
        MapVertexProperty<double> n_ ;
        MapVertexProperty<vec3> Kmin_ ;
        MapVertexProperty<vec3> Kmax_ ;
        MapVertexProperty<vec3> N_ ;
    } ;

    //=========================================================================

    // precondition1: P is inside the sphere
    // precondition2: P,V points to the outside of 
    //   the sphere (i.e. OP.V > 0)
    inline bool MapCurvature::SphereNeighborhood::clip_vector(
        const vec3& P, vec3& V
    ) const {
        vec3 W = P - center ;
        double a = length2(V) ;
        double b = 2.0 * dot(V, W) ;
        double c = length2(W) - radius*radius ;
        double delta = b*b - 4*a*c ;
        if(delta < 0) {
            // Should not happen, but happens sometimes (numerical precision)
            return true ;
        }
        double t = (-b + ::sqrt(delta)) / (2.0 * a) ;
        if(t < 0.0) {
            // Should not happen, but happens sometimes (numerical precision)
            return true ;
        }
        if(t >= 1.0) {
            // Inside the sphere
            return false ;
        }
        V.x = t * V.x ;
        V.y = t * V.y ;
        V.z = t * V.z ;
        return true ;
    }

    inline bool MapCurvature::CylinderNeighborhood::contains(
        const vec3& P
    ) const {
        vec3 W = P - center ;
        double x = dot(W, Xx) / length2(Xx) ;
        double y = dot(W, Yy) / length2(Yy) ;
        if((x*x + y*y) > 1) {
            return false ;
        } 
        double z = dot(W, Zz) / length2(Zz) ;
        if(z*z > 1) {
            return false ;
        }
        return true ;
    }

    inline bool MapCurvature::CylinderNeighborhood::clip_vector(
        const vec3& P, vec3& V
    ) const {
        if(!contains(P)) {
            return true ;
        }

        if(!contains(P + V)) {
            return true ;
        }
        
        return false ;
    }
    

}

#endif
