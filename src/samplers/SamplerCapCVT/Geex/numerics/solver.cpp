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
 

#include <Geex/numerics/solver.h>
#include <Geex/numerics/sparse_matrix_crs.h>
#include <Geex/numerics/sparse_matrix_conversion.h>
#include <Geex/numerics/quick_blas.h>
#include <Geex/basics/stopwatch.h>
#include <iostream>
#include <fstream>

namespace Geex {
    
    // The generic conjugate gradient routine, adapted to various Matrix data structures.
    template <class MatrixType, class VectorType> inline void solve_cg(
        const MatrixType& A, VectorType& x, const VectorType& b, double eps, unsigned int max_iter
    ) {
       
        QuickBLAS::flops_counter = 0 ;

        size_t N = x.size() ;

        VectorType r(N, alignment_for_SSE2) ;
        VectorType d(N, alignment_for_SSE2) ;
        VectorType h(N, alignment_for_SSE2) ;
        VectorType& Ad = h ;

        VectorType diag_inv(N, alignment_for_SSE2) ;
        for(index_t i=0; i<N; i++) {
            diag_inv[i] = (i >= A.diag.size() || A.diag[i] == 0.0) ? 1.0 : 1.0 / A.diag[i] ;
        }

        unsigned int its=0;
        double rh, alpha, beta;
        double bnorm2 = QuickBLAS::squared_nrm2(b) ;
        double err = eps * eps * bnorm2 ;
  
        A.mult(x,r) ;
        QuickBLAS::axpy(-1.,b,r);

        // d = PRECOND * r
        QuickBLAS::mul(diag_inv, r, d) ;

        QuickBLAS::copy(d,h);
        rh=QuickBLAS::dot(r,h);
        
        double cur_err ;

        SystemStopwatch watch ;
        ProcessorStopwatch p_watch ;

        while ( (cur_err = QuickBLAS::squared_nrm2(r))>err && its < max_iter) {
            if(!(its & 127)) {
                std::cerr << its << " : " << cur_err << " -- " << err << std::endl ;
            }

            A.mult(d,Ad);
            alpha=rh/QuickBLAS::dot(d,Ad);
            QuickBLAS::axpy(-alpha,d,x);
            QuickBLAS::axpy(-alpha,Ad,r);

            // h = PRECOND * r
            QuickBLAS::mul(diag_inv, r, h) ;

            beta=1./rh; rh=QuickBLAS::dot(r,h); beta*=rh;
            QuickBLAS::scal(beta,d);
            QuickBLAS::axpy(1.,h,d);
            ++its;
        }
        std::cerr << "CG used " << its << " iterations" << std::endl ;
        double seconds = watch.elapsed_user_time() ;
        std::cerr << "CG main loop time: " << seconds << std::endl ;
        std::cerr << "     Processor clock ticks: " << 1e-9 * double(p_watch.elapsed_time()) << " G" << std::endl ;
        double mflops = (1e-6 * double(QuickBLAS::flops_counter) / seconds) ;
        std::cerr << "MFlops = " << mflops << std::endl ;
    }

    // Computes the matrix permutation that optimizes memory accesses.
    // We use the RCMK (Reverse Cuthill McKee) ordering.
    static void compute_permutation(const SparseMatrix& A, Permutation& permutation) {
        permutation.allocate(A.n()) ;
        bool RCMK = true ; // Deactivated for the moment (there is a bug in there)
        if(RCMK) {
            SparseMatrixPatternCRS A_pattern ;
            ::Geex::convert_matrix(A, A_pattern) ;
            A_pattern.compute_RCMK_ordering(permutation) ;
            if(true) {
                std::cerr << "Bandwidth before permutation: " << A_pattern.bandwidth() << std::endl ;
                SparseMatrixCRS<double> A_crs ;
                ::Geex::convert_matrix(A, A_crs, true, permutation) ;            
                std::cerr << "Bandwidth after permutation: " << A_crs.bandwidth() << std::endl ;
            }
        } else {
            permutation.load_identity() ;
        }
    }

