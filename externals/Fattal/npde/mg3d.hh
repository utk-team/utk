#ifndef MG3D_HH
#define MG3D_HH

#include "tensor3d.h"
#include <math.h>
#include <stdio.h>

typedef void(*Smoother3d)(Tensor3d& X, Tensor3d& B, double hx, double hy, double hz) ;
typedef void(*Error3d)(Tensor3d& X, Tensor3d& B, double hx, double hy, double hz) ;
typedef void(*Residual3d)(Tensor3d& X, Tensor3d& B, Tensor3d& R, double hx, double hy, double hz) ;

void poisson_residual3d(Tensor3d& X,  Tensor3d& B, Tensor3d& R, double hx, double hy, double hz) {
  double sum_nei ;

  int xdiff = 1 ;
  int ydiff = X.len_x ;
  int zdiff = X.len_x * X.len_y ;

  int nx = X.len_x ;
  int ny = X.len_y ;
  int nz = X.len_z ;

  int nxm = nx - 1 ;
  int nym = ny - 1 ;
  int nzm = nz - 1 ;

  double mdx2 = 1.0 / (hx * hx) ;
  double mdy2 = 1.0 / (hy * hy) ;
  double mdz2 = 1.0 / (hz * hz) ;

  double xind ;
  int ind = 0 ;

  for(int z = 0 ; z < nz ; z++) {
    for(int y = 0 ; y < ny ; y++) {
      for(int x = 0 ; x < nx ; x++) {

	sum_nei = 0 ;
	
	xind = X[ind] ;

	if(x > 0)
	  sum_nei += (X[ind - xdiff] - xind) * mdx2 ;
#ifdef ZBC
	else
	  sum_nei += (0 - xind) * mdx2 ;
#endif

	if(x < nxm)
	  sum_nei += (X[ind + xdiff] - xind) * mdx2 ;
#ifdef ZBC
	else
	  sum_nei += (0 - xind) * mdx2 ;
#endif
	
	if(y > 0) 
	  sum_nei += (X[ind - ydiff] - xind) * mdy2 ;
#ifdef ZBC
	else
	  sum_nei += (0 - xind) * mdy2 ;
#endif
	
	if(y < nym)
	  sum_nei += (X[ind + ydiff] - xind) * mdy2 ;
#ifdef ZBC
	else
	  sum_nei += (0 - xind) * mdy2 ;
#endif
	
	if(z > 0)
	  sum_nei += (X[ind - zdiff] - xind) * mdz2 ;
#ifdef ZBC
	else
	  sum_nei += (0 - xind) * mdz2 ;
#endif

	if(z < nzm) 
	  sum_nei += (X[ind + zdiff] - xind) * mdz2 ;
#ifdef ZBC
	else
	  sum_nei += (0 - xind) * mdz2 ;
#endif

	R[ind] = B[ind] - sum_nei ;
	ind ++ ;
      }
    }
  }
  //  cout << "Residual = " << R.norm_max() << endl ;
}


void poisson_smoother3d(Tensor3d& X, Tensor3d& B, double hx, double hy, double hz) {
  double tw ;
  double sum_nei ;

  int xdiff = 1 ;
  int ydiff = X.len_x ;
  int zdiff = X.len_x * X.len_y ;

  int nx = X.len_x ;
  int ny = X.len_y ;
  int nz = X.len_z ;

  int nxm = nx - 1 ;
  int nym = ny - 1 ;
  int nzm = nz - 1 ;

  double mdx2 = 1.0 / (hx * hx) ;
  double mdy2 = 1.0 / (hy * hy) ;
  double mdz2 = 1.0 / (hz * hz) ;

  double xind ;
  int ind = 0 ;

  for(int z = 0 ; z < nz ; z++) {
    for(int y = 0 ; y < ny ; y++) {
      for(int x = 0 ; x < nx ; x++) {
	
	xind = X[ind] ;
	
	sum_nei = 0 ;
	tw = 0 ;

	if(x > 0) {
	  sum_nei += X[ind - xdiff] * mdx2 ;
	  tw += mdx2 ;
	}
#ifdef ZBC
	else
	  tw += mdx2 ;
#endif

	if(x < nxm) {
	  sum_nei += X[ind + xdiff] * mdx2 ;
	  tw += mdx2 ;
	}
#ifdef ZBC
	else
	  tw += mdx2 ;
#endif

	if(y > 0) {
	  sum_nei += X[ind - ydiff] * mdy2 ;
	  tw += mdy2 ;
	}
#ifdef ZBC
	else
	  tw += mdy2 ;
#endif

	if(y < nym) {
	  sum_nei += X[ind + ydiff] * mdy2 ;
	  tw += mdy2 ;
	}
#ifdef ZBC
	else
	  tw += mdy2 ;
#endif

	if(z > 0) {
	  sum_nei += X[ind - zdiff] * mdz2 ;
	  tw += mdz2 ;
	}
#ifdef ZBC
	else
	  tw += mdz2 ;
#endif

	if(z < nzm) {
	  sum_nei += X[ind + zdiff] * mdz2 ;
	  tw += mdz2 ;
	}
#ifdef ZBC
	else
	  tw += mdz2 ;
#endif

	X[ind] = (sum_nei - B[ind]) / tw ;
	
	ind ++ ;
      }
    }
  }
}

