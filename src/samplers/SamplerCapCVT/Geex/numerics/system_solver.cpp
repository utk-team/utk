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

#include <Geex/numerics/system_solver.h>
#include <Geex/numerics/sparse_matrix.h>
#include <Geex/basics/stopwatch.h>
#include <Geex/basics/assert.h>

#include <iostream>

namespace Geex {

    SystemSolver::SystemSolver() {
    }

    bool SystemSolver::needs_rows() const {
        return true ;
    }
    
    bool SystemSolver::needs_columns() const {
        return false ;
    }
    
    bool SystemSolver::supports_symmetric_storage() const {
        return false ;
    }

    //_______________________________________________________________________________________________

    DirectSystemSolver::DirectSystemSolver() {
        keep_permutation_matrix_ = false ;
        ordering_ = ORDER_COLAMD ;
    }
    
    void DirectSystemSolver::set_permutation_matrix(const Array1d<int>& perm) {
        perm_.allocate(perm.size()) ;
        for(unsigned int i=0; i<perm.size(); i++) {
            perm_(i) = perm(i) ;
        }
    }

    //_______________________________________________________________________________________________

}
