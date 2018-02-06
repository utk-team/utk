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
 
#ifndef __GEEX_NUMERICS_SYSTEM_SOLVER__
#define __GEEX_NUMERICS_SYSTEM_SOLVER__

#include <Geex/numerics/sparse_matrix.h>
#include <Geex/basics/arrays.h>
#include <Geex/basics/smart_pointer.h>
#include <Geex/basics/counted.h>

#include <map>

namespace Geex {

    class GEEX_API SystemSolver : public Counted {
    public:
        SystemSolver() ;
        virtual bool solve(const SparseMatrix& A, Vector& x, const Vector& b) = 0 ;
        virtual bool needs_rows() const ;                             // default returns true
        virtual bool needs_columns() const ;                       // default returns false
        virtual bool supports_symmetric_storage() const ; // default returns false
    } ;

    typedef SmartPointer<SystemSolver> SystemSolver_var ;

    //-----------------------------------------------------------------------------------------------------------------

    enum SystemSolverOrdering {
        ORDER_NONE,
        ORDER_DEFAULT,
        ORDER_COLAMD,                // Approximate minimum degree ordering
        ORDER_MMD_AT_PLUS_A  // Symmetric mode (for nearly symmetric matrices)
    } ;

    class GEEX_API DirectSystemSolver : public SystemSolver {
    public:
        DirectSystemSolver() ;
        bool keep_permutation_matrix() const  { return keep_permutation_matrix_ ; }
        void set_keep_permutation_matrix(bool x) { keep_permutation_matrix_ = x ; }
        void set_permutation_matrix(const Array1d<int>& perm) ;
        const Array1d<int>& permutation_matrix() const { return perm_ ; }
    protected:
        bool keep_permutation_matrix_ ;
        Array1d<int> perm_ ;
        SystemSolverOrdering ordering_ ;
    } ;

   //--------------------------------------------------------------------------------------------------------------------------------------    
}


#endif

