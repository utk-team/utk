#ifndef TENSOR3D_HH
#define TENSOR3D_HH

#include "defs.hh"
#include "../bimage/bimage.h"

class Tensor3d {
public:
  /* Data Members */
  
  int len_x, len_y, len_z , plane, length ;
  real * data ;

  /* Methods */
  
  Tensor3d() { len_x = len_y = len_z = plane = length = 0 ; data = NULL ; }
  Tensor3d(int lx, int ly, int lz) ;
  Tensor3d(Tensor3d& other) ;
  Tensor3d(char * filename) ;

  ~Tensor3d() { if(data) delete[] data ; data = NULL ; }
  void swap(Tensor3d& other) ;
  void empty() { len_x = len_y = plane = length = 0 ; if(data) delete[] data ; data = NULL ; }
  
  Tensor3d& operator=(Tensor3d& other) ;
  real& operator[](int i) { return data[i] ; }
  real& operator()(int x, int y, int z) { return data[x + y * len_x + z * plane] ; }
  inline real operator()(double x, double y, double z) ;
  void one_minus() ;

  real cube(double x, double y, double z) ;

  void add_mul(Tensor3d& other, double c) ;
  void zero_one() ;
  
  void Pow(double c) ;
    
  void set(int lx, int ly, int lz) ;
  void clear(real val = 0) ;

  void write(char * filename) ;
  void save(char * filename) { write(filename) ; }
  void read(char * filename) ;
  void save_char(char * filename) ;
  void read_char(char * filename) ;
  
  void restrictx2(Tensor3d& r) ;
  void prolongx2(Tensor3d& r) ;  
  void prolongx2z(Tensor3d& r) ;
  void restrict_full(Tensor3d& r) ;
  void restrict_half(Tensor3d& r) ;
  void restrict_none(Tensor3d& r) ;
  void prolong_full(Tensor3d& p) ;
  void prolong_none(Tensor3d& p) ;

  void show(int z = -1) ;
  void showH(int z = -1) ;

  void toImage(BImage& img, int z = -1) ;
  void toImageH(BImage& img, int z = -1) ;

  double norm_max() ;
  double norm_abs() ;
  double norm_euc() ;
  void Log() ;
  void Exp() ;
  
double min() ;
double max() ;

  Tensor3d& operator*=(real val) ;
  Tensor3d& operator+=(Tensor3d& other) ;
  Tensor3d& operator-=(Tensor3d& other) ;
   
   Tensor3d& operator+=(real v) ;
} ;

/******** Inline definintions ********/



real Tensor3d::operator()(double x, double y, double z) {
  int ix = (int) x ;
  int iy = (int) y ;
  int iz = (int) z ;

  double dx = x - ix ;
  double dy = y - iy ;
  double dz = z - iz ;
  double ddx = 1 - dx ;
  double ddy = 1 - dy ;
  double ddz = 1 - dz ;

  int i = ix + iy * len_x + iz * plane ;

  double vz = data[i] * ddx * ddy + data[i + 1] * dx * ddy +
    data[i + len_x] * ddx * dy + data[i + 1 + len_x] * dx * dy ;

  double vvz = data[i + plane] * ddx * ddy + data[i + 1 + plane] * dx * ddy +
    data[i + len_x + plane] * ddx * dy + data[i + 1 + len_x + plane] * dx * dy ;
  
  return vz * ddz + vvz * dz ;
}


#endif /* TENSOR3D_HH */
