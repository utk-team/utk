/*
 *  GXML/Graphite: Geometry and Graphics Programming Library + Utilities
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


#include <Geex/mathematics/matrix_util.h>
#include <Geex/basics/types.h>

#define __log2 __xxlog2 
extern "C" {
#ifdef gx_OS_LINUX
typedef int idxtype;
#define __parmetis_h__
# include <metis/metis.h>
#else   
//# include <numeric_stuff/METIS/metis.h>
# include <Geex/third_party/METIS/metis.h>
#endif   
}

namespace Geex {
    
    namespace MatrixUtil {

		//determinant of a 3x3 Matrix
		double det(const Matrix<double, 3> &m) 
		{
			double c0 = m(1,1)*m(2,2)-m(1,2)*m(2,1);
			double c1 = m(1,0)*m(2,2)-m(2,0)*m(1,2);
			double c2 = m(1,0)*m(2,1)-m(2,0)*m(1,1);
			return m(0,0)*c0 - m(0,1)*c1 + m(0,2)*c2;
		}

		// not for singular matrices - http://en.wikipedia.org/wiki/Eigenvalue_algorithm
		// Given symmetric 3x3 matrix M, compute the eigenvalues
		void sym_eig_values(const Matrix<double, 3> &mat, double eig_values[3]) 
		{
			double m = mat.trace()/3.;
			Matrix<double, 3> Id; Id.load_identity();
			Matrix<double, 3> K = mat-m*Id;
			double q = det(K)/2.;

			double p = 0;
			for(int i=0; i<3; i++)
				for(int j=0; j<3; j++)
					p += K(i,j)*K(i,j);
			p /= 6.;
			double phi;
			if (p!=0)
				phi = acos(gx_min(1-1E-12, gx_max(-1+1E-12, q/(p*sqrt(p)))))/3.;
			else
				phi = 0;
			if(phi<0)
				phi+=M_PI/3.;

			double a = m + 2.*sqrt(p)*cos(phi);
			double b = m - sqrt(p)*(cos(phi) + sqrt(3.)*sin(phi));
			double c = m - sqrt(p)*(cos(phi) - sqrt(3.)*sin(phi));		
			eig_values[0] = gx_max(gx_max(a,b),c);
			eig_values[1] = gx_max(gx_min(a,b),c);
			eig_values[2] = gx_min(gx_min(a,b),c);
		}

		void sym_eig_vectors(const Matrix<double, 3> &m, /*const*/ double eigval[3], Matrix<double, 3> &eigvec )
		{
			double vals[9];
			for (int i=0; i<3; i++)
				for (int j=0; j<3; j++)
					vals[i*3+j] = m(i,j);
			double eigen_vec[9];
			semi_definite_symmetric_eigen(vals, 3, eigen_vec, eigval);
		
			for (int i=0; i<3; i++)
				for (int j=0; j<3; j++)
					eigvec(i,j)=eigen_vec[i*3+j];

			return;

			Matrix<double, 3> id; id.load_identity();
			if (abs(eigval[0]-eigval[1])<1E-7*eigval[0] && abs(eigval[0]-eigval[2])<1E-7*eigval[0])
			{
				eigvec.load_identity();
				return;
			}

			Matrix<double, 3> m1;			
			Matrix<double, 3> m2;
			bool m2m3 = false;
			if (abs(eigval[0]-eigval[1])>1E-12)
			{
				m1 = m-eigval[0]*id;
				m2 = m-eigval[1]*id;
			}
			else
			{
				m1 = m-eigval[1]*id;
				m2 = m-eigval[2]*id;
				m2m3 = true;
			}

			m1.gaussian_elimination();
			m2.gaussian_elimination();


			double x1, y1, z1;
			if (m1(0,0)==0) {
				x1 = 1; y1 = 0; z1 = 0;
			} else {
				if (m1(1,1)!=0)
				{
					z1 = 1;				
					y1 = -m1(1,2)/m1(1,1)*z1;
					x1 = -(m1(0,1)*y1+m1(0,2)*z1)/m1(0,0);
				}
				else
				{
					z1 = 0;
					y1 = 1;
					x1 = -m1(0,1)/m1(0,0)*z1;
				}
				double n1 = sqrt(x1*x1+y1*y1+z1*z1);
				x1/=n1; y1/=n1; z1/=n1;
			}
			double x2, y2, z2;
			if (m2(0,0)==0) {
				x2 = 1; y2 = 0; z2 = 0;
			} else {
				if (m2(1,1)!=0)
				{
					z2 = 1;				
					y2 = -m2(1,2)/m2(1,1)*z2;
					x2 = -(m2(0,1)*y2+m2(0,2)*z2)/m2(0,0);
				}
				else
				{
					z2 = 0;
					y2 = 1;
					x2 = -m2(0,1)/m2(0,0)*z1;
				}
				double n2 = sqrt(x2*x2+y2*y2+z2*z2);
				x2/=n2; y2/=n2; z2/=n2;
			}
			
			// a cross product for the third eigen vector
			double x3 = y1*z2 - z1*y2;
			double y3 = z1*x2 - x1*z2;
			double z3 = x1*y2 - y1*x2;

			if (m2m3)
			{
				std::swap(x1,x3); std::swap(y1,y3); std::swap(y1,y3); //roll the matrix
				std::swap(x2,x3); std::swap(y2,y3); std::swap(y2,y3);
			}

			eigvec(0,0) = x1; eigvec(0,1) = x2; eigvec(0,2) = x3;
			eigvec(1,0) = y1; eigvec(1,1) = y2; eigvec(1,2) = y3;
			eigvec(2,0) = z1; eigvec(2,1) = z2; eigvec(2,2) = z3;
		}


		Matrix<double, 3> spd_pow(const Matrix<double, 3> &m, double power) // requires an SPD matrix so that V*d*inv(v) == V*d*transpose(V);
		{

			double eig_values[3];
			Matrix<double, 3> eig_vectors;			

			sym_eig_values(m, eig_values);
			sym_eig_vectors(m, eig_values, eig_vectors);

			Matrix<double, 3> result;

			for (int i=0; i<3; i++) {
				for (int j=0; j<3; j++) {
					result(i, j) = eig_vectors(j,i)*pow(abs(eig_values[i]), power); //there should be no abs due to SPD matrix, but with the loss of precision in the eig_values, this can happen
				}
			}			
			return eig_vectors*result;
		}


        static const double EPS = 0.00001 ;
        static int MAX_ITER = 100 ;
        
        void semi_definite_symmetric_eigen(
            const double *mat, int n, double *eigen_vec, double *eigen_val
        ) {
            double *a,*v;
            double a_norm,a_normEPS,thr,thr_nn;
            int nb_iter = 0;
            int jj;
            int i,j,k,ij,ik,l,m,lm,mq,lq,ll,mm,imv,im,iq,ilv,il,nn;
            int *index;
            double a_ij,a_lm,a_ll,a_mm,a_im,a_il;
            double a_lm_2;
            double v_ilv,v_imv;
            double x;
            double sinx,sinx_2,cosx,cosx_2,sincos;
            double delta;
        
            // Number of entries in mat
        
            nn = (n*(n+1))/2;
        
            // Step 1: Copy mat to a
        
            a = new double[nn];
        
            for( ij=0; ij<nn; ij++ ) {
                a[ij] = mat[ij];
            }
        
            // Ugly Fortran-porting trick: indices for a are between 1 and n
            a--;
        
            // Step 2 : Init diagonalization matrix as the unit matrix
            v = new double[n*n];
        
            ij = 0;
            for( i=0; i<n; i++ ) {
                for( j=0; j<n; j++ ) {
                    if( i==j ) {
                        v[ij++] = 1.0;
                    } else {
                        v[ij++] = 0.0;
                    }
                }
            }
        
            // Ugly Fortran-porting trick: indices for v are between 1 and n
            v--;
        
            // Step 3 : compute the weight of the non diagonal terms 
            ij     = 1  ;
            a_norm = 0.0;
            for( i=1; i<=n; i++ ) {
                for( j=1; j<=i; j++ ) {
                    if( i!=j ) {
                        a_ij    = a[ij];
                        a_norm += a_ij*a_ij;
                    }
                    ij++;
                }
            }
        
            if( a_norm != 0.0 ) {
        
                a_normEPS = a_norm*EPS;
                thr       = a_norm    ;
    
                // Step 4 : rotations
                while( thr > a_normEPS   &&   nb_iter < MAX_ITER ) {
        
                    nb_iter++;
                    thr_nn = thr / nn;
            
                    for( l=1  ; l< n; l++ ) {
                        for( m=l+1; m<=n; m++ ) {
                    
                            // compute sinx and cosx 
                    
                            lq   = (l*l-l)/2;
                            mq   = (m*m-m)/2;
                    
                            lm     = l+mq;
                            a_lm   = a[lm];
                            a_lm_2 = a_lm*a_lm;
                    
                            if( a_lm_2 < thr_nn ) {
                                continue ;
                            }
                    
                            ll   = l+lq;
                            mm   = m+mq;
                            a_ll = a[ll];
                            a_mm = a[mm];
                    
                            delta = a_ll - a_mm;
                    
                            if( delta == 0.0 ) {
                                x = - M_PI/4 ; 
                            } else {
                                x = - atan( (a_lm+a_lm) / delta ) / 2.0 ;
                            }

                            sinx    = sin(x)   ;
                            cosx    = cos(x)   ;
                            sinx_2  = sinx*sinx;
                            cosx_2  = cosx*cosx;
                            sincos  = sinx*cosx;
                    
                            // rotate L and M columns 
       
                            ilv = n*(l-1);
                            imv = n*(m-1);
                    
                            for( i=1; i<=n;i++ ) {
                                if( (i!=l) && (i!=m) ) {
                                    iq = (i*i-i)/2;
                            
                                    if( i<m )  { 
                                        im = i + mq; 
                                    } else {
                                        im = m + iq;
                                    }
                                    a_im = a[im];
                            
                                    if( i<l ) {
                                        il = i + lq; 
                                    } else {
                                        il = l + iq;
                                    }
                                    a_il = a[il];
                            
                                    a[il] =  a_il*cosx - a_im*sinx;
                                    a[im] =  a_il*sinx + a_im*cosx;
                                }
                        
                                ilv++;
                                imv++;
                        
                                v_ilv = v[ilv];
                                v_imv = v[imv];
                        
                                v[ilv] = cosx*v_ilv - sinx*v_imv;
                                v[imv] = sinx*v_ilv + cosx*v_imv;
                            } 
                    
                            x = a_lm*sincos; x+=x;
                    
                            a[ll] =  a_ll*cosx_2 + a_mm*sinx_2 - x;
                            a[mm] =  a_ll*sinx_2 + a_mm*cosx_2 + x;
                            a[lm] =  0.0;
                    
                            thr = fabs( thr - a_lm_2 );
                        }
                    }
                }         
            }
        
            // Step 5: index conversion and copy eigen values 
        
            // back from Fortran to C++
            a++;
        
            for( i=0; i<n; i++ ) {
                k = i + (i*(i+1))/2;
                eigen_val[i] = a[k];
            }
     
            delete[] a;
        
            // Step 6: sort the eigen values and eigen vectors 
        
            index = new int[n];
            for( i=0; i<n; i++ ) {
                index[i] = i;
            }
        
            for( i=0; i<(n-1); i++ ) {
                x = eigen_val[i];
                k = i;
            
                for( j=i+1; j<n; j++ ) {
                    if( x < eigen_val[j] ) {
                        k = j;
                        x = eigen_val[j];
                    }
                }
            
                eigen_val[k] = eigen_val[i];
                eigen_val[i] = x;
          
                jj       = index[k];
                index[k] = index[i];
                index[i] = jj;
            }


            // Step 7: save the eigen vectors 
    
            v++; // back from Fortran to to C++
        
            ij = 0;
            for( k=0; k<n; k++ ) {
                ik = index[k]*n;
                for( i=0; i<n; i++ ) {
                    eigen_vec[ij++] = v[ik++];
                }
            }
    
            delete[] v    ;
            delete[] index;
            return;
        }
    
