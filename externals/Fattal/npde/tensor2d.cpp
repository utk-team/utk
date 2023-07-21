#include <iomanip>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include "tensor2d.hh"

namespace fattal 
{
Tensor2d::Tensor2d(int lx, int ly) {
  data = NULL ;
  set(lx, ly) ;
}

Tensor2d::Tensor2d(Tensor2d& other) {
  data = NULL ;
  *this = other ;
}

Tensor2d::Tensor2d(const char * filename) {
  data = NULL ;
  read(filename) ;
}

void Tensor2d::show(char* title) {
  BImage img ;
  toImage(img) ;
   if(title)
     img.text(2,2,title) ;
  img.show() ;
}

void Tensor2d::showabs(char* text) {
  Tensor2d tmp = (*this) ;

  BImage img ;
  tmp.Abs() ;
  tmp.toImageBW(img) ;
  img.text(2, 2, text, 255, 255, 255) ;
  img.show() ;
}

void Tensor2d::showBW(char *title) {
  BImage img ;
  toImageBW(img) ;
     if(title)
     img.text(2,2,title) ;
  img.show() ;
}

void Tensor2d::showH(char* title) {
  BImage img ;
  toImageH(img) ;
     if(title)
     img.text(2,2,title) ;
  img.show() ;
}


void Tensor2d::swap(Tensor2d& other) {
   int tt = other.len_x ;
   other.len_x = len_x ;
   len_x = tt ;
   
   tt = other.len_y ;
   other.len_y = len_y ;
   len_y = tt ;

   real *t = data ; data = other.data ; other.data = t ;
} 

void Tensor2d::point(Tensor2d& other) {
   if(data)
     delete[] data ;
   
   len_x = other.len_x ;
   len_y = other.len_y ;
   
   data = other.data ;
}


Tensor2d& Tensor2d::operator=(Tensor2d& other) {
  set(other.len_x, other.len_y) ; 
  int length = len_x * len_y ;
  
  for(int i = 0 ; i < length ; i++)
    data[i] = other.data[i] ;
  
  return *this ;
}
void Tensor2d::Exp() {
  int length = len_x * len_y ;
  for(int i = 0 ; i < length ; i++)
    data[i] = exp(data[i]) ;
}

void Tensor2d::Log() {
  int length = len_x * len_y ;
  for(int i = 0 ; i < length ; i++)
    data[i] = log(data[i]) ;
}

void Tensor2d::saveImage(char* fn){
  BImage img;
  toImage(img);
  img.writeImage(fn) ;
}

void Tensor2d::saveImageH(char* fn){
  BImage img;
  toImageH(img);
  img.writeImage(fn) ;
}

void Tensor2d::saveImageBW(char* fn){
  BImage img;
  toImageBW(img);
  img.writeImage(fn) ;
}

void Tensor2d::toImageBW(BImage& image) {

  image.set(len_x, len_y) ;
   
  double min = INF, max = -INF ;

  int length = len_x * len_y ;
 
  for(int i = 0 ; i < length ; i++) {
      if (data[i] < min)
	min = data[i] ;
      if (data[i] > max)
	max = data[i] ;
    }
   
  double a ;
   
  if(fabs(max - min) < EPS) {
	
      cerr << "Image is constant" << endl ;
      a = 1 / max ;
    }
   
  else
    a = 255.0 / (max - min) ;
   
  for(unsigned int y = 0 ; y < (unsigned int)len_y ; y++ ) {
      for(int x = 0 ; x < len_x ; x++) {
	image(x,y,0) = (uchar)((data[x+y*len_x] - min) * a) ;
	image(x,y,1) = (uchar)((data[x+y*len_x] - min) * a) ;
	image(x,y,2) = (uchar)((data[x+y*len_x] - min) * a) ;
      }   
  }
}

void Tensor2d::print() {
  for(int y = 0 ; y < len_y ; y++) {
    for(int x = 0 ; x < len_x ; x++)
      cout << data[x+y*len_x] << " " ;
    cout << endl ;
  }
}

void Tensor2d::toImage(BImage& image) {

  image.set(len_x, len_y) ;

  double min = INF, max = -INF ;

  int length = len_x * len_y ;
  
  for(int i = 0 ; i < length ; i++) {
    if (data[i] < min)
      min = data[i] ;
    if (data[i] > max)
      max = data[i] ;
  }
  
  double a ;
  
  if(fabs(max - min) < EPS) {
    cerr << "Image is constant" << endl ;
    a = 1 / max ;
  }
  else
    a = 1.0 / (max - min) ;
  
  for(unsigned int y = 0 ; y < (unsigned int)len_y ; y++ ) {
    for(int x = 0 ; x < len_x ; x++) {

      image.jet(x,y,(data[x+y*len_x] - min) * a) ;
    }
  }
}

void Tensor2d::toImageH(BImage& image) {

  image.set(len_x, len_y) ;

  double min = INF, max = -INF ;

  int length = len_x * len_y ;
  
  for(int i = 0 ; i < length ; i++) {
    if (data[i] < min)
      min = data[i] ;
    if (data[i] > max)
      max = data[i] ;
  }
  
  double a ;
  
  if(fabs(max - min) < EPS) {
    cerr << "Image is constant" << endl ;
    a = 1 / max ;
  }
  else
    a = 1.0 / (max - min) ;
  
  for(unsigned int y = 0 ; y < (unsigned int)len_y ; y++ ) {
    for(int x = 0 ; x < len_x ; x++) {

      image.hot(x,y,(data[x+y*len_x] - min) * a) ;
    }
  }
}




void Tensor2d::add_mul(Tensor2d& other, double c) {
   int len = len_x * len_y ;
   
   for(int i = 0 ; i < len ; i++)
     data[i] += other.data[i] * c ;
}

void Tensor2d::set(int lx, int ly) {
   if(data != NULL && lx == len_x && ly == len_y)
     return ;
     
   len_x = lx ; len_y = ly ;

   int length = lx * ly ;
   
  if(data != NULL)
    delete[] data ;

  data = new real[length] ;

  if (!data) {
    cerr << "Allocation error of Tensor2d. (size req - " << length << ")" << endl ;
    exit(1) ;
  }
}


void Tensor2d::clear(real val) {
  int length = len_x * len_y ;
  for(int i = 0 ; i < length ; i++)
    data[i] = val ;
}

void Tensor2d::Rand(real val) {
  int length = len_x * len_y ;
  for(int i = 0 ; i < length ; i++)
    data[i] = val * (double)rand() / RAND_MAX ;
}


void Tensor2d::restrict_full(Tensor2d& r) {
  r.set((len_x - 1) / 2 + 1, (len_y - 1) / 2 + 1) ;
   
  int i, j ;

  for(int y = 1 ; y < r.len_y - 1 ; y++) {
    for(int x = 1 ; x < r.len_x - 1 ; x++) {

      i = x + y * r.len_x ;
      j = 2 * x + 2 * y * len_x ;

      r.data[i] = 0.25 * data[j] + 0.125 * (data[j+1] + data[j-1] + data[j+len_x] + data[j-len_x]) +
	0.0625 * (data[j+1+len_x] + data[j+1-len_x] + data[j-1+len_x] + data[j-1-len_x]) ;
    }
  }

  for(int x = 1 ; x < r.len_x - 1 ; x++) {
    i = x ;
    j = 2 * x ;

    r.data[i] = 0.333 * data[j] + 0.1666 * (data[j+1] + data[j-1] + data[j+len_x]) +
      0.08333 * (data[j+1+len_x] + data[j-1+len_x]) ;

    i += (r.len_y-1) * r.len_x ;
    j += (len_y-1) * len_x ;

    r.data[i] = 0.333 * data[j] + 0.1666 * (data[j+1] + data[j-1] + data[j-len_x]) +
      0.08333 * (data[j+1-len_x] + data[j-1-len_x]) ;
  }
  
  for(int y = 1 ; y < r.len_y - 1 ; y++) {
    i = y * r.len_x ;
    j = 2 * y * len_x ;
    
    r.data[i] = 0.333 * data[j] + 0.1666 * (data[j+1] + data[j-len_x] + data[j+len_x]) +
      0.08333 * (data[j+1+len_x] + data[j+1-len_x]) ;

    i += r.len_x - 1 ;
    j += len_x - 1 ;

     r.data[i] = 0.333 * data[j] + 0.1666 * (data[j-1] + data[j-len_x] + data[j+len_x]) +
      0.08333 * (data[j-1+len_x] + data[j-1-len_x]) ;
  }

  r.data[0] = 0.444 * data[0] + 0.222 * (data[1] + data[len_x]) + 0.111 * data[1+len_x] ;

  r.data[r.len_x-1] = 0.444 * data[len_x-1] + 0.222 * (data[len_x-2] + data[2*len_x-1]) + 0.111 * data[2*len_x-2] ;

  r.data[(r.len_y-1)*r.len_x] = 0.444 * data[(len_y-1)*len_x] + 0.222 * (data[(len_y-1)*len_x+1] + data[(len_y-2)*len_x]) +
    0.111 * data[(len_y-2)*len_x+1] ;

  r.data[(r.len_y-1)*r.len_x + r.len_x-1] = 0.444 * data[(len_y-1)*len_x+len_x-1] +
    0.222 * (data[(len_y-1)*len_x+len_x-2] + data[(len_y-1)*len_x-1]) + 0.111 * data[(len_y-1)*len_x-2] ;
}

void Tensor2d::restrict_half(Tensor2d& r) {
  r.set((len_x - 1) / 2 + 1, (len_y - 1) / 2 + 1) ;
  
  int i, j ;

  for(int y = 1 ; y < r.len_y - 1 ; y++) {
    for(int x = 1 ; x < r.len_x - 1 ; x++) {

      i = x + y * r.len_x ;
      j = 2 * x + 2 * y * len_x ;

      r.data[i] = 0.3333 * data[j] + 0.16666 * (data[j+1] + data[j-1] + data[j+len_x] + data[j-len_x]) ;
    }
  }

  for(int x = 1 ; x < r.len_x - 1 ; x++) {
    i = x ;
    j = 2 * x ;

    r.data[i] = 0.4 * data[j] + 0.2 * (data[j+1] + data[j-1] + data[j+len_x]) ;

    i += (r.len_y-1) * r.len_x ;
    j += (len_y-1) * len_x ;

    r.data[i] = 0.4 * data[j] + 0.2 * (data[j+1] + data[j-1] + data[j-len_x]) ;
  }
  
  for(int y = 1 ; y < r.len_y - 1 ; y++) {
    i = y * r.len_x ;
    j = 2 * y * len_x ;
    
    r.data[i] = 0.4 * data[j] + 0.2 * (data[j+1] + data[j-len_x] + data[j+len_x]) ;

    i += r.len_x - 1 ;
    j += len_x - 1 ;

    r.data[i] = 0.4 * data[j] + 0.2 * (data[j-1] + data[j-len_x] + data[j+len_x]) ;
  }

  r.data[0] = 0.5 * data[0] + 0.25 * (data[1] + data[len_x]) ;

  r.data[r.len_x-1] = 0.5 * data[len_x-1] + 0.25 * (data[len_x-2] + data[2*len_x-1]) ;

  r.data[(r.len_y-1)*r.len_x] = 0.5 * data[(len_y-1)*len_x] + 0.25 * (data[(len_y-1)*len_x+1] + data[(len_y-2)*len_x]) ;

  r.data[(r.len_y-1)*r.len_x + r.len_x-1] = 0.5 * data[(len_y-1)*len_x+len_x-1] +
    0.25 * (data[(len_y-1)*len_x+len_x-2] + data[(len_y-1)*len_x-1]) ;
}
  
void Tensor2d::restrict_none(Tensor2d& r) {
  r.set((len_x - 1) / 2 + 1, (len_y - 1) / 2 + 1) ;
  
  for(int y = 0 ; y < r.len_y ; y++) {
    for(int x = 0 ; x < r.len_x ; x++) {
      r.data[x+y*r.len_x] = data[2*x+2*y*len_x] ;
    }
  }
}

void Tensor2d::restrict_any(Tensor2d& r) {

  r.set((int)((double)len_x * 0.5), (int)((double)len_y * 0.5)) ;
  
  for(int y = 0 ; y < r.len_y ; y++) {
    for(int x = 0 ; x < r.len_x ; x++) {
      r(x,y) = (*this)(2*x, 2*y) ;
    }
  }
}

void Tensor2d::prolong_full(Tensor2d& p) {
  p.set((len_x - 1) * 2 + 1, (len_y - 1) * 2 + 1) ;

  p.data[0] = data[0] ;
 
  for(int y = 1 ; y < len_y ; y++) {
    p.data[y*2*p.len_x] = data[y*len_x] ;
    p.data[(y*2-1)*p.len_x] = 0.5 * (data[(y-1)*len_x] + data[y*len_x]) ;
  }
  
  for(int x = 1 ; x < len_x ; x++) {
    p.data[x*2] = data[x] ;
    p.data[x*2-1] = 0.5 * (data[x-1] + data[x]) ;
  }

  int i, j ;

  for(int y = 1 ; y < len_y ; y++) {
    for(int x = 1 ; x < len_x ; x++) {

      i = x + y * len_x ;
      j = 2 * x + 2 * y * p.len_x ;

      p.data[j] = data[i] ;

      p.data[j-1] = 0.5 * (data[i] + data[i-1]) ;
      p.data[j-p.len_x] = 0.5 * (data[i] + data[i-len_x]) ;
      
      p.data[j-1-p.len_x] = 0.25 * (data[i] + data[i-1] + data[i-len_x] + data[i-len_x-1]) ;
    }
  }
}

void Tensor2d::prolong_none(Tensor2d& p) {
  p.set((len_x - 1) * 2 + 1, (len_y - 1) * 2 + 1) ;

  int i, j ;

  real val ;

  p.data[0] = data[0] ;
 
  for(int y = 1 ; y < len_y ; y++) {
    p.data[y*2*p.len_x] = data[y*len_x] ;
    p.data[(y*2-1)*p.len_x] = data[y*len_x] ;
  }
  
  for(int x = 1 ; x < len_x ; x++) {
    p.data[x*2] = data[x] ;
    p.data[x*2-1] = data[x] ;
  }
  
  for(int y = 1 ; y < len_y ; y++) {
    for(int x = 1 ; x < len_x ; x++) {
      
      i = x + y * len_x ;
      j = 2 * x + 2 * y * p.len_x ;

      val = data[i] ;
       
      p.data[j] = val ;
      
      p.data[j-1] = val ;
      p.data[j-p.len_x] = val ;
      
      p.data[j-1-p.len_x] = val ;
    }
  }
}

void Tensor2d::prolong_any(Tensor2d& p) {
  int lx = len_x * 2 ; 
  int ly = len_y * 2 ; 

  p.set(lx, ly) ;

  for(int y = 0 ; y < ly ; y++) {
    for(int x = 0 ; x < lx ; x++) {
      
      double xx = (double)x * 0.5 - 0.5 ; 
      double yy = (double)y * 0.5 - 0.5 ;

      if(xx < 0)
 	xx = 0 ;
      
      if(yy < 0)
 	yy = 0 ;
      
      
      p(x,y) = (*this)(xx , yy) ;
    }
  }
}



void Tensor2d::save_text(const char * filename) {
  
  ofstream file(filename) ;

  file << setiosflags(ios::fixed) ;
  file << setprecision(10) ;

  for(int y = 0 ; y < len_y ; y++) {
    for(int x = 0 ; x < len_x ; x++) {
      file << (*this)(x,y) << " " ;
    }
    file << endl ;
  }

  file.close() ;
}
   


void Tensor2d::read_text(const char * filename, int lx, int ly)
{
   
   
  if(data)
    delete[] data ;
   
  data = new real[lx*ly] ;
   
  len_x = lx ;
  len_y = ly ;
   
  ifstream file(filename) ;
  int i = 0 ;
  for(int y = 0 ; y < len_y ; y++)
    {
	
	
      for(int x = 0 ; x < len_x ; x++) 
	{
	     
	     
	  file >> data[i++] ;
	}
	
	
    }
   
   
   
  file.close() ;
}





void Tensor2d::read(const char * file_name) {
  fstream in(file_name, ios::in | ios::binary) ;
  
  if(!in) {
    cerr << "Failure on reading " << file_name << endl ;
    exit(1) ;
  }

  in.read((char*)&len_x, sizeof(int)) ;
  in.read((char*)&len_y, sizeof(int)) ;

  int length = len_x * len_y ;

  if(data)
    delete[] data ;
 
  data = new real[length] ;
  
  in.read((char*)data, sizeof(real) * length) ;
  
  in.close() ;
}                                      

void Tensor2d::write(const char * file_name) {
  fstream out(file_name, ios::out | ios::binary) ;
  
  if(!out) {
    cerr << "Failure on writing " << file_name << endl ;
    exit(1) ;
  }

  out.write((char*)&len_x, sizeof(int)) ;
  out.write((char*)&len_y, sizeof(int)) ;

  int length = len_x * len_y ;
  
  out.write((char*)data, sizeof(real) * length) ;
  
  out.close() ;
}                                      

double Tensor2d::norm_max() {
  double norm = 0 ;

  int length = len_x * len_y ;
  
  for(int i = 0 ; i < length ; i++)
    if(fabs(data[i]) > norm)
      norm = fabs(data[i]) ;

  return norm ;

}
void Tensor2d::Pow(double c) {
int length = len_x * len_y ;
 
  for(int i = 0 ; i < length ; i++)
     data[i] = pow(data[i],c) ;
}



void Tensor2d::Pow2() {
int length = len_x * len_y ;
 
   double dat ;
  for(int i = 0 ; i < length ; i++)
     {
	dat = data[i] ;
     data[i] = dat * dat ;
     }
   
}



void Tensor2d::SPow(double c) {
int length = len_x * len_y ;
 
  for(int i = 0 ; i < length ; i++) 
     {
	double s = 1 ;
	if(data[i] < 0)
	  s = -1 ;
	
	data[i] = pow(fabs(data[i]),c) * s ;
     }
}


void Tensor2d::Abs() {
  int length = len_x * len_y ;
  
  for(int i = 0 ; i < length ; i++)
    if(data[i] < 0)
      data[i] = -data[i] ;
}

Tensor2d& Tensor2d::operator*=(Tensor2d& other) {
  int length = len_x * len_y ;
  
  for(int i = 0 ; i < length ; i++)
    data[i] *= other.data[i] ;
  return *this ;   
}

Tensor2d& Tensor2d::operator*=(real val) {
  int length = len_x * len_y ;
 
  for(int i = 0 ; i < length ; i++)
     data[i] *= val ;
  return *this ;  
}

Tensor2d& Tensor2d::operator+=(double s) {

  int length = len_x * len_y ;

  for(int i = 0 ; i < length ; i++)
    data[i] += s ;
  return *this ;
}

Tensor2d& Tensor2d::operator-=(double s) {

  int length = len_x * len_y ;

  for(int i = 0 ; i < length ; i++)
    data[i] -= s ;
  return *this ;
}


Tensor2d& Tensor2d::operator+=(Tensor2d& other) {

  int length = len_x * len_y ;

  if(len_x != other.len_x || len_y != other.len_y ) {
    cerr << "Vectors length mismatch! (op+)" << endl ;
    exit(1) ;
  }
  for(int i = 0 ; i < length ; i++)
    data[i] += other.data[i] ;
  return *this ;
}

Tensor2d& Tensor2d::operator-=(Tensor2d& other) {

  int length = len_x * len_y ;

  if(len_x != other.len_x || len_y != other.len_y ) {
    cerr << "Vectors length mismatch! (op-)" << endl ;
    exit(1) ;
  }
  for(int i = 0 ; i < length ; i++)
    data[i] -= other.data[i] ;

  return *this ;
}

double Tensor2d::norm_abs() {
  double norm = 0 ;

  int length = len_x * len_y ;
  
  for (int i = 0 ; i < length ; i++)
    norm += fabs(data[i]) ;

  return norm / length ;
}

double Tensor2d::norm_euc() {
  double norm = 0 ;

  int length = len_x * len_y ;
  
  for (int i = 0 ; i < length ; i++)
    norm += data[i] * data[i] ;

  return norm / length ;
}

real Tensor2d::min() 
{
   double min = 1e50 ;

   int length = len_x * len_y ;
   
   for(int i = 0 ; i < length ; i++)
     if(min > data[i])
       min = data[i] ;
   
   return min;
}

void Tensor2d::zero_one(double low, double high) 
{
   
   
   
int length = len_x * len_y ;
   double M = max() ;
   double m = min() ;
   
   double fac = (high - low) / (M - m) ; // * 0.999999 ;

   for(int i = 0 ; i < length ; i++) 
     data[i] = (data[i] - m) * fac + low ;
}


real Tensor2d::max() 
{
   double max = -1e50 ;

   int length = len_x * len_y ;
   
   for(int i = 0 ; i < length ; i++)
          if(max < data[i])
              max = data[i] ;
   
      return max;
     
}

real Tensor2d::mean() 
{
  double m = 0 ;

   int length = len_x * len_y ;
   
   for(int i = 0 ; i < length ; i++)
     m += data[i] ;
   
   return m / length ;
}

void Tensor2d::mean_var(double& m, double& v) 
{
   
   m = mean() ;
   
   v = 0 ;
   
   double w ;
   
   int length = len_x * len_y ;
   
   for(int i = 0 ; i < length ; i++) 
     {
	
	w = data[i] - m ;
	v += w * w ;
     }
   
   
   v /=  length ;
}



real Tensor2d::var() 
{

  double mn = mean() ;

  double v = 0 ;
  double w ;

   int length = len_x * len_y ;
   
   for(int i = 0 ; i < length ; i++) {
     w = data[i] - mn ;
     v += w * w ;
   }
   
   return v / length ;
}


void Tensor2d::resize(int factor, double* filter, int filter_len) {
  Tensor2d small = (*this) ;

  int nlx = len_x * factor ;
  int nly = len_y * factor ;

  set(nlx, nly) ;
  
  //int start = factor/2 ;

  for(int y = 0 ; y < nly ; y++) 
    for(int x = 0 ; x < nlx ; x++) 
      (*this)(x,y) = small(x/factor,y/factor) ;

  conv(filter, filter_len) ;
}

void Tensor2d::conv(double* filter, int filter_len) {

  Tensor2d large(len_x, len_y) ; 
  Tensor2d dense(len_x, len_y) ;

  dense.clear(0) ;
  large.clear(0) ;
  
  int filter_half = filter_len / 2 ;
  
  int xx ;

  for(int y = 0 ; y < len_y ; y++) 
    for(int x = 0 ; x < len_x ; x++) {
      for(int i = 0 ; i < filter_len ; i++) {
	xx = x+i-filter_half ;
	if(xx < 0 || xx >= len_x)
	  continue ;
	
	large(x,y) += (*this)(xx,y) * filter[i] ; 
	dense(x,y) += filter[i] ; 
      }
    }


  for(int y = 0 ; y < len_y ; y++) 
    for(int x = 0 ; x < len_x ; x++) {
      large(x,y) /= dense(x,y) ;
    }

  set(len_x, len_y) ;

  clear(0);
  dense.clear(0) ;

  int yy ;

  for(int y = 0 ; y < len_y ; y++) 
    for(int x = 0 ; x < len_x ; x++) {
      for(int i = 0 ; i < filter_len ; i++) {
	yy = y+i-filter_half ;
	if(yy < 0 || yy >= len_y)
	  continue ;
	
	(*this)(x,y) += large(x,yy) * filter[i] ; 
	dense(x,y) += filter[i] ; 
      }
    }

  for(int y = 0 ; y < len_y ; y++) 
    for(int x = 0 ; x < len_x ; x++) 
      (*this)(x,y) /= dense(x,y) ;

}





/*
void Tensor2d::resize(int factor, double* filter, int filter_len) {

  int nlx = len_x * factor ;
  int nly = len_y * factor ;

  Tensor2d large(nlx, nly) ;
  Tensor2d tlarge(nlx, nly) ; 
  Tensor2d dense(nlx, nly) ;
  Tensor2d tdense(nlx, nly) ;

  large.clear(0) ;
  tlarge.clear(0) ;
  tdense.clear(0) ;
  dense.clear(0) ;

  int start = factor/2 ;
  for(int y = 0 ; y < len_y ; y++) 
    for(int x = 0 ; x < len_x ; x++) {
      large(x*factor+start, y*factor+start) = (*this)(x,y) ;
      dense(x*factor+start, y*factor+start) = 1 ;
    }

  int filter_half = filter_len / 2 ;

  int xx ;
  large.show() ;
  for(int y = start ; y < nly ; y+=factor) 
    for(int x = 0 ; x < nlx ; x++) {
      for(int i = 0 ; i < filter_len ; i++) {
	xx = x+i-filter_half ;
	if(xx < 0 || xx >= nlx)
	  continue ;
	
	tlarge(x,y) += large(xx,y) * filter[i] ; 
	tdense(x,y) += dense(xx,y) * filter[i] ; 
      }
    }

  tlarge.show() ;

  for(int y = start ; y < nly ; y+=factor) 
    for(int x = 0 ; x < nlx ; x++) {
      tlarge(x,y) /= tdense(x,y) ;
      tdense(x,y) = 1 ;
    }

  set(nlx, nly) ;
  clear(0);

  dense.clear(0) ;

  int yy ;

  for(int y = 0 ; y < nly ; y++) 
    for(int x = 0 ; x < nlx ; x++) {
      for(int i = 0 ; i < filter_len ; i++) {
	yy = y+i-filter_half ;
	if(yy < 0 || yy >= nly)
	  continue ;
	
	(*this)(x,y) += tlarge(x,yy) * filter[i] ; 
	dense(x,y) += tdense(x,yy) * filter[i] ; 
      }
    }

  for(int y = 0 ; y < nly ; y++) 
    for(int x = 0 ; x < nlx ; x++) 
      (*this)(x,y) /= dense(x,y) ;
}
*/
};