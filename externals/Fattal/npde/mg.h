#ifndef MG_HH
#define MG_HH

#include "tensor2d.h"
#include "exact.h"
#include <math.h>
#include <stdio.h>

typedef void(*Smoother)(Tensor2d& X, Tensor2d& B, double hx, double hy) ;
typedef void(*Error)(Tensor2d& X, Tensor2d& B, double hx, double hy) ;
typedef void(*Residual)(Tensor2d& X, Tensor2d& B, Tensor2d& R, double hx, double hy) ;

void poisson_residual(Tensor2d& X,  Tensor2d& B, Tensor2d& R, double hx, double hy) {
  double nei ;
  double sum_nei ;
  for(int y = 0 ; y < X.len_y ; y++) {
    for(int x = 0 ; x < X.len_x ; x++) {
      nei = 0 ;
      sum_nei = 0 ;

      if(x > 0) {
	sum_nei += X(x-1, y) ;
	nei ++ ;
      }
      
      if(x < X.len_x - 1) {
	sum_nei += X(x+1, y) ;
	nei ++ ;
      }

      if(y > 0) {
	sum_nei += X(x, y-1) ;
	nei ++ ;
      }
      
      if(y < X.len_y - 1) {
	sum_nei += X(x, y+1) ;
	nei ++ ;
      }

      R(x,y) = B(x,y) - (sum_nei - nei * X(x,y)) / (hx*hx) ;
      
      // R(x,y) = B(x,y) - (sum_nei - nei * X(x,y)) / (hx*hx) ;
/*

      if(x == 0 && y == 0)
	R(x,y) = 0 ;
*/
      
    }
  }
}

void poisson_smoother(Tensor2d& X, Tensor2d& B, double hx, double hy) {
  double nei ;
  double sum_nei ;

  for(int y = 0 ; y < X.len_y ; y++) {
    for(int x = 0 ; x < X.len_x ; x++) {
            
      nei = 0 ;
      sum_nei = 0 ;
      
      if(x > 0) {
	sum_nei += X(x-1, y) ;
	nei ++ ;
      }
      
      if(x < X.len_x - 1) {
	sum_nei += X(x+1, y) ;
	nei ++ ;
      }

      if(y > 0) {
	sum_nei += X(x, y-1) ;
	nei ++ ;
      }
      
      if(y < X.len_y - 1) {
	sum_nei += X(x, y+1) ;
	nei ++ ;
      }
      
      X(x,y) = (sum_nei - B(x,y) * (hx * hx)) / nei ;  // / nei ;
/*
      if(x == 0 && y == 0) 
      X(x,y) = 0 ;
*/
    }
  }
}

void poisson_error(Tensor2d& X, Tensor2d& B, double hx, double hy) {
  double nei ;
  double sum_nei ;

  double e = 0 ;
  
  for(int y = 0 ; y < X.len_y ; y++) {
    for(int x = 0 ; x < X.len_x ; x++) {
      nei = 0 ;
      sum_nei = 0 ;

      if(x > 0) {
	sum_nei += X(x-1, y) ;
	nei ++ ;
      }
      
      if(x < X.len_x - 1) {
	sum_nei += X(x+1, y) ;
	nei ++ ;
      }

      if(y > 0) {
	sum_nei += X(x, y-1) ;
	nei ++ ;
      }
      
      if(y < X.len_y - 1) {
	sum_nei += X(x, y+1) ;
	nei ++ ;
      }

      if(x == 0 && y == 0) 
	continue ;
      /*
      double te = fabs(X(x,y) - (sum_nei - B(x,y) * (hx * hx)) / nei) ;
	if(e < te)
	e = te ;
      */
       e += fabs(B(x,y) - (sum_nei - nei * X(x,y)) / (hx*hx)) ;
    }
  }

  //  cout.precision(2) ;
  //  cout.setf(ios::scientific) ;
  cout << e / (X.len_x * X.len_y) << " " ;
}

