/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000 Bruno Levy
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
 

#include <Geex/mathematics/normal_cycle.h>
#include <Geex/mathematics/matrix_util.h>
//#include <OGF/math/linear_algebra/matrix_util.h>

namespace Geex {

//_________________________________________________________
    

    NormalCycle::NormalCycle() {
    }

    void NormalCycle::begin() {
        M_[0] = M_[1] = M_[2] = M_[3] = M_[4] = M_[5] = 0 ;
    }
    
    void NormalCycle::end() {

        double trace = M_[0] + M_[2] + M_[5] ;
        double s = 1e-6 * trace ;
		if (trace==0)
			s = 1e-6;
        M_[0] += s ;
        M_[2] += s ;
        M_[5] += s ;
	 
       
        double eigen_vectors[9] ;
        MatrixUtil::semi_definite_symmetric_eigen(M_, 3, eigen_vectors, eigen_value_) ;
            
        axis_[0] = vec3(
            eigen_vectors[0], eigen_vectors[1], eigen_vectors[2]
        ) ;
            
        axis_[1] = vec3(
            eigen_vectors[3], eigen_vectors[4], eigen_vectors[5]
        ) ;
        
        axis_[2] = vec3(
            eigen_vectors[6], eigen_vectors[7], eigen_vectors[8]
        ) ;
        
        // Normalize the eigen vectors
        
        for(int i=0; i<3; i++) {
            axis_[i] = normalize(axis_[i]) ;
        }

        // Sort the eigen vectors

        i_[0] = 0 ;
        i_[1] = 1 ;
        i_[2] = 2 ;

        double l0 = ::fabs(eigen_value_[0]) ;
        double l1 = ::fabs(eigen_value_[1]) ;
        double l2 = ::fabs(eigen_value_[2]) ;
        
        if(l1 > l0) {
            gx_swap(l0   , l1   ) ;
            gx_swap(i_[0], i_[1]) ;
        }
        if(l2 > l1) {
            gx_swap(l1   , l2   ) ;
            gx_swap(i_[1], i_[2]) ;
        }
        if(l1 > l0) {
            gx_swap(l0   , l1  ) ;
            gx_swap(i_[0],i_[1]) ;
        }

    }

    void NormalCycle::accumulate_dihedral_angle(
        const vec3& edge, double beta, double neigh_area
    ) {
        vec3 e = normalize(edge) ;
        
        double s = length(edge) * beta * neigh_area ;

        M_[0] += s * e.x * e.x ;
        M_[1] += s * e.x * e.y ;
        M_[2] += s * e.y * e.y ;
        M_[3] += s * e.x * e.z ;
        M_[4] += s * e.y * e.z ;
        M_[5] += s * e.z * e.z ;        
    }
    
//_________________________________________________________

}

