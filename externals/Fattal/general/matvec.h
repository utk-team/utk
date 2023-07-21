#ifndef MATVEC
#define MATVEC

#include <iostream.h>

class MatVec {
 public:
  MatVec(int n) {
    _n = n ;
    _mat = new double[_n * _n] ;
    _imat = new double[_n * _n] ;
    _vec = new double[_n] ;

    for(int i = 0 ; i < _n * _n ; i++)
      _imat[i] = _mat[i] = 0 ;

    for(int i = 0 ; i < _n ; i++) {
      _vec[i] = 0 ;
      _imat[i + i * _n] = 1 ;
    }
  }

  ~MatVec() { if(_mat) delete[] _mat ; if(_imat) delete[] _imat ; if(_vec) delete[] _vec ; }

  double& operator()(int j, int i) { return _mat[i+j*_n] ; }
  double& operator()(int i) { return _vec[i] ; }
  double& inv(int j, int i) { return _imat[i+j*_n] ; }
  
  int solve() {
    for(int i = 0 ; i < _n ; i++) {
      int j = i ;

      while(fabs(_mat[i+j*_n]) < 1e-10 && j < _n)
	j++ ;

      if(j == _n) {
	cerr << "Matrix uninvertable! " << i << endl ;
	//	print() ;
	return 0 ;
      }
      
      if(j > i)
	subs(i,j) ;

      div(i, _mat[i+i*_n]) ;

      for(j = 0 ; j < _n ; j++) {
	if(i == j)
	  continue ;
	subdiv(i,j) ;
      }
    }
    return 1 ;
  }

  void div(int i, double v) {
    _vec[i] /= v ;
    for(int k = 0 ;  k < _n ; k++) {
      _mat[k + i * _n] /= v ;
      _imat[k + i * _n] /= v ;
    }
  }

  void subs(int i, int j) {
    double t ;

    for(int k = 0 ;  k < _n ; k++) {
      t = _mat[k + i * _n] ;
      _mat[k + i * _n] = _mat[k + j * _n] ;
      _mat[k + j * _n] = t ;

      t = _imat[k + i * _n] ;
      _imat[k + i * _n] = _imat[k + j * _n] ;
      _imat[k + j * _n] = t ;
    }
    
    t = _vec[i] ;
    _vec[i] = _vec[j] ;
    _vec[j] = t ;
  }
  
  void subdiv(int i, int j) {
    double f = _mat[i + j * _n] / _mat[i + i * _n] ;
    
    for(int k = 0 ;  k < _n ; k++) {
      _mat[k + j * _n] -= f * _mat[k + i * _n] ;
      _imat[k + j * _n] -= f * _imat[k + i * _n] ;
    }
    
    _vec[j] -= f * _vec[i] ;
  }

  void print() {
    for(int j = 0 ; j < _n ; j++) {
      for(int i = 0 ; i < _n ; i++)
	cout << _mat[i + j * _n] << " " ;
      
      cout << "  " << _vec[j] << endl ;
    }
    cout << "         --------------------------- \n and inv: \n" << endl ;

    for(int j = 0 ; j < _n ; j++) {
      for(int i = 0 ; i < _n ; i++)
	cout << _imat[i + j * _n] << " " ;
      cout << endl ;
    }
    cout << "         ---------------------------" << endl ;
  }
  
 private:
  int _n ;
  double *_mat ;
  double *_imat ;
  double *_vec ;
  
} ;



#endif
