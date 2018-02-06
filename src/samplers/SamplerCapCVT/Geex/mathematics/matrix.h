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

#ifndef __GEEX_MATHEMATICS_MATRIX__
#define __GEEX_MATHEMATICS_MATRIX__

#include <Geex/basics/types.h>
#include <Geex/basics/assert.h>

#include <iostream>

namespace Geex {

//______________________________________________________________________


    template <class FT, int DIM> class Matrix ;
    template <class FT, int DIM> Matrix<FT, DIM> operator*(
        FT op1, const Matrix<FT, DIM>& op2
    ) ;


  /**
    * A class for representing dense matrices.
    */
    template <class FT, int DIM> class Matrix {
    public:
        Matrix() ;
        void load_zero() ;
        void load_identity() ;

        FT& operator()(int i, int j) ;
        const FT& operator()(int i, int j) const ;

        Matrix<FT, DIM>& operator+=(const Matrix<FT, DIM>& rhs) ;
        Matrix<FT, DIM>& operator-=(const Matrix<FT, DIM>& rhs) ;
        Matrix<FT, DIM>& operator*=(FT rhs) ;
        Matrix<FT, DIM>& operator/=(FT rhs) ;

        Matrix<FT, DIM> operator+(const Matrix<FT, DIM>& op2) const ;
        Matrix<FT, DIM> operator-(const Matrix<FT, DIM>& op2) const ;
        Matrix<FT, DIM> operator*(const Matrix<FT, DIM>& op2) const ;

		Matrix<FT, DIM> inverse(bool *is_singular, double threshold=1.0e-8) const ;
        Matrix<FT, DIM> inverse() const ;
        Matrix<FT, DIM> transpose() const ;
		void  gaussian_elimination();

		FT trace() const;
        void mult(const FT* x, FT* y) const ;

        // Routines for interfacing with Fortran, OpenGL etc...

        const FT* data() const { return &(coeff_[0][0]) ; }
        FT* data() { return &(coeff_[0][0]) ; }

        void get_lower_triangle(FT* store) {
            for(unsigned int i=0; i<DIM; i++) {
                for(unsigned int j=0; j<=i; j++) {
                    *store++ = coeff_[i][j] ;
                }
            }
        }

    private:
        FT coeff_[DIM][DIM] ;
    } ;

//_______________________________________________________________________

    template <class FT, int DIM> inline 
    Matrix<FT, DIM>::Matrix() {
        load_identity() ;
    }


    template <class FT, int DIM> inline 
    FT& Matrix<FT, DIM>::operator()(int i, int j) {
        gx_assert(i >= 0 && i < DIM) ;
        gx_assert(j >= 0 && j < DIM) ;
        return coeff_[i][j] ;
    }

    template <class FT, int DIM> inline 
    const FT& Matrix<FT, DIM>::operator()(int i, int j) const {
        gx_assert(i >= 0 && i < DIM) ;
        gx_assert(j >= 0 && j < DIM) ;
        return coeff_[i][j] ;
    }

    template <class FT, int DIM> inline 
    Matrix<FT, DIM>& Matrix<FT, DIM>::operator+=(const Matrix<FT, DIM>& rhs) {
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                coeff_[i][j] += rhs.coeff_[i][j] ;
            }
        }
        return *this ;
    }

    template <class FT, int DIM> inline 
    Matrix<FT, DIM>& Matrix<FT, DIM>::operator-=(const Matrix<FT, DIM>& rhs) {
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                coeff_[i][j] -= rhs.coeff_[i][j] ;
            }
        }
        return *this ;
    }

    template <class FT, int DIM> inline 
    Matrix<FT, DIM>& Matrix<FT, DIM>::operator*=(FT rhs) {
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                coeff_[i][j] *= rhs ;
            }
        }
        return *this ;
    }

    template <class FT, int DIM> inline 
    Matrix<FT, DIM>& Matrix<FT, DIM>::operator/=(FT rhs) {
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                coeff_[i][j] /= rhs ;
            }
        }
        return *this ;
    }

    template <class FT, int DIM> inline 
    Matrix<FT, DIM> Matrix<FT, DIM>::operator+(const Matrix<FT, DIM>& op2) const {
        Matrix<FT, DIM> result = *this ;
        result += op2 ;
        return result ;
    }

    template <class FT, int DIM> inline 
    Matrix<FT, DIM> Matrix<FT, DIM>::operator-(const Matrix<FT, DIM>& op2) const {
        Matrix<FT, DIM> result = *this ;
        result -= op2 ;
        return result ;
    
    }

    template <class FT, int DIM> inline 
    Matrix<FT, DIM> Matrix<FT, DIM>::operator*(const Matrix<FT, DIM>& op2) const {
        Matrix<FT, DIM> result ;
        result.load_zero() ;
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                for(int k=0; k<DIM; k++) {
                    result.coeff_[i][j] += coeff_[i][k] * op2.coeff_[k][j] ;
                }
            }
        }
        return result ;
    }

    template <class FT, int DIM> inline 
    std::ostream& operator << (std::ostream& output, const Matrix<FT, DIM>& m) {
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                output << m(i,j) << " " ;
            }
        }    
        return output ;
    }

    template <class FT, int DIM> inline 
    std::istream& operator >> (std::istream& input, Matrix<FT, DIM>& m) {
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                input >> m(i,j) ;
            }
        }    
        return input ;
    }



