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
 

#ifndef __GEEX_NUMERICS_LINEAR_SOLVER__
#define __GEEX_NUMERICS_LINEAR_SOLVER__

#include <Geex/numerics/vector.h>
#include <Geex/numerics/sparse_matrix.h>
#include <Geex/basics/types.h>
#include <Geex/basics/assert.h>
#include <Geex/basics/counted.h>
#include <Geex/basics/smart_pointer.h>

#include <vector>

namespace Geex {
    
    class GEEX_API SolverVariable {
    public:
        SolverVariable() : x_(0), index_(-1), locked_(false) { }
        
        double value() const { return x_; }
        void set_value(double x_in) { x_ = x_in ; }
        
        void lock()   { locked_ = true ; }
        void unlock() { locked_ = false ; }
        bool is_locked() const { return locked_ ; }
        
        signed_index_t index() const {
            gx_debug_assert(index_ != -1) ;
            return index_ ;
        }
        
    protected:
        void set_index(index_t index_in) {
            index_ = index_in ; 
        }
        
    private:
        double x_ ;
        signed_index_t index_ ;
        bool locked_ ;
        
        friend class Solver ;
    } ;

//____________________________________________________________________________

    class GEEX_API Solver : public Counted {
    public:
        Solver() ;
        Solver(size_t nb_variables) ;
        virtual ~Solver() ;

        void allocate(size_t nb_variables) ;
        size_t nb_variables() const { return nb_variables_ ; }
        
        SolverVariable& variable(index_t idx) { 
            gx_assert(idx >= 0 && idx < nb_variables_) ;
            return variable_[idx] ;
        }
            
        const SolverVariable& variable(index_t idx) const {
            gx_assert(idx >= 0 && idx < nb_variables_) ;
            return variable_[idx] ;
        }
        
        void set_variable_index(SolverVariable& var, index_t index) {
            var.set_index(index) ;
        }

        virtual bool solve() ;

        void begin_system() ;
        void end_system() ;
        
        void begin_row() ;
        void set_right_hand_side(double b) ;
        void add_coefficient(index_t iv, double a) ;
        void normalize_row(double weight = 1.0) ;
        void scale_row(double s) ;
        void end_row() ;

        void reset(bool clear_matrix = true) ;

    protected:
        void vector_to_variables(const Vector& x) ;
        void variables_to_vector(Vector& x) ;
        
        // Finite state automaton
        
        enum State {
            INITIAL, IN_SYSTEM, IN_ROW, CONSTRUCTED, SOLVED
        } ;
        
        std::string state_to_string(State s) ;
        void check_state(State s) ;
        void transition(State from, State to) ;

    protected:
        // User representation
        size_t nb_variables_ ;
        SolverVariable* variable_ ;

        // Internal representation
        Vector x_ ; 
        SparseMatrix G_ ;
        Vector b_ ;

        // Current row of A
        std::vector<double> af_ ;
        std::vector<index_t> if_ ;
        std::vector<double> al_ ;
        std::vector<double> xl_ ;
        double bk_ ;

        bool reset_no_matrix_ ;

        // Finite state automaton
        State state_ ;
    } ;

    typedef SmartPointer<Solver> Solver_var ;

//____________________________________________________________________________

} 

#endif
