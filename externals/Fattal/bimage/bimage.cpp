#include "bimage.hh"
#include <string.h>
#include <iostream>
#include <fstream>
// #include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

BImage::BImage(){
  _width = 0;
  _height = 0;
  _data = NULL ;
  _pdata = NULL ;
}

BImage::~BImage() {
  if(_data != NULL)
    delete[] _data;
}

BImage::BImage(const BImage& other){
  _width = 0;
  _height = 0;
  _pdata = NULL ;
  _data = NULL ;
  *this=other;
}

BImage::BImage(int width, int height) {
  _width = width;
  _height = height;
  _data = new unsigned char[_width * _height * DEPTH];
  _pdata = (Bpixel*)_data ;
}

BImage::BImage(const char * Image_name) {
  _width = 0;
  _height = 0;
  _pdata = NULL ;
  _data = NULL;
  readImage(Image_name);
}

void BImage::set(int width, int height) {
  _width = width ;
  _height = height ; 

  if(_data != NULL) {
    delete[] _data ;
    _data = NULL ;
  }

  _data = new unsigned char[_width * _height * DEPTH] ;
  _pdata = (Bpixel*)_data ;
}

void BImage::flip() {

  if(_data == NULL)
    return ;
  
  int len = _width * _height * 3 ;
  unsigned char* data = new unsigned char[len] ;

  for(int y = 0 ; y < _height ; y++) 
    for(int x = 0 ; x < _width ; x++) {
      int ind = (x + y * _width) * 3 ;
      int iind = (x + (_height - 1 - y) * _width) * 3 ;
      
      data[ind] = _data[iind + 2] ;
      data[ind+1] = _data[iind + 1] ;
      data[ind+2] = _data[iind] ;
    }


  delete[] _data ;

  _data = data ;
  _pdata = (Bpixel*)_data ;
}

int BImage::readImage(const char * Image_name) {
  ifstream f(Image_name,ios::binary);

  if(!f)
    return FAILURE;

  BITMAPFILEHEADER header1;
  BITMAPINFOHEADER header2;
  
  f.read((char*)&header1.bfType,sizeof(header1.bfType)) ;
  f.read((char*)&header1.bfSize,sizeof(header1.bfSize)) ;
  f.read((char*)&header1.bfReserved1,sizeof(header1.bfReserved1)) ;
  f.read((char*)&header1.bfReserved2,sizeof(header1.bfReserved2)) ;
  f.read((char*)&header1.bfOffBits,sizeof(header1.bfOffBits)) ;
 
  f.read((char*)&header2,sizeof(header2));
  
  // calculate the size (assuming 24 bits or 3 bytes per pixel).
  
  _height = header2.biHeight ;
  _width = header2.biWidth ;

  long size = _width * _height * 3 ;

  if(_data!=NULL)
    delete[] _data;
  
  _data = new unsigned char[size] ;
  _pdata = (Bpixel*)_data ;

  
  int extra = (_width * 3) % 4 ;
  
  if(extra > 0)
    extra = 4 - extra ;
  
  unsigned char gar[4] ;
  
  for(int i = 0 ; i < _height ; i++) {
    f.read((char*)&_data[i*_width*3], 3 * _width * sizeof(unsigned char));
    if(extra)
      f.read((char*)gar, extra * sizeof(unsigned char));
  }

  flip() ;
  
  f.close();
    
  return SUCCESS;
}

int BImage::writeSeq(const char * image_name, int num) {
  char nimage_name[128] ;
  
  sprintf(nimage_name, image_name, num) ;
  
  return writeImage(nimage_name) ;
}

