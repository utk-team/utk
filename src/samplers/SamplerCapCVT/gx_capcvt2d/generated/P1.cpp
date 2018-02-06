#include "P1.h"
namespace Geex {
   P1::P1() : Function(2,4,3){}
   void P1::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_7 = x(2);
double tmp_8 = x(0);
double tmp_10 = -tmp_8;
double tmp_11 = tmp_7+tmp_10;
double tmp_14 = p(1);
double f0 = -(1.0/2.0)*1.0/(tmp_11*tmp_14+p(0)*tmp_4)*(2.0*p(2)*tmp_4-((tmp_8+tmp_7)*tmp_11-tmp_4*(tmp_2+tmp_1))*tmp_14);
f(0) = f0;
}
{
double tmp_2 = x(2);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_7 = p(0);
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double f1 = -(1.0/2.0)*((tmp_5*(tmp_2+tmp_3)-tmp_11*(tmp_8+tmp_9))*tmp_7+2.0*tmp_5*p(2))/(tmp_5*p(1)+tmp_11*tmp_7);
f(1) = f1;
}
      }
      if(do_g) {
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_7 = x(2);
double tmp_8 = x(0);
double tmp_10 = -tmp_8;
double tmp_11 = tmp_7+tmp_10;
double tmp_14 = p(1);
double tmp_19 = tmp_11*tmp_14;
double tmp_20 = p(0);
double tmp_21 = tmp_20*tmp_4;
double tmp_22 = tmp_21+tmp_19;
double tmp_23 = 1.0/(tmp_22);
double g0_0 = -tmp_14*(tmp_23*tmp_23)*(2.0*tmp_4*p(2)-tmp_14*((tmp_8+tmp_7)*tmp_11-(tmp_2+tmp_1)*tmp_4))/2.0-tmp_8*tmp_14*tmp_23;
g(0,0) = g0_0;
}
{
double tmp_1 = p(1);
double tmp_2 = x(2);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_4+tmp_2;
double tmp_6 = tmp_1*tmp_5;
double tmp_7 = p(0);
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_16 = p(2);
double g0_1 = -1.0/(tmp_6+tmp_12)*(tmp_1*tmp_8+tmp_16)+tmp_7/pow(tmp_6+tmp_12,2.0)*(2.0*tmp_11*tmp_16+tmp_1*(tmp_11*(tmp_8+tmp_9)-(tmp_3+tmp_2)*tmp_5))/2.0;
g(0,1) = g0_1;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_7 = x(2);
double tmp_8 = x(0);
double tmp_10 = -tmp_8;
double tmp_11 = tmp_10+tmp_7;
double tmp_14 = p(1);
double tmp_19 = tmp_11*tmp_14;
double tmp_20 = p(0);
double tmp_21 = tmp_4*tmp_20;
double tmp_22 = tmp_19+tmp_21;
double tmp_23 = 1.0/(tmp_22);
double g0_2 = tmp_7*tmp_14*tmp_23+((tmp_4*(tmp_1+tmp_2)-tmp_11*(tmp_8+tmp_7))*tmp_14+2.0*tmp_4*p(2))*tmp_14*(tmp_23*tmp_23)/2.0;
g(0,2) = g0_2;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_7 = x(2);
double tmp_8 = x(0);
double tmp_10 = -tmp_8;
double tmp_11 = tmp_7+tmp_10;
double tmp_14 = p(1);
double tmp_16 = p(2);
double tmp_19 = tmp_11*tmp_14;
double tmp_20 = p(0);
double tmp_21 = tmp_4*tmp_20;
double tmp_22 = tmp_21+tmp_19;
double tmp_23 = 1.0/(tmp_22);
double g0_3 = (tmp_2*tmp_14+tmp_16)*tmp_23+tmp_20*(tmp_23*tmp_23)*((tmp_11*(tmp_8+tmp_7)-tmp_4*(tmp_2+tmp_1))*tmp_14-2.0*tmp_4*tmp_16)/2.0;
g(0,3) = g0_3;
}
{
double tmp_1 = p(1);
double tmp_2 = x(2);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = tmp_5*tmp_1;
double tmp_7 = p(0);
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = 1.0/(tmp_13);
double tmp_16 = p(2);
double g1_0 = -(2.0*tmp_5*tmp_16+tmp_7*(tmp_5*(tmp_2+tmp_3)-(tmp_8+tmp_9)*tmp_11))*(tmp_14*tmp_14)*tmp_1/2.0+tmp_14*(tmp_7*tmp_3+tmp_16);
g(1,0) = g1_0;
}
{
double tmp_1 = x(1);
double tmp_2 = p(1);
double tmp_3 = x(2);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = p(0);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_1+tmp_10;
double tmp_12 = tmp_8*tmp_11;
double g1_1 = (2.0*tmp_6*p(2)+(tmp_6*(tmp_4+tmp_3)-(tmp_1+tmp_9)*tmp_11)*tmp_8)/pow(tmp_7+tmp_12,2.0)*tmp_8/2.0+tmp_1/(tmp_7+tmp_12)*tmp_8;
g(1,1) = g1_1;
}
{
double tmp_1 = p(1);
double tmp_2 = x(2);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = tmp_5*tmp_1;
double tmp_7 = p(0);
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = 1.0/(tmp_13);
double tmp_16 = p(2);
double g1_2 = -(tmp_2*tmp_7+tmp_16)*tmp_14-((tmp_11*(tmp_9+tmp_8)-tmp_5*(tmp_3+tmp_2))*tmp_7-2.0*tmp_5*tmp_16)*(tmp_14*tmp_14)*tmp_1/2.0;
g(1,2) = g1_2;
}
{
double tmp_1 = p(1);
double tmp_2 = x(2);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = tmp_5*tmp_1;
double tmp_7 = p(0);
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double g1_3 = tmp_7*(tmp_7*(tmp_11*(tmp_8+tmp_9)-tmp_5*(tmp_2+tmp_3))-2.0*tmp_5*p(2))/pow(tmp_6+tmp_12,2.0)/2.0-tmp_7/(tmp_6+tmp_12)*tmp_9;
g(1,3) = g1_3;
}
      }
   }
}
