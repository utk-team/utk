#include "Ltheta10.h"
namespace Geex {
   Ltheta10::Ltheta10() : Function(1,7,0){}
   void Ltheta10::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(2);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = sin(tmp_1);
double tmp_13 = tmp_12*tmp_11;
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = x(4);
double tmp_16 = tmp_15+tmp_5;
double tmp_17 = tmp_2*tmp_16;
double tmp_18 = x(5);
double tmp_19 = -tmp_18;
double tmp_20 = tmp_19+tmp_8;
double tmp_21 = tmp_12*tmp_20;
double tmp_22 = tmp_21+tmp_17;
double tmp_24 = tmp_2*tmp_20;
double tmp_25 = -tmp_12*tmp_16;
double tmp_26 = tmp_25+tmp_24;
double tmp_27 = tmp_2*tmp_11;
double tmp_28 = -tmp_12*tmp_6;
double tmp_29 = tmp_28+tmp_27;
double f0 = (tmp_26*(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_14+(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))+(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29*tmp_29)+((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+(tmp_22*tmp_22)*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*(tmp_29*tmp_29)+(tmp_26*tmp_26*tmp_26)*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+((tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22)))+((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*((tmp_14*tmp_14)*(tmp_14*tmp_14))+((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))+(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*(tmp_14*tmp_14)+(tmp_26*tmp_26)*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*(tmp_14*tmp_14*tmp_14)+tmp_22*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))+(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_29+((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*((tmp_29*tmp_29)*(tmp_29*tmp_29))+(tmp_22*tmp_22*tmp_22)*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+((tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29)))+((tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))))*(tmp_22*tmp_29-tmp_14*tmp_26);
f(0) = f0;
}
      }
      if(do_g) {
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(2);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = sin(tmp_1);
double tmp_13 = tmp_12*tmp_11;
double tmp_14 = tmp_13+tmp_7;
double tmp_15 = x(4);
double tmp_16 = tmp_15+tmp_5;
double tmp_17 = tmp_16*tmp_2;
double tmp_18 = x(5);
double tmp_19 = -tmp_18;
double tmp_20 = tmp_19+tmp_8;
double tmp_21 = tmp_12*tmp_20;
double tmp_22 = tmp_21+tmp_17;
double tmp_24 = tmp_2*tmp_20;
double tmp_25 = -tmp_16*tmp_12;
double tmp_26 = tmp_24+tmp_25;
double tmp_27 = tmp_2*tmp_11;
double tmp_28 = -tmp_6*tmp_12;
double tmp_29 = tmp_27+tmp_28;
double g0_0 = (((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*((tmp_29*tmp_29)*(tmp_29*tmp_29))+((tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29)))+((tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22)))+((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))+(tmp_22*tmp_22*tmp_22)*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*(tmp_14*tmp_14)+(tmp_26*tmp_26)*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+((tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26)))+(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_29+(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))+tmp_22*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*(tmp_14*tmp_14*tmp_14)+((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+(tmp_26*tmp_26*tmp_26)*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*((tmp_14*tmp_14)*(tmp_14*tmp_14))+(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_14+(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))+tmp_26*(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29*tmp_29)+(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*(tmp_29*tmp_29)+(tmp_22*tmp_22)*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14))))*(tmp_2*tmp_26-tmp_12*tmp_14+tmp_22*tmp_12-tmp_2*tmp_29)+11.0*(tmp_22*tmp_29-tmp_26*tmp_14)*(tmp_12*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_29*tmp_29)*(tmp_29*tmp_29))+tmp_12*((tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))-(tmp_22*tmp_22*tmp_22)*tmp_2*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))-tmp_22*tmp_2*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+tmp_12*tmp_26*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))-(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*tmp_2*(tmp_14*tmp_14)+tmp_12*(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))-tmp_2*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+tmp_12*(tmp_26*tmp_26*tmp_26)*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))-((tmp_22*tmp_22)*(tmp_22*tmp_22))*tmp_2*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))-(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_2+tmp_12*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_29-(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_2*tmp_14-(tmp_22*tmp_22)*tmp_2*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+tmp_12*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29*tmp_29)-((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*tmp_2*(tmp_14*tmp_14*tmp_14)+tmp_12*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29)+tmp_12*(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+tmp_12*(tmp_26*tmp_26)*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))-(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*tmp_2*((tmp_14*tmp_14)*(tmp_14*tmp_14)));
g(0,0) = g0_0;
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
double tmp_14 = tmp_13+tmp_7;
double tmp_15 = x(3);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_9+tmp_16;
double tmp_18 = tmp_2*tmp_17;
double tmp_19 = x(2);
double tmp_20 = tmp_5+tmp_19;
double tmp_21 = -tmp_20*tmp_8;
double tmp_22 = tmp_18+tmp_21;
double tmp_24 = tmp_2*tmp_20;
double tmp_25 = tmp_17*tmp_8;
double tmp_26 = tmp_24+tmp_25;
double tmp_27 = tmp_12*tmp_2;
double tmp_28 = -tmp_6*tmp_8;
double tmp_29 = tmp_27+tmp_28;
double g0_1 = (((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*((tmp_22*tmp_22)*(tmp_22*tmp_22))+(tmp_14*tmp_14*tmp_14)*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+((tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29)))+(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_26*tmp_26*tmp_26)+(tmp_29*tmp_29)*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+tmp_14*(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))+((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*(tmp_26*tmp_26)+(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))+(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*tmp_22+((tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26)))+(tmp_29*tmp_29*tmp_29)*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))+((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*((tmp_26*tmp_26)*(tmp_26*tmp_26))+((tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22)))+(tmp_14*tmp_14)*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))+(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*(tmp_22*tmp_22*tmp_22)+tmp_29*(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))+(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_26+((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*(tmp_22*tmp_22))*(tmp_8*tmp_22-tmp_2*tmp_26-tmp_8*tmp_29+tmp_2*tmp_14)-11.0*(tmp_26*tmp_29-tmp_14*tmp_22)*(tmp_2*(tmp_29*tmp_29*tmp_29)*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+tmp_2*(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+(tmp_14*tmp_14)*tmp_8*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_8*tmp_26+((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*tmp_8*(tmp_26*tmp_26*tmp_26)+tmp_2*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*tmp_22+tmp_2*tmp_29*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*tmp_8*((tmp_26*tmp_26)*(tmp_26*tmp_26))+tmp_2*(tmp_29*tmp_29)*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*tmp_8*(tmp_26*tmp_26)+tmp_14*tmp_8*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))+tmp_2*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*(tmp_22*tmp_22*tmp_22)+((tmp_14*tmp_14)*(tmp_14*tmp_14))*tmp_8*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))+tmp_2*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*(tmp_22*tmp_22)+tmp_8*(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))+(tmp_14*tmp_14*tmp_14)*tmp_8*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+tmp_2*(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+tmp_2*((tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))+tmp_2*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_22*tmp_22)*(tmp_22*tmp_22))+(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_8);
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
double tmp_16 = tmp_12*tmp_2;
double tmp_17 = -tmp_6*tmp_8;
double tmp_18 = tmp_17+tmp_16;
double tmp_21 = x(2);
double tmp_22 = tmp_21+tmp_5;
double tmp_23 = tmp_22*tmp_2;
double tmp_24 = x(3);
double tmp_25 = -tmp_24;
double tmp_26 = tmp_25+tmp_9;
double tmp_27 = tmp_8*tmp_26;
double tmp_28 = tmp_27+tmp_23;
double tmp_30 = tmp_2*tmp_26;
double tmp_31 = -tmp_22*tmp_8;
double tmp_32 = tmp_31+tmp_30;
double g0_2 = -(tmp_18*tmp_2+tmp_8*tmp_14)*((tmp_32*(tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*(tmp_18*tmp_18*tmp_18)+tmp_32*(tmp_18*((tmp_18*tmp_18)*(tmp_18*tmp_18))*((tmp_18*tmp_18)*(tmp_18*tmp_18)))+(tmp_28*tmp_28)*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))+(tmp_28*((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_28*tmp_28)*(tmp_28*tmp_28)))*tmp_14+(((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_32*tmp_32)*(tmp_32*tmp_32)))*(tmp_18*tmp_18)+((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))*((tmp_14*tmp_14)*(tmp_14*tmp_14))+(tmp_32*tmp_32*tmp_32)*(tmp_18*(tmp_18*tmp_18*tmp_18)*(tmp_18*tmp_18*tmp_18))+((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))+((tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32)))+(tmp_28*(tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))*(tmp_14*tmp_14*tmp_14)+tmp_28*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+(tmp_32*tmp_32)*(((tmp_18*tmp_18)*(tmp_18*tmp_18))*((tmp_18*tmp_18)*(tmp_18*tmp_18)))+((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_18*tmp_18*tmp_18)*(tmp_18*tmp_18*tmp_18))+(tmp_32*((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_32*tmp_32)*(tmp_32*tmp_32)))*tmp_18+(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_18*(tmp_18*tmp_18)*(tmp_18*tmp_18))+(((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_28*tmp_28)*(tmp_28*tmp_28)))*(tmp_14*tmp_14)+(tmp_28*tmp_28*tmp_28)*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*((tmp_18*tmp_18)*(tmp_18*tmp_18))+((tmp_18*(tmp_18*tmp_18)*(tmp_18*tmp_18))*(tmp_18*(tmp_18*tmp_18)*(tmp_18*tmp_18)))+((tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))))-(tmp_18*tmp_28-tmp_32*tmp_14)*(2.0*tmp_28*tmp_2*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+8.0*(tmp_28*(tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))*tmp_2*(tmp_14*tmp_14)+5.0*((tmp_28*tmp_28)*(tmp_28*tmp_28))*tmp_2*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))-2.0*tmp_32*(((tmp_18*tmp_18)*(tmp_18*tmp_18))*((tmp_18*tmp_18)*(tmp_18*tmp_18)))*tmp_8+3.0*(tmp_28*tmp_28)*tmp_2*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+9.0*(((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_28*tmp_28)*(tmp_28*tmp_28)))*tmp_2*tmp_14+7.0*((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))*tmp_2*(tmp_14*tmp_14*tmp_14)-9.0*(((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_32*tmp_32)*(tmp_32*tmp_32)))*tmp_18*tmp_8-4.0*(tmp_32*tmp_32*tmp_32)*((tmp_18*tmp_18*tmp_18)*(tmp_18*tmp_18*tmp_18))*tmp_8+tmp_2*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+6.0*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))*tmp_2*((tmp_14*tmp_14)*(tmp_14*tmp_14))-8.0*(tmp_32*(tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*(tmp_18*tmp_18)*tmp_8-6.0*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_18*tmp_18)*(tmp_18*tmp_18))*tmp_8-7.0*((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*(tmp_18*tmp_18*tmp_18)*tmp_8-(tmp_18*((tmp_18*tmp_18)*(tmp_18*tmp_18))*((tmp_18*tmp_18)*(tmp_18*tmp_18)))*tmp_8+10.0*(tmp_28*((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_28*tmp_28)*(tmp_28*tmp_28)))*tmp_2-10.0*(tmp_32*((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_32*tmp_32)*(tmp_32*tmp_32)))*tmp_8+4.0*(tmp_28*tmp_28*tmp_28)*tmp_2*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))-3.0*(tmp_32*tmp_32)*(tmp_18*(tmp_18*tmp_18*tmp_18)*(tmp_18*tmp_18*tmp_18))*tmp_8-5.0*((tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_18*(tmp_18*tmp_18)*(tmp_18*tmp_18))*tmp_8);
g(0,2) = g0_2;
}
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(1);
double tmp_4 = x(5);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_5+tmp_3;
double tmp_7 = tmp_2*tmp_6;
double tmp_8 = x(4);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = sin(tmp_1);
double tmp_13 = -tmp_11*tmp_12;
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = x(3);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_3+tmp_16;
double tmp_18 = tmp_2*tmp_17;
double tmp_19 = x(2);
double tmp_20 = tmp_10+tmp_19;
double tmp_21 = -tmp_20*tmp_12;
double tmp_22 = tmp_18+tmp_21;
double tmp_24 = tmp_2*tmp_20;
double tmp_25 = tmp_17*tmp_12;
double tmp_26 = tmp_24+tmp_25;
double tmp_27 = tmp_2*tmp_11;
double tmp_28 = tmp_6*tmp_12;
double tmp_29 = tmp_28+tmp_27;
double g0_3 = -(tmp_2*tmp_29-tmp_14*tmp_12)*(((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*((tmp_22*tmp_22)*(tmp_22*tmp_22))+((tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26)))+((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*((tmp_29*tmp_29)*(tmp_29*tmp_29))+((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+(tmp_14*tmp_14)*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))+(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_29+((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+(tmp_26*tmp_26)*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_22+(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))+(tmp_14*tmp_14*tmp_14)*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+((tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22)))+((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))+(tmp_26*tmp_26*tmp_26)*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+tmp_14*(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_22*tmp_22*tmp_22)+((tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29)))+(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*(tmp_29*tmp_29)+tmp_26*(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29*tmp_29)+(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*(tmp_22*tmp_22))+(3.0*(tmp_26*tmp_26)*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*tmp_12+10.0*(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_12+9.0*tmp_2*tmp_14*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+tmp_2*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+5.0*((tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*tmp_12+10.0*tmp_2*(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+2.0*tmp_2*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_22+7.0*tmp_2*(tmp_14*tmp_14*tmp_14)*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+7.0*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29*tmp_29)*tmp_12+3.0*tmp_2*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_22*tmp_22)+4.0*tmp_2*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_22*tmp_22*tmp_22)+8.0*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29)*tmp_12+6.0*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_29*tmp_29)*(tmp_29*tmp_29))*tmp_12+8.0*tmp_2*(tmp_14*tmp_14)*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+4.0*(tmp_26*tmp_26*tmp_26)*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*tmp_12+(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*tmp_12+9.0*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_29*tmp_12+5.0*tmp_2*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_22*tmp_22)*(tmp_22*tmp_22))+2.0*tmp_26*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*tmp_12+6.0*tmp_2*((tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22)))*(tmp_14*tmp_26-tmp_29*tmp_22);
g(0,3) = g0_3;
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
double tmp_18 = tmp_17*tmp_2;
double tmp_19 = x(2);
double tmp_20 = tmp_19+tmp_5;
double tmp_21 = -tmp_20*tmp_8;
double tmp_22 = tmp_18+tmp_21;
double tmp_24 = tmp_20*tmp_2;
double tmp_25 = tmp_17*tmp_8;
double tmp_26 = tmp_25+tmp_24;
double tmp_27 = tmp_12*tmp_2;
double tmp_28 = -tmp_6*tmp_8;
double tmp_29 = tmp_28+tmp_27;
double g0_4 = (tmp_22*tmp_2+tmp_26*tmp_8)*((tmp_14*tmp_14)*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))+((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_26+(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))+((tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26)))+((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*((tmp_22*tmp_22)*(tmp_22*tmp_22))+((tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22)))+(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))+(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*tmp_22+((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*((tmp_26*tmp_26)*(tmp_26*tmp_26))+((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))+(tmp_29*tmp_29)*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+tmp_14*(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))+(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_26*tmp_26*tmp_26)+(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*(tmp_26*tmp_26)+(tmp_29*tmp_29*tmp_29)*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*(tmp_22*tmp_22)+(tmp_14*tmp_14*tmp_14)*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+((tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29)))+tmp_29*(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*(tmp_22*tmp_22*tmp_22))-(3.0*(tmp_29*tmp_29)*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*tmp_8-4.0*(tmp_14*tmp_14*tmp_14)*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*tmp_2-(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_2+5.0*((tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*tmp_8-9.0*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_26*tmp_2-2.0*tmp_14*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_2+7.0*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*(tmp_22*tmp_22*tmp_22)*tmp_8+8.0*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))*(tmp_22*tmp_22)*tmp_8+6.0*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_22*tmp_22)*(tmp_22*tmp_22))*tmp_8-5.0*((tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_2+(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_8+4.0*(tmp_29*tmp_29*tmp_29)*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*tmp_8-3.0*(tmp_14*tmp_14)*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*tmp_2+9.0*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*tmp_22*tmp_8+10.0*(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))*tmp_8-7.0*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_26*tmp_26*tmp_26)*tmp_2-10.0*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_2+2.0*tmp_29*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_8-6.0*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_2-8.0*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_26*tmp_26)*tmp_2)*(tmp_14*tmp_22-tmp_26*tmp_29);
g(0,4) = g0_4;
}
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(2);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_3+tmp_5;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = sin(tmp_1);
double tmp_13 = tmp_12*tmp_11;
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = x(4);
double tmp_16 = tmp_15+tmp_5;
double tmp_17 = tmp_16*tmp_2;
double tmp_18 = x(5);
double tmp_19 = -tmp_18;
double tmp_20 = tmp_19+tmp_8;
double tmp_21 = tmp_12*tmp_20;
double tmp_22 = tmp_21+tmp_17;
double tmp_24 = tmp_2*tmp_20;
double tmp_25 = -tmp_16*tmp_12;
double tmp_26 = tmp_25+tmp_24;
double tmp_27 = tmp_2*tmp_11;
double tmp_28 = -tmp_6*tmp_12;
double tmp_29 = tmp_28+tmp_27;
double g0_5 = -((tmp_26*tmp_26*tmp_26)*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+(tmp_22*tmp_22)*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))+(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))+(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_14+(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29*tmp_29)+((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*((tmp_14*tmp_14)*(tmp_14*tmp_14))+tmp_26*(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+((tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22)))+(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*(tmp_29*tmp_29)+((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*((tmp_29*tmp_29)*(tmp_29*tmp_29))+(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*(tmp_14*tmp_14*tmp_14)+tmp_22*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*(tmp_14*tmp_14)+(tmp_22*tmp_22*tmp_22)*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+((tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26)))+(tmp_26*tmp_26)*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+((tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29)))+(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_29+(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29)))*(tmp_12*tmp_29-tmp_2*tmp_14)-(6.0*tmp_2*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_29*tmp_29)*(tmp_29*tmp_29))+6.0*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))*tmp_12*((tmp_14*tmp_14)*(tmp_14*tmp_14))+7.0*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*tmp_12*(tmp_14*tmp_14*tmp_14)+9.0*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_12*tmp_14+3.0*(tmp_22*tmp_22)*tmp_12*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+5.0*tmp_2*((tmp_26*tmp_26)*(tmp_26*tmp_26))*(tmp_29*(tmp_29*tmp_29)*(tmp_29*tmp_29))+5.0*((tmp_22*tmp_22)*(tmp_22*tmp_22))*tmp_12*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))+10.0*tmp_2*(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))+2.0*tmp_22*tmp_12*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+8.0*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))*tmp_12*(tmp_14*tmp_14)+tmp_2*(tmp_29*((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+tmp_12*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))+4.0*(tmp_22*tmp_22*tmp_22)*tmp_12*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))+8.0*tmp_2*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29)+7.0*tmp_2*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*(tmp_29*tmp_29*tmp_29)+3.0*tmp_2*(tmp_26*tmp_26)*(tmp_29*(tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29))+10.0*(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))*tmp_12+2.0*tmp_2*tmp_26*(((tmp_29*tmp_29)*(tmp_29*tmp_29))*((tmp_29*tmp_29)*(tmp_29*tmp_29)))+9.0*tmp_2*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_29+4.0*tmp_2*(tmp_26*tmp_26*tmp_26)*((tmp_29*tmp_29*tmp_29)*(tmp_29*tmp_29*tmp_29)))*(tmp_22*tmp_29-tmp_14*tmp_26);
g(0,5) = g0_5;
}
{
double tmp_1 = x(6);
double tmp_2 = cos(tmp_1);
double tmp_3 = x(2);
double tmp_4 = x(0);
double tmp_5 = -tmp_4;
double tmp_6 = tmp_5+tmp_3;
double tmp_7 = tmp_6*tmp_2;
double tmp_8 = x(1);
double tmp_9 = x(3);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = sin(tmp_1);
double tmp_13 = tmp_11*tmp_12;
double tmp_14 = tmp_7+tmp_13;
double tmp_15 = x(5);
double tmp_16 = -tmp_15;
double tmp_17 = tmp_8+tmp_16;
double tmp_18 = tmp_2*tmp_17;
double tmp_19 = x(4);
double tmp_20 = tmp_5+tmp_19;
double tmp_21 = -tmp_20*tmp_12;
double tmp_22 = tmp_21+tmp_18;
double tmp_24 = tmp_2*tmp_20;
double tmp_25 = tmp_17*tmp_12;
double tmp_26 = tmp_24+tmp_25;
double tmp_28 = tmp_11*tmp_2;
double tmp_29 = -tmp_6*tmp_12;
double tmp_30 = tmp_29+tmp_28;
double g0_6 = -(tmp_14*tmp_22-tmp_30*tmp_26)*(3.0*tmp_30*(tmp_14*tmp_14)*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))+9.0*tmp_14*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_22-2.0*tmp_30*tmp_14*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))-10.0*(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_14-8.0*(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_14*(tmp_22*tmp_22)-7.0*(tmp_30*tmp_30*tmp_30)*tmp_26*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+7.0*tmp_30*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_26*tmp_26*tmp_26)-5.0*((tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_14*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))+10.0*(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))*tmp_22+8.0*tmp_30*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_26*tmp_26)-3.0*(tmp_30*tmp_30)*tmp_14*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))-tmp_14*(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+4.0*tmp_30*(tmp_14*tmp_14*tmp_14)*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))-9.0*(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_14*tmp_22-7.0*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_14*(tmp_22*tmp_22*tmp_22)+7.0*(tmp_14*tmp_14*tmp_14)*((tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*tmp_22+9.0*tmp_30*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_26+2.0*tmp_30*tmp_14*(((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))-(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_26+2.0*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_26*tmp_22-6.0*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_14*((tmp_22*tmp_22)*(tmp_22*tmp_22))-4.0*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_26*(tmp_22*tmp_22*tmp_22)-8.0*(tmp_30*tmp_30)*tmp_26*(tmp_22*(tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))-10.0*tmp_26*(tmp_22*((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))-2.0*(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_26*tmp_22+5.0*tmp_30*((tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))+4.0*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_26*tmp_26*tmp_26)*tmp_22+5.0*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_22+3.0*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_26*tmp_26)*tmp_22+10.0*tmp_30*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))-5.0*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_26*((tmp_22*tmp_22)*(tmp_22*tmp_22))+6.0*tmp_30*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_26*tmp_26)*(tmp_26*tmp_26))+6.0*((tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_26*(tmp_26*tmp_26)*(tmp_26*tmp_26))*tmp_22-9.0*tmp_30*tmp_26*(((tmp_22*tmp_22)*(tmp_22*tmp_22))*((tmp_22*tmp_22)*(tmp_22*tmp_22)))+tmp_30*(tmp_26*((tmp_26*tmp_26)*(tmp_26*tmp_26))*((tmp_26*tmp_26)*(tmp_26*tmp_26)))-3.0*(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_26*(tmp_22*tmp_22)+8.0*(tmp_14*tmp_14)*(tmp_26*(tmp_26*tmp_26*tmp_26)*(tmp_26*tmp_26*tmp_26))*tmp_22-6.0*((tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_26*(tmp_22*(tmp_22*tmp_22)*(tmp_22*tmp_22))-4.0*(tmp_30*tmp_30*tmp_30)*tmp_14*((tmp_22*tmp_22*tmp_22)*(tmp_22*tmp_22*tmp_22))+(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_22);
g(0,6) = g0_6;
}
      }
   }
}
