#include "P0.h"
namespace Geex {
   P0::P0() : Function(2,0,2){}
   void P0::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double f0 = p(0);
f(0) = f0;
}
{
double f1 = p(1);
f(1) = f1;
}
      }
      if(do_g) {
      }
   }
}
