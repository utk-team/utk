#ifndef SIDE1_STATIC_FILTER_H
#define SIDE1_STATIC_FILTER_H

#include <fenv.h>

inline int side1( double p1x, double p1y, double p1z, double p2x, double p2y, double p2z, double qx, double qy, double qz) {
    double p1x_bound = fabs(p1x);
    double p2x_bound = fabs(p2x);
    double v1x_bound;
    double v1x;
    v1x_bound = (p1x_bound + p2x_bound);
    v1x = (p1x - p2x);
    double p1y_bound = fabs(p1y);
    double p2y_bound = fabs(p2y);
    double v1y_bound;
    double v1y;
    v1y_bound = (p1y_bound + p2y_bound);
    v1y = (p1y - p2y);
    double p1z_bound = fabs(p1z);
    double p2z_bound = fabs(p2z);
    double v1z_bound;
    double v1z;
    v1z_bound = (p1z_bound + p2z_bound);
    v1z = (p1z - p2z);
    double qx_bound = fabs(qx);
    double v2x_bound;
    double v2x;
    v2x_bound = ((2.00000000000000000000e+00 * qx_bound) + (p1x_bound + p2x_bound));
    v2x = ((2.00000000000000000000e+00 * qx) - (p1x + p2x));
    double qy_bound = fabs(qy);
    double v2y_bound;
    double v2y;
    v2y_bound = ((2.00000000000000000000e+00 * qy_bound) + (p1y_bound + p2y_bound));
    v2y = ((2.00000000000000000000e+00 * qy) - (p1y + p2y));
    double qz_bound = fabs(qz);
    double v2z_bound;
    double v2z;
    v2z_bound = ((2.00000000000000000000e+00 * qz_bound) + (p1z_bound + p2z_bound));
    v2z = ((2.00000000000000000000e+00 * qz) - (p1z + p2z));
    int int_tmp_result;
    if( ((fabs((((v1x * v2x) + (v1y * v2y)) + (v1z * v2z))) > ((((v1x_bound * v2x_bound) + (v1y_bound * v2y_bound)) + (v1z_bound * v2z_bound)) * 1.33226762955018784851e-15)) && (fetestexcept( FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID ) == 0)) )
    {
        int_tmp_result = (((((v1x * v2x) + (v1y * v2y)) + (v1z * v2z)) < 0.00000000000000000000e+00) ? -1 : 1);
    } 
    else 
    {
        feclearexcept( FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID );
        return FPG_UNCERTAIN_VALUE;
    } 
    return int_tmp_result;
} 

#endif