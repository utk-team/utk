#include "P3.h"
namespace Geex {
   P3::P3() : Function(2,6,0){}
   void P3::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = x(0);
double tmp_2 = x(2);
double tmp_3 = -tmp_1;
double tmp_4 = tmp_3+tmp_2;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_13+tmp_3;
double f0 = tmp_1-1.0/(-2.0000000000000000e+00*tmp_8*tmp_4+2.0000000000000000e+00*tmp_14*tmp_12)*(tmp_8*((tmp_12*tmp_12)+(tmp_4*tmp_4))-((tmp_14*tmp_14)+(tmp_8*tmp_8))*tmp_12);
f(0) = f0;
}
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_13+tmp_3;
double f1 = tmp_5-(tmp_14*((tmp_12*tmp_12)+(tmp_4*tmp_4))-tmp_4*((tmp_14*tmp_14)+(tmp_8*tmp_8)))/(-2.0000000000000000e+00*tmp_8*tmp_4+2.0000000000000000e+00*tmp_14*tmp_12);
f(1) = f1;
}
      }
      if(do_g) {
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_9 = -2.0000000000000000e+00*tmp_4*tmp_8;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_13+tmp_3;
double tmp_15 = 2.0000000000000000e+00*tmp_14*tmp_12;
double g0_0 = (((tmp_4*tmp_4)+(tmp_12*tmp_12))*tmp_8-tmp_12*((tmp_8*tmp_8)+(tmp_14*tmp_14)))/pow(tmp_9+tmp_15,2.0)*(2.0000000000000000e+00*tmp_10-2.0000000000000000e+00*tmp_6)+2.0*1.0/(tmp_9+tmp_15)*(tmp_4*tmp_8-tmp_14*tmp_12)+1.0;
g(0,0) = g0_0;
}
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_3+tmp_13;
double tmp_15 = 2.0000000000000000e+00*tmp_12*tmp_14;
double tmp_16 = tmp_9+tmp_15;
double tmp_17 = 1.0/(tmp_16);
double g0_1 = (-2.0000000000000000e+00*tmp_1+2.0000000000000000e+00*tmp_13)*(tmp_8*((tmp_4*tmp_4)+(tmp_12*tmp_12))-((tmp_8*tmp_8)+(tmp_14*tmp_14))*tmp_12)*(tmp_17*tmp_17)+tmp_17*((tmp_8*tmp_8)-(tmp_4*tmp_4)-(tmp_12*tmp_12)+(tmp_14*tmp_14));
g(0,1) = g0_1;
}
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_3+tmp_13;
double tmp_15 = 2.0000000000000000e+00*tmp_12*tmp_14;
double g0_2 = 1.0/pow(tmp_9+tmp_15,2.0)*(2.0000000000000000e+00*tmp_6-2.0000000000000000e+00*tmp_5)*(tmp_8*((tmp_12*tmp_12)+(tmp_4*tmp_4))-tmp_12*((tmp_14*tmp_14)+(tmp_8*tmp_8)))-2.0*1.0/(tmp_9+tmp_15)*tmp_8*tmp_4;
g(0,2) = g0_2;
}
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_3+tmp_13;
double tmp_15 = 2.0000000000000000e+00*tmp_14*tmp_12;
double tmp_16 = tmp_9+tmp_15;
double tmp_17 = 1.0/(tmp_16);
double g0_3 = -(tmp_17*tmp_17)*(2.0000000000000000e+00*tmp_2-2.0000000000000000e+00*tmp_13)*(((tmp_14*tmp_14)+(tmp_8*tmp_8))*tmp_12-tmp_8*((tmp_4*tmp_4)+(tmp_12*tmp_12)))-tmp_17*((tmp_14*tmp_14)+(tmp_8*tmp_8)-2.0*tmp_8*tmp_12);
g(0,3) = g0_3;
}
{
double tmp_1 = x(1);
double tmp_3 = x(3);
double tmp_6 = x(2);
double tmp_7 = x(0);
double tmp_8 = -tmp_7;
double tmp_9 = tmp_6+tmp_8;
double tmp_10 = x(5);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_1+tmp_11;
double tmp_13 = -2.0000000000000000e+00*tmp_9*tmp_12;
double tmp_14 = -tmp_3;
double tmp_15 = tmp_1+tmp_14;
double tmp_16 = x(4);
double tmp_17 = tmp_16+tmp_8;
double tmp_18 = 2.0000000000000000e+00*tmp_15*tmp_17;
double tmp_19 = tmp_13+tmp_18;
double tmp_20 = 1.0/(tmp_19);
double g0_4 = 2.0*tmp_15*tmp_17*tmp_20+(2.0000000000000000e+00*tmp_1-2.0000000000000000e+00*tmp_3)*(tmp_12*((tmp_15*tmp_15)+(tmp_9*tmp_9))-tmp_15*((tmp_17*tmp_17)+(tmp_12*tmp_12)))*(tmp_20*tmp_20);
g(0,4) = g0_4;
}
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_5;
double tmp_13 = x(4);
double tmp_14 = tmp_3+tmp_13;
double tmp_15 = 2.0000000000000000e+00*tmp_12*tmp_14;
double tmp_16 = tmp_15+tmp_9;
double tmp_17 = 1.0/(tmp_16);
double g0_5 = -(2.0*tmp_12*tmp_8-(tmp_4*tmp_4)-(tmp_12*tmp_12))*tmp_17+(-2.0000000000000000e+00*tmp_2+2.0000000000000000e+00*tmp_1)*(tmp_17*tmp_17)*(tmp_8*((tmp_4*tmp_4)+(tmp_12*tmp_12))-((tmp_14*tmp_14)+(tmp_8*tmp_8))*tmp_12);
g(0,5) = g0_5;
}
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_3+tmp_13;
double tmp_15 = 2.0000000000000000e+00*tmp_12*tmp_14;
double tmp_16 = tmp_9+tmp_15;
double tmp_17 = 1.0/(tmp_16);
double g1_0 = (((tmp_12*tmp_12)+(tmp_4*tmp_4))*tmp_14-((tmp_14*tmp_14)+(tmp_8*tmp_8))*tmp_4)*(2.0000000000000000e+00*tmp_10-2.0000000000000000e+00*tmp_6)*(tmp_17*tmp_17)+((tmp_12*tmp_12)+(tmp_4*tmp_4)-(tmp_14*tmp_14)-(tmp_8*tmp_8))*tmp_17;
g(1,0) = g1_0;
}
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_5;
double tmp_13 = x(4);
double tmp_14 = tmp_3+tmp_13;
double g1_1 = -1.0/pow(2.0000000000000000e+00*tmp_12*tmp_14-2.0000000000000000e+00*tmp_8*tmp_4,2.0)*(-2.0000000000000000e+00*tmp_1+2.0000000000000000e+00*tmp_13)*(((tmp_8*tmp_8)+(tmp_14*tmp_14))*tmp_4-((tmp_4*tmp_4)+(tmp_12*tmp_12))*tmp_14);
g(1,1) = g1_1;
}
{
double tmp_1 = x(1);
double tmp_2 = x(5);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(4);
double tmp_6 = x(0);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_9 = x(2);
double tmp_10 = tmp_7+tmp_9;
double tmp_13 = -2.0000000000000000e+00*tmp_4*tmp_10;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_1+tmp_15;
double tmp_17 = 2.0000000000000000e+00*tmp_16*tmp_8;
double g1_2 = (((tmp_10*tmp_10)+(tmp_16*tmp_16))*tmp_8-((tmp_8*tmp_8)+(tmp_4*tmp_4))*tmp_10)*(-2.0000000000000000e+00*tmp_1+2.0000000000000000e+00*tmp_2)/pow(tmp_17+tmp_13,2.0)+1.0/(tmp_17+tmp_13)*((tmp_8*tmp_8)-2.0*tmp_10*tmp_8+(tmp_4*tmp_4));
g(1,2) = g1_2;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(2);
double tmp_6 = x(0);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = x(5);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_1+tmp_10;
double tmp_12 = -2.0000000000000000e+00*tmp_8*tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = 2.0000000000000000e+00*tmp_14*tmp_4;
double g1_3 = -(tmp_8*((tmp_11*tmp_11)+(tmp_14*tmp_14))-tmp_14*((tmp_8*tmp_8)+(tmp_4*tmp_4)))/pow(tmp_12+tmp_15,2.0)*(-2.0000000000000000e+00*tmp_13+2.0000000000000000e+00*tmp_6)+2.0*tmp_14*tmp_4/(tmp_12+tmp_15);
g(1,3) = g1_3;
}
{
double tmp_1 = x(4);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(2);
double tmp_6 = tmp_3+tmp_5;
double tmp_9 = x(1);
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_9+tmp_11;
double tmp_15 = x(5);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_16+tmp_9;
double tmp_18 = -2.0000000000000000e+00*tmp_17*tmp_6;
double tmp_19 = 2.0000000000000000e+00*tmp_4*tmp_12;
double g1_4 = (2.0*tmp_4*tmp_6-(tmp_6*tmp_6)-(tmp_12*tmp_12))/(tmp_19+tmp_18)+(tmp_4*((tmp_6*tmp_6)+(tmp_12*tmp_12))-tmp_6*((tmp_17*tmp_17)+(tmp_4*tmp_4)))/pow(tmp_19+tmp_18,2.0)*(-2.0000000000000000e+00*tmp_10+2.0000000000000000e+00*tmp_9);
g(1,4) = g1_4;
}
{
double tmp_1 = x(2);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(5);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(3);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(4);
double tmp_14 = tmp_13+tmp_3;
double tmp_15 = 2.0000000000000000e+00*tmp_12*tmp_14;
double g1_5 = -2.0*1.0/(tmp_9+tmp_15)*tmp_8*tmp_4-(((tmp_8*tmp_8)+(tmp_14*tmp_14))*tmp_4-tmp_14*((tmp_4*tmp_4)+(tmp_12*tmp_12)))/pow(tmp_9+tmp_15,2.0)*(-2.0000000000000000e+00*tmp_2+2.0000000000000000e+00*tmp_1);
g(1,5) = g1_5;
}
      }
   }
}
