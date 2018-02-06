#include "L4.h"
namespace Geex {
   L4::L4() : Function(1,6,4){}
   void L4::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = p(2);
double tmp_2 = x(4);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = tmp_5*tmp_1;
double tmp_7 = p(3);
double tmp_8 = x(5);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = p(0);
double tmp_15 = tmp_5*tmp_14;
double tmp_16 = p(1);
double tmp_17 = tmp_11*tmp_16;
double tmp_18 = tmp_15+tmp_17;
double tmp_19 = x(3);
double tmp_20 = tmp_10+tmp_19;
double tmp_21 = tmp_20*tmp_16;
double tmp_22 = x(2);
double tmp_23 = -tmp_22;
double tmp_24 = tmp_23+tmp_3;
double tmp_25 = -tmp_24*tmp_14;
double tmp_26 = tmp_25+tmp_21;
double tmp_28 = tmp_20*tmp_7;
double tmp_29 = -tmp_24*tmp_1;
double tmp_30 = tmp_28+tmp_29;
double f0 = -(tmp_18*tmp_30-tmp_26*tmp_13)*((tmp_30*tmp_30)*(tmp_13*tmp_13)+tmp_18*(tmp_26*tmp_26*tmp_26)+((tmp_26*tmp_26)*(tmp_26*tmp_26))+(tmp_30*tmp_30*tmp_30)*tmp_13+((tmp_18*tmp_18)*(tmp_18*tmp_18))+(tmp_18*tmp_18*tmp_18)*tmp_26+tmp_30*(tmp_13*tmp_13*tmp_13)+((tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_18*tmp_18)*(tmp_26*tmp_26)+((tmp_30*tmp_30)*(tmp_30*tmp_30)));
f(0) = f0;
}
      }
      if(do_g) {
{
double tmp_1 = p(2);
double tmp_2 = x(4);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_4+tmp_2;
double tmp_6 = tmp_1*tmp_5;
double tmp_7 = p(3);
double tmp_8 = x(5);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(3);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = tmp_7*tmp_15;
double tmp_17 = x(2);
double tmp_18 = -tmp_17;
double tmp_19 = tmp_3+tmp_18;
double tmp_20 = -tmp_1*tmp_19;
double tmp_21 = tmp_20+tmp_16;
double tmp_24 = p(0);
double tmp_25 = tmp_5*tmp_24;
double tmp_26 = p(1);
double tmp_27 = tmp_11*tmp_26;
double tmp_28 = tmp_25+tmp_27;
double tmp_31 = tmp_26*tmp_15;
double tmp_32 = -tmp_19*tmp_24;
double tmp_33 = tmp_32+tmp_31;
double g0_0 = 5.0*(tmp_1*(tmp_13*tmp_13)*tmp_21+(tmp_28*tmp_28*tmp_28)*tmp_24+tmp_28*(tmp_33*tmp_33)*tmp_24+(tmp_33*tmp_33*tmp_33)*tmp_24+(tmp_28*tmp_28)*tmp_33*tmp_24+tmp_1*tmp_13*(tmp_21*tmp_21)+tmp_1*(tmp_21*tmp_21*tmp_21)+tmp_1*(tmp_13*tmp_13*tmp_13))*(tmp_28*tmp_21-tmp_13*tmp_33)-((tmp_28*tmp_28*tmp_28)*tmp_33+(tmp_28*tmp_28)*(tmp_33*tmp_33)+(tmp_13*tmp_13)*(tmp_21*tmp_21)+(tmp_13*tmp_13*tmp_13)*tmp_21+tmp_13*(tmp_21*tmp_21*tmp_21)+((tmp_33*tmp_33)*(tmp_33*tmp_33))+((tmp_21*tmp_21)*(tmp_21*tmp_21))+((tmp_13*tmp_13)*(tmp_13*tmp_13))+tmp_28*(tmp_33*tmp_33*tmp_33)+((tmp_28*tmp_28)*(tmp_28*tmp_28)))*(tmp_1*tmp_33-tmp_1*tmp_28-tmp_21*tmp_24+tmp_13*tmp_24);
g(0,0) = g0_0;
}
{
double tmp_1 = p(2);
double tmp_2 = x(4);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = tmp_5*tmp_1;
double tmp_7 = p(3);
double tmp_8 = x(5);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_12+tmp_6;
double tmp_14 = x(3);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = p(1);
double tmp_17 = tmp_15*tmp_16;
double tmp_18 = x(2);
double tmp_19 = -tmp_18;
double tmp_20 = tmp_19+tmp_3;
double tmp_21 = p(0);
double tmp_22 = -tmp_21*tmp_20;
double tmp_23 = tmp_17+tmp_22;
double tmp_25 = tmp_21*tmp_5;
double tmp_26 = tmp_11*tmp_16;
double tmp_27 = tmp_26+tmp_25;
double tmp_28 = tmp_15*tmp_7;
double tmp_29 = -tmp_1*tmp_20;
double tmp_30 = tmp_29+tmp_28;
double g0_1 = 5.0*(tmp_7*(tmp_13*tmp_13*tmp_13)+tmp_30*tmp_7*(tmp_13*tmp_13)+(tmp_30*tmp_30*tmp_30)*tmp_7+tmp_27*(tmp_23*tmp_23)*tmp_16+(tmp_27*tmp_27)*tmp_23*tmp_16+(tmp_27*tmp_27*tmp_27)*tmp_16+(tmp_30*tmp_30)*tmp_7*tmp_13+(tmp_23*tmp_23*tmp_23)*tmp_16)*(tmp_27*tmp_30-tmp_23*tmp_13)+(tmp_30*tmp_16-tmp_23*tmp_7+tmp_27*tmp_7-tmp_13*tmp_16)*((tmp_30*tmp_30*tmp_30)*tmp_13+(tmp_27*tmp_27)*(tmp_23*tmp_23)+(tmp_30*tmp_30)*(tmp_13*tmp_13)+((tmp_23*tmp_23)*(tmp_23*tmp_23))+(tmp_27*tmp_27*tmp_27)*tmp_23+((tmp_13*tmp_13)*(tmp_13*tmp_13))+((tmp_27*tmp_27)*(tmp_27*tmp_27))+((tmp_30*tmp_30)*(tmp_30*tmp_30))+tmp_27*(tmp_23*tmp_23*tmp_23)+tmp_30*(tmp_13*tmp_13*tmp_13));
g(0,1) = g0_1;
}
{
double tmp_1 = p(2);
double tmp_2 = x(4);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = tmp_5*tmp_1;
double tmp_7 = p(3);
double tmp_8 = x(5);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_12+tmp_6;
double tmp_14 = p(0);
double tmp_15 = tmp_5*tmp_14;
double tmp_16 = p(1);
double tmp_17 = tmp_11*tmp_16;
double tmp_18 = tmp_15+tmp_17;
double tmp_19 = x(3);
double tmp_20 = tmp_19+tmp_10;
double tmp_21 = tmp_20*tmp_16;
double tmp_22 = x(2);
double tmp_23 = -tmp_22;
double tmp_24 = tmp_23+tmp_3;
double tmp_25 = -tmp_24*tmp_14;
double tmp_26 = tmp_25+tmp_21;
double tmp_28 = tmp_7*tmp_20;
double tmp_29 = -tmp_24*tmp_1;
double tmp_30 = tmp_28+tmp_29;
double g0_2 = (tmp_26*tmp_13-tmp_18*tmp_30)*((tmp_18*tmp_18*tmp_18)*tmp_14+2.0*(tmp_18*tmp_18)*tmp_14*tmp_26+4.0*tmp_14*(tmp_26*tmp_26*tmp_26)+tmp_1*(tmp_13*tmp_13*tmp_13)+3.0*tmp_1*(tmp_30*tmp_30)*tmp_13+3.0*tmp_18*tmp_14*(tmp_26*tmp_26)+2.0*tmp_1*tmp_30*(tmp_13*tmp_13)+4.0*tmp_1*(tmp_30*tmp_30*tmp_30))+(tmp_14*tmp_13-tmp_18*tmp_1)*(tmp_30*(tmp_13*tmp_13*tmp_13)+((tmp_30*tmp_30)*(tmp_30*tmp_30))+tmp_18*(tmp_26*tmp_26*tmp_26)+(tmp_18*tmp_18)*(tmp_26*tmp_26)+((tmp_18*tmp_18)*(tmp_18*tmp_18))+((tmp_26*tmp_26)*(tmp_26*tmp_26))+(tmp_18*tmp_18*tmp_18)*tmp_26+(tmp_30*tmp_30)*(tmp_13*tmp_13)+((tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_30*tmp_30*tmp_30)*tmp_13);
g(0,2) = g0_2;
}
{
double tmp_1 = p(3);
double tmp_2 = x(4);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = p(0);
double tmp_7 = tmp_5*tmp_6;
double tmp_8 = x(5);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = p(1);
double tmp_13 = tmp_11*tmp_12;
double tmp_14 = tmp_7+tmp_13;
double tmp_16 = p(2);
double tmp_17 = tmp_5*tmp_16;
double tmp_18 = tmp_11*tmp_1;
double tmp_19 = tmp_18+tmp_17;
double tmp_22 = x(3);
double tmp_23 = tmp_10+tmp_22;
double tmp_24 = tmp_23*tmp_12;
double tmp_25 = x(2);
double tmp_26 = -tmp_25;
double tmp_27 = tmp_26+tmp_3;
double tmp_28 = -tmp_27*tmp_6;
double tmp_29 = tmp_28+tmp_24;
double tmp_31 = tmp_1*tmp_23;
double tmp_32 = -tmp_27*tmp_16;
double tmp_33 = tmp_31+tmp_32;
double g0_3 = -(tmp_14*tmp_1-tmp_19*tmp_12)*(((tmp_33*tmp_33)*(tmp_33*tmp_33))+tmp_33*(tmp_19*tmp_19*tmp_19)+tmp_14*(tmp_29*tmp_29*tmp_29)+((tmp_29*tmp_29)*(tmp_29*tmp_29))+((tmp_19*tmp_19)*(tmp_19*tmp_19))+(tmp_33*tmp_33)*(tmp_19*tmp_19)+((tmp_14*tmp_14)*(tmp_14*tmp_14))+(tmp_14*tmp_14)*(tmp_29*tmp_29)+(tmp_33*tmp_33*tmp_33)*tmp_19+(tmp_14*tmp_14*tmp_14)*tmp_29)+(3.0*tmp_14*(tmp_29*tmp_29)*tmp_12+2.0*(tmp_14*tmp_14)*tmp_29*tmp_12+4.0*tmp_1*(tmp_33*tmp_33*tmp_33)+3.0*tmp_1*(tmp_33*tmp_33)*tmp_19+4.0*(tmp_29*tmp_29*tmp_29)*tmp_12+tmp_1*(tmp_19*tmp_19*tmp_19)+(tmp_14*tmp_14*tmp_14)*tmp_12+2.0*tmp_1*tmp_33*(tmp_19*tmp_19))*(tmp_19*tmp_29-tmp_14*tmp_33);
g(0,3) = g0_3;
}
{
double tmp_1 = x(3);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = p(1);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = x(0);
double tmp_8 = x(2);
double tmp_9 = -tmp_8;
double tmp_10 = tmp_7+tmp_9;
double tmp_11 = p(0);
double tmp_12 = -tmp_11*tmp_10;
double tmp_13 = tmp_12+tmp_6;
double tmp_14 = p(2);
double tmp_16 = p(3);
double tmp_17 = tmp_4*tmp_16;
double tmp_18 = -tmp_14*tmp_10;
double tmp_19 = tmp_18+tmp_17;
double tmp_22 = x(4);
double tmp_23 = -tmp_7;
double tmp_24 = tmp_23+tmp_22;
double tmp_25 = tmp_24*tmp_14;
double tmp_26 = x(5);
double tmp_27 = tmp_26+tmp_3;
double tmp_28 = tmp_27*tmp_16;
double tmp_29 = tmp_25+tmp_28;
double tmp_30 = tmp_24*tmp_11;
double tmp_31 = tmp_5*tmp_27;
double tmp_32 = tmp_31+tmp_30;
double g0_4 = -(tmp_11*tmp_19-tmp_14*tmp_13)*((tmp_13*tmp_13*tmp_13)*tmp_32+(tmp_29*tmp_29*tmp_29)*tmp_19+(tmp_29*tmp_29)*(tmp_19*tmp_19)+(tmp_13*tmp_13)*(tmp_32*tmp_32)+((tmp_29*tmp_29)*(tmp_29*tmp_29))+((tmp_13*tmp_13)*(tmp_13*tmp_13))+((tmp_19*tmp_19)*(tmp_19*tmp_19))+((tmp_32*tmp_32)*(tmp_32*tmp_32))+tmp_13*(tmp_32*tmp_32*tmp_32)+tmp_29*(tmp_19*tmp_19*tmp_19))+(tmp_29*tmp_13-tmp_19*tmp_32)*(3.0*tmp_14*(tmp_29*tmp_29)*tmp_19+2.0*tmp_11*(tmp_13*tmp_13)*tmp_32+tmp_14*(tmp_19*tmp_19*tmp_19)+4.0*tmp_11*(tmp_32*tmp_32*tmp_32)+2.0*tmp_14*tmp_29*(tmp_19*tmp_19)+3.0*tmp_11*tmp_13*(tmp_32*tmp_32)+tmp_11*(tmp_13*tmp_13*tmp_13)+4.0*tmp_14*(tmp_29*tmp_29*tmp_29));
g(0,4) = g0_4;
}
{
double tmp_1 = p(2);
double tmp_2 = x(4);
double tmp_3 = x(0);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = tmp_5*tmp_1;
double tmp_7 = p(3);
double tmp_8 = x(5);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = p(0);
double tmp_15 = tmp_14*tmp_5;
double tmp_16 = p(1);
double tmp_17 = tmp_11*tmp_16;
double tmp_18 = tmp_15+tmp_17;
double tmp_19 = x(3);
double tmp_20 = tmp_10+tmp_19;
double tmp_21 = tmp_20*tmp_16;
double tmp_22 = x(2);
double tmp_23 = -tmp_22;
double tmp_24 = tmp_23+tmp_3;
double tmp_25 = -tmp_24*tmp_14;
double tmp_26 = tmp_21+tmp_25;
double tmp_28 = tmp_20*tmp_7;
double tmp_29 = -tmp_24*tmp_1;
double tmp_30 = tmp_29+tmp_28;
double g0_5 = (tmp_18*(tmp_26*tmp_26*tmp_26)+((tmp_18*tmp_18)*(tmp_18*tmp_18))+(tmp_30*tmp_30)*(tmp_13*tmp_13)+((tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_30*tmp_30*tmp_30)*tmp_13+((tmp_30*tmp_30)*(tmp_30*tmp_30))+(tmp_18*tmp_18)*(tmp_26*tmp_26)+tmp_30*(tmp_13*tmp_13*tmp_13)+(tmp_18*tmp_18*tmp_18)*tmp_26+((tmp_26*tmp_26)*(tmp_26*tmp_26)))*(tmp_26*tmp_7-tmp_30*tmp_16)-(3.0*(tmp_18*tmp_18)*tmp_26*tmp_16+2.0*tmp_18*(tmp_26*tmp_26)*tmp_16+3.0*tmp_30*(tmp_13*tmp_13)*tmp_7+4.0*(tmp_18*tmp_18*tmp_18)*tmp_16+2.0*(tmp_30*tmp_30)*tmp_13*tmp_7+(tmp_30*tmp_30*tmp_30)*tmp_7+(tmp_26*tmp_26*tmp_26)*tmp_16+4.0*(tmp_13*tmp_13*tmp_13)*tmp_7)*(tmp_18*tmp_30-tmp_26*tmp_13);
g(0,5) = g0_5;
}
      }
   }
}
