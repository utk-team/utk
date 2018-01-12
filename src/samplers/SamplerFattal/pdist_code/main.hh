#include "../npde/tensor2d.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../general/nor_dist.h"
#include "../general/fft.h"
//#include "hdrimage.h"
#include <getopt.h>
#include <string>
#include <cassert>
#include <iomanip>

namespace fattal
{

#define RELAX_MAG 0.05
#define MAX_KER_DIM 32
//#define MIN_SIGMA 1
//#define MAX_SIGMA 100 // maximal sigma to relax
#define KER_AMP 1
#define EXP_TAB_LEN 512
#define MAX_INIT_NDOTS 1100000
#define RAD 1.5
#define KER_SIZE_FAC 2.8 // lower this! times RAD
#define INACT_POINTS 0
#define ACTIVE_POINTS 1
#define TARGET_SIGMA 0.5

#define PERIODIC

int inactive_level = 3 ;

int iters = 100 ;
int fl_iters = 100 ; // iterations at the fine level
int citers = 0 ; // after which we check for escaping

int save_every = 0 ;
int frm = 0 ;

int out_res = 512 ;
Tensor2d SP(out_res, out_res) ;

double orad ;
double* Exp_tab ;
double exp_fac ;

unsigned long* Pow2_tab ;

// std::string param_output;
// unsigned int param_nb_points = 1024;
// unsigned int param_nb_real = 1;
// float param_temperature = 0.5;

struct Dot 
{
  double x, y ;
  unsigned char density_log4 ; // density = 4^density_log4
  unsigned char non_mult_levels ;
} ;

void snap_shot(Dot* dots, int ndots, int it, int nx, int ny) ;
void reorder_dots(int nx, int ny, Dot*& dots, int ndots) ;
void build_init_sample(double*& osigma, double*& rho,
		       int nx, int ny, Dot*& dots, int& ndots) ;
void init_fields(double* rho, int nx, int ny, double*& sigma,
		 double*& sigma_x, double*& sigma_y, double*& osigma) ;
void relax(double temperature, Dot*& dots, int& ndots, double* CA, double* rho,
	   double* osigma, double* sigma_x, double* sigma_y, int nx, int ny,
	   int iters, int save_every=0) ;
void upsample_dots(Dot*& dots, int& ndots, double* sigma, int nx, int ny) ;
void render_points(double* A, double* osigma, int nx, int ny, Dot* dots, int ndots, int act) ;

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

/********************************/
/********************************/

void init_fields(double* rho, int nx, int ny, double*& sigma,
		 double*& sigma_x, double*& sigma_y, double*& osigma) {

  int len = nx * ny ;

  if(sigma)
    delete[] sigma ;
  if(sigma_x)
    delete[] sigma_x ;
  if(sigma_y)
    delete[] sigma_y ;
  if(osigma)
    delete[] osigma ;

  sigma = new double[len] ;
  sigma_x = new double[len] ;
  sigma_y = new double[len] ;
  osigma = new double[len] ;

  for(int i = 0 ; i < len ; i++) {

      osigma[i] = sqrt(rho[i]) ;

      sigma[i] = 0 ; // 1.0 / osigma[i] ;
      //      if(sigma[i] > 100 || osigma[i] < 0.01)
      //	sigma[i] = 100 ;
    }

  int nxm = nx - 1 ;
  int nym = ny - 1 ;

  for(int y = 0 ; y < ny ; y++)
    for(int x = 0 ; x < nx ; x++) {
      int i = x + y * nx ;
      if(x > 0)
	if(x < nxm)
	  sigma_x[i] = 0.5 * (sigma[i+1] - sigma[i-1]) ;
	else
	  sigma_x[i] = sigma[i] - sigma[i-1] ;
      else
	sigma_x[i] = sigma[i+1] - sigma[i] ;

      if(y > 0)
	if(y < nym)
	  sigma_y[i] = 0.5 * (sigma[i+nx] - sigma[i-nx]) ;
	else
	  sigma_y[i] = sigma[i] - sigma[i-nx] ;
      else
	sigma_y[i] = sigma[i+nx] - sigma[i] ;
    }
}




/********************************/
/********************************/

void build_init_sample(double*& osigma, double*& rho,
		       int nx, int ny, Dot*& dots, int& ndots) {

  //cout << "Initial throwing" << endl ;

  int len = nx * ny ;

  double* A = new double[len] ;
  double* D = new double[len] ;

  for(int i = 0 ; i < len ; i++) {
    A[i] = 0 ;
    D[i] = rho[i] ;
  }

  dots = new Dot[MAX_INIT_NDOTS] ;
  ndots = 0 ;

  unsigned* P = new unsigned[len] ;

  double* K = new double[len] ;

  get_perm(P,len) ;

  int lx, ly, wx, wy ;

  double total_rho = 0 ;
  for(int i = 0 ; i < len ; i++)
    total_rho += rho[i] ;

  for(int i = 0 ; i < len ; i++) {

    if(total_rho < 0)
      break ;

    int pi = P[i] ;

    int cx = pi % nx ;
    int cy = (pi - cx) / nx ;

    // if(ABS(D[pi]) < rho[pi] * 0.5) // check this once in a while
    //      continue ;

    unsigned char pdensity_log4 = 0 ;

    if(rho[pi] > 1)
      pdensity_log4 = (unsigned char)(log(rho[pi]) / log(4.0) + 0.9999) ;

    kernel(osigma[pi], Pow2_tab[pdensity_log4], cx, cy, lx, ly, wx, wy, K, nx, ny) ;

    double err_D = 0 ;
    double err_DK = 0 ;

    double* pK = K ;

    double v ;


#ifndef PERIODIC

    for(int y = 0 ; y < wy ; y++) {
      double* pD = &D[lx + (y + ly) * nx] ;

      for(int x = 0 ; x < wx ; x++) {
	v = *pD ;

	err_D += ABS(v) ;
	v -= *pK ;

	err_DK += ABS(v) ;
	pK++; pD++ ;
      }
    }

#else

    for(int y = 0 ; y < wy ; y++) {
      int ofy = ((y+ly)%ny)*nx ;
      for(int x = 0 ; x < wx ; x++) {
	int j = (x+lx) % nx + ofy ;

	v = D[j] ;

	err_D += ABS(v) ;
	v -= *pK ;

	err_DK += ABS(v) ;

	pK++ ;
      }
    }

#endif

    // Accept proposed point

    if(err_D > err_DK) {

      dots[ndots].x = cx ;
      dots[ndots].y = cy ;
      dots[ndots].density_log4 = pdensity_log4 ;
      dots[ndots].non_mult_levels = 0 ;

      ndots++ ;

      pK = K ;

#ifndef PERIODIC

      for(int y = 0 ; y < wy ; y++) {
	int j = lx + (y + ly) * nx ;
	double* pD = &D[j] ;
	double* pA = &A[j] ;

	for(int x = 0 ; x < wx ; x++) {

	  total_rho -= *pK ;
	  *pD -= *pK ;
	  *pA += *pK ;
	  pD++ ; pA++ ; pK++ ;
	}
      }
#else

      for(int y = 0 ; y < wy ; y++) {
	int ofy = ((y+ly)%ny)*nx ;
	for(int x = 0 ; x < wx ; x++) {
	  int j = (x+lx) % nx + ofy ;

	  total_rho -= *pK ;
	  D[j] -= *pK ;
	  A[j] += *pK ;
	  pK ++ ;
	}
      }

#endif

    }
  }

  //cout << "Number of dots created = " << ndots << endl ;


  delete[] P ;
  delete[] K ;
  delete[] A ;
  delete[] D ;
}

/********************************/
/********************************/


void reorder_dots(int nx, int ny, Dot*& dots, int ndots) {


  int len = nx * ny ;
  int* indices = new int[len] ;

  for(int i = 0 ; i < len ; i++)
    indices[i] = -1 ;

  for(int i = 0 ; i < ndots ; i++) {
    int xy = (int)dots[i].x + nx * ((int)dots[i].y) ;

    if(indices[xy] < 0)
      indices[xy] = i ;
    //else
      //cout << dots[i].x / nx << " , " << dots[i].y / ny << endl;
  }

  Dot* new_dots = new Dot[ndots] ;

  int ni = 0 ;

  for(int i = 0 ; i < len ; i++) {
    int ind = indices[i] ;

    if(ind >= 0) {
      new_dots[ni++] = dots[ind] ;
    }
  }

  if(ni != ndots) {
    //cout << "Error reordering " << ni << " " << ndots << endl ;
    exit(1) ;
  }

  delete[] dots ;

  dots = new_dots ;

  delete[] indices ;
}

/********************************/
/********************************/


void relax(double temperature, Dot*& dots, int& ndots, double* CA, double* rho,
	   double* osigma, double* sigma_x, double* sigma_y, int nx, int ny,
	   int iters, int save_every) {

  //cout << "Stochastic relaxations:" << endl ;

  int nxm = nx - 1 ;
  int nym = ny - 1 ;

  int len = nx * ny ;

  double* A = new double[len] ;

  for(int i = 0 ; i < len ; i++)
    A[i] = CA[i] ;

  render_points(A, osigma, nx, ny, dots, ndots, ACTIVE_POINTS) ;

  double* D = new double[len] ;

  for(int i = 0 ; i < len ; i++)
    D[i] = (rho[i] - A[i]) ;

  double diff_x, diff_y ;

  NorDist nd ;

  double DT, energy_fac, random_fac ;

  //double ox, oy;

  for(int it = 0 ; it < iters ; it++) {

    disp(it, iters) ;

    for(int i = 0 ; i < ndots ; i++) {

      Dot& cdot = dots[i] ;

      if(cdot.non_mult_levels >= inactive_level)
      	continue ;

      unsigned long density_sqrt = Pow2_tab[cdot.density_log4] ;

      //ox = cdot.x ; oy = cdot.y ;

      double cdosig = interp(osigma, cdot.x, cdot.y, nx, ny, nxm, nym) ;

      DT = RELAX_MAG * (temperature * RAD / density_sqrt / cdosig) ;

      energy_fac = DT / (2 * temperature) ;
      random_fac = sqrt(DT) ;

      kernel_remove_dif(D, A, cdosig, density_sqrt,
			interp(sigma_x, cdot.x, cdot.y, nx, ny, nxm, nym),
			interp(sigma_y, cdot.x, cdot.y, nx, ny, nxm, nym),
			cdot.x, cdot.y, diff_x, diff_y, nx, ny) ;

      cdot.x -= diff_x * energy_fac ;
      cdot.y -= diff_y * energy_fac ;

      if(it < iters-1) {
	cdot.x += nd.sample() * random_fac ;
	cdot.y += nd.sample() * random_fac ;
      }


#ifndef PERIODIC

      if(it > citers) {
	if(cdot.x <= 0 || cdot.y <= 0 ||
	   cdot.x >= nx || cdot.y >= ny) {

	  ndots -- ;

	  for(int j = i ; j < ndots ; j++)
	    dots[j] = dots[j+1] ;

	  continue ;
	}
      }
      else {
	if(cdot.x < 0) cdot.x = ox ;
	if(cdot.y < 0) cdot.y = oy ;
	if(cdot.x >= nx-1) cdot.x = ox ;
	if(cdot.y >= ny-1) cdot.y = oy ;
      }

#else
      if(cdot.x < 0) cdot.x += nx ;
      if(cdot.y < 0) cdot.y += ny ;
      if(cdot.x >= nx) cdot.x -= nx ;
      if(cdot.y >= ny) cdot.y -= ny ;
#endif

      // Adding new kernel

      add_kernel(interp(osigma, cdot.x, cdot.y, nx, ny, nxm, nym),
		 density_sqrt, cdot.x, cdot.y, A, D, nx, ny) ;
    }

    /*
    double mx = 0 ;

    for(int i = 0 ; i < ndots ; i++)
      mx += cos(dots[i].x / nx * 2.0 * M_PI) ;

    df << it << " " << mx / ndots << endl ;
    */

    if(save_every > 0 && it % save_every == 0) {

      snap_shot(dots, ndots, nx, ny) ;

      char fn[128] ;
      BImage img ;
      show(A, img, nx, ny) ;
      img.resize(out_res / nx) ;
      sprintf(fn,"a%.3d.bmp",frm++) ;

      img.writeImage(fn) ;
    }
  }


  //  df.close() ;

  double sa = 0 ;
  double sr = 0 ;
  for(int i = 0 ; i < len ; i++) {
    sa += A[i] ;
    sr += rho[i] ;
  }

  //cout << "Total matter in A and rho " << sa << " " << sr << " " << (double)((int)(sa / sr * 1000))/10 << "%" << endl ;


  delete[] A ;
  delete[] D ;

  //file.close() ;
}

/********************************/
/********************************/


void upsample_dots(Dot*& dots, int& ndots, double* sigma, int nx, int ny) {

  int nx2 = nx * 2 ;
  int ny2 = ny * 2 ;

  int nxm = nx - 1 ;
  int nym = ny - 1 ;

  //cout << "Interpolating new dots: " << ndots << " -> " << flush ;

  NorDist nd ;

  Dot* new_dots = new Dot[ndots * 4] ;

  int j = 0 ;

  for(int i = 0 ; i < ndots ; i++) {

    Dot& cdot = dots[i] ;

    if(cdot.density_log4 == 0) {

      Dot& new_dot = new_dots[j++] ;

      new_dot.x = 2.0 * cdot.x ;
      new_dot.y = 2.0 * cdot.y ;

      new_dot.density_log4 = 0 ;
      new_dot.non_mult_levels = cdot.non_mult_levels + 1 ;

      continue ;
    }


    int mul = 4 ;

    double r = (RAD * Pow2_tab[cdot.density_log4] *
		interp(sigma, cdot.x, cdot.y, nx, ny, nxm, nym)) ;

    for(int k = 0 ; k < mul ; k++) { // MAKE random angle
      int dx, dy ;

      if(k == 0) {
	dx = 1 ;
	dy = 1 ;
      }
      if(k == 1) {
	dx = -1 ;
	dy = -1 ;
      }
      if(k == 2) {
	dx = -1 ;
	dy = 1 ;
      }
      if(k == 3) {
	dx = 1 ;
	dy = -1 ;
      }

      dx = r * nd.sample() ;
      dy = r * nd.sample() ;

      //      dx *= r * 0.9 ;
      //      dy *= r * 0.9 ;

      double xx = 2.0 * cdot.x + dx ;
      double yy = 2.0 * cdot.y + dy ;

#ifndef PERIODIC

      if(xx > nx2)
	xx = nx2 - dx ;
      if(yy > ny2)
	yy = ny2 - dy ;
      if(xx < 0)
	xx = 0 ;
      if(yy < 0)
	yy = 0 ;
#else
      if(xx > nx2)
	xx -= nx2 ;
      if(yy > ny)
	yy -= ny2 ;
      if(xx < 0)
	xx += nx2 ;
      if(yy < 0)
	yy += ny2 ;
#endif

      Dot& new_dot = new_dots[j++] ;

      new_dot.x = xx ;
      new_dot.y = yy ;

      new_dot.density_log4 = cdot.density_log4 - 1 ;

      new_dot.non_mult_levels = 0 ;
    }
  }

  delete[] dots ;

  dots = new_dots ;
  ndots = j ;
  //cout << ndots << endl ;
}

/********************************/
/********************************/


void render_points(double* A, double* osigma, int nx, int ny,
		   Dot* dots, int ndots, int act) {

  int nxm = nx - 1 ;
  int nym = ny - 1 ;

  double* tD = new double[nx*ny] ;

  BImage img ;
  //  show(A, img, nx, ny) ; img.show() ;

  for(int i = 0 ; i < ndots ; i++) {
    Dot& cdot = dots[i] ;

    // entire op inside kernel

    if((cdot.non_mult_levels >= inactive_level && act) ||
       (cdot.non_mult_levels < inactive_level && !act))
      continue ;

    add_kernel(interp(osigma, cdot.x, cdot.y, nx, ny, nxm, nym),
	       Pow2_tab[cdot.density_log4], cdot.x, cdot.y, A, tD, nx, ny) ;
  }
  delete[] tD ;
}

}