void MG3D(Smoother3d smoother3d, Residual3d residual3d, Tensor3d& X, Tensor3d& B, 
	  double hx, double hy, double hz, int depth, int iters, int ncycle) {

  // cout << "Starting Full V-Cycle MultiGrid..." << endl ;

  int len_x = X.len_x ;
  int len_y = X.len_y ;
  int len_z = X.len_z ;
  
  Tensor3d E[depth] ;    // Error
  Tensor3d D[depth] ;    // Defect

  E[0] = X ;
  D[0] = B ;

  for(int up_to_level = depth - 1 ; up_to_level > 0 ; up_to_level--) {

    for(int nc = 0 ; nc < ncycle ; nc++) {
    
    /* Going down */

    for(int level = 0 ; level < up_to_level ; level ++) {

      //      for(int i = 0 ; i < level ; i++)
	// cout << ' ' ;
      // cout << "\\\t" ;
  
      double grid_mult = pow(2, level) ;

      // cout << "[" << (len_x-1) / grid_mult + 1 << "," << (len_y-1) / grid_mult + 1 << "," << (len_z-1) / grid_mult + 1 << "]\t" ;

      for(int i = 0 ; i < iters ; i++)
	smoother3d(E[level], D[level], hx * grid_mult, hy * grid_mult, hz * grid_mult) ;
      
      Tensor3d R(D[level].len_x, D[level].len_y, D[level].len_z) ;

      residual3d(E[level], D[level], R, hx *  grid_mult, hy * grid_mult, hz * grid_mult) ;

      R.restrictx2(D[level+1]) ;

      double next_grid_div = pow(2, level+1) ;
      
      E[level+1].set((int)(len_x / next_grid_div), (int)(len_y / next_grid_div), (int)(len_z / next_grid_div)) ;
      E[level+1].clear(0) ;    // Zeros prediction for error

      // cout << endl ;
    }

    /* End of goind down */
    
    //    for(int i = 0 ; i < up_to_level ; i++)
	  // cout << ' ' ;
    // cout << ">\t" ;
    
    double grid_mult = pow(2, up_to_level) ;
    
    // cout << "[" << (len_x-1) / grid_mult + 1 << "," << (len_y-1) / grid_mult + 1 <<  "," << (len_z-1) / grid_mult + 1 << "]\t" ;
    
    for(int i = 0 ; i < iters * 2 ; i++)
	  smoother3d(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult, hz * grid_mult) ;
    
    for(int i = 0 ; i < iters ; i++)
	  smoother3d(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult, hz * grid_mult) ;

    // cout << endl ;
  
    /* Going up*/

    for(int level = up_to_level-1 ; level >= 0 ; level --) {

      //      for(int i = 0 ; i < level ; i++)
	// cout << ' ' ;
      // cout << "/\t" ;

      double grid_mult = pow(2, level) ;
    
      // cout << "[" << (len_x - 1) / grid_mult + 1 << "," << (len_y - 1) / grid_mult + 1 << "," << (len_z - 1) / grid_mult + 1 << "]\t" ;

      Tensor3d tE ;

#ifdef ZBC
      E[level+1].prolongx2z(tE) ;
#else
      E[level+1].prolongx2(tE) ;
#endif

      // error(E[level],D[level],  hx * grid_mult, hy * grid_mult) ;
      
      E[level] += tE ;
      // error(E[level],D[level],  hx * grid_mult, hy * grid_mult) ;

      for(int i = 0 ; i < iters ; i++)
	smoother3d(E[level], D[level], hx * grid_mult, hy * grid_mult, hz * grid_mult) ;

      // cout << endl ;
    }

    /* End of going up */
    }
        
  }
  
  X = E[0] ;
}

#endif /* MG3D_HH */