void MG(Smoother smoother, Error error, Residual residual, Tensor2d& X, Tensor2d& B, double hx, double hy, int depth, int iters, int ncycle) {
  cout << "Starting Full V-Cycle MultiGrid..." << endl ;
  int len_x = X.len_x ;
  int len_y = X.len_y ;
  
  Tensor2d E[depth] ;    // Error
  Tensor2d D[depth] ;    // Defect

  E[0] = X ;
  D[0] = B ;

  for(int up_to_level = depth - 1 ; up_to_level > 0 ; up_to_level--) {

    for(int nc = 0 ; nc < ncycle ; nc++) {
    
    /* Going down */

    for(int level = 0 ; level < up_to_level ; level ++) {

      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "\\\t" ;
  
      double grid_mult = pow(2, level) ;

      cout << "[" << (len_x-1) / grid_mult + 1 << "," << (len_y-1) / grid_mult + 1 << "]\t" ;

      for(int i = 0 ; i < iters ; i++) {
	smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
	// error(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
      }
      
      Tensor2d R(D[level].len_x, D[level].len_y) ;

      residual(E[level], D[level], R, hx *  grid_mult, hy * grid_mult) ;
/*
      ZImage image ;
      R.toImage(image, 1) ;
      char filename[256] ;
      sprintf(filename, "deb-%.2d-%.2d.tif", level, nc) ;
      image.writeImage(filename) ;
  */    
      R.restrict_full(D[level+1]) ;

      double next_grid_div = pow(2, level+1) ;
      
      E[level+1].set((int)((len_x-1) / next_grid_div + 1), (int)((len_y-1) / next_grid_div + 1)) ;
      E[level+1].clear(0) ;    // Zeros prediction for error

      cout << endl ;
    }

    /* End of goind down */
    
    for(int i = 0 ; i < up_to_level ; i++)
	  cout << ' ' ;
    cout << ">\t" ;
    
    double grid_mult = pow(2, up_to_level) ;
    
    cout << "[" << (len_x-1) / grid_mult + 1 << "," << (len_y-1) / grid_mult + 1 << "]\t" ;
    
    for(int i = 0 ; i < iters * 2 ; i++) {
	  smoother(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult) ;
	  // error(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult) ;
    }
    
    for(int i = 0 ; i < iters ; i++)
	  smoother(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult) ;
    cout << endl ;
  
    /* Going up*/

    for(int level = up_to_level-1 ; level >= 0 ; level --) {

      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "/\t" ;

      double grid_mult = pow(2, level) ;
    
      cout << "[" << (len_x - 1) / grid_mult + 1 << "," << (len_y - 1) / grid_mult + 1 << "]\t" ;

      Tensor2d tE ;

      E[level+1].prolong_full(tE) ;
      
      // error(E[level],D[level],  hx * grid_mult, hy * grid_mult) ;
      
      E[level] += tE ;
      // error(E[level],D[level],  hx * grid_mult, hy * grid_mult) ;

      for(int i = 0 ; i < iters ; i++) {
	smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
	// error(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
      }

      cout << endl ;
    }

    /* End of going up */
    }
        
  }
  
  X = E[0] ;
}


