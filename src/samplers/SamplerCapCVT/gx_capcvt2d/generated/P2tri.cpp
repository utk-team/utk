#include "P2tri.h"
namespace Geex {
   P2tri::P2tri() : Function(6,8,0){}
   void P2tri::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double f0 = x(0);
f(0) = f0;
}
{
double f1 = x(1);
f(1) = f1;
}
{
double tmp_1 = x(0);
double tmp_2 = x(1);
double tmp_3 = x(3);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_4+tmp_2;
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_1+tmp_7;
double tmp_10 = x(5);
double tmp_11 = -tmp_2;
double tmp_12 = tmp_11+tmp_10;
double tmp_14 = x(4);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_1;
double f2 = tmp_1+(tmp_5*((tmp_16*tmp_16)+(tmp_12*tmp_12))+tmp_12*((tmp_5*tmp_5)+(tmp_8*tmp_8)))/(-2.0000000000000000e+00*tmp_5*tmp_16-2.0000000000000000e+00*tmp_12*tmp_8);
f(2) = f2;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(4);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_2;
double f3 = tmp_2-1.0/(-2.0000000000000000e+00*tmp_12*tmp_4-2.0000000000000000e+00*tmp_8*tmp_16)*(tmp_12*((tmp_8*tmp_8)+(tmp_4*tmp_4))-((tmp_16*tmp_16)+(tmp_12*tmp_12))*tmp_8);
f(3) = f3;
}
{
double tmp_1 = x(0);
double tmp_2 = x(1);
double tmp_3 = x(3);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_1+tmp_7;
double tmp_10 = x(7);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_2+tmp_11;
double tmp_14 = x(6);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_1;
double f4 = (((tmp_16*tmp_16)+(tmp_12*tmp_12))*tmp_5-tmp_12*((tmp_8*tmp_8)+(tmp_5*tmp_5)))/(-2.0000000000000000e+00*tmp_5*tmp_16+2.0000000000000000e+00*tmp_12*tmp_8)+tmp_1;
f(4) = f4;
}
{
double tmp_1 = x(0);
double tmp_2 = x(6);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(7);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_1;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_5;
double f5 = 1.0/(2.0000000000000000e+00*tmp_12*tmp_8-2.0000000000000000e+00*tmp_16*tmp_4)*(((tmp_12*tmp_12)+(tmp_16*tmp_16))*tmp_4-tmp_12*((tmp_4*tmp_4)+(tmp_8*tmp_8)))+tmp_5;
f(5) = f5;
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
double g0_2 = 0.0;
g(0,2) = g0_2;
}
{
double g0_3 = 0.0;
g(0,3) = g0_3;
}
{
double g0_4 = 0.0;
g(0,4) = g0_4;
}
{
double g0_5 = 0.0;
g(0,5) = g0_5;
}
{
double g0_6 = 0.0;
g(0,6) = g0_6;
}
{
double g0_7 = 0.0;
g(0,7) = g0_7;
}
{
double g1_0 = 0.0;
g(1,0) = g1_0;
}
{
double g1_1 = 1.0;
g(1,1) = g1_1;
}
{
double g1_2 = 0.0;
g(1,2) = g1_2;
}
{
double g1_3 = 0.0;
g(1,3) = g1_3;
}
{
double g1_4 = 0.0;
g(1,4) = g1_4;
}
{
double g1_5 = 0.0;
g(1,5) = g1_5;
}
{
double g1_6 = 0.0;
g(1,6) = g1_6;
}
{
double g1_7 = 0.0;
g(1,7) = g1_7;
}
{
double tmp_1 = x(5);
double tmp_3 = x(3);
double tmp_6 = x(1);
double tmp_7 = -tmp_3;
double tmp_8 = tmp_6+tmp_7;
double tmp_9 = x(0);
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_9+tmp_11;
double tmp_14 = -tmp_6;
double tmp_15 = tmp_1+tmp_14;
double tmp_17 = x(4);
double tmp_18 = -tmp_17;
double tmp_19 = tmp_9+tmp_18;
double tmp_23 = -2.0000000000000000e+00*tmp_12*tmp_15;
double tmp_24 = -2.0000000000000000e+00*tmp_19*tmp_8;
double tmp_25 = tmp_24+tmp_23;
double tmp_26 = 1.0/(tmp_25);
double g2_0 = -(-2.0000000000000000e+00*tmp_1+2.0000000000000000e+00*tmp_3)*(tmp_15*((tmp_12*tmp_12)+(tmp_8*tmp_8))+((tmp_15*tmp_15)+(tmp_19*tmp_19))*tmp_8)*(tmp_26*tmp_26)+2.0*(tmp_12*tmp_15+tmp_19*tmp_8)*tmp_26+1.0;
g(2,0) = g2_0;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(4);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(3);
double tmp_14 = -tmp_13;
double tmp_15 = tmp_2+tmp_14;
double tmp_16 = -2.0000000000000000e+00*tmp_12*tmp_15;
double g2_1 = -1.0/(tmp_9+tmp_16)*((tmp_8*tmp_8)-(tmp_4*tmp_4)+(tmp_15*tmp_15)-(tmp_12*tmp_12))-(2.0000000000000000e+00*tmp_10-2.0000000000000000e+00*tmp_6)*(tmp_15*((tmp_4*tmp_4)+(tmp_12*tmp_12))+((tmp_8*tmp_8)+(tmp_15*tmp_15))*tmp_4)/pow(tmp_9+tmp_16,2.0);
g(2,1) = g2_1;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(4);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(3);
double tmp_14 = -tmp_13;
double tmp_15 = tmp_14+tmp_2;
double tmp_16 = -2.0000000000000000e+00*tmp_12*tmp_15;
double g2_2 = -2.0*1.0/(tmp_16+tmp_9)*tmp_8*tmp_4-(((tmp_15*tmp_15)+(tmp_8*tmp_8))*tmp_4+((tmp_4*tmp_4)+(tmp_12*tmp_12))*tmp_15)*(2.0000000000000000e+00*tmp_1-2.0000000000000000e+00*tmp_2)/pow(tmp_16+tmp_9,2.0);
g(2,2) = g2_2;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(3);
double tmp_6 = -tmp_5;
double tmp_7 = tmp_2+tmp_6;
double tmp_9 = x(0);
double tmp_10 = x(4);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_9+tmp_11;
double tmp_14 = x(2);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_9+tmp_15;
double tmp_17 = -2.0000000000000000e+00*tmp_4*tmp_16;
double tmp_18 = -2.0000000000000000e+00*tmp_12*tmp_7;
double g2_3 = -1.0/pow(tmp_18+tmp_17,2.0)*(tmp_4*((tmp_16*tmp_16)+(tmp_7*tmp_7))+((tmp_4*tmp_4)+(tmp_12*tmp_12))*tmp_7)*(2.0000000000000000e+00*tmp_9-2.0000000000000000e+00*tmp_10)-1.0/(tmp_18+tmp_17)*((tmp_4*tmp_4)+2.0*tmp_4*tmp_7+(tmp_12*tmp_12));
g(2,3) = g2_3;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(4);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_5;
double tmp_13 = x(3);
double tmp_14 = -tmp_13;
double tmp_15 = tmp_14+tmp_2;
double tmp_16 = -2.0000000000000000e+00*tmp_12*tmp_15;
double g2_4 = -2.0*tmp_12*tmp_15/(tmp_16+tmp_9)-(tmp_4*((tmp_15*tmp_15)+(tmp_8*tmp_8))+tmp_15*((tmp_12*tmp_12)+(tmp_4*tmp_4)))/pow(tmp_16+tmp_9,2.0)*(2.0000000000000000e+00*tmp_2-2.0000000000000000e+00*tmp_13);
g(2,4) = g2_4;
}
{
double tmp_1 = x(0);
double tmp_3 = x(2);
double tmp_6 = x(1);
double tmp_7 = x(3);
double tmp_8 = -tmp_7;
double tmp_9 = tmp_6+tmp_8;
double tmp_10 = -tmp_3;
double tmp_11 = tmp_10+tmp_1;
double tmp_13 = x(5);
double tmp_14 = -tmp_6;
double tmp_15 = tmp_13+tmp_14;
double tmp_17 = x(4);
double tmp_18 = -tmp_17;
double tmp_19 = tmp_18+tmp_1;
double tmp_23 = -2.0000000000000000e+00*tmp_15*tmp_11;
double tmp_24 = -2.0000000000000000e+00*tmp_19*tmp_9;
double tmp_25 = tmp_23+tmp_24;
double tmp_26 = 1.0/(tmp_25);
double g2_5 = -(tmp_26*tmp_26)*(2.0000000000000000e+00*tmp_3-2.0000000000000000e+00*tmp_1)*(tmp_9*((tmp_19*tmp_19)+(tmp_15*tmp_15))+((tmp_9*tmp_9)+(tmp_11*tmp_11))*tmp_15)+tmp_26*((tmp_9*tmp_9)+2.0*tmp_9*tmp_15+(tmp_11*tmp_11));
g(2,5) = g2_5;
}
{
double g2_6 = 0.0;
g(2,6) = g2_6;
}
{
double g2_7 = 0.0;
g(2,7) = g2_7;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_9 = -2.0000000000000000e+00*tmp_8*tmp_4;
double tmp_10 = x(4);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_5;
double tmp_13 = x(3);
double tmp_14 = -tmp_13;
double tmp_15 = tmp_2+tmp_14;
double tmp_16 = -2.0000000000000000e+00*tmp_12*tmp_15;
double g3_0 = -((tmp_4*tmp_4)+(tmp_12*tmp_12)-(tmp_15*tmp_15)-(tmp_8*tmp_8))/(tmp_9+tmp_16)+1.0/pow(tmp_9+tmp_16,2.0)*(-2.0000000000000000e+00*tmp_1+2.0000000000000000e+00*tmp_13)*(tmp_8*((tmp_4*tmp_4)+(tmp_12*tmp_12))-tmp_12*((tmp_15*tmp_15)+(tmp_8*tmp_8)));
g(3,0) = g3_0;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(4);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_2+tmp_15;
double g3_1 = -(1.0/2.0)*1.0/pow(tmp_12*tmp_4+tmp_8*tmp_16,2.0)*(tmp_12*((tmp_4*tmp_4)+(tmp_8*tmp_8))-tmp_8*((tmp_16*tmp_16)+(tmp_12*tmp_12)))*(tmp_7+tmp_10);
g(3,1) = g3_1;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(4);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_2+tmp_15;
double tmp_20 = tmp_4*tmp_12;
double tmp_21 = tmp_8*tmp_16;
double tmp_22 = tmp_20+tmp_21;
double tmp_23 = 1.0/(tmp_22);
double g3_2 = -tmp_23*((tmp_4*tmp_4)+(tmp_8*tmp_8)-2.0*tmp_8*tmp_12)/2.0+(((tmp_4*tmp_4)+(tmp_8*tmp_8))*tmp_12-tmp_8*((tmp_12*tmp_12)+(tmp_16*tmp_16)))*(tmp_23*tmp_23)*tmp_4/2.0;
g(3,2) = g3_2;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_9 = tmp_8*tmp_4;
double tmp_10 = x(4);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(3);
double tmp_14 = -tmp_13;
double tmp_15 = tmp_2+tmp_14;
double tmp_16 = tmp_12*tmp_15;
double g3_3 = 1.0/pow(tmp_9+tmp_16,2.0)*tmp_12*(((tmp_12*tmp_12)+(tmp_4*tmp_4))*tmp_8-tmp_12*((tmp_15*tmp_15)+(tmp_8*tmp_8)))/2.0+1.0/(tmp_9+tmp_16)*tmp_12*tmp_15;
g(3,3) = g3_3;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(4);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_2;
double tmp_20 = tmp_4*tmp_12;
double tmp_21 = tmp_16*tmp_8;
double tmp_22 = tmp_21+tmp_20;
double tmp_23 = 1.0/(tmp_22);
double g3_4 = -tmp_16*(((tmp_16*tmp_16)+(tmp_12*tmp_12))*tmp_8-tmp_12*((tmp_8*tmp_8)+(tmp_4*tmp_4)))*(tmp_23*tmp_23)/2.0-(2.0*tmp_12*tmp_8-(tmp_16*tmp_16)-(tmp_12*tmp_12))*tmp_23/2.0;
g(3,4) = g3_4;
}
{
double tmp_1 = x(5);
double tmp_2 = x(1);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(4);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_5;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_2;
double tmp_20 = tmp_4*tmp_12;
double tmp_21 = tmp_16*tmp_8;
double tmp_22 = tmp_21+tmp_20;
double tmp_23 = 1.0/(tmp_22);
double g3_5 = tmp_4*tmp_23*tmp_12+(tmp_23*tmp_23)*(((tmp_16*tmp_16)+(tmp_12*tmp_12))*tmp_8-((tmp_8*tmp_8)+(tmp_4*tmp_4))*tmp_12)*tmp_12/2.0;
g(3,5) = g3_5;
}
{
double g3_6 = 0.0;
g(3,6) = g3_6;
}
{
double g3_7 = 0.0;
g(3,7) = g3_7;
}
{
double tmp_1 = x(7);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_5 = x(1);
double tmp_6 = tmp_5+tmp_3;
double tmp_7 = x(0);
double tmp_8 = x(2);
double tmp_9 = -tmp_8;
double tmp_10 = tmp_9+tmp_7;
double tmp_12 = -tmp_1;
double tmp_13 = tmp_12+tmp_5;
double tmp_15 = x(6);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_7+tmp_16;
double g4_0 = (1.0/2.0)*(tmp_6*((tmp_17*tmp_17)+(tmp_13*tmp_13))-((tmp_10*tmp_10)+(tmp_6*tmp_6))*tmp_13)/pow(tmp_6*tmp_17-tmp_10*tmp_13,2.0)*(tmp_1+tmp_3);
g(4,0) = g4_0;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_10 = x(7);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_1+tmp_11;
double tmp_14 = x(6);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_5;
double tmp_20 = tmp_12*tmp_8;
double tmp_21 = -tmp_4*tmp_16;
double tmp_22 = tmp_20+tmp_21;
double tmp_23 = 1.0/(tmp_22);
double g4_1 = -tmp_23*((tmp_8*tmp_8)-(tmp_16*tmp_16)-(tmp_12*tmp_12)+(tmp_4*tmp_4))/2.0-(tmp_14+tmp_7)*(tmp_23*tmp_23)*(((tmp_16*tmp_16)+(tmp_12*tmp_12))*tmp_4-((tmp_8*tmp_8)+(tmp_4*tmp_4))*tmp_12)/2.0;
g(4,1) = g4_1;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(7);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_1;
double tmp_14 = x(6);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_5+tmp_15;
double tmp_20 = tmp_8*tmp_12;
double tmp_21 = -tmp_4*tmp_16;
double tmp_22 = tmp_20+tmp_21;
double tmp_23 = 1.0/(tmp_22);
double g4_2 = -(tmp_23*tmp_23)*(((tmp_4*tmp_4)+(tmp_8*tmp_8))*tmp_12-((tmp_16*tmp_16)+(tmp_12*tmp_12))*tmp_4)*tmp_12/2.0+tmp_8*tmp_23*tmp_12;
g(4,2) = g4_2;
}
{
double tmp_1 = x(1);
double tmp_2 = x(7);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(3);
double tmp_6 = -tmp_5;
double tmp_7 = tmp_1+tmp_6;
double tmp_9 = x(0);
double tmp_10 = x(6);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_9;
double tmp_16 = x(2);
double tmp_17 = -tmp_16;
double tmp_18 = tmp_17+tmp_9;
double tmp_19 = tmp_4*tmp_18;
double tmp_20 = -tmp_7*tmp_12;
double g4_3 = 1.0/pow(tmp_20+tmp_19,2.0)*(tmp_4*((tmp_7*tmp_7)+(tmp_18*tmp_18))-tmp_7*((tmp_12*tmp_12)+(tmp_4*tmp_4)))*tmp_12/2.0+(2.0*tmp_4*tmp_7-(tmp_12*tmp_12)-(tmp_4*tmp_4))/(tmp_20+tmp_19)/2.0;
g(4,3) = g4_3;
}
{
double g4_4 = 0.0;
g(4,4) = g4_4;
}
{
double g4_5 = 0.0;
g(4,5) = g4_5;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(7);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_1+tmp_11;
double tmp_14 = x(6);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_5+tmp_15;
double tmp_20 = tmp_8*tmp_12;
double tmp_21 = -tmp_4*tmp_16;
double tmp_22 = tmp_21+tmp_20;
double tmp_23 = 1.0/(tmp_22);
double g4_6 = (((tmp_4*tmp_4)+(tmp_8*tmp_8))*tmp_12-tmp_4*((tmp_12*tmp_12)+(tmp_16*tmp_16)))*tmp_4*(tmp_23*tmp_23)/2.0-tmp_4*tmp_23*tmp_16;
g(4,6) = g4_6;
}
{
double tmp_1 = x(1);
double tmp_2 = x(3);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(7);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_1;
double tmp_14 = x(6);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_5+tmp_15;
double tmp_20 = tmp_8*tmp_12;
double tmp_21 = -tmp_16*tmp_4;
double tmp_22 = tmp_21+tmp_20;
double tmp_23 = 1.0/(tmp_22);
double g4_7 = -tmp_8*(((tmp_4*tmp_4)+(tmp_8*tmp_8))*tmp_12-((tmp_12*tmp_12)+(tmp_16*tmp_16))*tmp_4)*(tmp_23*tmp_23)/2.0-tmp_23*(2.0*tmp_4*tmp_12-(tmp_4*tmp_4)-(tmp_8*tmp_8))/2.0;
g(4,7) = g4_7;
}
{
double tmp_1 = x(0);
double tmp_2 = x(6);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(7);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_1+tmp_11;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_5+tmp_15;
double tmp_21 = tmp_12*tmp_8;
double tmp_22 = -tmp_16*tmp_4;
double tmp_23 = tmp_22+tmp_21;
double tmp_24 = 1.0/(tmp_23);
double g5_0 = -((tmp_4*tmp_4)-(tmp_16*tmp_16)+(tmp_8*tmp_8)-(tmp_12*tmp_12))*tmp_24/2.0-(tmp_6+tmp_15)*(((tmp_4*tmp_4)+(tmp_8*tmp_8))*tmp_12-((tmp_16*tmp_16)+(tmp_12*tmp_12))*tmp_4)*(tmp_24*tmp_24)/2.0;
g(5,0) = g5_0;
}
{
double tmp_1 = x(0);
double tmp_2 = x(6);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(7);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_1;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_5;
double g5_1 = -(1.0/2.0)*(tmp_4*((tmp_16*tmp_16)+(tmp_12*tmp_12))-tmp_12*((tmp_8*tmp_8)+(tmp_4*tmp_4)))/pow(tmp_4*tmp_16-tmp_12*tmp_8,2.0)*(tmp_11+tmp_2);
g(5,1) = g5_1;
}
{
double tmp_1 = x(0);
double tmp_2 = x(6);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(1);
double tmp_6 = x(7);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_1;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_5+tmp_15;
double tmp_20 = tmp_8*tmp_12;
double tmp_21 = -tmp_4*tmp_16;
double tmp_22 = tmp_21+tmp_20;
double tmp_23 = 1.0/(tmp_22);
double g5_2 = tmp_23*((tmp_4*tmp_4)+(tmp_8*tmp_8)-2.0*tmp_4*tmp_12)/2.0+tmp_8*(((tmp_16*tmp_16)+(tmp_12*tmp_12))*tmp_4-tmp_12*((tmp_4*tmp_4)+(tmp_8*tmp_8)))*(tmp_23*tmp_23)/2.0;
g(5,2) = g5_2;
}
{
double tmp_1 = x(0);
double tmp_2 = x(6);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(1);
double tmp_6 = x(7);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_1+tmp_11;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_5;
double tmp_20 = tmp_12*tmp_8;
double tmp_21 = -tmp_4*tmp_16;
double tmp_22 = tmp_21+tmp_20;
double tmp_23 = 1.0/(tmp_22);
double g5_3 = -(tmp_23*tmp_23)*tmp_4*(tmp_4*((tmp_16*tmp_16)+(tmp_12*tmp_12))-((tmp_8*tmp_8)+(tmp_4*tmp_4))*tmp_12)/2.0-tmp_23*tmp_4*tmp_16;
g(5,3) = g5_3;
}
{
double g5_4 = 0.0;
g(5,4) = g5_4;
}
{
double g5_5 = 0.0;
g(5,5) = g5_5;
}
{
double tmp_1 = x(1);
double tmp_2 = x(7);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = x(0);
double tmp_6 = x(2);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_7+tmp_5;
double tmp_9 = tmp_4*tmp_8;
double tmp_10 = x(6);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_5+tmp_11;
double tmp_13 = x(3);
double tmp_14 = -tmp_13;
double tmp_15 = tmp_1+tmp_14;
double tmp_16 = -tmp_12*tmp_15;
double g5_6 = 1.0/pow(tmp_16+tmp_9,2.0)*(((tmp_4*tmp_4)+(tmp_12*tmp_12))*tmp_8-tmp_12*((tmp_8*tmp_8)+(tmp_15*tmp_15)))*tmp_15/2.0-1.0/(tmp_16+tmp_9)*((tmp_8*tmp_8)-2.0*tmp_12*tmp_8+(tmp_15*tmp_15))/2.0;
g(5,6) = g5_6;
}
{
double tmp_1 = x(0);
double tmp_2 = x(6);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = x(1);
double tmp_6 = x(7);
double tmp_7 = -tmp_6;
double tmp_8 = tmp_5+tmp_7;
double tmp_10 = x(2);
double tmp_11 = -tmp_10;
double tmp_12 = tmp_11+tmp_1;
double tmp_14 = x(3);
double tmp_15 = -tmp_14;
double tmp_16 = tmp_15+tmp_5;
double tmp_20 = tmp_12*tmp_8;
double tmp_21 = -tmp_16*tmp_4;
double tmp_22 = tmp_21+tmp_20;
double tmp_23 = 1.0/(tmp_22);
double g5_7 = -(tmp_12*((tmp_4*tmp_4)+(tmp_8*tmp_8))-((tmp_12*tmp_12)+(tmp_16*tmp_16))*tmp_4)*tmp_12*(tmp_23*tmp_23)/2.0+tmp_12*tmp_8*tmp_23;
g(5,7) = g5_7;
}
      }
   }
}
