
#ifndef __GEEX_MATHEMATICS_SHEWCHUK__
#define __GEEX_MATHEMATICS_SHEWCHUK__

#include <Geex/basics/types.h>

#ifdef SINGLE
#define REAL float
#else
#define REAL double                      /* float or double */
#endif

/*    orient2d(pa, pb, pc)                                                   */
/*    orient2dfast(pa, pb, pc)                                               */
/*    orient3d(pa, pb, pc, pd)                                               */
/*    orient3dfast(pa, pb, pc, pd)                                           */
/*    incircle(pa, pb, pc, pd)                                               */
/*    incirclefast(pa, pb, pc, pd)                                           */
/*    insphere(pa, pb, pc, pd, pe)                                           */
/*    inspherefast(pa, pb, pc, pd, pe)                                       */

void GEEX_API exactinit();

REAL GEEX_API orient2d(
  REAL *pa, REAL *pb, REAL *pc
);

REAL GEEX_API orient2dfast(
  REAL *pa, REAL *pb, REAL *pc
);

REAL GEEX_API orient3d(
  REAL *pa, REAL *pb, REAL *pc, REAL *pd
);

REAL GEEX_API orient3dfast(
  REAL *pa, REAL *pb, REAL *pc, REAL *pd
);

double GEEX_API orient3dexact(
  REAL *pa, REAL *pb, REAL *pc, REAL *pd
);

REAL GEEX_API incircle(
  REAL *pa, REAL *pb, REAL *pc, REAL *pd
);

REAL GEEX_API incirclefast(
  REAL *pa, REAL *pb, REAL *pc, REAL *pd
);

REAL GEEX_API insphere(
  REAL *pa, REAL *pb, REAL *pc, REAL *pd, 
  REAL *pe
);

REAL GEEX_API inspherefast(
  REAL *pa, REAL *pb, REAL *pc, REAL *pd, 
  REAL *pe
);

#endif