    // This is the interface to the solver, adapted to the SparseMatrix (dynamic)
    // data structure. It converts the input matrices and vectors into data formats
    // optimized for the fast solver.

    static void solve_cg(const SparseMatrix& A_in, Vector& x_in, Vector& b_in) {

        // The solver converges much much quicker with double precision numbers.
        typedef double CoeffType ;
        typedef Array1d<CoeffType> VectorType ;

        // With 2x2 blocking, a matrix block fits exactly in a L1 cache line and in
        // two SSE2 registers, this is good !
        // Note that the SparseMatrixBCRS x vector mutliply uses a specialization
        // with SSE code (see sparse_matrix_crs.h).
        typedef SparseMatrixBCRS<CoeffType, 2, 2> MatrixType ;

        size_t N0 = A_in.n() ;
        std::cerr << "N0 = " << N0 << std::endl ;
        Permutation permutation; 
        compute_permutation(A_in, permutation) ;

        MatrixType A ;
        ::Geex::convert_matrix(A_in, A, permutation) ;

        std::cerr << "filling ratio:" << (A.filling_ratio() * 100.0) << "%" << std::endl ;
        std::cerr << "max nb blocs per row:" << A.max_number_of_blocs_in_row() << std::endl ;
        if(false) {
            std::ofstream out("matrix.dat") ;
            A.print(out) ;
//            ::OGF::output_matrix(A, out) ;
        }

        size_t N = A.n() ; // Can be greater than N0 due to blocking
        N = QuickBLAS::aligned_size(N, sizeof(CoeffType)) ;
        
        std::cerr <<"N = " << N << std::endl ;

        unsigned int max_iter = 5 * N ;
        double eps = 1e-6  ;
        
        std::cerr << "nb iters = " << max_iter << "  threshold = " << eps << std::endl ;

        VectorType b(N, alignment_for_SSE2) ;
        VectorType x(N, alignment_for_SSE2) ;

        permutation.invert_permute_vector(b_in, b) ;
        permutation.invert_permute_vector(x_in, x) ;

        solve_cg(A, x, b, eps, max_iter) ;

        permutation.permute_vector(x, x_in) ;

    }

    Solver::Solver() {
        nb_variables_ = 0 ;
        variable_ = nil ;
        state_= INITIAL ;
        reset_no_matrix_ = false ;
    }

    Solver::Solver(size_t nb_variables) {
        nb_variables_ = 0 ;
        variable_ = nil ;
        state_= INITIAL ;
        allocate(nb_variables) ;
    }

    Solver::~Solver() {
        delete[] variable_ ;
        variable_ = nil ;
    }

    void Solver::allocate(size_t nb_variables) {
        gx_assert(state_ == INITIAL) ;
        gx_assert(nb_variables_ == 0) ;
        nb_variables_ = nb_variables ;
        variable_ = new SolverVariable[nb_variables] ;
    }

    bool Solver::solve() {
        check_state(CONSTRUCTED) ;
        solve_cg(G_, x_, b_) ;
        vector_to_variables(x_) ;
//        transition(CONSTRUCTED, SOLVED) ;
        return true ;
    }

    void Solver::begin_system() {
        transition(INITIAL, IN_SYSTEM) ;

        // Enumerate free variables.
        index_t index = 0 ;
        for(index_t ii=0; ii < nb_variables() ; ii++) {
            SolverVariable& v = variable(ii) ;
            if(!v.is_locked()) {
                set_variable_index(v,index) ;
                index++ ;
            }
        }

        // Size of the system to solve.
        size_t n = index ;
        
        SparseMatrix::Storage storage = SparseMatrix::ROWS ;
        bool sym = false ;

        x_.allocate(n) ;
        b_.allocate(n) ;

        if(!reset_no_matrix_) {
            G_.allocate(n,n, storage, sym) ;
        }

        x_.zero() ;
        b_.zero() ;
        variables_to_vector(x_) ;
    }
    
    void Solver::end_system() {
        transition(IN_SYSTEM, CONSTRUCTED) ;
    }
    
