#include "f2c.h"

/* [Bruno]: temporary removed */
#ifdef WIN32
void c_exp(void* r, void* z) {
	abort() ;
}
#else
#ifdef KR_headers
extern double exp(), cos(), sin();

 VOID c_exp(r, z) complex *r, *z;
#else
#undef abs
#include "math.h"

void c_exp(complex *r, complex *z)
#endif
{
	double expx, zi = z->i;

	expx = exp(z->r);
	r->r = expx * cos(zi);
	r->i = expx * sin(zi);
	}
#endif
