#include "L12.h"
namespace Geex {
   L12::L12() : Function(1,6,4){}
   void L12::eval(bool do_f, bool do_g, bool do_H) {
      if(do_f) {
{
double tmp_1 = x(0);
double tmp_2 = x(2);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = p(0);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(1);
double tmp_8 = x(3);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = -tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(5);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = p(3);
double tmp_17 = tmp_15*tmp_16;
double tmp_18 = x(4);
double tmp_19 = -tmp_1;
double tmp_20 = tmp_18+tmp_19;
double tmp_21 = p(2);
double tmp_22 = tmp_20*tmp_21;
double tmp_23 = tmp_17+tmp_22;
double tmp_25 = tmp_11*tmp_16;
double tmp_26 = -tmp_4*tmp_21;
double tmp_27 = tmp_26+tmp_25;
double tmp_28 = tmp_15*tmp_7;
double tmp_29 = tmp_20*tmp_5;
double tmp_30 = tmp_29+tmp_28;
double f0 = (tmp_23*tmp_13+tmp_27*tmp_30)*((tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))*tmp_30-(((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))+(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))-((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))*(tmp_27*tmp_27)+(tmp_13*tmp_13*tmp_13)*(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))-(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))*((tmp_27*tmp_27)*(tmp_27*tmp_27))-(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))*(tmp_27*tmp_27*tmp_27)-(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))-((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))-(((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23)))-((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))*(tmp_30*tmp_30)-(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))-(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_27-((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))+(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))+(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))*(tmp_30*tmp_30*tmp_30)-(tmp_23*tmp_23*tmp_23)*(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))-(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))*((tmp_30*tmp_30)*(tmp_30*tmp_30))+tmp_13*(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))-(((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27)))-((tmp_13*tmp_13)*(tmp_13*tmp_13))*(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))-(((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30)))-(tmp_23*tmp_23)*((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))-tmp_23*(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))-(tmp_13*tmp_13)*((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))-((tmp_23*tmp_23)*(tmp_23*tmp_23))*(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27))));
f(0) = f0;
}
      }
      if(do_g) {
{
double tmp_1 = p(0);
double tmp_2 = x(5);
double tmp_3 = x(1);
double tmp_4 = -tmp_3;
double tmp_5 = tmp_2+tmp_4;
double tmp_6 = p(3);
double tmp_7 = tmp_6*tmp_5;
double tmp_8 = x(4);
double tmp_9 = x(0);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = p(2);
double tmp_13 = tmp_12*tmp_11;
double tmp_14 = tmp_7+tmp_13;
double tmp_16 = p(1);
double tmp_17 = tmp_5*tmp_16;
double tmp_18 = tmp_11*tmp_1;
double tmp_19 = tmp_18+tmp_17;
double tmp_21 = x(3);
double tmp_22 = tmp_21+tmp_4;
double tmp_23 = tmp_22*tmp_6;
double tmp_24 = x(2);
double tmp_25 = -tmp_24;
double tmp_26 = tmp_25+tmp_9;
double tmp_27 = -tmp_12*tmp_26;
double tmp_28 = tmp_27+tmp_23;
double tmp_30 = tmp_1*tmp_26;
double tmp_31 = -tmp_22*tmp_16;
double tmp_32 = tmp_30+tmp_31;
double g0_0 = 13.0*(tmp_14*tmp_32+tmp_28*tmp_19)*(tmp_12*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))+tmp_12*(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*(tmp_28*tmp_28)+tmp_12*(tmp_14*tmp_14*tmp_14)*(((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_28*tmp_28)*(tmp_28*tmp_28)))+((tmp_32*tmp_32)*(tmp_32*tmp_32))*tmp_1*(tmp_19*(tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19))+tmp_12*(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*((tmp_28*tmp_28)*(tmp_28*tmp_28))+(((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_32*tmp_32)*(tmp_32*tmp_32)))*tmp_1*(tmp_19*tmp_19*tmp_19)+tmp_12*(tmp_14*tmp_14)*(tmp_28*((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_28*tmp_28)*(tmp_28*tmp_28)))+tmp_12*(tmp_14*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))+tmp_12*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))+tmp_12*tmp_14*((tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28)))-(tmp_32*tmp_32*tmp_32)*tmp_1*(((tmp_19*tmp_19)*(tmp_19*tmp_19))*((tmp_19*tmp_19)*(tmp_19*tmp_19)))+tmp_12*((tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_28*(tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))+tmp_12*((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_28-(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*tmp_1*((tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19))+((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*tmp_1*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))-(tmp_32*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32)))*tmp_1+(tmp_32*tmp_32)*tmp_1*(tmp_19*((tmp_19*tmp_19)*(tmp_19*tmp_19))*((tmp_19*tmp_19)*(tmp_19*tmp_19)))+tmp_12*(tmp_28*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28)))+((tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32)))*tmp_1*tmp_19+tmp_12*(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*(tmp_28*tmp_28*tmp_28)-tmp_32*tmp_1*((tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19)))+tmp_1*(tmp_19*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19)))-(tmp_32*((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_32*tmp_32)*(tmp_32*tmp_32)))*tmp_1*(tmp_19*tmp_19)-(tmp_32*(tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*tmp_1*((tmp_19*tmp_19)*(tmp_19*tmp_19)))-((tmp_32*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32)))*tmp_19-(((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32)))+(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_19*(tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19))+(tmp_32*tmp_32*tmp_32)*(tmp_19*((tmp_19*tmp_19)*(tmp_19*tmp_19))*((tmp_19*tmp_19)*(tmp_19*tmp_19)))-(tmp_14*tmp_14)*((tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28)))-((tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32))*(tmp_32*(tmp_32*tmp_32)*(tmp_32*tmp_32)))*(tmp_19*tmp_19)-tmp_14*(tmp_28*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28)))-((tmp_14*tmp_14)*(tmp_14*tmp_14))*(((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_28*tmp_28)*(tmp_28*tmp_28)))-((tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*((tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19))+(tmp_32*(tmp_32*tmp_32*tmp_32)*(tmp_32*tmp_32*tmp_32))*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))-(((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14)))-(((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_32*tmp_32)*(tmp_32*tmp_32)))*((tmp_19*tmp_19)*(tmp_19*tmp_19))+(tmp_32*((tmp_32*tmp_32)*(tmp_32*tmp_32))*((tmp_32*tmp_32)*(tmp_32*tmp_32)))*(tmp_19*tmp_19*tmp_19)+tmp_32*(tmp_19*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19)))-((tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))*(tmp_28*tmp_28)-(((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))*((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28)))-((tmp_32*tmp_32)*(tmp_32*tmp_32))*(((tmp_19*tmp_19)*(tmp_19*tmp_19))*((tmp_19*tmp_19)*(tmp_19*tmp_19)))-(tmp_14*((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*(tmp_28*tmp_28*tmp_28)-(((tmp_14*tmp_14)*(tmp_14*tmp_14))*((tmp_14*tmp_14)*(tmp_14*tmp_14)))*((tmp_28*tmp_28)*(tmp_28*tmp_28))-(((tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19))*((tmp_19*tmp_19*tmp_19)*(tmp_19*tmp_19*tmp_19)))-(tmp_14*(tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*(tmp_28*(tmp_28*tmp_28)*(tmp_28*tmp_28))-((tmp_14*tmp_14*tmp_14)*(tmp_14*tmp_14*tmp_14))*((tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))-(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_28*(tmp_28*tmp_28*tmp_28)*(tmp_28*tmp_28*tmp_28))-(tmp_14*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14))*(tmp_14*(tmp_14*tmp_14)*(tmp_14*tmp_14)))*tmp_28-(tmp_14*tmp_14*tmp_14)*(tmp_28*((tmp_28*tmp_28)*(tmp_28*tmp_28))*((tmp_28*tmp_28)*(tmp_28*tmp_28)))-(tmp_32*tmp_32)*((tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))*(tmp_19*(tmp_19*tmp_19)*(tmp_19*tmp_19))))*(tmp_12*tmp_32-tmp_14*tmp_1+tmp_1*tmp_28+tmp_12*tmp_19);
g(0,0) = g0_0;
}
{
double tmp_1 = x(0);
double tmp_2 = x(2);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = p(0);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(1);
double tmp_8 = x(3);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = -tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(5);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = p(3);
double tmp_17 = tmp_16*tmp_15;
double tmp_18 = x(4);
double tmp_19 = -tmp_1;
double tmp_20 = tmp_19+tmp_18;
double tmp_21 = p(2);
double tmp_22 = tmp_21*tmp_20;
double tmp_23 = tmp_22+tmp_17;
double tmp_25 = tmp_16*tmp_11;
double tmp_26 = -tmp_21*tmp_4;
double tmp_27 = tmp_25+tmp_26;
double tmp_28 = tmp_15*tmp_7;
double tmp_29 = tmp_5*tmp_20;
double tmp_30 = tmp_28+tmp_29;
double g0_1 = (tmp_27*tmp_7-tmp_23*tmp_7+tmp_16*tmp_13+tmp_16*tmp_30)*((tmp_30*tmp_30)*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+(tmp_27*tmp_27)*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))+tmp_27*(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))+(((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27)))+((tmp_30*tmp_30)*(tmp_30*tmp_30))*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))+((tmp_27*tmp_27)*(tmp_27*tmp_27))*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))+(((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30)))-tmp_30*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))-(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*((tmp_13*tmp_13)*(tmp_13*tmp_13))+((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23)-(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13*tmp_13)+((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13)+(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23*tmp_23)+(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*((tmp_23*tmp_23)*(tmp_23*tmp_23))+(((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))+(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))+((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+(((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23)))+(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_23-(tmp_30*tmp_30*tmp_30)*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))+(tmp_27*tmp_27*tmp_27)*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))-(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_13-(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))+13.0*((tmp_30*tmp_30*tmp_30)*tmp_7*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))+tmp_16*tmp_27*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))+tmp_16*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+tmp_16*((tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_7*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+tmp_16*((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_23+tmp_16*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))+tmp_16*(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))-tmp_7*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+tmp_16*(tmp_27*tmp_27*tmp_27)*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))+tmp_16*(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23)-(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7*(tmp_13*tmp_13*tmp_13)+tmp_16*(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_23*tmp_23)*(tmp_23*tmp_23))+tmp_16*(tmp_27*tmp_27)*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))-((tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_7*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+tmp_30*tmp_7*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+tmp_16*(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23*tmp_23)+(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7+(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7*(tmp_13*tmp_13)+(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_7*((tmp_13*tmp_13)*(tmp_13*tmp_13))-(tmp_30*tmp_30)*tmp_7*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))+tmp_16*(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))-((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_7*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))-((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7*tmp_13)*(tmp_23*tmp_13+tmp_27*tmp_30);
g(0,1) = g0_1;
}
{
double tmp_1 = x(0);
double tmp_2 = x(2);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = p(0);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(1);
double tmp_8 = x(3);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = -tmp_7*tmp_11;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(5);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = p(3);
double tmp_17 = tmp_16*tmp_15;
double tmp_18 = x(4);
double tmp_19 = -tmp_1;
double tmp_20 = tmp_19+tmp_18;
double tmp_21 = p(2);
double tmp_22 = tmp_20*tmp_21;
double tmp_23 = tmp_17+tmp_22;
double tmp_25 = tmp_16*tmp_11;
double tmp_26 = -tmp_21*tmp_4;
double tmp_27 = tmp_26+tmp_25;
double tmp_28 = tmp_7*tmp_15;
double tmp_29 = tmp_20*tmp_5;
double tmp_30 = tmp_29+tmp_28;
double g0_2 = ((tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))+(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))*(tmp_27*tmp_27*tmp_27)+(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))*((tmp_27*tmp_27)*(tmp_27*tmp_27))+(tmp_30*tmp_30)*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))+(((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23)))+((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))*(tmp_27*tmp_27)+(tmp_23*tmp_23*tmp_23)*(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))+(((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30)))+((tmp_30*tmp_30)*(tmp_30*tmp_30))*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))-tmp_30*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))+(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_27+(((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27)))-(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))-(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13*tmp_13)+(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*((tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_23*tmp_23)*((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))+((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13)-(tmp_30*tmp_30*tmp_30)*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))+((tmp_23*tmp_23)*(tmp_23*tmp_23))*(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))+(((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))+tmp_23*(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))-(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))-(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_13)*(tmp_23*tmp_5-tmp_30*tmp_21)-(tmp_30*tmp_27+tmp_23*tmp_13)*(4.0*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_21*(tmp_27*tmp_27*tmp_27)-6.0*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*tmp_5+(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_21+(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_5+8.0*((tmp_23*tmp_23)*(tmp_23*tmp_23))*tmp_21*(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))+3.0*(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13)*tmp_5+9.0*(tmp_30*tmp_30*tmp_30)*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))*tmp_5+7.0*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*tmp_21*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))+12.0*tmp_21*(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))+5.0*(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_13*tmp_13)*(tmp_13*tmp_13))*tmp_5-12.0*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))*tmp_5+9.0*(tmp_23*tmp_23*tmp_23)*tmp_21*(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))-10.0*(tmp_30*tmp_30)*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))*tmp_5-4.0*(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13*tmp_13)*tmp_5+2.0*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_21*tmp_27+7.0*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*tmp_5+10.0*(tmp_23*tmp_23)*tmp_21*(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))+11.0*tmp_30*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))*tmp_5+6.0*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*tmp_21*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))+3.0*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_21*(tmp_27*tmp_27)+5.0*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*tmp_21*((tmp_27*tmp_27)*(tmp_27*tmp_27))-8.0*((tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*tmp_5-2.0*((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_13*tmp_5+11.0*tmp_23*tmp_21*((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))));
g(0,2) = g0_2;
}
{
double tmp_1 = x(0);
double tmp_2 = x(2);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_3+tmp_1;
double tmp_5 = p(0);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(1);
double tmp_8 = x(3);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_10+tmp_8;
double tmp_12 = -tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(5);
double tmp_15 = tmp_10+tmp_14;
double tmp_16 = p(3);
double tmp_17 = tmp_16*tmp_15;
double tmp_18 = x(4);
double tmp_19 = -tmp_1;
double tmp_20 = tmp_19+tmp_18;
double tmp_21 = p(2);
double tmp_22 = tmp_21*tmp_20;
double tmp_23 = tmp_22+tmp_17;
double tmp_25 = tmp_16*tmp_11;
double tmp_26 = -tmp_21*tmp_4;
double tmp_27 = tmp_25+tmp_26;
double tmp_28 = tmp_15*tmp_7;
double tmp_29 = tmp_5*tmp_20;
double tmp_30 = tmp_28+tmp_29;
double g0_3 = -(tmp_13*tmp_23+tmp_27*tmp_30)*(6.0*tmp_16*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+2.0*tmp_16*tmp_27*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))-2.0*tmp_13*((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7-6.0*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_7+11.0*tmp_16*((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_23+tmp_16*(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))+9.0*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))*(tmp_30*tmp_30*tmp_30)*tmp_7+5.0*tmp_16*((tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+10.0*tmp_16*(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23)+(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7+4.0*tmp_16*(tmp_27*tmp_27*tmp_27)*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))+7.0*tmp_16*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))+11.0*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))*tmp_30*tmp_7+5.0*((tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_7+3.0*tmp_16*(tmp_27*tmp_27)*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))+8.0*tmp_16*(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_23*tmp_23)*(tmp_23*tmp_23))-4.0*(tmp_13*tmp_13*tmp_13)*(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7+9.0*tmp_16*(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23*tmp_23)-10.0*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))*(tmp_30*tmp_30)*tmp_7+12.0*tmp_16*(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))+7.0*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_7+3.0*(tmp_13*tmp_13)*(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7-8.0*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_7-12.0*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))*tmp_7)+(tmp_23*tmp_7-tmp_16*tmp_30)*((tmp_13*tmp_13)*((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))+(((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23)))+(tmp_27*tmp_27)*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))+tmp_27*(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))+((tmp_13*tmp_13)*(tmp_13*tmp_13))*(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))+((tmp_27*tmp_27)*(tmp_27*tmp_27))*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))+(((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))-tmp_13*(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))+((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))-(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))+((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23)+(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))*((tmp_30*tmp_30)*(tmp_30*tmp_30))-(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))*(tmp_30*tmp_30*tmp_30)+(((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30)))+(((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27)))+((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))*(tmp_30*tmp_30)+(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))+(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23*tmp_23)+(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*((tmp_23*tmp_23)*(tmp_23*tmp_23))+((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_23-(tmp_13*tmp_13*tmp_13)*(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))+(tmp_27*tmp_27*tmp_27)*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))-(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))*tmp_30-(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30)));
g(0,3) = g0_3;
}
{
double tmp_1 = x(0);
double tmp_2 = x(2);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = p(0);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(1);
double tmp_8 = x(3);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = -tmp_11*tmp_7;
double tmp_13 = tmp_6+tmp_12;
double tmp_14 = x(5);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = p(3);
double tmp_17 = tmp_15*tmp_16;
double tmp_18 = x(4);
double tmp_19 = -tmp_1;
double tmp_20 = tmp_18+tmp_19;
double tmp_21 = p(2);
double tmp_22 = tmp_21*tmp_20;
double tmp_23 = tmp_17+tmp_22;
double tmp_25 = tmp_11*tmp_16;
double tmp_26 = -tmp_21*tmp_4;
double tmp_27 = tmp_25+tmp_26;
double tmp_28 = tmp_15*tmp_7;
double tmp_29 = tmp_5*tmp_20;
double tmp_30 = tmp_28+tmp_29;
double g0_4 = -((tmp_27*tmp_27)*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))-(tmp_30*tmp_30*tmp_30)*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))-(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))-(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_13+(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*((tmp_13*tmp_13)*(tmp_13*tmp_13))-(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13*tmp_13)-(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+((tmp_27*tmp_27)*(tmp_27*tmp_27))*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))+(((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23)))+((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13)+tmp_27*(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))+((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))+(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*((tmp_23*tmp_23)*(tmp_23*tmp_23))+(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23*tmp_23)+((tmp_30*tmp_30)*(tmp_30*tmp_30))*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))+(((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27)))+(((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30)))+((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23)-tmp_30*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+(tmp_30*tmp_30)*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+(tmp_27*tmp_27*tmp_27)*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))+(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_23+(((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))+(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23)))*(tmp_5*tmp_27+tmp_21*tmp_13)+(tmp_5*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))-2.0*tmp_5*tmp_30*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))-6.0*tmp_5*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))-5.0*tmp_21*(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_23*tmp_23)*(tmp_23*tmp_23))-10.0*tmp_21*(tmp_27*tmp_27)*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))-6.0*tmp_21*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))+11.0*tmp_5*((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_13+5.0*tmp_5*((tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))-3.0*tmp_21*(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23)-9.0*tmp_21*(tmp_27*tmp_27*tmp_27)*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))-tmp_21*(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))-4.0*tmp_5*(tmp_30*tmp_30*tmp_30)*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))-8.0*tmp_21*((tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))-12.0*tmp_5*(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))-10.0*tmp_5*(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13)-2.0*tmp_21*((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_23+7.0*tmp_5*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+3.0*tmp_5*(tmp_30*tmp_30)*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))-11.0*tmp_21*tmp_27*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))-8.0*tmp_5*(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_13*tmp_13)*(tmp_13*tmp_13))-7.0*tmp_21*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))+9.0*tmp_5*(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13*tmp_13)-12.0*tmp_21*(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))-4.0*tmp_21*(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23*tmp_23))*(tmp_23*tmp_13+tmp_27*tmp_30);
g(0,4) = g0_4;
}
{
double tmp_1 = x(0);
double tmp_2 = x(2);
double tmp_3 = -tmp_2;
double tmp_4 = tmp_1+tmp_3;
double tmp_5 = p(0);
double tmp_6 = tmp_5*tmp_4;
double tmp_7 = p(1);
double tmp_8 = x(3);
double tmp_9 = x(1);
double tmp_10 = -tmp_9;
double tmp_11 = tmp_8+tmp_10;
double tmp_12 = -tmp_11*tmp_7;
double tmp_13 = tmp_12+tmp_6;
double tmp_14 = x(5);
double tmp_15 = tmp_14+tmp_10;
double tmp_16 = p(3);
double tmp_17 = tmp_15*tmp_16;
double tmp_18 = x(4);
double tmp_19 = -tmp_1;
double tmp_20 = tmp_18+tmp_19;
double tmp_21 = p(2);
double tmp_22 = tmp_21*tmp_20;
double tmp_23 = tmp_22+tmp_17;
double tmp_25 = tmp_11*tmp_16;
double tmp_26 = -tmp_21*tmp_4;
double tmp_27 = tmp_25+tmp_26;
double tmp_28 = tmp_15*tmp_7;
double tmp_29 = tmp_5*tmp_20;
double tmp_30 = tmp_28+tmp_29;
double g0_5 = -(5.0*(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_23*tmp_23)*(tmp_23*tmp_23))*tmp_16-5.0*((tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_7*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_16-9.0*(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7*(tmp_13*tmp_13*tmp_13)+10.0*(tmp_27*tmp_27)*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_16+6.0*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*tmp_16+12.0*(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_16-tmp_7*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+4.0*(tmp_30*tmp_30*tmp_30)*tmp_7*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))+3.0*(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23)*tmp_16+9.0*(tmp_27*tmp_27*tmp_27)*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_16+6.0*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*tmp_7*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))-7.0*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_7*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))+8.0*((tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*tmp_16-3.0*(tmp_30*tmp_30)*tmp_7*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))+12.0*(tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7+2.0*((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_23*tmp_16-11.0*((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7*tmp_13+4.0*(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23*tmp_23)*tmp_16+2.0*tmp_30*tmp_7*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))+8.0*(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*tmp_7*((tmp_13*tmp_13)*(tmp_13*tmp_13))+10.0*(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_7*(tmp_13*tmp_13)+11.0*tmp_27*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))*tmp_16+7.0*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*tmp_16)*(tmp_23*tmp_13+tmp_27*tmp_30)+(tmp_13*tmp_16+tmp_27*tmp_7)*((tmp_30*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*tmp_13+(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))-tmp_27*(tmp_23*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))+(tmp_30*tmp_30*tmp_30)*(tmp_13*((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))-(((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23)))-((tmp_27*tmp_27)*(tmp_27*tmp_27))*(((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))-((tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30))*(tmp_30*(tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13)-(tmp_27*tmp_27)*((tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23)))-((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))+(tmp_30*(tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))-(((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*((tmp_13*tmp_13)*(tmp_13*tmp_13))+(tmp_30*((tmp_30*tmp_30)*(tmp_30*tmp_30))*((tmp_30*tmp_30)*(tmp_30*tmp_30)))*(tmp_13*tmp_13*tmp_13)-(((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27)))+tmp_30*(tmp_13*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13)))-(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))-(tmp_27*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*tmp_23-(((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30))*((tmp_30*tmp_30*tmp_30)*(tmp_30*tmp_30*tmp_30)))-((tmp_30*tmp_30)*(tmp_30*tmp_30))*(((tmp_13*tmp_13)*(tmp_13*tmp_13))*((tmp_13*tmp_13)*(tmp_13*tmp_13)))-(tmp_27*tmp_27*tmp_27)*(tmp_23*((tmp_23*tmp_23)*(tmp_23*tmp_23))*((tmp_23*tmp_23)*(tmp_23*tmp_23)))-((tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27))*(tmp_27*(tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23)-(((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*((tmp_23*tmp_23)*(tmp_23*tmp_23))-(tmp_27*((tmp_27*tmp_27)*(tmp_27*tmp_27))*((tmp_27*tmp_27)*(tmp_27*tmp_27)))*(tmp_23*tmp_23*tmp_23)-(((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13))*((tmp_13*tmp_13*tmp_13)*(tmp_13*tmp_13*tmp_13)))-(tmp_27*(tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*(tmp_23*(tmp_23*tmp_23)*(tmp_23*tmp_23))-((tmp_27*tmp_27*tmp_27)*(tmp_27*tmp_27*tmp_27))*((tmp_23*tmp_23*tmp_23)*(tmp_23*tmp_23*tmp_23))-(tmp_30*tmp_30)*((tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))*(tmp_13*(tmp_13*tmp_13)*(tmp_13*tmp_13))));
g(0,5) = g0_5;
}
      }
   }
}
