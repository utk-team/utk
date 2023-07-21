/**************************************************************/
/*                                                            */
/*   Class BIMAGE - Basic RGB image manipulations.            */
/*   Written by: Raanan Fattal                                */
/*                                                            */
/**************************************************************/

#ifndef _BIMAGE_HH
#define _BIMAGE_HH

#include <math.h>

#define DEPTH 3
#define FAILURE 0
#define SUCCESS 1

typedef unsigned char uchar;
typedef unsigned int uint;

class Bpixel {
 public:
  Bpixel() { r = g = b = 0 ; } ;
  Bpixel(uchar pr, uchar pg, uchar pb) { r = pr ; g = pg ; b = pb ; } ;
  Bpixel(uchar v) { r = g = b = v ; } ;
  Bpixel(const Bpixel& other) { r = other.r ; g = other.g ; b = other.b ; }
  Bpixel& operator=(const Bpixel& other) {  r = other.r ; g = other.g ; b = other.b ; return *this ; }

  Bpixel(Bpixel& p1, double w1,
	 Bpixel& p2, double w2,
	 Bpixel& p3, double w3,
	 Bpixel& p4, double w4) {
    r = (uchar)(p1.r * w1 + p2.r * w2 + p3.r * w3 + p4.r * w4 + 0.5) ;
    g = (uchar)(p1.g * w1 + p2.g * w2 + p3.g * w3 + p4.g * w4 + 0.5) ;
    b = (uchar)(p1.b * w1 + p2.b * w2 + p3.b * w3 + p4.b * w4 + 0.5) ;
  }
  
  double gray() { return 0.3333 * (r+g+b) ; }
  double Y() { return 0.299 * r + 0.587 * g + 0.114 * b ; }
  
  uchar r, g, b ;
} ;

class BImage {
public:
  BImage() ;
  ~BImage() ;
  BImage(const BImage& other) ;
  BImage(const char * image_name) ;
  BImage(int width, int height) ;

  BImage& operator=(const BImage& other) ;
  void set(int width, int height) ;
  
  int readImage(const char * image_name) ;
  int read(const char * image_name) { return readImage(image_name) ; }
  int load(const char * image_name) { return readImage(image_name) ; }
  
  int writeImage(const char * image_name) ;
  int writeSeq(const char * image_name, int num) ;
  int write(const char * image_name) { return writeImage(image_name) ; }
  int save(const char * image_name) { return writeImage(image_name) ; }
  
  uchar * getImageData() { return _data ; }

  int width() { return _width ; }
  int getWidth() { return _width ; }
  
  int height() { return _height ; }
  int getHeight() { return _height ; }

  inline void jet(int x, int y, double v) ;
  inline void hot(int x, int y, double v) ;
  inline Bpixel& operator()(int x, int y) ;  
  inline Bpixel& operator[](int i) { return _pdata[i] ; }      

  inline uchar& operator()(int x,int y, int col) ; //fast
  inline uchar& getPixel(int x,int y, int col) ; //checks bounds
  inline uchar operator()(double x, double y, int col) ;
  inline Bpixel operator()(double x, double y) ;

  void text(int x, int y, char* str, int rr=-1, int gg=-1, int bb=-1) ;
  
  //  void resize(int new_width, int new_height, FilterType ft = FBox, double blur = 1.0) ;

  void resize(int fact) ;
  void resize(int w, int h) ;
  void resize_half() ; 

  void show() ;
  void close() ;
  
  void blt(BImage& source, int x_dst, int y_dst) ;
  void blt(BImage& source, int x_src, int y_src, int x_dst, int y_dst, int x_size=0, int y_size=0) ;
  
  void clear(uchar r=0, uchar g=0, uchar b=0, uchar a=255) ;
  void normalize() ;
  void invert() ;

  uchar max() ;
  uchar min() ;
 
private:
  int _width, _height ;
  uchar * _data ;
  Bpixel * _pdata ;

  void flip() ;
   
};



