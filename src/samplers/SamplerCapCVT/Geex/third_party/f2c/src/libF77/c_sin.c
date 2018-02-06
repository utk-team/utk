#include "f2c.h"
 
/* [Bruno]: temporary removed */
#ifdef WIN32
void c_sin(void* r, void* z) {
	abort() ;
}
#else
#ifdef KR_headers
extern double sin(), cos(), sinh(), cosh();

VOID c_sin(r, z) complex *r, *z;
#else
#undef abs
#include "math.h"

void c_sin(complex *r, complex *z)
#endif
{
	double zi = z->i, zr = z->r;
	r->r = sin(zr) * cosh(zi);
	r->i = cos(zr) * sinh(zi);
	}
#endif
