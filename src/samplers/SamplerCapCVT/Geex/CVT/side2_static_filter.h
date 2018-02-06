#ifndef SIDE2_STATIC_FILTER_H
#define SIDE2_STATIC_FILTER_H

#include <fenv.h>

inline int side2( 
		  int& s_result, int& s_denom,
		  double p1x, double p1y, double p1z, double p2x, double p2y, double p2z, double p3x, double p3y, double p3z, double q1x, double q1y, double q1z, double q2x, double q2y, double q2z
		  ) {
    double q2z_bound = fabs(q2z);
    double q1z_bound = fabs(q1z);
    double t17_bound;
    double t17;
    t17_bound = (q2z_bound + q1z_bound);
    t17 = (q2z - q1z);
    double q2y_bound = fabs(q2y);
    double q1y_bound = fabs(q1y);
    double t18_bound;
    double t18;
    t18_bound = (q2y_bound + q1y_bound);
    t18 = (q2y - q1y);
    double q2x_bound = fabs(q2x);
    double q1x_bound = fabs(q1x);
    double t19_bound;
    double t19;
    t19_bound = (q2x_bound + q1x_bound);
    t19 = (q2x - q1x);
    double p1z_bound = fabs(p1z);
    double p3z_bound = fabs(p3z);
    double t20_bound;
    double t20;
    t20_bound = (p1z_bound + p3z_bound);
    t20 = (p1z - p3z);
    double p1y_bound = fabs(p1y);
    double p3y_bound = fabs(p3y);
    double t21_bound;
    double t21;
    t21_bound = (p1y_bound + p3y_bound);
    t21 = (p1y - p3y);
    double p1x_bound = fabs(p1x);
    double p3x_bound = fabs(p3x);
    double t22_bound;
    double t22;
    t22_bound = (p1x_bound + p3x_bound);
    t22 = (p1x - p3x);
    double denom_bound;
    double denom;
    denom_bound = (((t19_bound * t22_bound) + (t18_bound * t21_bound)) + (t17_bound * t20_bound));
    denom = (((t19 * t22) + (t18 * t21)) + (t17 * t20));
    int int_tmp_result;
    if( ((fabs(denom) > (denom_bound * 1.11022302462515654042e-15)) && (fetestexcept( FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID ) == 0)) )
    {
        int_tmp_result = ((denom < 0.00000000000000000000e+00) ? -1 : 1);
    } 
    else 
    {
        feclearexcept( FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID );
        return FPG_UNCERTAIN_VALUE;
    } 
    s_denom = int_tmp_result;
    double p2x_bound = fabs(p2x);
    double t28_bound;
    double t28;
    t28_bound = (p1x_bound + p2x_bound);
    t28 = (p1x - p2x);
    double t27_bound;
    double t27;
    t27_bound = (p1x_bound + (2 * q1x_bound));
    t27 = (p1x - (2 * q1x));
    double p2y_bound = fabs(p2y);
    double t26_bound;
    double t26;
    t26_bound = (p1y_bound + p2y_bound);
    t26 = (p1y - p2y);
    double t25_bound;
    double t25;
    t25_bound = (p1y_bound + (2 * q1y_bound));
    t25 = (p1y - (2 * q1y));
    double p2z_bound = fabs(p2z);
    double t24_bound;
    double t24;
    t24_bound = (p1z_bound + p2z_bound);
    t24 = (p1z - p2z);
    double t23_bound;
    double t23;
    t23_bound = (p1z_bound + (2 * q1z_bound));
    t23 = (p1z - (2 * q1z));
    double result_bound;
    double result;
    result_bound = (((((t28_bound * t19_bound) + (t26_bound * t18_bound)) + (t24_bound * t17_bound)) * ((((p3x_bound + t27_bound) * t22_bound) + ((p3y_bound + t25_bound) * t21_bound)) + ((p3z_bound + t23_bound) * t20_bound))) + ((((t28_bound * (p2x_bound + t27_bound)) + (t26_bound * (p2y_bound + t25_bound))) + (t24_bound * (p2z_bound + t23_bound))) * denom_bound));
    result = (((((t28 * t19) + (t26 * t18)) + (t24 * t17)) * ((((p3x + t27) * t22) + ((p3y + t25) * t21)) + ((p3z + t23) * t20))) + ((((t28 * (-p2x - t27)) + (t26 * (-p2y - t25))) + (t24 * (-p2z - t23))) * denom));
    int int_tmp_result_FFWKCAA;
    if( ((fabs(result) > (result_bound * 3.10862446895043831319e-15)) && (fetestexcept( FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID ) == 0)) )
    {
        int_tmp_result_FFWKCAA = ((result < 0.00000000000000000000e+00) ? -1 : 1);
    } 
    else 
    {
        feclearexcept( FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID );
        return FPG_UNCERTAIN_VALUE;
    } 
    s_result = int_tmp_result_FFWKCAA;
    return 0;
} 

#endif
