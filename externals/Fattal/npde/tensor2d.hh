#ifndef TENSOR2D_HH
#define TENSOR2D_HH

#include "defs.hh"
#include "../bimage/bimage.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

namespace fattal
{
class Tensor2d {
public:
  /* Members */
  
  int len_x, len_y ;
  real * data ;
   
  /* Methods */

  Tensor2d() { len_x = len_y = 0 ; data = NULL ; }
  Tensor2d(int lx, int ly) ;
  Tensor2d(const char * filename) ;
  Tensor2d(Tensor2d& other) ;

  ~Tensor2d() { if(data) delete[] data ; data = NULL ; }

  void point(Tensor2d& other) ;
  void swap(Tensor2d& other) ;
  void empty() { len_x = len_y = 0 ; if(data) delete[] data ; data = NULL ; }
  Tensor2d& operator=(Tensor2d& other) ;

  void add_mul(Tensor2d& other, double c) ;

  real& operator()(int x, int y) { return data[x + y * len_x] ; }  
  real& operator[](int i) { return data[i] ; }
  inline real operator()(double x, double y) ;
  inline void put(double x, double y, double v=1) ;

  void set(int lx, int ly) ;
  void clear(real val = 0) ;
   void Rand(real val = 1) ;

  void write(const char * filename) ;
  void save(const char * filename) { write(filename) ; }
  void read(const char * filename) ;
  void save_text(const char * filename) ;
  void read_text(const char * filename, int lx, int ly) ;
  

  void restrict_full(Tensor2d& r) ;
  void restrict_any(Tensor2d& r) ;
  void restrict_half(Tensor2d& r) ;
  void restrict_none(Tensor2d& r) ;
  void prolong_full(Tensor2d& p) ;
  void prolong_any(Tensor2d& p) ;
  void prolong_none(Tensor2d& p) ;

  void print() ;

  void Log() ;
  void Exp() ;
  
  void zero_one(double low=0, double high=1) ;	      
  void Pow(double c) ;
   void Pow2() ;
   void SPow(double c) ;
  void Abs() ;

  void resize(int factor, double* filter, int filter_len) ;
  void conv(double* filter, int filter_len) ;
  
  double norm_max() ;
  double norm_abs() ;
  double norm_euc() ;

  real min() ;
  real max() ;
  real mean() ;
  real var() ;
 
   void mean_var(double& m, double& v) ;
   
  Tensor2d& operator*=(real val) ;
  Tensor2d& operator*=(Tensor2d& other) ;
  Tensor2d& operator+=(Tensor2d& other) ;
  Tensor2d& operator-=(Tensor2d& other) ;

  Tensor2d& operator+=(double v) ;
  Tensor2d& operator-=(double v) ;

  void toImage(BImage& image) ;
  void toImageH(BImage& image) ;
  void toImageBW(BImage& image) ;
  void show(char* title=NULL) ;
  void showBW(char* title=NULL) ; 
  void showH(char* title=NULL) ;
  void showabs(char* text = NULL) ;
  void saveImage(char* fn) ;
  void saveImageH(char* fn) ;
  void saveImageBW(char* fn) ;

} ;

real Tensor2d::operator()(double x, double y) {
  int ix = (int)x ;
  int iy = (int)y ;
  double dx = x - ix ;
  double dy = y - iy ;
  double odx = 1 - dx ;
  double ody = 1 - dy ;
  
  return (*this)(ix,iy) * odx * ody + (*this)(ix+1,iy) * dx * ody +
    (*this)(ix,iy+1) * odx * dy +(*this)(ix+1,iy+1) * dx * dy  ;
}


void Tensor2d::put(double x, double y, double v) {
  int ix = (int)x ;
  int iy = (int)y ;
  double dx = x - ix ;
  double dy = y - iy ;
  double odx = 1 - dx ;
  double ody = 1 - dy ;

  (*this)(ix, iy) += odx * ody * v ;
  (*this)(ix+1, iy) += dx * ody * v ;
  (*this)(ix, iy+1) += odx * dy * v ;
  (*this)(ix+1, iy+1) += dx * dy * v ;
}

};

#endif /* TENSOR2D_HH */