//_______________________________________________________________________

    template <class FT, int DIM> void
    Matrix<FT, DIM>::load_zero() {
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                coeff_[i][j] = FT(0) ;
            }
        }
    }

    template <class FT, int DIM> void
    Matrix<FT, DIM>::load_identity() {
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                coeff_[i][j] = (i==j) ? FT(1) : FT(0) ;
            }
        }
    }

    template <class FT, int DIM> Matrix<FT, DIM> 
    Matrix<FT, DIM>::inverse() const {
        FT val, val2;
        int i, j, k, ind;
        Matrix<FT, DIM> tmp = (*this);
        Matrix<FT, DIM> result;
    
        result.load_identity();
    
        for (i = 0; i != DIM; i++) {
            val = tmp(i,i);			/* find pivot */
            ind = i;
            for (j = i + 1; j != DIM; j++) {
                if (fabs(tmp(j,i)) > fabs(val)) {
                    ind = j;
                    val = tmp(j,i);
                }
            }
            
            if (ind != i) {			
                for (j = 0; j != DIM; j++) {
                    val2 = result(i,j);
                    result(i,j) = result(ind,j);
                    result(ind,j) = val2;           /* swap columns */
                    val2 = tmp(i,j);
                    tmp(i,j) = tmp(ind,j);
                    tmp(ind,j) = val2;
                }
            }
            
            gx_assert(val != 0.0);
            
            for (j = 0; j != DIM; j++) {
                tmp(i,j)    /= val;
                result(i,j) /= val;
            }
        
            for (j = 0; j != DIM; j++) {		
                if (j == i)
                    continue;                       /* eliminate column */
                val = tmp(j,i);
                for (k = 0; k != DIM; k++) {
                    tmp(j,k)     -= tmp(i,k)     * val;
                    result(j,k)  -= result(i,k)  * val;
                }
            }
        }
    
        return result;
    }

	template <class FT, int DIM> FT 
		Matrix<FT, DIM>::trace() const
	{
		FT result = FT(0);
		for (int i=0; i<DIM; i++)
			result+=coeff_[i][i];
		return result;
	}

	template <class FT, int DIM> void 
		Matrix<FT, DIM>::gaussian_elimination() // produces the UPPER RIGHT triangle matrix of the LU factorization
	{
		for (int k = 0; k<DIM; k++) {
			// Find pivot for column k:
			int i_max = k; 
			FT max_abs = coeff_[k][k];
			for (int i=k; i<DIM; i++) {
				if (fabs(coeff_[i][k])>max_abs) {
					max_abs = fabs(coeff_[i][k]);
					i_max = i;
				}
			}
			if (coeff_[i_max][k] == FT(0))		   
				continue; //error "Matrix is singular!"

			for (int i=0; i<DIM; i++)
				std::swap(coeff_[k][i], coeff_[i_max][i]);

			for (int i = k + 1; i< DIM; i++) {				
				for (int j = k + 1; j<DIM; j++) {
					coeff_[i][j] -= coeff_[k][j] * (coeff_[i][k] / coeff_[k][k]);
				}
				//Fill lower triangular matrix with zeros:
				coeff_[i][k] = FT(0);
			}
		}
	}

	template <class FT, int DIM> Matrix<FT, DIM> 
	Matrix<FT, DIM>::inverse(bool *is_singular, double threshold) const {
		FT val, val2;
		int i, j, k, ind;
		Matrix<FT, DIM> tmp = (*this);
		Matrix<FT, DIM> result;

		result.load_identity();

		if(is_singular) {
			*is_singular = false;
		}

		for (i = 0; i != DIM; i++) {
			val = tmp(i,i);			/* find pivot */
			ind = i;
			for (j = i + 1; j != DIM; j++) {
				if (fabs(tmp(j,i)) > fabs(val)) {
					ind = j;
					val = tmp(j,i);
				}
			}

			if (ind != i) {			
				for (j = 0; j != DIM; j++) {
					val2 = result(i,j);
					result(i,j) = result(ind,j);
					result(ind,j) = val2;           /* swap columns */
					val2 = tmp(i,j);
					tmp(i,j) = tmp(ind,j);
					tmp(ind,j) = val2;
				}
			}

			if(is_singular) {
				if (fabs(val) < threshold) {
					*is_singular = true;
					return result;
				}
			} else {
				gx_assert(val != 0.0) ;
			}

			for (j = 0; j != DIM; j++) {
				tmp(i,j)    /= val;
				result(i,j) /= val;
			}

			for (j = 0; j != DIM; j++) {		
				if (j == i)
					continue;                       /* eliminate column */
				val = tmp(j,i);
				for (k = 0; k != DIM; k++) {
					tmp(j,k)     -= tmp(i,k)     * val;
					result(j,k)  -= result(i,k)  * val;
				}
			}
		}

		return result;
	}

    template <class FT, int DIM> Matrix<FT, DIM> 
    Matrix<FT, DIM>::transpose() const {
        Matrix<FT, DIM> result ;
        for(int i=0; i<DIM; i++) {
            for(int j=0; j<DIM; j++) {
                result(i,j) = (*this)(j,i) ;
            }
        }
        return result ;
    }

    template <class FT, int N> inline void
    Matrix<FT,N>::mult(const FT* x, FT* y) const {
        for(int i=0; i<N; i++) {
            y[i] = 0 ;
            for(int j=0; j<N; j++) {
                y[i] += (*this)(i,j)*x[j] ;
            }
        }
    }

	template <class FT, int DIM> Matrix<FT, DIM> operator*(
		FT op1, const Matrix<FT, DIM>& op2
		) 
	{
		Matrix<FT, DIM> result(op2);
		for (int i=0; i<DIM; i++)
			for (int j=0; j<DIM; j++)
				result(i,j)*=op1;
		return result;
	}

//_______________________________________________________________________

}

#endif