int BImage::writeImage(const char * Image_name) {

  if(_data == NULL)
    return FAILURE;
  
  BITMAPFILEHEADER header1;
  BITMAPINFOHEADER header2;

  int extra = (_width * 3) % 4 ;
  
  if(extra > 0)
    extra = 4 - extra ;
	
  header1.bfType= 256 * 'M' + 'B' ;
  header1.bfSize= _width * _height * 3 + 54 + extra * _height ;
  header1.bfReserved1 = 0 ;
  header1.bfReserved2 = 0 ;	
  header1.bfOffBits = 54 ;	

  header2.biSize = 40 ;
  header2.biHeight = _height ;
  header2.biWidth = _width ;
  header2.biBitCount = 24 ;
  header2.biCompression = 0 ;
  header2.biSizeImage = _height * _width * 3 + extra * _height ;
  header2.biPlanes = 1 ;
  header2.biXPelsPerMeter = 5000 ;
  header2.biYPelsPerMeter = 5000 ;
  header2.biClrUsed = 0 ;
  header2.biClrImportant = 0 ;
  ofstream f(Image_name,ios::binary);

  f.write((char*)&header1.bfType,sizeof(header1.bfType)) ;
  f.write((char*)&header1.bfSize,sizeof(header1.bfSize)) ;
  f.write((char*)&header1.bfReserved1,sizeof(header1.bfReserved1)) ;
  f.write((char*)&header1.bfReserved2,sizeof(header1.bfReserved2)) ;
  f.write((char*)&header1.bfOffBits,sizeof(header1.bfOffBits)) ;
  
  f.write((char*)&header2,sizeof(header2));

  flip() ;

  unsigned char gar[4] ;
  
  for(int i = 0 ; i < 4 ; i++)
    gar[i] = 0 ;
  
  for(int i = 0 ; i < _height ; i++) {
    f.write((char*)&_data[i*_width*3], 3 * _width * sizeof(unsigned char));
    if(extra)
      f.write((char*)gar, extra * sizeof(unsigned char));
  }

  flip() ;
 
  f.close();
	
  return SUCCESS;
}

BImage& BImage::operator=(const BImage& other){
  _width = other._width;
  _height = other._height;
  
  if(_data!=NULL){
    delete[] _data;
    _pdata = NULL ;
    _data = NULL ;
  }
  
  if(other._data!=NULL) {
    long size = _width * _height * DEPTH;
    _data = new unsigned char[size];
    _pdata = (Bpixel*)_data ;
    
    for(long i=0;i<size;i++)
      _data[i] = other._data[i];
  }

  return *this;
}

void BImage::resize(int fact) {
  int new_width, new_height ;
  
  if(fact < 0) {
    fact = - fact ;
     
     new_width = _width / fact ;
     new_height = _height / fact ;

     BImage img(new_width, new_height) ;

     for(int y = 0 ; y < new_height ; y++) {
       for(int x = 0 ; x < new_width ; x++) {
	 double r = 0, g = 0, b = 0 ;
	 
	 for(int dy = 0 ; dy < fact ; dy++)
	   for(int dx = 0 ; dx < fact ; dx++) {
	  	     r += (*this)(x*fact+dx,y*fact+dy).r ;
	     g += (*this)(x*fact+dx,y*fact+dy).g ;
	     b += (*this)(x*fact+dx,y*fact+dy).b ;
	    
	   }

	 r /= (double)(fact * fact) ;
	 g /= (double)(fact * fact) ;
	 b /= (double)(fact * fact) ;
	 
	 img(x,y).r = (uchar)r ; img(x,y).g = (uchar)g ; img(x,y).b = (uchar)b ; 
       }
     }
     (*this) = img ;
  }
  else {
     new_width = _width * fact ;
     new_height = _height * fact ;
     
     BImage img(new_width, new_height) ;
     
     for(int y = 0 ; y < _height ; y++) {
       for(int x = 0 ; x < _width ; x++) {
	 
	 for(int dy = 0 ; dy < fact ; dy++)
	   for(int dx = 0 ; dx < fact ; dx++)
	     img(x * fact + dx, y * fact + dy) = (*this)(x,y) ;
       }
     }

     (*this) = img ;
  }

 
}

void BImage::resize(int w, int h) {
  
  BImage new_image(w,h) ;
  
  double fx = (double)_width / (double)w ;
  double fy = (double)_height / (double)h ;
  
  for(int y = 0 ; y < h ; y++) 
    for(int x = 0 ; x < w ; x++) 
      new_image(x,y) = (*this)(x * fx, y * fy) ;
	
  (*this) = new_image ;
}



