#include "L6.h"
namespace Geex {
   L6::L6() : Function(1,6,4){}
   void L6::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = x(3);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = p(3);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(2);
double tmp_8 = x(2);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(4);
double tmp_15 = tmp_10+tmp_14;
double tmp_16 = tmp_15*tmp_7;
double tmp_17 = x(5);
double tmp_18 = tmp_17+tmp_3;
double tmp_19 = tmp_5*tmp_18;
double tmp_20 = tmp_16+tmp_19;
double tmp_22 = p(0);
double tmp_23 = tmp_22*tmp_15;
double tmp_24 = p(1);
double tmp_25 = tmp_18*tmp_24;
double tmp_26 = tmp_23+tmp_25;
double tmp_27 = tmp_22*tmp_11;
double tmp_28 = tmp_24*tmp_4;
double tmp_29 = tmp_28+tmp_27;
double f0 = -((tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_29+((tmp_20*tmp_20*tmp_20)*(tmp_20*tmp_20*tmp_20))+(tmp_26*tmp_26)*((tmp_29*tmp_29)*(tmp_29*tmp_29))+tmp_20*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+((tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_29*tmp_29)+((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+(tmp_20*tmp_20*tmp_20)*(tmp_13*tmp_13*tmp_13)+(tmp_26*tmp_26*tmp_26)*(tmp_29*tmp_29*tmp_29)+((tmp_20*tmp_20)*(tmp_20*tmp_20))*(tmp_13*tmp_13)+tmp_26*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))+(tmp_20*tmp_20)*((tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_20*(tmp_20*tmp_20)*(tmp_20*tmp_20))*tmp_13)*(tmp_26*tmp_13-tmp_20*tmp_29);
f(0) = f0;
}
      }
      if(do_g) {
{
double tmp_1 = x(3);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = p(3);
double tmp_6 = tmp_4*tmp_5;
double tmp_7 = p(2);
double tmp_8 = x(2);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = tmp_7*tmp_11;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(4);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = tmp_7*tmp_15;
double tmp_17 = x(5);
double tmp_18 = tmp_17+tmp_3;
double tmp_19 = tmp_18*tmp_5;
double tmp_20 = tmp_16+tmp_19;
double tmp_22 = p(0);
double tmp_23 = tmp_22*tmp_15;
double tmp_24 = p(1);
double tmp_25 = tmp_18*tmp_24;
double tmp_26 = tmp_23+tmp_25;
double tmp_28 = tmp_22*tmp_11;
double tmp_29 = tmp_4*tmp_24;
double tmp_30 = tmp_29+tmp_28;
double g0_0 = -7.0*(tmp_20*tmp_30-tmp_26*tmp_13)*((tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_22+tmp_20*tmp_7*((tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_20*tmp_20)*tmp_7*(tmp_13*tmp_13*tmp_13)+(tmp_26*tmp_26*tmp_26)*tmp_22*(tmp_30*tmp_30)+((tmp_20*tmp_20)*(tmp_20*tmp_20))*tmp_7*tmp_13+tmp_7*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_20*(tmp_20*tmp_20)*(tmp_20*tmp_20))*tmp_7+tmp_26*tmp_22*((tmp_30*tmp_30)*(tmp_30*tmp_30))+(tmp_26*tmp_26)*tmp_22*(tmp_30*tmp_30*tmp_30)+((tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_22*tmp_30+tmp_22*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))+(tmp_20*tmp_20*tmp_20)*tmp_7*(tmp_13*tmp_13))-(tmp_26*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))+tmp_20*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_26*tmp_26*tmp_26)*(tmp_30*tmp_30*tmp_30)+((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+(tmp_20*tmp_20*tmp_20)*(tmp_13*tmp_13*tmp_13)+((tmp_20*tmp_20*tmp_20)*(tmp_20*tmp_20*tmp_20))+((tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_30*tmp_30)+((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+((tmp_20*tmp_20)*(tmp_20*tmp_20))*(tmp_13*tmp_13)+(tmp_20*tmp_20)*((tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_30+((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))+(tmp_26*tmp_26)*((tmp_30*tmp_30)*(tmp_30*tmp_30))+(tmp_20*(tmp_20*tmp_20)*(tmp_20*tmp_20))*tmp_13)*(tmp_20*tmp_22+tmp_7*tmp_30-tmp_26*tmp_7-tmp_13*tmp_22);
g(0,0) = g0_0;
}
{
double tmp_1 = x(4);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = p(2);
double tmp_6 = tmp_4*tmp_5;
double tmp_7 = p(3);
double tmp_8 = x(5);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = p(1);
double tmp_16 = p(0);
double tmp_17 = x(2);
double tmp_18 = tmp_17+tmp_3;
double tmp_19 = tmp_16*tmp_18;
double tmp_20 = x(3);
double tmp_21 = tmp_20+tmp_10;
double tmp_22 = tmp_14*tmp_21;
double tmp_23 = tmp_19+tmp_22;
double tmp_25 = tmp_4*tmp_16;
double tmp_26 = tmp_11*tmp_14;
double tmp_27 = tmp_26+tmp_25;
double tmp_29 = tmp_7*tmp_21;
double tmp_30 = tmp_18*tmp_5;
double tmp_31 = tmp_30+tmp_29;
double g0_1 = -((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*tmp_27+(tmp_23*tmp_23)*((tmp_27*tmp_27)*(tmp_27*tmp_27))+tmp_13*(tmp_31*(tmp_31*tmp_31)*(tmp_31*tmp_31))+((tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_27*tmp_27)+(tmp_13*tmp_13*tmp_13)*(tmp_31*tmp_31*tmp_31)+(tmp_23*tmp_23*tmp_23)*(tmp_27*tmp_27*tmp_27)+((tmp_31*tmp_31*tmp_31)*(tmp_31*tmp_31*tmp_31))+((tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_31*tmp_31)+((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+(tmp_13*tmp_13)*((tmp_31*tmp_31)*(tmp_31*tmp_31))+((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))+tmp_23*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))+(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*tmp_31+((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))*(tmp_23*tmp_7-tmp_7*tmp_27-tmp_14*tmp_31+tmp_14*tmp_13)+7.0*(tmp_31*tmp_27-tmp_23*tmp_13)*(tmp_7*(tmp_13*tmp_13)*(tmp_31*tmp_31*tmp_31)+tmp_14*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))+tmp_7*tmp_13*((tmp_31*tmp_31)*(tmp_31*tmp_31))+tmp_14*(tmp_23*tmp_23)*(tmp_27*tmp_27*tmp_27)+tmp_14*tmp_23*((tmp_27*tmp_27)*(tmp_27*tmp_27))+tmp_14*(tmp_23*tmp_23*tmp_23)*(tmp_27*tmp_27)+tmp_7*(tmp_13*tmp_13*tmp_13)*(tmp_31*tmp_31)+tmp_14*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))+tmp_7*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+tmp_14*((tmp_23*tmp_23)*(tmp_23*tmp_23))*tmp_27+tmp_7*(tmp_31*(tmp_31*tmp_31)*(tmp_31*tmp_31))+tmp_7*((tmp_13*tmp_13)*(tmp_13*tmp_13))*tmp_31);
g(0,1) = g0_1;
}
{
double tmp_1 = x(3);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = p(3);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(2);
double tmp_8 = x(2);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(4);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = tmp_15*tmp_7;
double tmp_17 = x(5);
double tmp_18 = tmp_17+tmp_3;
double tmp_19 = tmp_18*tmp_5;
double tmp_20 = tmp_16+tmp_19;
double tmp_22 = p(0);
double tmp_23 = tmp_15*tmp_22;
double tmp_24 = p(1);
double tmp_25 = tmp_18*tmp_24;
double tmp_26 = tmp_23+tmp_25;
double tmp_28 = tmp_11*tmp_22;
double tmp_29 = tmp_24*tmp_4;
double tmp_30 = tmp_29+tmp_28;
double g0_2 = (tmp_20*tmp_22-tmp_7*tmp_26)*((tmp_20*tmp_20)*((tmp_13*tmp_13)*(tmp_13*tmp_13))+tmp_30*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))+((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+(tmp_20*(tmp_20*tmp_20)*(tmp_20*tmp_20))*tmp_13+(tmp_30*tmp_30*tmp_30)*(tmp_26*tmp_26*tmp_26)+((tmp_20*tmp_20)*(tmp_20*tmp_20))*(tmp_13*tmp_13)+((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+((tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_26*tmp_26)+((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))+(tmp_20*tmp_20*tmp_20)*(tmp_13*tmp_13*tmp_13)+(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_26+(tmp_30*tmp_30)*((tmp_26*tmp_26)*(tmp_26*tmp_26))+tmp_20*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+((tmp_20*tmp_20*tmp_20)*(tmp_20*tmp_20*tmp_20)))+(6.0*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_22+(tmp_20*(tmp_20*tmp_20)*(tmp_20*tmp_20))*tmp_7+6.0*tmp_7*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+3.0*(tmp_20*tmp_20*tmp_20)*tmp_7*(tmp_13*tmp_13)+4.0*(tmp_30*tmp_30*tmp_30)*(tmp_26*tmp_26)*tmp_22+5.0*((tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_26*tmp_22+(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_22+3.0*(tmp_30*tmp_30)*(tmp_26*tmp_26*tmp_26)*tmp_22+2.0*tmp_30*((tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_22+5.0*tmp_20*tmp_7*((tmp_13*tmp_13)*(tmp_13*tmp_13))+2.0*((tmp_20*tmp_20)*(tmp_20*tmp_20))*tmp_7*tmp_13+4.0*(tmp_20*tmp_20)*tmp_7*(tmp_13*tmp_13*tmp_13))*(tmp_30*tmp_20-tmp_26*tmp_13);
g(0,2) = g0_2;
}
{
double tmp_1 = x(4);
double tmp_2 = x(0);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = p(2);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(3);
double tmp_8 = x(5);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_12+tmp_6;
double tmp_14 = p(1);
double tmp_16 = p(0);
double tmp_17 = tmp_16*tmp_4;
double tmp_18 = tmp_11*tmp_14;
double tmp_19 = tmp_17+tmp_18;
double tmp_22 = x(3);
double tmp_23 = tmp_10+tmp_22;
double tmp_24 = tmp_23*tmp_7;
double tmp_25 = x(2);
double tmp_26 = tmp_25+tmp_3;
double tmp_27 = tmp_5*tmp_26;
double tmp_28 = tmp_24+tmp_27;
double tmp_30 = tmp_26*tmp_16;
double tmp_31 = tmp_14*tmp_23;
double tmp_32 = tmp_30+tmp_31;
double g0_3 = (((tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_19*tmp_19)+(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*tmp_28+(tmp_13*tmp_13)*((tmp_28*tmp_28)*(tmp_28*tmp_28))+(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*tmp_19+((tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_28*tmp_28)+(tmp_32*tmp_32)*((tmp_19*tmp_19)*(tmp_19*tmp_19))+((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))+(tmp_13*tmp_13*tmp_13)*(tmp_28*tmp_28*tmp_28)+tmp_32*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))+((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))+tmp_13*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))+(tmp_32*tmp_32*tmp_32)*(tmp_19*tmp_19*tmp_19)+((tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19))+((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))*(tmp_14*tmp_13-tmp_7*tmp_19)-(2.0*tmp_7*((tmp_13*tmp_13)*(tmp_13*tmp_13))*tmp_28+5.0*tmp_14*((tmp_32*tmp_32)*(tmp_32*tmp_32))*tmp_19+tmp_7*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+tmp_14*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))+6.0*tmp_7*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))+3.0*tmp_7*(tmp_13*tmp_13*tmp_13)*(tmp_28*tmp_28)+4.0*tmp_14*(tmp_32*tmp_32*tmp_32)*(tmp_19*tmp_19)+3.0*tmp_14*(tmp_32*tmp_32)*(tmp_19*tmp_19*tmp_19)+4.0*tmp_7*(tmp_13*tmp_13)*(tmp_28*tmp_28*tmp_28)+2.0*tmp_14*tmp_32*((tmp_19*tmp_19)*(tmp_19*tmp_19))+5.0*tmp_7*tmp_13*((tmp_28*tmp_28)*(tmp_28*tmp_28))+6.0*tmp_14*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32)))*(tmp_19*tmp_28-tmp_32*tmp_13);
g(0,3) = g0_3;
}
{
double tmp_1 = p(2);
double tmp_2 = p(0);
double tmp_3 = x(2);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = x(3);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = p(1);
double tmp_13 = tmp_12*tmp_11;
double tmp_14 = tmp_7+tmp_13;
double tmp_16 = p(3);
double tmp_17 = tmp_16*tmp_11;
double tmp_18 = tmp_1*tmp_6;
double tmp_19 = tmp_18+tmp_17;
double tmp_22 = x(4);
double tmp_23 = tmp_22+tmp_5;
double tmp_24 = tmp_1*tmp_23;
double tmp_25 = x(5);
double tmp_26 = tmp_25+tmp_10;
double tmp_27 = tmp_16*tmp_26;
double tmp_28 = tmp_24+tmp_27;
double tmp_30 = tmp_2*tmp_23;
double tmp_31 = tmp_12*tmp_26;
double tmp_32 = tmp_31+tmp_30;
double g0_4 = (tmp_28*tmp_14-tmp_32*tmp_19)*(3.0*(tmp_32*tmp_32)*(tmp_14*tmp_14*tmp_14)*tmp_2+3.0*(tmp_19*tmp_19*tmp_19)*tmp_1*(tmp_28*tmp_28)+2.0*tmp_32*((tmp_14*tmp_14)*(tmp_14*tmp_14))*tmp_2+6.0*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*tmp_2+5.0*tmp_19*tmp_1*((tmp_28*tmp_28)*(tmp_28*tmp_28))+2.0*((tmp_19*tmp_19)*(tmp_19*tmp_19))*tmp_1*tmp_28+4.0*(tmp_19*tmp_19)*tmp_1*(tmp_28*tmp_28*tmp_28)+(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))*tmp_1+4.0*(tmp_32*tmp_32*tmp_32)*(tmp_14*tmp_14)*tmp_2+(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*tmp_2+5.0*((tmp_32*tmp_32)*(tmp_32*tmp_32))*tmp_14*tmp_2+6.0*tmp_1*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28)))+(((tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19))+(tmp_32*tmp_32*tmp_32)*(tmp_14*tmp_14*tmp_14)+((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))+tmp_19*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))+((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))+tmp_32*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))+(tmp_19*tmp_19*tmp_19)*(tmp_28*tmp_28*tmp_28)+((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*tmp_14+(tmp_32*tmp_32)*((tmp_14*tmp_14)*(tmp_14*tmp_14))+((tmp_19*tmp_19)*(tmp_19*tmp_19))*(tmp_28*tmp_28)+(tmp_19*tmp_19)*((tmp_28*tmp_28)*(tmp_28*tmp_28))+((tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_14*tmp_14)+(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))*tmp_28)*(tmp_1*tmp_14-tmp_19*tmp_2);
g(0,4) = g0_4;
}
{
double tmp_1 = p(3);
double tmp_2 = p(0);
double tmp_3 = x(2);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = x(3);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = p(1);
double tmp_13 = tmp_12*tmp_11;
double tmp_14 = tmp_13+tmp_7;
double tmp_16 = tmp_11*tmp_1;
double tmp_17 = p(2);
double tmp_18 = tmp_6*tmp_17;
double tmp_19 = tmp_18+tmp_16;
double tmp_22 = x(4);
double tmp_23 = tmp_5+tmp_22;
double tmp_24 = tmp_17*tmp_23;
double tmp_25 = x(5);
double tmp_26 = tmp_10+tmp_25;
double tmp_27 = tmp_26*tmp_1;
double tmp_28 = tmp_24+tmp_27;
double tmp_30 = tmp_2*tmp_23;
double tmp_31 = tmp_12*tmp_26;
double tmp_32 = tmp_31+tmp_30;
double g0_5 = -(tmp_12*tmp_19-tmp_14*tmp_1)*((tmp_14*tmp_14)*((tmp_32*tmp_32)*(tmp_32*tmp_32))+((tmp_19*tmp_19)*(tmp_19*tmp_19))*(tmp_28*tmp_28)+(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*tmp_32+((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))+(tmp_19*tmp_19)*((tmp_28*tmp_28)*(tmp_28*tmp_28))+((tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_32*tmp_32)+(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))*tmp_28+((tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19))+tmp_19*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))+(tmp_14*tmp_14*tmp_14)*(tmp_32*tmp_32*tmp_32)+((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))+tmp_14*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))+((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+(tmp_19*tmp_19*tmp_19)*(tmp_28*tmp_28*tmp_28))+(6.0*tmp_1*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))+4.0*tmp_12*(tmp_14*tmp_14)*(tmp_32*tmp_32*tmp_32)+5.0*tmp_12*tmp_14*((tmp_32*tmp_32)*(tmp_32*tmp_32))+tmp_1*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))+3.0*tmp_12*(tmp_14*tmp_14*tmp_14)*(tmp_32*tmp_32)+2.0*tmp_1*((tmp_19*tmp_19)*(tmp_19*tmp_19))*tmp_28+6.0*tmp_12*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))+4.0*tmp_1*(tmp_19*tmp_19)*(tmp_28*tmp_28*tmp_28)+5.0*tmp_1*tmp_19*((tmp_28*tmp_28)*(tmp_28*tmp_28))+2.0*tmp_12*((tmp_14*tmp_14)*(tmp_14*tmp_14))*tmp_32+tmp_12*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))+3.0*tmp_1*(tmp_19*tmp_19*tmp_19)*(tmp_28*tmp_28))*(tmp_14*tmp_28-tmp_32*tmp_19);
g(0,5) = g0_5;
}
      }
   }
}
