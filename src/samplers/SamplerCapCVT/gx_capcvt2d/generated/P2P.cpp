#include "P2P.h"
namespace Geex {
   P2P::P2P() : Function(2,2,0){}
   void P2P::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double f0 = x(0);
f(0) = f0;
}
{
double f1 = x(1);
f(1) = f1;
}
      }
      if(do_g) {
{
double g0_0 = 1.0;
g(0,0) = g0_0;
}
{
double g0_1 = 0.0;
g(0,1) = g0_1;
}
{
double g1_0 = 0.0;
g(1,0) = g1_0;
}
{
double g1_1 = 1.0;
g(1,1) = g1_1;
}
      }
   }
}