void BImage::resize_half() {
  
  int nw = _width / 2 ;
  int nh = _height / 2 ;

  BImage new_image(nw,nh) ;
    
  for(int y = 0 ; y < nh ; y++) 
    for(int x = 0 ; x < nw ; x++) {
      new_image(x,y).r = (uchar)(0.25*((double)(*this)(x*2, y*2).r+(double)(*this)(x*2+1, y*2).r+
				       (double)(*this)(x*2, y*2+1).r+(double)(*this)(x*2+1, y*2+1).r)) ;
      new_image(x,y).g = (uchar)(0.25*((double)(*this)(x*2, y*2).g+(double)(*this)(x*2+1, y*2).g+
				       (double)(*this)(x*2, y*2+1).g+(double)(*this)(x*2+1, y*2+1).g)) ;
      new_image(x,y).b = (uchar)(0.25*((double)(*this)(x*2, y*2).b+(double)(*this)(x*2+1, y*2).b+
				       (double)(*this)(x*2, y*2+1).b+(double)(*this)(x*2+1, y*2+1).b)) ;
    }     
  (*this) = new_image ;
}


void BImage::clear(uchar r, uchar g, uchar b, uchar a) {
  if(_data==NULL)
    return;
  
  long size = _width * _height * DEPTH;

  for(long i=0;i<size;){
    _data[i++] = r;
    _data[i++] = g;
    _data[i++] = b;
    //    _data[i++] = a;
  }
}

uchar BImage::min() {
  uchar m = 255 ;
  int len = _width * _height * DEPTH ;
  
  for(int i = 0 ; i < len ; i++)
    if(_data[i] < m)
      m = _data[i] ;

  return m ;
}

uchar BImage::max() {
  uchar m = 0 ;
  int len = _width * _height * DEPTH ;
  
  for(int i = 0 ; i < len ; i++)
    if(_data[i] > m)
      m = _data[i] ;

  return m ;
}

void BImage::normalize(){
  if(_data==NULL)
    return;
  
  long size = _width * _height * DEPTH;

  double mmax = max() ;
  double mmin = min() ;
  
  if(mmax == mmin)
    return;
  
  double factor = 255.499 / (mmax-mmin) ;
  
  for(long i=0;i<size;i++) {
    _data[i] = (unsigned char)((_data[i]-mmin)*factor);
  }
}

void BImage::invert(){
  if(_data==NULL)
    return;
  
  long size = _width * _height * DEPTH;
  
  for(long i=0;i<size;i++) {
    _data[i] = 255 - _data[i] ;
  }
}


void BImage::blt(BImage& source, int x_src, int y_src,
		 int x_dst, int y_dst, int x_size, int y_size){
  if(_data==NULL)
    return;

  if(x_size == 0) {
    x_size = source.width() ;
    y_size = source.height() ;
  }

   int sx,sy,dx,dy ;
   int w = source.width() ;
   int h = source.height() ;
     
  for(int x=0;x<x_size;x++){
    for(int y=0;y<y_size;y++){
       sx = x_dst+x ;
       sy = y_dst+y ;
       dx = x_src+x ;
       dy = y_src+y ;
       
       if(sx < 0 || sy < 0 || sx >= _width || sy >= _height || dx < 0 || dy < 0 || dx >= w || dy >= h)
	 continue ;
       
      operator()(x_dst+x,y_dst+y)=source(x_src+x,y_src+y);
    }
  }
}


void BImage::blt(BImage& source, int x_dst, int y_dst) {
  if(_data==NULL)
    return;
  
  int w = source.width() ;
  int h = source.height() ;
  
  for(int x=0;x<w;x++){
    for(int y=0;y<h;y++){
      operator()(x_dst+x,y_dst+y) = source(x,y) ;
    }
  }
}

/*
void BImage::resize(int new_width, int new_height, FilterType ft, double blur) {
  if(_data==NULL)
    return ;

  ExceptionInfo ei ;
  
  GetExceptionInfo(&ei) ;
  
  Image *Image = ConstituteImage ( _width, _height, "RGB", CharPixel, _data, &ei) ;

  if(Image==NULL)
    return ;

  GetExceptionInfo(&ei) ;
  
  Image *resized_Image = ResizeImage (Image, new_width, new_height, (FilterTypes)ft, blur, &ei) ;
 
  if(_data)
    delete[] _data ;

  _width = new_width ;
  _height = new_height ;
  
  _data = new unsigned char[_width * _height * DEPTH] ;
  _pdata = (Bpixel*)_data ;
 
  if(_data == NULL)
    return ;

  PixelPacket * pp ;

  pp = GetImagePixels(resized_Image, 0, 0, resized_Image->columns, resized_Image->rows) ;

  for(int i = 0 ; i < _width * _height ; i++) {
    _data[i * DEPTH] = (pp[i].red >> 8) ;
    _data[i * DEPTH + 1] = (pp[i].green >> 8) ;
    _data[i * DEPTH + 2] = (pp[i].blue >> 8);
    //    _data[i * DEPTH + 3] = pp[i].opacity ;
  }
  
  DestroyImage(Image) ;
  DestroyImage(resized_Image) ;
}
*/  

