#include "L.h"
namespace Geex {
   L::L() : Function(1,6,0){}
   void L::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(3);
double tmp_6 = tmp_3+tmp_5;
double tmp_8 = x(0);
double tmp_9 = x(2);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = x(4);
double tmp_13 = -tmp_8;
double tmp_14 = tmp_13+tmp_12;
double f0 = -((tmp_6*tmp_6)+tmp_4*tmp_6-tmp_11*tmp_14+(tmp_14*tmp_14)+(tmp_11*tmp_11)+(tmp_4*tmp_4))*(tmp_4*tmp_11+tmp_6*tmp_14);
f(0) = f0;
}
      }
      if(do_g) {
{
double tmp_1 = x(4);
double tmp_2 = x(0);
double tmp_4 = x(2);
double tmp_6 = x(3);
double tmp_7 = x(1);
double tmp_8 = -tmp_7;
double tmp_9 = tmp_6+tmp_8;
double tmp_10 = -tmp_2;
double tmp_11 = tmp_10+tmp_1;
double tmp_13 = -tmp_4;
double tmp_14 = tmp_13+tmp_2;
double tmp_15 = x(5);
double tmp_16 = tmp_15+tmp_8;
double g0_0 = (tmp_6-tmp_15)*((tmp_14*tmp_14)+(tmp_9*tmp_9)+(tmp_16*tmp_16)+tmp_16*tmp_9+(tmp_11*tmp_11)-tmp_11*tmp_14)-3.0*(tmp_9*tmp_11+tmp_16*tmp_14)*(2.0*tmp_2-tmp_1-tmp_4);
g(0,0) = g0_0;
}
{
double tmp_1 = x(3);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(4);
double tmp_6 = x(0);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_6+tmp_11;
double tmp_13 = x(5);
double tmp_14 = tmp_3+tmp_13;
double g0_1 = -3.0*(tmp_8*tmp_4+tmp_14*tmp_12)*(2.0*tmp_2-tmp_1-tmp_13)+(tmp_11+tmp_5)*((tmp_12*tmp_12)+(tmp_14*tmp_14)-tmp_8*tmp_12+(tmp_4*tmp_4)+tmp_14*tmp_4+(tmp_8*tmp_8));
g(0,1) = g0_1;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(3);
double tmp_6 = tmp_5+tmp_3;
double tmp_8 = x(0);
double tmp_9 = x(2);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = x(4);
double tmp_13 = -tmp_8;
double tmp_14 = tmp_12+tmp_13;
double g0_2 = ((tmp_6*tmp_6)+tmp_4*tmp_6+(tmp_11*tmp_11)-tmp_11*tmp_14+(tmp_14*tmp_14)+(tmp_4*tmp_4))*tmp_4-(2.0*tmp_9+tmp_12-3.0*tmp_8)*(tmp_11*tmp_4+tmp_14*tmp_6);
g(0,2) = g0_2;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(3);
double tmp_6 = tmp_3+tmp_5;
double tmp_8 = x(0);
double tmp_9 = x(2);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = x(4);
double tmp_13 = -tmp_8;
double tmp_14 = tmp_13+tmp_12;
double g0_3 = (tmp_11*tmp_4+tmp_6*tmp_14)*(3.0*tmp_2-2.0*tmp_5-tmp_1)+tmp_14*(tmp_11*tmp_14-(tmp_4*tmp_4)-(tmp_6*tmp_6)-tmp_6*tmp_4-(tmp_14*tmp_14)-(tmp_11*tmp_11));
g(0,3) = g0_3;
}
{
double tmp_1 = x(3);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(4);
double tmp_6 = x(0);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_6+tmp_11;
double tmp_13 = x(5);
double tmp_14 = tmp_3+tmp_13;
double g0_4 = -((tmp_8*tmp_8)+(tmp_4*tmp_4)+(tmp_12*tmp_12)+(tmp_14*tmp_14)-tmp_12*tmp_8+tmp_14*tmp_4)*tmp_4+(tmp_12*tmp_14+tmp_8*tmp_4)*(3.0*tmp_6-2.0*tmp_5-tmp_10);
g(0,4) = g0_4;
}
{
double tmp_1 = x(3);
double tmp_2 = x(5);
double tmp_4 = x(1);
double tmp_7 = -tmp_4;
double tmp_8 = tmp_7+tmp_1;
double tmp_9 = x(4);
double tmp_10 = x(0);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_9+tmp_11;
double tmp_14 = x(2);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_10+tmp_15;
double tmp_17 = tmp_7+tmp_2;
double g0_5 = -(tmp_16*tmp_17+tmp_12*tmp_8)*(2.0*tmp_2+tmp_1-3.0*tmp_4)-tmp_16*((tmp_17*tmp_17)+(tmp_12*tmp_12)+(tmp_8*tmp_8)+tmp_8*tmp_17+(tmp_16*tmp_16)-tmp_16*tmp_12);
g(0,5) = g0_5;
}
      }
   }
}
