#ifndef FMG_HH
#define FMG_HH

#include "tensor2d.h"
#include <math.h>
#include <stdio.h>

class TensorField {
public:
  
  int get_len_x() { return _len_x ; }
  int get_len_y() { return _len_y ; }
  int get_depth() { return _depth ; }
  
  int level_len_x(int level) { return (int)((_len_x - 1) / pow(2, level) + 1) ; }
  int level_len_y(int level) { return (int)((_len_y - 1) / pow(2, level) + 1) ; }
  
  virtual void smooth(int level) = 0 ;

  virtual void set_init_guess(void) = 0 ;
 
  virtual void calc_next_level_residual(int level) = 0 ;
  virtual double residual() = 0 ;
  
  virtual void zero_next_level(int level) = 0 ;
  virtual void add_prolonged_prev_level(int level) = 0 ;
  virtual void advance_in_time() = 0 ;
  virtual ~TensorField() { }
  
 protected:

  int _len_x ;
  int _len_y ;
  int _depth ;
} ;

void Field_TMG(TensorField *tf, int iters, int ncycle) ;
void Field_FMG(TensorField *tf, int iters, int ncycle) ;
void Field_MG(TensorField *tf, int iters, int ncycle) ;
void Field_MGN(TensorField *tf, int iters, int ncycle) ;

#endif /* FMG_HH */