void BImage::show() {
  char filename[20] ;
  char rand_name[9] ;

  for(int i = 0; i < 8 ; i++)
    rand_name[i] = (int)((double)rand() / RAND_MAX * 26) + 'a' ;
  rand_name[8] = 0 ;

  sprintf(filename,"/tmp/%s.bmp", rand_name) ;
  
  writeImage(filename) ;
  
  // if(fork() == 0) {
    //    if(fork() == 0)
     //execl("/usr/bin/kview", "kview", filename, NULL) ;
   //  execl("/usr/bin/qiv", "qiv -e", filename, NULL) ;
//      else {
//       sleep(400);
//       execl("/bin/rm", "rm", filename, NULL) ;
//     }
  //}  
}


void BImage::close() {
  // if(fork() == 0) {
  //  execl("/usr/bin/killall", "killall qiv -9", NULL, NULL) ;
  // }   
}


void BImage::text(int x, int y, char* str, int rr, int gg, int bb) {

  /*
  BImage font("/cs/phd/raananf/new/libs/bimage/ascii2.bmp") ;

  int i = 0 ;

  int col = 16 ;
  int w = 33 ;
  int h = 18 ;

  int lx = 7 ;
  int hx = 16 ;

  int ly = 1 ;
  int hy = 5 ;

  int cx = 45 ;
  int cy = 131 ;
  */
  BImage font("/home/raananf/libs/bimage/ascii4.bmp") ;

  int i = 0 ;

  int col = 16 ;
  int w = 72 ;
  int h = 18 ;

  int lx = 5 ;
  int hx = 58 ;

  int ly = 2 ;
  int hy = 2 ;

  int cx = 1 ;
  int cy = 1 ;
  
   uchar color ;
   
  while(str[i]!=0) {
    int yy = (str[i] - 32) % col ;
    int xx = ((str[i] - 32) - yy) / col ;

    for(int dy = ly ; dy < h-hy ; dy++)
      for(int dx = lx ; dx < w-hx ; dx++) {
	//	font(cx + xx * w + dx, cy + yy * h + dy).r = font(cx + xx * w + dx, cy + yy * h + dy).r / 2 ;

	if(font(cx + xx * w + dx, cy + yy * h + dy).r==0) {
	  int tx = x+dx+(w - hx-lx)*i ;
	  int ty = y+dy ;

	  if(tx < 0 || ty < 0 || tx >= _width || ty >= _height)
	    continue ;

	  if(rr == -1 && gg == -1 && bb == -1) {
	     
	     color = (*this)(x+dx+(w - hx-lx)*i, y+dy).r ;
	     
	     if(color > 128)
	       color = 0 ;
	     else
	       color = 255 ;
	     
	     (*this)(tx, ty).r = color ;
	     
	     color = (*this)(x+dx+(w - hx-lx)*i, y+dy).g ;
	     
	     if(color > 128)
	       color = 0 ;
	     else
	       color = 255 ;
	     
	     (*this)(tx, ty).g = color ;
	     
	     color = (*this)(x+dx+(w - hx-lx)*i, y+dy).b ;
	     
	     if(color > 128)
	       color = 0 ;
	     else
	       color = 255 ;
	     
	     (*this)(tx, ty).b = color ;
	  }
	  else {
	    (*this)(tx, ty).r = (uchar)rr ;
	    (*this)(tx, ty).g = (uchar)gg ;
	    (*this)(tx, ty).b = (uchar)bb ;
	  }
	}
	
	  //	  (*this)(x+dx+(w - hx-lx)*i, y+dy) = font(cx + xx * w + dx, cy + yy * h + dy) ;
        
      }

    i++ ;
  }

  //  font.show() ;
  
}