    void Solver::begin_row() {
        transition(IN_SYSTEM, IN_ROW) ;
        af_.clear() ;
        if_.clear() ;
        al_.clear() ;
        xl_.clear() ;
        bk_ = 0.0 ;
    }
    
    void Solver::set_right_hand_side(double b) {
        check_state(IN_ROW) ;
        bk_ = b ;
    }
    
    void Solver::add_coefficient(index_t iv, double a) {
        check_state(IN_ROW) ;
        SolverVariable& v = variable(iv) ;
        if(v.is_locked()) {
            al_.push_back(a) ;
            xl_.push_back(v.value()) ;
        } else {
            af_.push_back(a) ;
            if_.push_back(v.index()) ;
        }
    }
    
    void Solver::normalize_row(double weight) {
        check_state(IN_ROW) ;
        double norm = 0.0 ;
        size_t nf = (size_t)af_.size() ;
        for(index_t i=0; i<nf; i++) {
            norm += af_[i] * af_[i] ;
        }
        size_t nl = (size_t)al_.size() ;
        for(index_t i=0; i<nl; i++) {
            norm += al_[i] * al_[i] ;
        }
        norm = ::sqrt(norm) ;
        scale_row(weight / norm) ;
    }
    
    void Solver::scale_row(double s) {
        check_state(IN_ROW) ;
        s = ::fabs(s) ;
        size_t nf = (size_t)af_.size() ;
        for(index_t i=0; i<nf; i++) {
            af_[i] *= s ;
        }
        size_t nl = (size_t)al_.size() ;
        for(index_t i=0; i<nl; i++) {
            al_[i] *= s ;
        }
        bk_ *= s ; 
    }
    
    void Solver::end_row() {
        
        size_t nf = (size_t)af_.size() ;
        size_t nl = (size_t)al_.size() ;

        if(!reset_no_matrix_) {
            for(index_t i=0; i<nf; i++) {
                for(index_t j=0; j<nf; j++) {
                    G_.add(if_[i], if_[j], af_[i] * af_[j]) ;
                }
            }
        }

        double S = - bk_ ;
                
        for(index_t j=0; j<nl; j++) {
            S += al_[j] * xl_[j] ;
        }
                
        for(index_t i=0; i<nf; i++) {
            b_(if_[i]) -= af_[i] * S ;
        }

        transition(IN_ROW, IN_SYSTEM) ; 
    }

    // _______________________________________________________________

    void Solver::vector_to_variables(const Vector& x) {
        for(index_t ii=0; ii < nb_variables(); ii++) {
            SolverVariable& v = variable(ii) ;
            if(!v.is_locked()) {
                v.set_value(x(v.index())) ;
            }
        }
    }

    void Solver::variables_to_vector(Vector& x) {
        for(index_t ii=0; ii < nb_variables(); ii++) {
            SolverVariable& v = variable(ii) ;
            if(!v.is_locked()) {
                x(v.index()) = v.value() ;
            }
        }
    }


    // _______________________________________________________________
        
    std::string Solver::state_to_string(State s) {
        switch(s) {
        case INITIAL:
            return "initial" ;
        case IN_SYSTEM:
            return "in system" ;
        case IN_ROW: 
            return "in row" ;
        case CONSTRUCTED: 
            return "constructed" ;
        case SOLVED:
            return "solved" ;
        } 
        // Should not go there.
        gx_assert(false) ;
        return "undefined" ;
    }
        
    void Solver::check_state(State s) {
        if(state_ != s) {
            std::cerr << "Wrong state, expected: "
                      << state_to_string(s)
                      << " got:"
                      << state_to_string(state_)
                      << std::endl ;
            std::cerr << "exiting ..." << std::endl ;
        }
        gx_assert(state_ == s) ;
    }
        
    void Solver::transition(State from, State to) {
        check_state(from) ;
        state_ = to ;
    }

    void Solver::reset(bool clear_matrix) {
        if(clear_matrix) {
            G_.clear() ;
            reset_no_matrix_ = false ;
        } else {
            reset_no_matrix_ = true ;
        }
        x_.set_all(0.0) ;
        b_.set_all(0.0) ;
        state_ = INITIAL ;
    }
    
}