//_________________________________________________________

	static void check_matrix(int n, int* rowptr, int* colind) {
	    std::cerr << "Check matrix, n = " << n << std::endl ;
	    for(int i=0; i<n; i++) {
		gx_assert(rowptr[i] >= 0) ;
		gx_assert(rowptr[i+1] >= 0) ;
		gx_assert(rowptr[i+1] >= rowptr[i]) ;
		for(int jj=rowptr[i]; jj<rowptr[i+1]; jj++) {
		    int j = colind[jj] ;
		    gx_assert(j < n) ;
		}
	    }
	}

	void partition(
	    int n, int* rowptr, int* colind, int nb_parts, int* part
        ) {

#ifdef gx_DEBUG
	    check_matrix(n, rowptr, colind) ;
#endif
	    int options[1000] ; // Not sure whether this is sufficient
	                        // (but METIS_NOPTIONS is not defined ...)
	    options[0] = 0 ;    // use default values
	    int zero = 0;
	    int edgecut = 0;
	    METIS_PartGraphRecursive(
		&n, rowptr, colind,  // The matrix
		nil, nil,            // No vertex weight, no edge weight
		&zero,               // No weights
		&zero,               // C-style indexing
		&nb_parts,
		options,
		&edgecut,
		part
  	    ) ;
	}
	

    }
}
