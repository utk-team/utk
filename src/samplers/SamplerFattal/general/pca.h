#ifndef _PCA
#define _PCA

#include <math.h>
#include <iostream.h>

void rotate(double* D, double* R, int i, int j, int n) {
  double *tmp = new double[n] ;

  double theta = 0.5 * (D[i+n*i] - D[j+n*j]) / D[j+i*n] ;
  double sgn_theta = 1 ;

  if(theta < 0)
    sgn_theta = -1 ;
  
  double t = sgn_theta / (theta * sgn_theta + sqrt(theta * theta + 1)) ;

  double c = 1.0 / sqrt(t*t+1) ;
  double s = t * c ;
  
  for(int k = 0 ; k < n ; k++) {
    tmp[k] = D[k+i*n] ;
    
    D[k+i*n] = D[k+i*n] * c + D[k+j*n] * s ;
    D[k+j*n] = D[k+j*n] * c - tmp[k] * s ; 

    tmp[k] = R[k+i*n] ;
    
    R[k+i*n] = R[k+i*n] * c + R[k+j*n] * s ;
    R[k+j*n] = R[k+j*n] * c - tmp[k] * s ; 
  }
  
  for(int k = 0 ; k < n ; k++) {
    tmp[k] = D[i+k*n] ;
    
    D[i+k*n] = D[i+k*n] * c + D[j+k*n] * s ;
    D[j+k*n] = D[j+k*n] * c - tmp[k] * s ; 
  }

  delete[] tmp ;
}

void sort(double* D, double* R, int n) {
  int *ind = new int[n] ;

  for(int i = 0 ; i < n ; i++)
    ind[i] = i ;
  
  for(int i = 0 ; i < n ; i++)
    for(int j = 0 ; j < n ; j++)
      if(D[ind[j]+ind[j]*n] > D[ind[i] + ind[i]*n]) {

	int ti = ind[i] ;
	ind[i] = ind[j] ;
	ind[j] = ti ;
      }

  double *tD = new double[n*n] ;
  double *tR = new double[n*n] ;

  for(int i = 0 ; i < n ; i++) {
    for(int j = 0 ; j < n ; j++) {
      tD[j+i*n] = D[ind[j]+ind[i]*n] ;
      tR[j+i*n] = R[j+ind[i]*n] ;
    }
  }

  for(int i = 0 ; i < n*n ; i++) {
    D[i] = tD[i] ;
    R[i] = tR[i] ;
  }

  delete[] tD ;
  delete[] tR ;
  delete[] ind ;
}

/* Given a symmetric matrix A of n x n dimensions,
   this functions returns an orthonormal matrix R and a diagonal
   matrix D such that: A = Rt D R. The eigen values/vectors
   are sorted from lowest to highest
*/

void jacobi(double* A, double* D, double* R, int n) {

  for(int i = 0 ; i < n*n ; i++) {
    D[i] = A[i] ;
    R[i] = 0 ;
  }

  for(int i = 0 ; i < n ; i++)
    R[i+i*n] = 1 ;

  int iter = 0 ;
  int updated = 0 ;
  
  do {
    updated = 0 ;
    for(int i = 0 ; i < n ; i++)
      for(int j = 0 ; j < n ; j++) {
	if(i == j)
	  continue ;
	
	if(fabs(D[j+i*n]) > 1e-14) {
	  rotate(D, R, i, j, n) ;
	  updated = 1 ;
	}
	else {
	  D[j+i*n] = D[i+j*n] = 0 ;
	}
      }

    iter++ ;
    
  } while(updated && iter < 50) ;

  sort(D, R, n) ;
}

/* Computes the covariance matrix and the mean vector. n - dim, m - number
   C is a n by n matrix, EX is a vector of size n and the
   data is of size n*m.
*/

void cov(double* C, double* EX, double* X, int n, int m) {

  for(int j = 0 ; j < n ; j++)
    EX[j] = 0 ;
  
  for(int i = 0 ; i < m ; i++)
    for(int j = 0 ; j < n ; j++)
      EX[j] += X[j+i*n] ;
    
  for(int j = 0 ; j < n ; j++) 
    EX[j] /= m ;

  for(int i = 0 ; i < n ; i++)
    for(int j = i ; j < n ; j++) {

      double c = 0 ;
      
      for(int k = 0 ; k < m ; k++)
	c += (X[i + k * n] - EX[i]) * (X[j + k * n] - EX[j]) ;
      
      C[i+j*n] = C[j+i*n] = c / m ; 
    }
}


/* Computes a weighted covariance matrix and the mean vector. n - dim, m - number
   C is a n by n matrix, EX is a vector of size n and the
   data is of size n*m.
*/

void wcov(double* C, double* EX, double* X, double* W, int n, int m) {

  double S = 0 ;
  
  for(int j = 0 ; j < n ; j++)
    EX[j] = 0 ;
  
  for(int i = 0 ; i < m ; i++) {
    for(int j = 0 ; j < n ; j++)
      EX[j] += X[j+i*n] * W[i] ;
  
    S += W[i] ;
  }
  
  for(int j = 0 ; j < n ; j++) 
    EX[j] /= S ;

  for(int i = 0 ; i < n ; i++)
    for(int j = i ; j < n ; j++) {

      double c = 0 ;
      double S = 0 ;
      
      for(int k = 0 ; k < m ; k++) {
	c += (X[i + k * n] - EX[i]) * (X[j + k * n] - EX[j]) * W[k] ;
	S += W[k] ;
      }
      
      C[i+j*n] = C[j+i*n] = c / S ; 
    }
}


#endif /* _PCA */
