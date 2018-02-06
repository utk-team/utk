#include "Ltheta2.h"
namespace Geex {
   Ltheta2::Ltheta2() : Function(1,7,0){}
   void Ltheta2::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(4);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = sin(tmp_1);
double tmp_9 = x(1);
double tmp_10 = x(5);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_9+tmp_11;
double tmp_13 = tmp_12*tmp_8;
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = x(3);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_9+tmp_16;
double tmp_18 = tmp_2*tmp_17;
double tmp_19 = x(2);
double tmp_20 = -tmp_19;
double tmp_21 = tmp_20+tmp_4;
double tmp_22 = tmp_21*tmp_8;
double tmp_23 = tmp_22+tmp_18;
double tmp_25 = tmp_6*tmp_8;
double tmp_26 = -tmp_12*tmp_2;
double tmp_27 = tmp_25+tmp_26;
double tmp_28 = tmp_8*tmp_17;
double tmp_29 = -tmp_21*tmp_2;
double tmp_30 = tmp_29+tmp_28;
double f0 = -(tmp_27*tmp_30+tmp_14*tmp_23)*(tmp_27*tmp_23-(tmp_30*tmp_30)-(tmp_23*tmp_23)-tmp_14*tmp_30-(tmp_27*tmp_27)-(tmp_14*tmp_14));
f(0) = f0;
}
      }
      if(do_g) {
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(4);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_5+tmp_3;
double tmp_7 = tmp_2*tmp_6;
double tmp_8 = sin(tmp_1);
double tmp_9 = x(1);
double tmp_10 = x(5);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_9+tmp_11;
double tmp_13 = tmp_12*tmp_8;
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = x(3);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_9+tmp_16;
double tmp_18 = tmp_2*tmp_17;
double tmp_19 = x(2);
double tmp_20 = -tmp_19;
double tmp_21 = tmp_20+tmp_4;
double tmp_22 = tmp_21*tmp_8;
double tmp_23 = tmp_18+tmp_22;
double tmp_25 = tmp_8*tmp_6;
double tmp_26 = -tmp_12*tmp_2;
double tmp_27 = tmp_26+tmp_25;
double tmp_28 = tmp_8*tmp_17;
double tmp_29 = -tmp_2*tmp_21;
double tmp_30 = tmp_29+tmp_28;
double g0_0 = -3.0*(tmp_27*tmp_8+tmp_30*tmp_2-tmp_8*tmp_23+tmp_2*tmp_14)*(tmp_14*tmp_23+tmp_27*tmp_30)-((tmp_14*tmp_14)-tmp_27*tmp_23+tmp_30*tmp_14+(tmp_23*tmp_23)+(tmp_30*tmp_30)+(tmp_27*tmp_27))*(tmp_2*tmp_23+tmp_30*tmp_8+tmp_27*tmp_2-tmp_8*tmp_14);
g(0,0) = g0_0;
}
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(1);
double tmp_4 = x(3);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = sin(tmp_1);
double tmp_8 = tmp_7*tmp_6;
double tmp_9 = x(0);
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_9;
double tmp_13 = -tmp_2*tmp_12;
double tmp_14 = tmp_13+tmp_8;
double tmp_16 = x(4);
double tmp_17 = -tmp_9;
double tmp_18 = tmp_17+tmp_16;
double tmp_19 = tmp_18*tmp_7;
double tmp_20 = x(5);
double tmp_21 = -tmp_20;
double tmp_22 = tmp_21+tmp_3;
double tmp_23 = -tmp_22*tmp_2;
double tmp_24 = tmp_23+tmp_19;
double tmp_26 = tmp_6*tmp_2;
double tmp_27 = tmp_7*tmp_12;
double tmp_28 = tmp_27+tmp_26;
double tmp_30 = tmp_18*tmp_2;
double tmp_31 = tmp_7*tmp_22;
double tmp_32 = tmp_30+tmp_31;
double g0_1 = 3.0*(tmp_32*tmp_28+tmp_24*tmp_14)*(tmp_7*tmp_14+tmp_2*tmp_28-tmp_24*tmp_2+tmp_7*tmp_32)-(tmp_14*tmp_2-tmp_7*tmp_28-tmp_32*tmp_2-tmp_24*tmp_7)*((tmp_14*tmp_14)+(tmp_28*tmp_28)+(tmp_24*tmp_24)-tmp_24*tmp_28+(tmp_32*tmp_32)+tmp_14*tmp_32);
g(0,1) = g0_1;
}
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(4);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = sin(tmp_1);
double tmp_9 = x(1);
double tmp_10 = x(5);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_9+tmp_11;
double tmp_13 = tmp_12*tmp_8;
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = x(3);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_16+tmp_9;
double tmp_18 = tmp_2*tmp_17;
double tmp_19 = x(2);
double tmp_20 = -tmp_19;
double tmp_21 = tmp_20+tmp_4;
double tmp_22 = tmp_21*tmp_8;
double tmp_23 = tmp_22+tmp_18;
double tmp_25 = tmp_6*tmp_8;
double tmp_26 = -tmp_12*tmp_2;
double tmp_27 = tmp_26+tmp_25;
double tmp_28 = tmp_8*tmp_17;
double tmp_29 = -tmp_2*tmp_21;
double tmp_30 = tmp_29+tmp_28;
double g0_2 = ((tmp_27*tmp_27)+(tmp_30*tmp_30)+(tmp_23*tmp_23)+(tmp_14*tmp_14)+tmp_14*tmp_30-tmp_27*tmp_23)*(tmp_2*tmp_27-tmp_8*tmp_14)+(tmp_14*tmp_23+tmp_27*tmp_30)*(2.0*tmp_2*tmp_30+tmp_2*tmp_14-2.0*tmp_8*tmp_23+tmp_8*tmp_27);
g(0,2) = g0_2;
}
{
double tmp_1 = x(6);
double tmp_2 = sin(tmp_1);
double tmp_3 = x(4);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_5+tmp_3;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = cos(tmp_1);
double tmp_9 = x(1);
double tmp_10 = x(5);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_9+tmp_11;
double tmp_13 = -tmp_8*tmp_12;
double tmp_14 = tmp_13+tmp_7;
double tmp_16 = tmp_6*tmp_8;
double tmp_17 = tmp_2*tmp_12;
double tmp_18 = tmp_17+tmp_16;
double tmp_21 = x(3);
double tmp_22 = -tmp_21;
double tmp_23 = tmp_9+tmp_22;
double tmp_24 = tmp_8*tmp_23;
double tmp_25 = x(2);
double tmp_26 = -tmp_25;
double tmp_27 = tmp_26+tmp_4;
double tmp_28 = tmp_27*tmp_2;
double tmp_29 = tmp_24+tmp_28;
double tmp_30 = tmp_2*tmp_23;
double tmp_31 = -tmp_27*tmp_8;
double tmp_32 = tmp_30+tmp_31;
double g0_3 = -(tmp_14*tmp_2+tmp_18*tmp_8)*((tmp_14*tmp_14)+tmp_18*tmp_32-tmp_14*tmp_29+(tmp_18*tmp_18)+(tmp_29*tmp_29)+(tmp_32*tmp_32))-(tmp_18*tmp_2+2.0*tmp_8*tmp_29-tmp_14*tmp_8+2.0*tmp_2*tmp_32)*(tmp_14*tmp_32+tmp_18*tmp_29);
g(0,3) = g0_3;
}
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(1);
double tmp_4 = x(3);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = x(0);
double tmp_9 = x(2);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = sin(tmp_1);
double tmp_13 = tmp_12*tmp_11;
double tmp_14 = tmp_7+tmp_13;
double tmp_16 = tmp_6*tmp_12;
double tmp_17 = -tmp_2*tmp_11;
double tmp_18 = tmp_17+tmp_16;
double tmp_21 = x(4);
double tmp_22 = -tmp_8;
double tmp_23 = tmp_22+tmp_21;
double tmp_24 = tmp_23*tmp_12;
double tmp_25 = x(5);
double tmp_26 = -tmp_25;
double tmp_27 = tmp_26+tmp_3;
double tmp_28 = -tmp_2*tmp_27;
double tmp_29 = tmp_28+tmp_24;
double tmp_30 = tmp_23*tmp_2;
double tmp_31 = tmp_12*tmp_27;
double tmp_32 = tmp_31+tmp_30;
double g0_4 = (tmp_2*tmp_14+tmp_12*tmp_18)*((tmp_14*tmp_14)-tmp_29*tmp_14+tmp_32*tmp_18+(tmp_32*tmp_32)+(tmp_18*tmp_18)+(tmp_29*tmp_29))-(tmp_12*tmp_14-tmp_18*tmp_2-2.0*tmp_32*tmp_2-2.0*tmp_29*tmp_12)*(tmp_29*tmp_18+tmp_32*tmp_14);
g(0,4) = g0_4;
}
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(4);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_5+tmp_3;
double tmp_7 = tmp_2*tmp_6;
double tmp_8 = sin(tmp_1);
double tmp_9 = x(1);
double tmp_10 = x(5);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_9;
double tmp_13 = tmp_8*tmp_12;
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = x(3);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_9+tmp_16;
double tmp_18 = tmp_2*tmp_17;
double tmp_19 = x(2);
double tmp_20 = -tmp_19;
double tmp_21 = tmp_4+tmp_20;
double tmp_22 = tmp_8*tmp_21;
double tmp_23 = tmp_22+tmp_18;
double tmp_25 = tmp_8*tmp_6;
double tmp_26 = -tmp_2*tmp_12;
double tmp_27 = tmp_25+tmp_26;
double tmp_28 = tmp_8*tmp_17;
double tmp_29 = -tmp_2*tmp_21;
double tmp_30 = tmp_28+tmp_29;
double g0_5 = (tmp_2*tmp_30-tmp_8*tmp_23)*(tmp_14*tmp_30+(tmp_27*tmp_27)+(tmp_30*tmp_30)-tmp_27*tmp_23+(tmp_14*tmp_14)+(tmp_23*tmp_23))+(2.0*tmp_2*tmp_27-2.0*tmp_8*tmp_14-tmp_2*tmp_23-tmp_8*tmp_30)*(tmp_14*tmp_23+tmp_27*tmp_30);
g(0,5) = g0_5;
}
{
double g0_6 = 0.0;
g(0,6) = g0_6;
}
      }
   }
}
