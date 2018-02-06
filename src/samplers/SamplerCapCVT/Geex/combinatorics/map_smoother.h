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
 

#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_properties.h>
#include <Geex/numerics/solver.h>

namespace Geex {

    class GEEX_API MapSmoother {
    public:
        MapSmoother() ;
        void set_solver(Solver* solver) { solver_ = solver ; }
        void set_weight(double w) { weight_ = w ; }
        void smooth(Map* map) ;

    protected:
        void build_matrix(index_t coord) ;
        void map_to_solver(index_t coord) ;
        void solver_to_map(index_t coord) ;

    private:
        Solver_var solver_ ;
        Map* map_ ;
        bool has_locked_vertices_ ;
        double weight_ ; 
        MapVertexLock is_locked_ ;
        MapVertexProperty<index_t> vertex_id_ ;
        MapVertexProperty<vec3> geometry_ ;

    private:
        MapSmoother(const MapSmoother& rhs) ;
        MapSmoother& operator=(const MapSmoother& rhs) ;
    } ;

}