typedef unsigned int       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef long LONG;

typedef struct tagBITMAPFILEHEADER { 
  WORD    bfType; 
  DWORD   bfSize; 
  WORD    bfReserved1; 
  WORD    bfReserved2; 
  DWORD   bfOffBits;
} BITMAPFILEHEADER ;

typedef struct tagBITMAPINFOHEADER{
  DWORD  biSize;
  DWORD  biWidth;
  DWORD  biHeight;
  WORD   biPlanes;
  WORD   biBitCount;
  DWORD  biCompression;
  DWORD  biSizeImage;
  DWORD  biXPelsPerMeter;
  DWORD  biYPelsPerMeter;
  DWORD  biClrUsed;
  DWORD  biClrImportant;

} BITMAPINFOHEADER;



inline unsigned char& BImage::operator()(int x,int y, int col) {
  return _data[(x + y * _width) * DEPTH + col];
}

inline Bpixel& BImage::operator()(int x,int y) {
  return _pdata[(x + y * _width)] ;
}

inline uchar BImage::operator()(double x, double y, int col) {
  int ix = (int)x ;
  int iy = (int)y ;
  double dx = x - ix ;
  double dy = y - iy ;
	   
  return (uchar)(operator()(ix, iy, col) * (1-dx) * (1-dy) + operator()(ix + 1, iy, col) * dx * (1-dy) +
         operator()(ix, iy+1, col) * (1-dx) * dy + operator()(ix + 1, iy + 1, col) * dx * dy + 0.5) ;
}

inline Bpixel BImage::operator()(double x, double y) {
  int ix = (int)x ;
  int iy = (int)y ;
  double dx = x - ix ;
  double dy = y - iy ;
	   
  return Bpixel(_pdata[ix+iy*_width], (1-dx) * (1-dy),
		_pdata[ix+iy*_width+1], (dx) * (1-dy),
		_pdata[ix+(iy+1)*_width], (1-dx) * (dy),
		_pdata[ix+(iy+1)*_width+1], (dx) * (dy)) ; 
}

inline unsigned char& BImage::getPixel(int x,int y, int col) {  
  if(x<0)
    x=0;
  if(y<0)
    y=0;
  if(x>=_width)
    x=_width-1;
  if(y>=_height)
    y=_height-1;
  
  return _data[(x + y * _width) * DEPTH + col];
}

inline void BImage::jet(int x, int y, double v) {
  double r = 0, g = 0, b = 0 ;
  double s = 4 ;
  double t = 1.5 ; 
  
  r = t - fabs(v - 0.75) * s ;
  g = t - fabs(v - 0.5) * s ;  
  b = t - fabs(v - 0.25) * s ;   
  
  if(r < 0) r = 0 ;
  if(g < 0) g = 0 ;    
  if(b < 0) b = 0 ;    
  if(r > 1.0) r = 1.0 ;
  if(g > 1.0) g = 1.0 ;      
  if(b > 1.0) b = 1.0 ;      
  
  
  _data[(x + y * _width) * DEPTH] = (uchar)(r * 255.0) ;     
  _data[(x + y * _width) * DEPTH + 1] = (uchar)(g * 255.0) ;            
  _data[(x + y * _width) * DEPTH + 2] = (uchar)(b * 255.0) ;   
}

inline void BImage::hot(int x, int y, double v) {
  v *=3 ;

  double r = 0, g = 0, b = 0 ;
  
  if(v < 1)
    r = v ;
  else
    if(v < 2) {
      r = 1 ;
      g = v - 1 ;
    }
    else {
      r = g = 1 ;
      b = v ;
    }
  
  _data[(x + y * _width) * DEPTH] = (uchar)(r * 255.0) ;     
  _data[(x + y * _width) * DEPTH + 1] = (uchar)(g * 255.0) ;            
  _data[(x + y * _width) * DEPTH + 2] = (uchar)(b * 255.0) ;   
}

#endif /* _BIMAGE_HH */
