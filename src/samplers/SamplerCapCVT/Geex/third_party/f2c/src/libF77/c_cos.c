#include "f2c.h"
 

/* [Bruno]: temporary removed */
#ifdef WIN32
void c_cos(void* r, void* z) {
	abort() ;
}
#else
#ifdef KR_headers
extern double sin(), cos(), sinh(), cosh();
VOID c_cos(r, z) complex *r, *z;
#else
#undef abs
#include "math.h"

void c_cos(complex *r, complex *z)
#endif
{
	double zi = z->i, zr = z->r;
	r->r =   cos(zr) * cosh(zi);
	r->i = - sin(zr) * sinh(zi);
	}
#endif
