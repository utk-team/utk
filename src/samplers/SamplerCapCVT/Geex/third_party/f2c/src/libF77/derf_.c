#include "f2c.h"
#ifndef WIN32
#ifdef KR_headers
double erf();
double derf_(x) doublereal *x;
#else
extern double erf(double);
double derf_(doublereal *x)
#endif
{
return( erf(*x) );
}
#endif