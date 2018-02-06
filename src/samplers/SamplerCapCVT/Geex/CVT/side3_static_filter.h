#ifndef SIDE3_STATIC_FILTER_H
#define SIDE3_STATIC_FILTER_H

#include <fenv.h>

inline int side3(
			 int& s_result, int& s_denom,
			  double p1x, double p1y, double p1z, double p2x, double p2y, double p2z, double p3x, double p3y, double p3z, double p4x, double p4y, double p4z, double q1x, double q1y, double q1z, double q2x, double q2y, double q2z, double q3x, double q3y, double q3z
) {
    double p4z_bound = fabs(p4z);
    double p1z_bound = fabs(p1z);
    double t46_bound;
    double t46;
    t46_bound = (p4z_bound + p1z_bound);
    t46 = (p4z - p1z);
    double p4y_bound = fabs(p4y);
    double p1y_bound = fabs(p1y);
    double t47_bound;
    double t47;
    t47_bound = (p4y_bound + p1y_bound);
    t47 = (p4y - p1y);
    double p4x_bound = fabs(p4x);
    double p1x_bound = fabs(p1x);
    double t48_bound;
    double t48;
    t48_bound = (p4x_bound + p1x_bound);
    t48 = (p4x - p1x);
    double t55_bound;
    double t55;
    t55_bound = (((t48_bound * (p1x_bound + p4x_bound)) + (t47_bound * (p1y_bound + p4y_bound))) + (t46_bound * (p1z_bound + p4z_bound)));
    t55 = (((t48 * (p1x + p4x)) + (t47 * (p1y + p4y))) + (t46 * (p1z + p4z)));
    double p3z_bound = fabs(p3z);
    double t49_bound;
    double t49;
    t49_bound = (p1z_bound + p3z_bound);
    t49 = (-p1z + p3z);
    double p3y_bound = fabs(p3y);
    double t50_bound;
    double t50;
    t50_bound = (p1y_bound + p3y_bound);
    t50 = (-p1y + p3y);
    double p3x_bound = fabs(p3x);
    double t51_bound;
    double t51;
    t51_bound = (p1x_bound + p3x_bound);
    t51 = (-p1x + p3x);
    double t54_bound;
    double t54;
    t54_bound = (((t51_bound * (p1x_bound + p3x_bound)) + (t50_bound * (p1y_bound + p3y_bound))) + (t49_bound * (p1z_bound + p3z_bound)));
    t54 = (((t51 * (p1x + p3x)) + (t50 * (p1y + p3y))) + (t49 * (p1z + p3z)));
    double q3z_bound = fabs(q3z);
    double q1z_bound = fabs(q1z);
    double t40_bound;
    double t40;
    t40_bound = (q3z_bound + q1z_bound);
    t40 = (q3z - q1z);
    double q3x_bound = fabs(q3x);
    double q1x_bound = fabs(q1x);
    double t42_bound;
    double t42;
    t42_bound = (q3x_bound + q1x_bound);
    t42 = (q3x - q1x);
    double q2z_bound = fabs(q2z);
    double t43_bound;
    double t43;
    t43_bound = (q2z_bound + q1z_bound);
    t43 = (q2z - q1z);
    double q2x_bound = fabs(q2x);
    double t45_bound;
    double t45;
    t45_bound = (q2x_bound + q1x_bound);
    t45 = (q2x - q1x);
    double t37_bound;
    double t37;
    t37_bound = ((t43_bound * t42_bound) + (t45_bound * t40_bound));
    t37 = ((t43 * t42) - (t45 * t40));
    double q3y_bound = fabs(q3y);
    double q1y_bound = fabs(q1y);
    double t41_bound;
    double t41;
    t41_bound = (q3y_bound + q1y_bound);
    t41 = (q3y - q1y);
    double q2y_bound = fabs(q2y);
    double t44_bound;
    double t44;
    t44_bound = (q2y_bound + q1y_bound);
    t44 = (q2y - q1y);
    double t38_bound;
    double t38;
    t38_bound = ((t44_bound * t40_bound) + (t43_bound * t41_bound));
    t38 = ((t44 * t40) - (t43 * t41));
    double t39_bound;
    double t39;
    t39_bound = ((t45_bound * t41_bound) + (t44_bound * t42_bound));
    t39 = ((t45 * t41) - (t44 * t42));
    double t53_bound;
    double t53;
    t53_bound = (2 * (((t38_bound * q1x_bound) + (t37_bound * q1y_bound)) + (t39_bound * q1z_bound)));
    t53 = (2 * (((t38 * q1x) + (t37 * q1y)) + (t39 * q1z)));
    double t34_bound;
    double t34;
    t34_bound = ((t48_bound * t37_bound) + (t47_bound * t38_bound));
    t34 = ((t48 * t37) - (t47 * t38));
    double t33_bound;
    double t33;
    t33_bound = ((t47_bound * t39_bound) + (t46_bound * t37_bound));
    t33 = ((t47 * t39) - (t46 * t37));
    double t32_bound;
    double t32;
    t32_bound = ((t46_bound * t38_bound) + (t48_bound * t39_bound));
    t32 = ((t46 * t38) - (t48 * t39));
    double denom_bound;
    double denom;
    denom_bound = (((t51_bound * t33_bound) + (t50_bound * t32_bound)) + (t49_bound * t34_bound));
    denom = (((t51 * t33) + (t50 * t32)) + (t49 * t34));
    int int_tmp_result;
    if( ((fabs(denom) > (denom_bound * 2.44249065417534438893e-15)) && (fetestexcept( FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID ) == 0)) )
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
    double p2y_bound = fabs(p2y);
    double p2z_bound = fabs(p2z);
    double result_bound;
    double result;
    result_bound = ((((p1x_bound + p2x_bound) * ((((t33_bound * t54_bound) + (((t49_bound * t37_bound) + (t50_bound * t39_bound)) * t55_bound)) + (((t50_bound * t46_bound) + (t49_bound * t47_bound)) * t53_bound)) + (denom_bound * (p1x_bound + p2x_bound)))) + ((p1y_bound + p2y_bound) * ((((t32_bound * t54_bound) + (((t51_bound * t39_bound) + (t49_bound * t38_bound)) * t55_bound)) + (((t49_bound * t48_bound) + (t51_bound * t46_bound)) * t53_bound)) + (denom_bound * (p1y_bound + p2y_bound))))) + ((p1z_bound + p2z_bound) * ((((t34_bound * t54_bound) + (((t50_bound * t38_bound) + (t51_bound * t37_bound)) * t55_bound)) + (((t51_bound * t47_bound) + (t50_bound * t48_bound)) * t53_bound)) + (denom_bound * (p1z_bound + p2z_bound)))));
    result = ((((p1x - p2x) * ((((t33 * t54) + (((t49 * t37) - (t50 * t39)) * t55)) + (((t50 * t46) - (t49 * t47)) * t53)) - (denom * (p1x + p2x)))) + ((p1y - p2y) * ((((t32 * t54) + (((t51 * t39) - (t49 * t38)) * t55)) + (((t49 * t48) - (t51 * t46)) * t53)) - (denom * (p1y + p2y))))) + ((p1z - p2z) * ((((t34 * t54) + (((t50 * t38) - (t51 * t37)) * t55)) + (((t51 * t47) - (t50 * t48)) * t53)) - (denom * (p1z + p2z)))));
    int int_tmp_result_FFWKCAA;
    if( ((fabs(result) > (result_bound * 4.44089209850062616169e-15)) && (fetestexcept( FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID ) == 0)) )
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