void FMG(Smoother smoother, Error error, Residual residual, Tensor2d& X, Tensor2d& B, double hx, double hy, int depth, int iters, int ncycle) {
  cout << "Starting Full MultiGrid Alg..." << endl ;
  int len_x = X.len_x ;
  int len_y = X.len_y ;
  
  Tensor2d E[depth] ;    // Error
  Tensor2d D[depth] ;    // Defect

  D[0] = B ;

  for(int l = 0 ; l < depth - 1 ; l++) {
    D[l].restrict_full(D[l+1]) ;
    cout << D[l+1].len_x << endl ;
  }

  E[depth-1].set(D[depth-1].len_x, D[depth-1].len_y) ;
  
  E[depth-1].clear(0) ;    // Initial guess  for(int i = 0 ; i < level ; i++)

  int level = depth - 1 ;

  for(int i = 0 ; i < level ; i++)
    cout << ' ' ;
  cout << ">\t" ;
  
  double grid_mult = pow(2, level) ;
  
  cout << "[" << (len_x-1) / grid_mult + 1 << "," << (len_y-1) / grid_mult + 1 << "]\t" ;
  
  for(int i = 0 ; i < iters ; i++) {
    smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
    // error(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
  }

  cout << endl ;
  
  level = depth - 2 ;

  grid_mult = pow(2, level) ;
      
  for(int i = 0 ; i < level ; i++)
    cout << ' ' ;
  
  cout << "/\t" ;
  
  cout << "[" << (len_x - 1) / grid_mult + 1 << "," << (len_y - 1) / grid_mult + 1 << "]\t" ;
  
  Tensor2d tE ;
  
  E[level+1].prolong_full(E[level]) ;
  
  for(int i = 0 ; i < iters ; i++) {
    smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
    // error(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
  }
  
  cout << endl ;

  for(int from_level = depth - 2 ; from_level >= 0 ; from_level--) {

    for(int nc = 0 ; nc < ncycle ; nc++) {
    
    /* Going down */

    for(int level = from_level ; level < depth - 1 ; level ++) {

      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "\\\t" ;
  
      double grid_mult = pow(2, level) ;

      cout << "[" << (len_x-1) / grid_mult + 1 << "," << (len_y-1) / grid_mult + 1 << "]\t" ;

      for(int i = 0 ; i < iters ; i++) {
	smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
	// error(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
      }

      Tensor2d R(D[level].len_x, D[level].len_y) ;

      residual(E[level], D[level], R, hx *  grid_mult, hy * grid_mult) ;

      //      cout << "\t|B| = " << R.norm_max() ;

      R.restrict_full(D[level+1]) ;

      double next_grid_div = pow(2, level+1) ;
      
      E[level+1].set((int)((len_x-1) / next_grid_div + 1), (int)((len_y-1) / next_grid_div + 1)) ;
      E[level+1].clear(0) ;    // Zeros prediction for error

      cout << endl ;
    }

    /* End of goind down */
    /*    
    cout << " <E> " << flush ;

    int up_to_level  = depth - 1 ;

    double grid_mult = pow(2, up_to_level) ;
    
    int dimx = (int)((double)(len_x-1) / (double)grid_mult + 1) ;
    int dimy = (int)((double)(len_y-1) / (double)grid_mult + 1) ;
    
    int n = dimx * dimy ;

    float **mat = new (float*)[n+1] ;
    float **omat = new (float*)[n+1] ;
    float *vb = new float[n+1] ;
    float *ovb = new float[n+1] ;
    
    for(int i = 0 ; i < n+1 ; i++) {
      mat[i] = new float[n+1] ;
      omat[i] = new float[n+1] ;
    }

    for(int i = 0 ; i < n+1 ; i++)
      for(int j = 0 ; j < n+1 ; j++){
	mat[i][j] = 0 ;
	omat[i][j] = 0 ;
      }
     
    int k = 1 ;

    for(int y = 0 ; y < dimy ; y++) {
      for(int x = 0 ; x < dimx ; x++) {
	double nei = 0 ;
	if(x > 0) {
	  mat[k][k-1] = -1 ;
	  omat[k][k-1] = -1 ;
	  nei ++ ;
	}

	if(x < dimx-1) {
	  mat[k][k+1] = -1 ;
	  omat[k][k+1] = -1 ;
	  nei ++ ;
	}

	if(y > 0) {
	  mat[k][k-dimx] = -1 ;
	  omat[k][k-dimx] = -1 ;
	  nei ++ ;
	}

	if(y < dimy - 1) {
	  mat[k][k+dimx] = -1 ;
	  omat[k][k+dimx] = -1 ;
	  nei ++ ;
	}
	
	mat[k][k] = nei ;
	omat[k][k] = nei ;
	
	vb[k] = -D[up_to_level](x,y) * (hx * grid_mult) * (hx * grid_mult) ;

	ovb[k] = vb[k] ;
	k++ ;
      }
    }

    int *indx = new int[n+1] ;
    float d ;
    
    // ludcmp(mat, n, indx, &d) ; 
    // lubksb(mat, n, indx, vb) ;
    

    for(int h = 0 ; h < n+1 ; h++)
      mat[1][h] = 0 ;

    mat[1][1] = 1 ;
    vb[1] = 0 ;
    
    exact(mat, vb,n) ;
    
    k = 1 ;
    
    for(int y = 0 ; y < dimy ; y++)
      for(int x = 0 ; x < dimx ; x++)
	E[up_to_level](x,y) = vb[k++] ;
    
    error(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult) ;

    cout << endl ;
    */
    
    for(int i = 0 ; i < depth - 1 ; i++)
      cout << ' ' ;
    cout << ">\t" ;
     
    double grid_mult = pow(2, depth - 1) ;
    
    cout << "[" << (len_x-1) / grid_mult + 1 << "," << (len_y-1) / grid_mult + 1 << "]\t" ;
          
    for(int i = 0 ; i < iters * 2; i++) {
      smoother(E[depth - 1], D[depth - 1], hx * grid_mult, hy * grid_mult) ;
    }

    cout << endl ;
    
    /* Going up*/

    for(int level = depth - 2 ; level >= from_level ; level --) {

      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "/\t" ;

      double grid_mult = pow(2, level) ;
    
      cout << "[" << (len_x - 1) / grid_mult + 1 << "," << (len_y - 1) / grid_mult + 1 << "]\t" ;

      Tensor2d tE ;
          
      E[level+1].prolong_full(tE) ;
      
      // error(E[level],D[level],  hx * grid_mult, hy * grid_mult) ;
      
      E[level] += tE ;

      // error(E[level],D[level],  hx * grid_mult, hy * grid_mult) ;

      for(int i = 0 ; i < iters ; i++) {
	smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
	// error(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
      }

      cout << endl ;
    }

    /* End of going up */
    }
    
    if(from_level > 0) {
      int level = from_level - 1 ;

      double grid_mult = pow(2, level) ;
      
      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "/\t" ;

      cout << "[" << (len_x - 1) / grid_mult + 1 << "," << (len_y - 1) / grid_mult + 1 << "]\t" ;

      Tensor2d tE ;
      
      E[level+1].prolong_full(E[level]) ;
      
      for(int i = 0 ; i < iters ; i++) {
	smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
	// error(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
      }
    
    cout << endl ;
    }
    
    

  }

  X = E[0] ;
}



