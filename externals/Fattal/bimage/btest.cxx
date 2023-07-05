#include "bimage.hh"
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){

  
  BImage image(257,256) ;
  image.clear(255,255,255) ;

  image.text(10,10,"abc ABC 123 @$%& ][{}.,:") ; // fghijklmnopquvwxyz") ;

  for(int i = 0 ; i < 16 ; i++) {
    for(int j = 2 ; j < 8 ; j++)
      cout << (char)(j*16+i) << "\t" ;
    cout << endl ;
  }

  image.writeImage("a.bmp") ;
  image.show() ;

  //image.close() ;

  
 return 0 ;
}
