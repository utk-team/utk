#ifndef _NOR_DIST
#define _NOR_DIST

#include <time.h>
#include <math.h>

#define SAMP_NUM 32767
#define SUM_NUM 64

class NorDist {
 public:
 
  NorDist(int n = SAMP_NUM, int seed=0) {
    
    if(seed == 0)
      srand ( time(NULL) );
    else
      srand(seed) ;
    
    nsamp = n ;
    samples = new double[nsamp] ;
    
    double fact = sqrt(12.0 / SUM_NUM) ;
    
    double fix_mean = (double)SUM_NUM / 2.0 ;

    for(int i = 0 ; i < nsamp ; i++) {
      double v = 0 ;
      
      for(int k = 0 ; k < SUM_NUM ; k++)	
	v += (double)rand() / RAND_MAX ;
      
      v -= fix_mean ;

      samples[i] = v * fact ;
    }  
    ci = 0 ;
  }

  double sample() {
    ci++ ;

    if(ci >= nsamp)
      ci = 0 ;
    return samples[ci] ;
  }

  ~NorDist() {
    delete[] samples ;
  }

  double* samples ;
  int nsamp ;
  int ci ;
  
} ;

#endif