void MGG(Smoother smoother, Error error, Residual residual, Tensor2d& X, Tensor2d& B, 
	 double hx, double hy, int depth, int iters, int ncycle) {
  
  int len_x = X.len_x ;
  int len_y = X.len_y ;
  
  Tensor2d E[depth] ;    // Error
  Tensor2d D[depth] ;    // Defect

  E[0] = X ;
  D[0] = B ;

  for(int up_to_level = depth - 1 ; up_to_level > 0 ; up_to_level--) {

    for(int nc = 0 ; nc < ncycle ; nc++) {
    
    /* Going down */

    for(int level = 0 ; level < up_to_level ; level ++) {

      double grid_mult = pow(2, level) ;

      for(int i = 0 ; i < iters ; i++) {
	smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
      }
      
      Tensor2d R(D[level].len_x, D[level].len_y) ;

      residual(E[level], D[level], R, hx *  grid_mult, hy * grid_mult) ;

       cout << "resid = " << R.norm_max() << endl ;

      R.restrict_any(D[level+1]) ;

      double next_grid_div = pow(2, level+1) ;
      
      E[level+1].set(D[level+1].len_x, D[level+1].len_y) ;
      E[level+1].clear(0) ;    // Zeros prediction for error

    }

    /* End of goind down */
    
    
    double grid_mult = pow(2, up_to_level) ;
        
    for(int i = 0 ; i < iters * 2 ; i++) {
	  smoother(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult) ;
	  // error(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult) ;
    }
    
    for(int i = 0 ; i < iters ; i++)
      smoother(E[up_to_level], D[up_to_level], hx * grid_mult, hy * grid_mult) ;
    
    /* Going up*/

    for(int level = up_to_level-1 ; level >= 0 ; level --) {

      double grid_mult = pow(2, level) ;
    
      Tensor2d tE ;

      E[level+1].prolong_any(tE) ;
      
      // error(E[level],D[level],  hx * grid_mult, hy * grid_mult) ;
      
      E[level] += tE ;
      // error(E[level],D[level],  hx * grid_mult, hy * grid_mult) ;

      for(int i = 0 ; i < iters ; i++) {
	smoother(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
	// error(E[level], D[level], hx * grid_mult, hy * grid_mult) ;
      }
    }

    /* End of going up */
    }
        
  }
  
  X = E[0] ;
}

#endif /* MG_HH */
