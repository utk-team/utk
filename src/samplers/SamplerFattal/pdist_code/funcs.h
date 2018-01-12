#include "../general/fft.h"

namespace fattal
{

#define ABS(a) (((a) < 0) ? -(a) : (a))

void kernel(double osigma, unsigned long density_sqrt, double cx, double cy,
	    int& lx, int& ly, int& wx, int& wy, double* K, int nx, int ny) {

  osigma /= density_sqrt ;

  int klen = (int)(KER_SIZE_FAC / osigma+1) ;

  if(klen > MAX_KER_DIM)
    klen = MAX_KER_DIM ;

  int ix = (int)cx ;
  int iy = (int)cy ;

  lx = ix - klen ;
  ly = iy - klen ;
  wx = ix + klen ;
  wy = iy + klen ;

#ifndef PERIODIC

  if(lx < 0)
    lx = 0 ;
  if(ly < 0)
    ly = 0 ;
  if(wx >= nx)
    wx = nx-1 ;
  if(wy >= ny)
    wy = ny-1 ;

#endif

  double tx = cx - lx ;
  double ty = cy - ly ;
  
  wx = wx - lx + 1 ;
  wy = wy - ly + 1 ;

#ifdef PERIODIC

  if(lx < 0)
    lx += nx ;  

  if(ly < 0)
    ly += ny ;  

#endif

  double dx, dy ;

  int tv ;

  double* pK = K ;

  double osigma2 = osigma * osigma ;
  double osig2_den2 = density_sqrt * density_sqrt * osigma2 ;
  double fac = exp_fac * osigma2 ;
  double v ;

  for(int y = 0 ; y < wy ; y++)
    for(int x = 0 ; x < wx ; x++) {

      dx = x - tx ;
      dy = y - ty ;

      tv = (int)(fac*(dx*dx+dy*dy)) ;

      if(tv < EXP_TAB_LEN) { // remove this if
	v = osig2_den2 * Exp_tab[tv] ;

	*pK = v ; 
      }
      else
	*pK = 0 ;
      
      pK++ ;
    }
}


/*****************************/
/*****************************/


void add_kernel(double osigma, unsigned long density_sqrt, double cx, double cy,
		double* A, double* D, int nx, int ny) {

  osigma /= density_sqrt ;

  int klen = (int)(KER_SIZE_FAC / osigma+1) ;

  if(klen > MAX_KER_DIM)
    klen = MAX_KER_DIM ;

  int ix = (int)cx ;
  int iy = (int)cy ;

  int lx = ix - klen ;
  int ly = iy - klen ;
  int wx = ix + klen ;
  int wy = iy + klen ;

#ifndef PERIODIC

  if(lx < 0)
    lx = 0 ;
  if(ly < 0)
    ly = 0 ;
  if(wx >= nx)
    wx = nx-1 ;
  if(wy >= ny)
    wy = ny-1 ;

#endif

  double tx = cx - lx ;
  double ty = cy - ly ;
  
  wx = wx - lx + 1 ;
  wy = wy - ly + 1 ;

#ifdef PERIODIC

  if(lx < 0)
    lx += nx ;  

  if(ly < 0)
    ly += ny ;  

#endif

  double dx, dy;// dy2 ;

  int tv ;

  double osigma2 = osigma * osigma ;
  double osig2_den2 = density_sqrt * density_sqrt * osigma2 ;
  double fac = exp_fac * osigma2 ;
  double v ;

#ifndef PERIODIC
      
      for(int y = 0 ; y < wy ; y++) {
	int j = lx + (y + ly) * nx ;
	double* pD = &D[j] ;
	double* pA = &A[j] ;

	dy = y - ty ;
	dy2 = dy * dy ;
	
	for(int x = 0 ; x < wx ; x++) {

	  dx = x - tx ;

	  tv = (int)(fac*(dx*dx+dy2)) ;
	  
	  if(tv < EXP_TAB_LEN) {
	    v = osig2_den2 * Exp_tab[tv] ;
	    *pD -= v ;
	    *pA += v ;
	  }
	  
	  pD++ ; pA++ ;
	}
      }
#else

      for(int y = 0 ; y < wy ; y++) {
	int ofy = ((y+ly)%ny)*nx ;
	for(int x = 0 ; x < wx ; x++) {
	  int j = (x+lx) % nx + ofy ; 
	  dx = x - tx ;
	  dy = y - ty ;
	  
	  tv = (int)(fac*(dx*dx+dy*dy)) ;
	  
	  if(tv < EXP_TAB_LEN) {
	    v = osig2_den2 * Exp_tab[tv] ;
	    D[j] -= v ;
	    A[j] += v ;
	  }
	}
      }

#endif
      
}



/*****************************/
/*****************************/

void kernel_remove_dif(double* D, double* A, double osigma, 
		       unsigned long density_sqrt,
		       double sigma_x, double sigma_y, double cx, double cy,  
		       double& diff_x, double& diff_y, int nx, int ny) {

  osigma /= density_sqrt ;

  int klen = (int)(KER_SIZE_FAC / osigma+1) ;
  
  if(klen > MAX_KER_DIM)
    klen = MAX_KER_DIM ;
  
  int ix = (int)cx ;
  int iy = (int)cy ;

  int lx = ix - klen ;
  int ly = iy - klen ;
  int wx = ix + klen ;
  int wy = iy + klen ;

#ifndef PERIODIC

  if(lx < 0)
    lx = 0 ;
  if(ly < 0)
    ly = 0 ;
  if(wx >= nx)
    wx = nx-1 ;
  if(wy >= ny)
    wy = ny-1 ;
#endif


  double tx = cx - lx ;
  double ty = cy - ly ;
  
  wx = wx - lx + 1 ;
  wy = wy - ly + 1 ;

#ifdef PERIODIC
  if(lx < 0)
    lx += nx ;  

  if(ly < 0)
    ly += ny ;  
#endif
  

  double dx, dy, dy2 ;

  int tv ;

  double v ;

  double osigma2 = osigma * osigma ;
  double osig2_den2 = density_sqrt * density_sqrt * osigma2 ;
  double fac = exp_fac * osigma2 ;

  diff_x = 0 ;
  diff_y = 0 ;

  double nr;// tmp ;

  double *pD, *pA ; 

  double sgnD = 0 ;

  for(int y = 0 ; y < wy ; y++) {

#ifndef PERIODIC
      int j = lx + (y+ly) * nx ;
      pD = &D[j] ;
      pA = &A[j] ;
#else
      int ofy = ((y+ly)%ny) * nx ;
#endif
      
      dy = y - ty ;
      dy2 = dy * dy ;

      for(int x = 0 ; x < wx ; x++) {
	
#ifdef PERIODIC
	
	int j = (x+lx) % nx + ofy ; 
	pD = &D[j] ;
	pA = &A[j] ;
#endif
      
	dx = x - tx ;
		
	nr = dx * dx + dy2 ;
	
	tv = (int)(fac * nr) ;
	
      if(tv < EXP_TAB_LEN) {

	v = osig2_den2 * Exp_tab[tv] ;

	if(*pD <=0)
	  sgnD = 2 * v * osigma  ;
	else
	  sgnD = -2 * v * osigma ;

	//	tmp = (1 - nr * osigma2) * 0 ;
	//	diff_x += sgnD * (sigma_x * tmp + osigma * dx) ;
	//	diff_y += sgnD * (sigma_y * tmp + osigma * dy) ;
	diff_x += sgnD * (osigma * dx) * 2 ;
	diff_y += sgnD * (osigma * dy) * 2 ;

	*pD += v ;
	*pA -= v ;
      }

#ifndef PERIODIC
	pD ++ ; pA ++ ;
#endif
    }
  }
}




void get_perm(unsigned* P, int len) {
  
  for(int i = 0 ; i < len ; i++)
    P[i] = i ;
  
  int j ;
  unsigned t ;

  for(int i = 0 ; i < len ; i++) {
    j = (unsigned)((double)len * (double)rand() / RAND_MAX) ;
    
    if(j >= len)
      j = len-1 ;

    t = P[i] ;
    P[i] = P[j] ;
    P[j] = t ;
  }
}

int prgs = 0 ;

void disp(int t, int tot) {

  if(t >= tot-1) {  
    cout << "\b\b\b\b\bdone!" << endl ;
    prgs = 0 ;
    return ;
  }

  double y = (double)t / tot ;
  if(prgs < (int)(y * 20)) {
    prgs = (int)(y * 20) ;
    for(int i = 0 ; i < 50 ; i++)
      cout << "\b" ;
    
    cout << "Progress:    " ;
    
    cout << "[" ;
    for(int i = 0 ; i < prgs ; i++)
      cout << ":" ;
    
    for(int i = prgs ; i < 19 ; i++)
      cout << " " ;
    
    cout << "]   " << prgs * 5 << "%  " << flush ;
  }
}

void show(double* I,  BImage& img, int lx, int ly) {

  Tensor2d T(lx, ly) ;
  
  int i = 0 ;
  for(int y = 0 ; y < ly ; y++)
    for(int x = 0 ; x < lx ; x++)
      T(x,y) = -I[i++] ;

  T.toImageBW(img);
}

double interp(double* I, double x, double y, int nx, int ny, int nxm, int nym) {

  x -= 0.4999 ;
  y -= 0.4999 ;

  if(x < 0)
    x = 0 ;
  if(y < 0)
    y = 0 ;

  if(x > nxm)
    x = nxm ;
  if(y > nym)
    y = nym ;

  int ix = (int)x ;
  int iy = (int)y ;

  int ixp = ix + 1 ;
  int iyp = iy + 1 ;

  if(ixp >= nx)
    ix = ixp = nxm ;

  if(iyp >= ny)
    iy = iyp = nym ;

  double dx = x - ix ;
  double dy = y - iy ;
  double odx = 1.0 - dx ;
  double ody = 1.0 - dy ;

  return (I[ix + iy * nx]   * odx * ody + 
	  I[ixp + iy * nx]  * dx  * ody + 
	  I[ix + iyp * nx]  * odx * dy  + 
	  I[ixp + iyp * nx] * dx  * dy) ;
}

void upsample(double*& CA, int nx, int ny, int& mx, int& my) {

  cout << "Upsampling approx..." << endl ;

  mx = 2 * nx ;
  my = 2 * ny ;

  int nxm = nx - 1 ;
  int nym = ny - 1 ;

  double* tCA = new double[mx * my] ;

  double dx = (double)nx / (double)mx ;
  double dy = (double)ny / (double)my ;

  int i = 0 ;

  double yy = 0.5 * dy ;
  for(int y = 0 ; y < my ; y++) {
    double xx = 0.5 * dx ;

    for(int x = 0 ; x < mx ; x++) {
      tCA[i++] = 0.25 * interp(CA, xx, yy, nx, ny, nxm, nym) ;
      xx += dx ;
    }
    yy += dy ;
  }

  delete[] CA ;
  
  CA = tCA ;
}



void downsample(double* I, int nx, int ny, double*& J, int& mx, int& my) {

  mx = (int)(nx / 2) ;
  my = (int)(ny / 2) ;

  J = new double[mx * my] ;

  int i = 0 ;

  for(int y = 0 ; y < my ; y++)
    for(int x = 0 ; x < mx ; x++) {
      int k = 2 * (x + y * nx) ;
      J[i++] = I[k] + I[k+1] + I[k+nx] + I[k+nx+1] ;
    }
}


/********************************/
/********************************/


void snap_shot(Dot* dots, int ndots, int nx, int ny) {
  
  BImage img ;
  cmplx** cpx = new cmplx*[out_res] ;
  
  for(int i = 0 ; i < out_res ; i++)
    cpx[i] = new cmplx[out_res] ;
      
      Tensor2d U(2*out_res,out_res) ;
      U.clear(1) ;
      
      for(int y = 0 ; y < out_res ; y++)
	for(int x = 0 ; x < out_res ; x++) {
	  cpx[x][y].a = 0 ;
	  cpx[x][y].b = 0 ;
	}
      
      for(int i = 0 ; i < ndots ; i++) {
	int ix = (int)(dots[i].x / nx * out_res + 0.49999 + out_res) % out_res ; 
	int iy = (int)(dots[i].y / ny * out_res + 0.49999 + out_res) % out_res ; 

	int ipx = (ix + 1) % out_res ;
	int ipy = (iy + 1) % out_res ;


	cpx[ix][iy].a = 1 ;


	int imx = (ix - 1 + out_res) % out_res ;
	int imy = (iy - 1 + out_res) % out_res ;

	U(ix,iy) = 0 ;
	U(ipx,iy) = 0.5 ;
	U(imx,iy) = 0.5 ;
	U(ix,ipy) = 0.5 ;
	U(ix,imy) = 0.5 ;

	U(ipx,ipy) = 0.8 ;
	U(imx,ipy) = 0.8 ;
	U(ipx,imy) = 0.8 ;
	U(imx,imy) = 0.8 ;
      }
      
      FFT2D(cpx, out_res, out_res, 1) ;

      double spm = 0 ;
      double spn = 0 ;

      SP *= 0.95 ;
      
      for(int y = 0 ; y < out_res ; y++)
	for(int x = 0 ; x < out_res ; x++) {
	  if(x == 0 && y == 0)
	    SP(x,y) = 0 ;
	  else
	    SP(x,y) += 0.05 * sqrt(cpx[x][y].a*cpx[x][y].a + cpx[x][y].b * cpx[x][y].b);

	  if(x < out_res / 8 && y < out_res / 8) {
	    spm += SP(x,y) ;
	    spn ++ ;
	  }
	}
      
      double nf = 0.75 / SP.max() ;
      
      for(int y = 0 ; y < out_res/2 ; y++)
	for(int x = 0 ; x < out_res/2 ; x++)	  
	  U(3*out_res/2-x-1,out_res/2+y) = 
	  U(3*out_res/2-x-1,out_res/2-y-1) = 
	  U(3*out_res/2+x,out_res/2-y-1) = 
	  U(3*out_res/2+x,out_res/2+y) = SP(x,y) * nf ;
      
      
      U.toImageBW(img) ;
      
      char fn[128] ;
      sprintf(fn,"itr%.3d.bmp", frm) ;
      img.writeImage(fn) ;

      for(int i = 0 ; i < out_res ; i++)
	delete[] cpx[i] ;

      delete[] cpx ;
}

}
