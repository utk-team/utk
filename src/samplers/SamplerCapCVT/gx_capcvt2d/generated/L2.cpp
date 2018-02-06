#include "L2.h"
namespace Geex {
   L2::L2() : Function(1,6,4){}
   void L2::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = p(3);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(2);
double tmp_8 = x(2);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = -tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(5);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_1;
double tmp_17 = tmp_5*tmp_16;
double tmp_18 = x(4);
double tmp_19 = tmp_18+tmp_10;
double tmp_20 = -tmp_7*tmp_19;
double tmp_21 = tmp_17+tmp_20;
double tmp_23 = p(1);
double tmp_24 = tmp_23*tmp_16;
double tmp_25 = p(0);
double tmp_26 = -tmp_25*tmp_19;
double tmp_27 = tmp_24+tmp_26;
double tmp_28 = tmp_4*tmp_23;
double tmp_29 = -tmp_25*tmp_11;
double tmp_30 = tmp_28+tmp_29;
double f0 = -(tmp_27*tmp_13-tmp_21*tmp_30)*(tmp_21*tmp_13+(tmp_13*tmp_13)+(tmp_27*tmp_27)+(tmp_21*tmp_21)+tmp_27*tmp_30+(tmp_30*tmp_30));
f(0) = f0;
}
      }
      if(do_g) {
{
double tmp_1 = p(2);
double tmp_2 = x(1);
double tmp_3 = x(5);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = p(3);
double tmp_7 = tmp_5*tmp_6;
double tmp_8 = x(4);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = -tmp_11*tmp_1;
double tmp_13 = tmp_12+tmp_7;
double tmp_15 = p(0);
double tmp_16 = x(3);
double tmp_17 = -tmp_16;
double tmp_18 = tmp_2+tmp_17;
double tmp_19 = p(1);
double tmp_20 = tmp_18*tmp_19;
double tmp_21 = x(2);
double tmp_22 = tmp_21+tmp_10;
double tmp_23 = -tmp_15*tmp_22;
double tmp_24 = tmp_20+tmp_23;
double tmp_26 = tmp_18*tmp_6;
double tmp_27 = -tmp_1*tmp_22;
double tmp_28 = tmp_27+tmp_26;
double tmp_30 = tmp_5*tmp_19;
double tmp_31 = -tmp_15*tmp_11;
double tmp_32 = tmp_30+tmp_31;
double g0_0 = -3.0*(tmp_1*tmp_28+tmp_15*tmp_24+tmp_1*tmp_13+tmp_15*tmp_32)*(tmp_32*tmp_28-tmp_24*tmp_13)-((tmp_24*tmp_24)+(tmp_28*tmp_28)+tmp_24*tmp_32+(tmp_13*tmp_13)+tmp_13*tmp_28+(tmp_32*tmp_32))*(tmp_15*tmp_28-tmp_15*tmp_13+tmp_1*tmp_32-tmp_24*tmp_1);
g(0,0) = g0_0;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = p(1);
double tmp_6 = tmp_4*tmp_5;
double tmp_7 = p(0);
double tmp_8 = x(2);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = -tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_15 = x(5);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_1+tmp_16;
double tmp_18 = tmp_17*tmp_5;
double tmp_19 = x(4);
double tmp_20 = tmp_10+tmp_19;
double tmp_21 = -tmp_7*tmp_20;
double tmp_22 = tmp_18+tmp_21;
double tmp_24 = p(3);
double tmp_25 = tmp_24*tmp_17;
double tmp_26 = p(2);
double tmp_27 = -tmp_20*tmp_26;
double tmp_28 = tmp_27+tmp_25;
double tmp_30 = tmp_24*tmp_4;
double tmp_31 = -tmp_11*tmp_26;
double tmp_32 = tmp_30+tmp_31;
double g0_1 = (tmp_24*tmp_13+tmp_28*tmp_5-tmp_24*tmp_22-tmp_32*tmp_5)*((tmp_28*tmp_28)+(tmp_22*tmp_22)+(tmp_32*tmp_32)+tmp_13*tmp_22+(tmp_13*tmp_13)+tmp_32*tmp_28)-3.0*(tmp_22*tmp_5+tmp_24*tmp_32+tmp_24*tmp_28+tmp_13*tmp_5)*(tmp_32*tmp_22-tmp_13*tmp_28);
g(0,1) = g0_1;
}
{
double tmp_1 = p(2);
double tmp_2 = x(1);
double tmp_3 = x(5);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_4+tmp_2;
double tmp_6 = p(3);
double tmp_7 = tmp_6*tmp_5;
double tmp_8 = x(4);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = -tmp_1*tmp_11;
double tmp_13 = tmp_7+tmp_12;
double tmp_15 = p(0);
double tmp_16 = x(3);
double tmp_17 = -tmp_16;
double tmp_18 = tmp_17+tmp_2;
double tmp_19 = p(1);
double tmp_20 = tmp_19*tmp_18;
double tmp_21 = x(2);
double tmp_22 = tmp_10+tmp_21;
double tmp_23 = -tmp_22*tmp_15;
double tmp_24 = tmp_20+tmp_23;
double tmp_26 = tmp_6*tmp_18;
double tmp_27 = -tmp_1*tmp_22;
double tmp_28 = tmp_26+tmp_27;
double tmp_30 = tmp_19*tmp_5;
double tmp_31 = -tmp_15*tmp_11;
double tmp_32 = tmp_30+tmp_31;
double g0_2 = (tmp_32*tmp_24+(tmp_24*tmp_24)+(tmp_13*tmp_13)+(tmp_28*tmp_28)+tmp_13*tmp_28+(tmp_32*tmp_32))*(tmp_1*tmp_32-tmp_13*tmp_15)-(tmp_13*tmp_24-tmp_32*tmp_28)*(tmp_1*tmp_13+2.0*tmp_1*tmp_28+2.0*tmp_15*tmp_24+tmp_32*tmp_15);
g(0,2) = g0_2;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = p(3);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(2);
double tmp_8 = x(2);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = -tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(5);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_1+tmp_15;
double tmp_17 = tmp_5*tmp_16;
double tmp_18 = x(4);
double tmp_19 = tmp_18+tmp_10;
double tmp_20 = -tmp_19*tmp_7;
double tmp_21 = tmp_17+tmp_20;
double tmp_23 = p(1);
double tmp_24 = tmp_23*tmp_16;
double tmp_25 = p(0);
double tmp_26 = -tmp_19*tmp_25;
double tmp_27 = tmp_24+tmp_26;
double tmp_28 = tmp_23*tmp_4;
double tmp_29 = -tmp_11*tmp_25;
double tmp_30 = tmp_29+tmp_28;
double g0_3 = -(tmp_21*tmp_5+2.0*tmp_5*tmp_13+2.0*tmp_30*tmp_23+tmp_27*tmp_23)*(tmp_21*tmp_30-tmp_27*tmp_13)+(tmp_27*tmp_5-tmp_21*tmp_23)*((tmp_13*tmp_13)+tmp_27*tmp_30+(tmp_21*tmp_21)+(tmp_30*tmp_30)+(tmp_27*tmp_27)+tmp_21*tmp_13);
g(0,3) = g0_3;
}
{
double tmp_1 = p(2);
double tmp_2 = x(1);
double tmp_3 = x(5);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_4+tmp_2;
double tmp_6 = p(3);
double tmp_7 = tmp_6*tmp_5;
double tmp_8 = x(4);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = -tmp_1*tmp_11;
double tmp_13 = tmp_7+tmp_12;
double tmp_15 = p(0);
double tmp_16 = x(3);
double tmp_17 = -tmp_16;
double tmp_18 = tmp_17+tmp_2;
double tmp_19 = p(1);
double tmp_20 = tmp_19*tmp_18;
double tmp_21 = x(2);
double tmp_22 = tmp_10+tmp_21;
double tmp_23 = -tmp_22*tmp_15;
double tmp_24 = tmp_20+tmp_23;
double tmp_26 = tmp_6*tmp_18;
double tmp_27 = -tmp_1*tmp_22;
double tmp_28 = tmp_26+tmp_27;
double tmp_30 = tmp_19*tmp_5;
double tmp_31 = -tmp_15*tmp_11;
double tmp_32 = tmp_31+tmp_30;
double g0_4 = -(tmp_1*tmp_24-tmp_28*tmp_15)*(tmp_32*tmp_24+(tmp_13*tmp_13)+(tmp_24*tmp_24)+tmp_13*tmp_28+(tmp_28*tmp_28)+(tmp_32*tmp_32))-(tmp_13*tmp_24-tmp_32*tmp_28)*(2.0*tmp_1*tmp_13+2.0*tmp_32*tmp_15+tmp_15*tmp_24+tmp_1*tmp_28);
g(0,4) = g0_4;
}
{
double tmp_1 = p(1);
double tmp_2 = x(1);
double tmp_3 = x(3);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = p(3);
double tmp_7 = tmp_5*tmp_6;
double tmp_8 = p(2);
double tmp_9 = x(2);
double tmp_10 = x(0);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_9;
double tmp_13 = -tmp_8*tmp_12;
double tmp_14 = tmp_7+tmp_13;
double tmp_16 = tmp_5*tmp_1;
double tmp_17 = p(0);
double tmp_18 = -tmp_17*tmp_12;
double tmp_19 = tmp_18+tmp_16;
double tmp_22 = x(5);
double tmp_23 = -tmp_22;
double tmp_24 = tmp_2+tmp_23;
double tmp_25 = tmp_24*tmp_6;
double tmp_26 = x(4);
double tmp_27 = tmp_11+tmp_26;
double tmp_28 = -tmp_8*tmp_27;
double tmp_29 = tmp_25+tmp_28;
double tmp_31 = tmp_24*tmp_1;
double tmp_32 = -tmp_17*tmp_27;
double tmp_33 = tmp_31+tmp_32;
double g0_5 = (tmp_14*tmp_33-tmp_29*tmp_19)*(tmp_14*tmp_6+tmp_1*tmp_19+2.0*tmp_1*tmp_33+2.0*tmp_29*tmp_6)-(tmp_6*tmp_19-tmp_14*tmp_1)*((tmp_33*tmp_33)+(tmp_14*tmp_14)+(tmp_19*tmp_19)+(tmp_29*tmp_29)+tmp_14*tmp_29+tmp_33*tmp_19);
g(0,5) = g0_5;
}
      }
   }
}
