/* lmbm.f -- translated by f2c (version 20060506).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__0 = 0;
static integer c_n1 = -1;
static integer c_n2 = -2;
static integer c_n3 = -3;
static integer c_n4 = -4;
static integer c_n5 = -5;
static doublereal c_b20 = 1e-12;
static doublereal c_b21 = 1e60;

/* *********************************************************************** */


/*     LMBM includes the following subroutines */

/*     S   LMBMU         Initialization for limited memory bundle */
/*                         subroutine. */
/*     S   LMBM          Limited memory bundle subroutine for nonsmooth */
/*                         large-scale optimization. */


/* *********************************************************************** */

/*     * SUBROUTINE LMBMU * */


/*     * Purpose * */

/*     Initialization for limited memory bundle subroutine for */
/*     large-scale unconstrained nonsmooth optimization. */


/*     * Calling sequence * */

/*     CALL LMBMU(N,NA,MCU,MC,NW,X,F,RPAR,IPAR,IOUT,TIME,RTIM,W) */


/*     * Parameters * */

/*     II  N             Number of variables. */
/*     II  NA            Maximum bundle dimension, NA >= 2. */
/*     IU  MC            Maximum number of stored corrections, MC >= 3. */
/*     II  MCU           Upper limit for maximum number of stored */
/*                         corrections, MCU >= MC. */
/*     RU  X(N)          Vector of variables. */
/*     RO  F             Value of the objective function. */
/*     RI  TIME          Maximum CPU-time in seconds. If TIME <= 0.0 */
/*                         the maximum time is ignored. REAL argument. */
/*     RI  RTIM(2)       Auxiliary array. REAL array */
/*                         On output RTIM(1) contains the execution time. */
/*     RI  RPAR(8)       Real parameters: */
/*           RPAR(1)       Tolerance for change of function values. */
/*           RPAR(2)       Second Tolerance for change of function values. */
/*           RPAR(3)       Tolerance for the function value. */
/*           RPAR(4)       Tolerance for the first termination criterion. */
/*           RPAR(5)       Tolerance for the second termination criterion. */
/*           RPAR(6)       Distance measure parameter, 0 <= RPAR(6). */
/*           RPAR(7)       Line search parameter, 0 < RPAR(7) < 0.25. */
/*           RPAR(8)       Maximum stepsize, 1 < RPAR(8). */
/*                           If RPAR(I) <= 0 for I=1,3,4,5,7, and 8 the */
/*                           default value of the parameter will be used. */
/*                           If RPAR(2) < 0 the the parameter and the */
/*                           corresponding termination criterion will be */
/*                           ignored. If RPAR(2) = 0 default value will */
/*                           be used. If RPAR(6) < 0 the default value */
/*                           will be used. */
/*     II  IPAR(7)       Integer paremeters: */
/*           IPAR(1)       Exponent for distance measure. */
/*           IPAR(2)       Maximum number of iterations. */
/*           IPAR(3)       Maximum number of function evaluations. */
/*           IPAR(4)       Maximum number of iterations with changes of */
/*                           function values smaller than RPAR(1). */
/*           IPAR(5)       Printout specification: */
/*                             -1  - No printout. */
/*                              0  - Only the error messages. */
/*                              1  - The final values of the objective */
/*                                   function. */
/*                              2  - The final values of the objective */
/*                                   function and the most serious */
/*                                   warning messages. */
/*                              3  - The whole final solution. */
/*                              4  - At each iteration values of the */
/*                                   objective function. */
/*                              5  - At each iteration the whole */
/*                                   solution */
/*           IPAR(6)       Selection of the method: */
/*                              0  - Limited memory bundle method. */
/*                              1  - L-BFGS bundle method. */
/*           IPAR(7)       Selection of the scaling: */
/*                              0  - Scaling at every iteration with STU/UTU. */
/*                              1  - Scaling at every iteration with STS/STU. */
/*                              2  - Interval scaling with STU/UTU. */
/*                              3  - Interval scaling with STS/STU. */
/*                              4  - Preliminary scaling with STU/UTU. */
/*                              5  - Preliminary scaling with STS/STU. */
/*                              6  - No scaling. */
/*     IO  IOUT(3)       Integer parameters: */
/*           IOUT(1)       Number of used iterations. */
/*           IOUT(2)       Number of used function evaluations. */
/*           IOUT(3)       Cause of termination: */
/*                              1  - The problem has been solved. */
/*                                   with desired accuracy. */
/*                              2  - Changes in function values < RPAR(1) */
/*                                   in IPAR(4) subsequent iterations. */
/*                              3  - Changes in function values < RPAR(2) */
/*                                   *SMALL*MAX(|F_k|,|F_k+1|,1), where */
/*                                   SMALL is the smallest positive */
/*                                   number such that 1.0 + SMALL > 1.0. */
/*                              4  - Number of function calls > IPAR(3). */
/*                              5  - Number of iterations > IPAR(2). */
/*                              6  - Time limit exceeded. */
/*                              7  - F < RPAR(3). */
/*                             -1  - Two consecutive restarts or number */
/*                                   of restarts > maximum number of */
/*                                   restarts. */
/*                             -2  - TMAX < TMIN in two subsequent */
/*                                   iterations. */
/*                             -3  - Failure in function or subgradient */
/*                                   calculations (assigned by the user). */
/*                             -4  - Failure in attaining the demanded */
/*                                   accuracy. */
/*                             -5  - Invalid input parameters. */
/*                             -6  - Not enough working space. */
/*     RA  W(NW)         Work vector. */
/*     II  NW            Dimension of the work vector W: */
/*                         NW >= 1 + 9*N + 2*N*NA + 3*NA + 2*N*(MCU+1) */
/*                               + 3*(MCU+2)*(MCU+1)/2 + 9*(MCU+1) */


/*     * Subprograms used * */

/*     S   LMBM         Limited memory bundle method for nonsmooth */
/*                        optimization. */
/*     S   WPRINT       Printout the error and warning messages. */
/*     S   GETIME       Execution time. */



/*     Napsu Karmitsa (2002 - 2004, last modified 2007) */

/* Subroutine */ int lmbmu_(integer *n, integer *na, integer *mcu, integer *
	mc, integer *nw, doublereal *x, doublereal *f, doublereal *rpar, 
	integer *ipar, integer *iout, real *time, real *rtim, doublereal *w)
{
    static integer ld, lc, lg, ls, lu, laf, lga, lag, lgp, lax, lrm, lsm, lum,
	     lxo, ltn1, ltn2;
    static real fini;
    extern /* Subroutine */ int lmbm_(integer *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *, integer *, integer *, integer *, integer *, integer *,
	     integer *, integer *, integer *, real *, real *);
    static integer ltmc1, ltmc2, ltmc3, ltmc4, ltmc5, ltmc6, ltmat;
    static real start;
    extern /* Subroutine */ int getime_(real *, real *);
    static integer lsmtgp, lumtgp, lumtum;
    extern /* Subroutine */ int wprint_(integer *, integer *, integer *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Subroutines */
/*     CPU-time */

/*     CPU-time */

    /* Parameter adjustments */
    --w;
    --rtim;
    --iout;
    --ipar;
    --rpar;
    --x;

    /* Function Body */
    getime_(&start, &rtim[1]);

/*     Initialization and error checking */

    iout[3] = 0;
    if (*n <= 0) {
	iout[3] = -5;
	wprint_(&iout[3], &ipar[5], &c__1);
	return 0;
    }
    if (*mcu < 3) {
	iout[3] = -5;
	wprint_(&iout[3], &ipar[5], &c__2);
	return 0;
    }
    if (*na < 2) {
	iout[3] = -5;
	wprint_(&iout[3], &ipar[5], &c__3);
	return 0;
    }
    if (rpar[7] >= .25) {
	iout[3] = -5;
	wprint_(&iout[3], &ipar[5], &c__4);
	return 0;
    }
    if (*nw < *n * 9 + 1 + (*n << 1) * *na + *na * 3 + (*n << 1) * (*mcu + 1) 
	    + (*mcu + 2) * 3 * (*mcu + 1) / 2 + (*mcu + 1) * 9) {
	iout[3] = -6;
	wprint_(&iout[3], &ipar[5], &c__0);
	return 0;
    }
    if (ipar[6] > 1 || ipar[6] < 0) {
	ipar[6] = 0;
    }
    if (ipar[7] > 6 || ipar[7] < 0) {
	ipar[7] = 2;
    }
    if (*mc > *mcu) {
	*mc = *mcu;
	wprint_(&iout[3], &ipar[5], &c_n1);
    }
    if (*mc <= 0) {
	*mc = 3;
    }

/*     Pointers for working array W */

    lxo = 1;
    ls = lxo + *n;
    lg = ls + *n;
    lgp = lg + *n;
    lga = lgp + *n;
    lu = lga + *n;
    ld = lu + *n;
    lax = ld + *n;
    lag = lax + *n * *na;
    laf = lag + *n * *na;
    lsm = laf + *na * 3;
    lum = lsm + *n * (*mcu + 1);
    lrm = lum + *n * (*mcu + 1);
    lumtum = lrm + (*mcu + 2) * (*mcu + 1) / 2;
    lc = lumtum + (*mcu + 2) * (*mcu + 1) / 2;
    lsmtgp = lc + *mcu + 1;
    lumtgp = lsmtgp + *mcu + 1;
    ltmc1 = lumtgp + *mcu + 1;
    ltmc2 = ltmc1 + *mcu + 1;
    ltmc3 = ltmc2 + *mcu + 1;
    ltmc4 = ltmc3 + *mcu + 1;
    ltmc5 = ltmc4 + *mcu + 1;
    ltmc6 = ltmc5 + *mcu + 1;
    ltn1 = ltmc6 + *mcu + 1;
    ltn2 = ltn1 + *n;
    ltmat = ltn2 + *n;

/*     Solution */

    lmbm_(n, na, mc, mcu, &x[1], &w[lxo], &w[ls], &w[lg], &w[lgp], &w[lga], &
	    w[lu], &w[ld], f, &w[lax], &w[lag], &w[laf], &w[lsm], &w[lum], &w[
	    lrm], &w[lumtum], &w[lc], &w[lsmtgp], &w[lumtgp], &w[ltmc1], &w[
	    ltmc2], &w[ltmc3], &w[ltmc4], &w[ltmc5], &w[ltmc6], &w[ltn1], &w[
	    ltn2], &w[ltmat], &rpar[1], &rpar[2], &rpar[3], &rpar[4], &rpar[5]
	    , &rpar[6], &rpar[7], &rpar[8], &ipar[2], &ipar[3], &ipar[1], &
	    ipar[4], &ipar[5], &ipar[6], &ipar[7], &iout[1], &iout[2], &iout[
	    3], time, &rtim[1]);

/*     CPU-time */

    getime_(&fini, &rtim[1]);
    rtim[1] = fini - start;
    return 0;
} /* lmbmu_ */

/* *********************************************************************** */

/*     * SUBROUTINE LMBM * */


/*     * Purpose * */

/*     Limited memory bundle subroutine for nonsmooth optimization. */


/*     * Calling sequence * */

/*     CALL LMBM(N,NA,MC,MCU,X,XO,S,G,GP,GA,U,D,F,AX,AG,AF,SM,UM,RM, */
/*    &     UMTUM,C,SMTGP,UMTGP,TMPMC1,TMPMC2,TMPMC3,TMPMC4,TMPMC5, */
/*    &     TMPMC6,TMPN1,TMPN2,TMPMAT,TOLF,TOLB,TOLG,TOLG2,ETA,EPSL, */
/*    &     XMAX,MIT,MFE,MOS,MTESF,IPRINT,METHOD,ISCALE,NIT,NFE,ITERM, */
/*    &     TIME,RTIM) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     II  NA              Maximum bundle dimension. */
/*     IU  MC              Maximum number of stored corrections. */
/*     II  MCU             Upper limit for maximum number of stored */
/*                           corrections, MCU >= MC. */
/*     RU  X(N)            Vector of variables. */
/*     RA  XO(N)           Previous vector of variables. */
/*     RA  G(N)            Subgradient of the objective function. */
/*     RA  GP(N)           Previous subgradient of the objective function. */
/*     RA  GA(N)           Aggregate subgradient. */
/*     RA  S(N)            Difference of current and previous variables. */
/*     RA  U(N)            Difference of current and previous */
/*                           subgradients. */
/*     RA  D(N)            Search direction. */
/*     RO  F               Value of the objective function. */
/*     RA  AX(N*NA)        Matrix whose columns are bundle points. */
/*     RA  AG(N*NA)        Matrix whose columns are bundle subgradients. */
/*     RA  AF(3*NA)        Vector of bundle values. */
/*     RA  SM(N*(MC+1))    Matrix whose columns are stored differences of */
/*                           variables. */
/*     RA  UM(N*(MC+1))    Matrix whose columns are stored subgradient */
/*                           differences. */
/*     RA  RM((MC+2)*(MC+1)/2)  Upper triangular matrix stored columnwise */
/*                                in the one-dimensional array. */
/*     RU  UMTUM((MC+2)*(MC+1)/2)  Auxiliary matrix: TRANS(UM)*UM. */
/*     RU  C(MC+1)         Diagonal matrix. */
/*     RU  SMTGP(MC+1)     Auxiliary vector. */
/*     RU  UMTGP(MC+1)     Auxiliary vector. */
/*     RA  TMPMC#(MC+1)    Auxiliary arrays; # = 1,...,6. */
/*     RA  TMPN#(N)        Auxiliary arrays; # = 1,2. */
/*     RA  TMPMAT((MC+1)*(MC)/2)   Auxiliary matrix. */
/*     RI  TIME            Maximum CPU-time in seconds. If TIME <= 0.0 */
/*                           the maximum time is ignored. REAL argument. */
/*     RI  RTIM(2)         Auxiliary array. REAL array. */
/*                           On input RTIM(1) contains the starting time. */
/*     RI  TOLF            Tolerance for change of function values. */
/*     RI  TOLF2           Second tolerance for change of function */
/*                           values. */
/*     RI  TOLB            Tolerance for the function value. */
/*     RI  TOLG            Tolerance for the first termination criterion. */
/*     RI  TOLG2           Tolerance for the second termination criterion. */
/*     RI  ETA             Distance measure parameter, ETA >= 0. */
/*     RI  EPSL            Line search parameter, 0 < EPSL < 0.25. */
/*     RI  XMAX            Maximum stepsize, 1 < XMAX. */
/*     II  MIT             Maximun number of iterations. */
/*     II  MFE             Maximun number of function evaluations. */
/*     II  MOS             Exponent for distance measure. */
/*     II  MTESF           Maximum number of iterations with changes of */
/*                           function values smaller than TOLF. */
/*     II  IPRINT          Printout specification: */
/*                            -1  - No printout. */
/*                             0  - Only the error messages. */
/*                             1  - The final values of the objective */
/*                                  function. */
/*                             2  - The final values of the objective */
/*                                  function and the most serious */
/*                                  warning messages. */
/*                             3  - The whole final solution. */
/*                             4  - At each iteration values of the */
/*                                  objective function. */
/*                             5  - At each iteration the whole */
/*                                  solution */
/*     II  METHOD          Selection of the method: */
/*                             0  - Limited memory bundle method. */
/*                             1  - L-BFGS bundle method. */
/*     II  ISCALE          Selection of the scaling: */
/*                             0  - Scaling at every iteration */
/*                                  with STU/UTU. */
/*                             1  - Scaling at every iteration */
/*                                  with STS/STU. */
/*                             2  - Interval scaling with STU/UTU. */
/*                             3  - Interval scaling with STS/STU. */
/*                             4  - Preliminary scaling with STU/UTU. */
/*                             5  - Preliminary scaling with STS/STU. */
/*                             6  - No scaling. */
/*     IO  NIT             Number of used iterations. */
/*     IO  NFE             Number of used function evaluations. */
/*     IO  ITERM           Cause of termination: */
/*                             1  - The problem has been solved. */
/*                                  with desired accuracy. */
/*                             2  - (F - FO) < TOLF in MTESF */
/*                                  subsequent iterations. */
/*                             3  - (F - FO) < TOLF*SMALL*MAX(|F|,|FO|,1). */
/*                             4  - Number of function calls > MFE. */
/*                             5  - Number of iterations > MIT. */
/*                             6  - Time limit exceeded. */
/*                             7  - F < TOLB. */
/*                            -1  - Two consecutive restarts. */
/*                            -2  - Number of restarts > maximum number */
/*                                  of restarts. */
/*                            -3  - Failure in function or subgradient */
/*                                  calculations (assigned by the user). */
/*                            -4  - Failure in attaining the demanded */
/*                                  accuracy. */
/*                            -5  - Invalid input parameters. */
/*                            -6  - Not enough working space. */


/*     * Local parameters * */

/*     I   MAXEPS          Maximum number of consecutive equal stopping */
/*                           criterions. */
/*     I   MAXNRS          Maximum number of restarts. */
/*     R   ETA9            Maximum for real numbers. */
/*     R   FMIN            Smallest acceptable value of the function. */
/*     R   TMIN            Minimum stepsize. */
/*     R   LENGTHD         Direction vector length. */
/*     R   RHO             Correction parameter. */


/*     * Local variables * */

/*     I   INEW            Index for the circular arrays. */
/*     I   IBUN            Index for the circular arrays in bundle. */
/*     I   IBFGS           Index of the type of BFGS update. */
/*     I   ISR1            Index of the type of SR1 update. */
/*     I   ITERS           Null step indicator. */
/*                              0  - Null step. */
/*                              1  - Serious step. */
/*     I   IC              Correction indicator. */
/*     I   ICN             Correction indicator for null steps. */
/*     I   IFLAG           Index for adaptive version: */
/*                              0  - Maximum number of stored corrections */
/*                                     has not been changed. */
/*                              1  - Maximum number of stored corrections */
/*                                     has been changed. */
/*     I   NAC             Current size of the bundle. */
/*     I   MCC             Current number of stored corrections. */
/*     I   MCINIT          Initial maximum number of stored corrections. */
/*     I   NEPS            Number of consecutive equal stopping */
/*                           criterions. */
/*     I   NNK             Consecutive null steps counter. */
/*     I   NRES            Number of consecutive restarts. */
/*     I   NRESS           Number of consecutive restarts in case of */
/*                           TMAX<TMIN. */
/*     I   NCRES           Number of restars. */
/*     I   NTESF           Number of tests on function decrease. */
/*     R   ALFN            Locality measure. */
/*     R   ALFV            Aggregate locality measure. */
/*     R   EPSR            Line search parameter. */
/*     R   GAMMA           Scaling parameter. */
/*     R   P               Directional derivative. */
/*     R   FO              Previous value of objective function. */
/*     R   DNORM           Euclidean norm of the direction vector. */
/*     R   GNORM           Euclidean norm of the aggregate subgradient */
/*                           vector. */
/*     R   XNORM           Stopping criterion. */
/*     R   PXNORM          Previous stopping criterion. */
/*     R   T               Stepsize. */
/*     R   TMAX            Maximum stepsize. */
/*     R   THETA           Correction parameter for stepsize. */
/*     R   SMALL           The smallest positive number such that */
/*                           1.0 + SMALL > 1.0. */



/*     * Subprograms used * */

/*     S   AGBFGS          Simplified subgradient aggregation. */
/*     S   AGGSR1          Subgradient aggregation. */
/*     S   AGSKIP          Subgradient aggregation using BFGS update. */
/*     S   DLBFGS          Computing the search direction by limited */
/*                           memory BFGS update. */
/*     S   DLSKIP          Skipping the updates and computing the search */
/*                           direction by limited memory BFGS update. */
/*     S   DLSR1           Computing the search direction by limited */
/*                           memory SR1 update. */
/*     S   DOBUN           Bundle selection. */
/*     S   LLS             Line search using function values and */
/*                           derivatives. */
/*     S   RESTAR          Initialization. */
/*     S   TINIT           Calculation of initial step size. */
/*     S   COPY            Copying of a vector. */
/*     S   GETIME          Execution time. */
/*     S   XDIFFY          Difference of two vectors. */
/*     S   RPRINT          Printout the results. */
/*     S   WPRINT          Printout the error and warning messages. */
/*     RF  EPS0            The smallest positive number such that */
/*                           1.0 + EPS0 > 1.0. */
/*     RF  VDOT            Dot product of two vectors. */


/*     * EXTERNAL SUBROUTINES * */

/*     SE  FUNDER          Computation of the value and the subgradient of */
/*                         the objective function. Calling sequence: */
/*                         CALL FUNDER(N,X,F,G,ITERM), where N is a number of */
/*                         variables, X(N) is a vector of variables, F is */
/*                         the value of the objective function, G(N) is */
/*                         the subgradient of the objective function, and */
/*                         ITERM is the error indicator. */



/*     Napsu Karmitsa (2002 - 2004, last modified 2007) */


/* Subroutine */ int lmbm_(integer *n, integer *na, integer *mc, integer *mcu,
	 doublereal *x, doublereal *xo, doublereal *s, doublereal *g, 
	doublereal *gp, doublereal *ga, doublereal *u, doublereal *d__, 
	doublereal *f, doublereal *ax, doublereal *ag, doublereal *af, 
	doublereal *sm, doublereal *um, doublereal *rm, doublereal *umtum, 
	doublereal *c__, doublereal *smtgp, doublereal *umtgp, doublereal *
	tmpmc1, doublereal *tmpmc2, doublereal *tmpmc3, doublereal *tmpmc4, 
	doublereal *tmpmc5, doublereal *tmpmc6, doublereal *tmpn1, doublereal 
	*tmpn2, doublereal *tmpmat, doublereal *tolf, doublereal *tolf2, 
	doublereal *tolb, doublereal *tolg, doublereal *tolg2, doublereal *
	eta, doublereal *epsl, doublereal *xmax, integer *mit, integer *mfe, 
	integer *mos, integer *mtesf, integer *iprint, integer *method, 
	integer *iscale, integer *nit, integer *nfe, integer *iterm, real *
	time, real *rtim)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(void);
    double sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static doublereal p, t;
    static integer ic;
    static doublereal fo;
    static integer mcc, mal, icn, nnk;
    extern /* Subroutine */ int lls_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *, integer *, 
	    integer *, integer *);
    extern doublereal eps0_(void);
    static integer isr1;
    static doublereal alfn, alfv;
    static real ctim;
    static integer ibun, inew, neps, nres;
    static doublereal epsr, tmax;
    extern doublereal vdot_(integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int copy_(integer *, doublereal *, doublereal *);
    static integer nout;
    extern /* Subroutine */ int dlsr1_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     doublereal *, integer *), copy2_(integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    static integer iflag;
    static doublereal gamma;
    static integer ibfgs;
    static doublereal theta;
    extern /* Subroutine */ int dobun_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *);
    static doublereal small;
    static integer ncres, ntesf;
    static doublereal dnorm, gnorm;
    static integer iters;
    extern /* Subroutine */ int tinit_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *);
    static integer nress;
    static doublereal xnorm;
    extern /* Subroutine */ int aggsr1_(integer *, integer *, integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *)
	    , agbfgs_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *)
	    , dlbfgs_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    getime_(real *, real *), agskip_(integer *, integer *, integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *);
    static integer mcinit;
    extern /* Subroutine */ int funder_(integer *, doublereal *, doublereal *,
	     doublereal *, integer *), dlskip_(integer *, integer *, integer *
	    , integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *)
	    , xdiffy_(integer *, doublereal *, doublereal *, doublereal *), 
	    restar_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *, integer *), rprint_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *);
    extern lmbmnewiter_(integer *, integer *, doublereal *, doublereal *, doublereal *, doublereal *); //added by liuyang

    static real strtim;
    static doublereal pxnorm;
    extern /* Subroutine */ int wprint_(integer *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___29 = { 0, 6, 0, "(1X,'Entry to LMBM:')", 0 };
    static cilist io___30 = { 0, 6, 0, "(1X,'Entry to LBB:')", 0 };
    static cilist io___66 = { 0, 6, 0, "(1X,'Exit from LMBM:')", 0 };
    static cilist io___67 = { 0, 6, 0, "(1X,'Exit from LBB:')", 0 };


/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Functions */
/*     External Subroutines */
/*     Intrinsic Functions */
/*     Computational Time */
/*     Parameters */
    /* Parameter adjustments */
    --rtim;
    --tmpmat;
    --tmpn2;
    --tmpn1;
    --tmpmc6;
    --tmpmc5;
    --tmpmc4;
    --tmpmc3;
    --tmpmc2;
    --tmpmc1;
    --umtgp;
    --smtgp;
    --c__;
    --umtum;
    --rm;
    --um;
    --sm;
    --af;
    --ag;
    --ax;
    --d__;
    --u;
    --ga;
    --gp;
    --g;
    --s;
    --xo;
    --x;

    /* Function Body */
    if (*iprint > 3) {
	if (*method == 0) {
	    s_wsfe(&io___29);
	    e_wsfe();
	}
	if (*method == 1) {
	    s_wsfe(&io___30);
	    e_wsfe();
	}
    }

/*     Initialization */

    nout = 0;
    *nit = 0;
    *nfe = 0;
    ntesf = 0;
    nres = 1;
    ncres = -1;
    nress = 0;
    neps = 0;
    *iterm = 0;
    iters = 1;
    nnk = 0;
    isr1 = 0;
    alfn = 0.;
    alfv = 0.;
    mcinit = *mc;
    small = eps0_();
    strtim = rtim[1];
    if (*tolf <= 0.) {
	*tolf = 1e-8;
    }
    if (*tolf2 == 0.) {
	*tolf2 = 1e4;
    }
    if (*tolb == 0.) {
	*tolb = small - 1e60;
    }
    if (*tolg <= 0.) {
	*tolg = 1e-6;
    }
    if (*tolg2 <= 0.) {
	*tolg2 = *tolg;
    }
    if (*xmax <= 0.) {
	*xmax = 1.5;
    }
    if (*eta < 0.) {
	*eta = .5;
    }
    if (*epsl <= 0.) {
	*epsl = 1e-4;
    }
    if (*mos <= 0) {
	*mos = 2;
    }
    if (*mtesf <= 0) {
	*mtesf = 10;
    }
    if (*mit <= 0) {
	*mit = 10000;
    }
    if (*mfe <= 0) {
	*mfe = 20000;
    }
    tmax = *xmax;
    xnorm = 1e60;
    epsr = small + .25;
    if (*epsl * 2. >= epsr) {
	epsr = *epsl * 2. + small;
	if (epsr >= .5) {
	    wprint_(iterm, iprint, &c_n2);
	}
    }

/*     Computation of the value and the subgradient of the objective */
/*     function and the search direction for the first iteration */

    funder_(n, &x[1], f, &g[1], iterm);
    ++(*nfe);
    if (*iterm != 0) {
	goto L900;
    }
    restar_(n, mc, &mcc, &mcinit, &inew, &ibun, &ibfgs, &iters, &gp[1], &g[1],
	     &nnk, &alfv, &alfn, &gamma, &d__[1], &ic, &icn, &mal, &ncres, &
	    iflag);
    dobun_(n, na, &mal, &x[1], &g[1], f, &ax[1], &ag[1], &af[1], &iters, &
	    ibun);
    goto L200;

/*     Start of the iteration */

L100:

/*     Serious step initialization */

    if (iters > 0) {
	icn = 0;
	alfn = 0.;
	alfv = 0.;
    }

/*     Direction finding */

    if (iters > 0) {

/*     BFGS update and direction determination */

	dlbfgs_(n, mc, &mcc, &inew, &ibfgs, &iflag, &d__[1], &g[1], &gp[1], &
		s[1], &u[1], &sm[1], &um[1], &rm[1], &umtum[1], &c__[1], &
		smtgp[1], &umtgp[1], &gamma, &tmpmc1[1], &tmpmc2[1], &tmpmc3[
		1], &tmpmc4[1], &tmpn1[1], &small, method, iscale);
    } else {
	if (*method == 0) {

/*     SR1 update and direction determination */

	    dlsr1_(n, mc, &mcc, &inew, &isr1, &iflag, &d__[1], &gp[1], &ga[1],
		     &s[1], &u[1], &sm[1], &um[1], &rm[1], &umtum[1], &c__[1],
		     &smtgp[1], &umtgp[1], &gamma, &tmpmc1[1], &tmpmc2[1], &
		    tmpmc3[1], &tmpmc4[1], &tmpmc5[1], &tmpmc6[1], &tmpn1[1], 
		    &tmpn2[1], &tmpmat[1], &nnk, &small, iprint);
	    ibfgs = 0;
	} else {

/*     BFGS skipping and direction determination */

	    dlskip_(n, mc, &mcc, &inew, &ibfgs, &iflag, &d__[1], &ga[1], &sm[
		    1], &um[1], &rm[1], &umtum[1], &c__[1], &tmpmc1[1], &
		    tmpmc2[1], &gamma, &tmpmc3[1], &tmpmc4[1], &tmpmc5[1], &
		    tmpn1[1], iscale);
	}
    }
L200:

/*     Computational time */

    if (*time > 0.f) {
	getime_(&ctim, &rtim[1]);
	if (ctim - strtim > *time) {
	    *iterm = 6;
	    goto L900;
	}
    }

/*     Computation of norms */

    if (iters > 0) {
	gnorm = vdot_(n, &g[1], &g[1]);
	dnorm = sqrt(vdot_(n, &d__[1], &d__[1]));
	p = vdot_(n, &g[1], &d__[1]);
    } else {
	gnorm = vdot_(n, &ga[1], &ga[1]);
	dnorm = sqrt(vdot_(n, &d__[1], &d__[1]));
	p = vdot_(n, &ga[1], &d__[1]);
    }

/*     Test on descent direction */

    if (p + small * sqrt(gnorm) * dnorm <= 0.) {
	nres = 0;
    } else {
	++nres;
	if (nres == 1) {
	    wprint_(iterm, iprint, &c_n3);
	    restar_(n, mc, &mcc, &mcinit, &inew, &ibun, &ibfgs, &iters, &gp[1]
		    , &g[1], &nnk, &alfv, &alfn, &gamma, &d__[1], &ic, &icn, &
		    mal, &ncres, &iflag);
	    if (ncres > 2000) {
		nout = 2000;
		*iterm = -2;
		goto L900;
	    }
	    dobun_(n, na, &mal, &x[1], &g[1], f, &ax[1], &ag[1], &af[1], &
		    iters, &ibun);
	    goto L200;
	}
	nout = -1;
	*iterm = -1;
	goto L900;
    }

/*     Stopping criterion */

    ++(*nit);
    lmbmnewiter_(nit, nfe, &x[1], f, &g[1], &gnorm);
    pxnorm = xnorm;
    xnorm = -p + alfv * 2.;

/*     Tests for termination */

    if (xnorm <= *tolg * 1e3 && (mcc > 0 || ibfgs == 2)) {
	if (gnorm * .5 + alfv <= *tolg2 && xnorm <= *tolg) {
	    *iterm = 1;
	    goto L900;
	}
	if (*mc < *mcu && iflag == 0) {
	    ++(*mc);
	    iflag = 1;
	}
    }
    if (*nfe >= *mfe) {
	nout = *mfe;
	*iterm = 4;
	goto L900;
    }
    if (*nit >= *mit) {
	nout = *mit;
	*iterm = 5;
	goto L900;
    }
    if (*f <= *tolb) {
	*iterm = 7;
	goto L900;
    }
    if (iters == 0) {
	if ((d__1 = xnorm - pxnorm, abs(d__1)) <= small) {
	    ++neps;
	    if (neps > 20) {
		*iterm = -4;
		goto L900;
	    }
	} else {
	    neps = 0;
	}
    } else {
	neps = 0;
    }

/*     Correction */

    if (-p < gnorm * 1e-12 || icn == 1) {
	xnorm += gnorm * 1e-12;
	dnorm = sqrt(dnorm * dnorm - p * 2e-12 + gnorm * 
		9.9999999999999992e-25);
	if (iters > 0) {
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		d__[i__] -= g[i__] * 1e-12;
/* L230: */
	    }
	} else {
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		d__[i__] -= ga[i__] * 1e-12;
/* L240: */
	    }
	    icn = 1;
	}
	ic = 1;
    } else {
	ic = 0;
    }
    if (pxnorm < xnorm && nnk > 2) {
	wprint_(iterm, iprint, &c_n4);
    }
    d__1 = gnorm * .5 + alfv;
    rprint_(n, nit, nfe, &x[1], f, &xnorm, &d__1, iterm, iprint);

/*     Preparation of line search */

    fo = *f;
    if (iters > 0) {
	copy2_(n, &x[1], &xo[1], &g[1], &gp[1]);
    }
    if (dnorm > 0.) {
	tmax = *xmax / dnorm;
    }
    if (tmax > 1e-12) {
	nress = 0;
    } else {
	++nress;
	if (nress == 1) {
	    wprint_(iterm, iprint, &c_n5);
	    restar_(n, mc, &mcc, &mcinit, &inew, &ibun, &ibfgs, &iters, &gp[1]
		    , &g[1], &nnk, &alfv, &alfn, &gamma, &d__[1], &ic, &icn, &
		    mal, &ncres, &iflag);
	    if (ncres > 2000) {
		nout = 2000;
		*iterm = -2;
		goto L900;
	    }
	    dobun_(n, na, &mal, &x[1], &g[1], f, &ax[1], &ag[1], &af[1], &
		    iters, &ibun);
	    goto L200;
	}
	*iterm = -1;
	goto L900;
    }

/*     Initial step size */

    tinit_(n, na, &mal, &x[1], &af[1], &ag[1], &ax[1], &ibun, &d__[1], f, &p, 
	    &t, &tmax, &c_b20, eta, &c_b21, mos, &iters);

/*     Line search with directional derivatives which allows null steps */

    theta = 1.;
    if (dnorm > 1e20) {
	theta = 1e20 / dnorm;
    }
    lls_(n, &x[1], &g[1], &d__[1], &xo[1], &t, &fo, f, &p, &alfn, &c_b20, &
	    dnorm, &xnorm, &theta, epsl, &epsr, eta, mos, &iters, nfe, &nnk, 
	    iterm);
    if (*iterm != 0) {
	goto L900;
    }
    if (*tolf2 >= 0.) {
/* Computing MAX */
	d__2 = abs(*f), d__3 = abs(fo), d__2 = max(d__2,d__3);
	if ((d__1 = fo - *f, abs(d__1)) <= *tolf2 * small * max(d__2,1.) && 
		iters == 1) {
	    *iterm = 3;
	    goto L900;
	}
    }
    if ((d__1 = fo - *f, abs(d__1)) <= *tolf) {
	++ntesf;
	if (ntesf >= *mtesf && iters == 1) {
	    *iterm = 2;
	    goto L900;
	}
    } else {
	ntesf = 0;
    }

/*     Bundle updating */

    dobun_(n, na, &mal, &x[1], &g[1], f, &ax[1], &ag[1], &af[1], &iters, &
	    ibun);

/*     Computation of variables difference */

    xdiffy_(n, &x[1], &xo[1], &s[1]);

/*     Computation of aggregate values and gradients difference */

    if (iters == 0) {
	++nnk;
	if (nnk == 1) {
	    copy_(n, &gp[1], &tmpn1[1]);
	    xdiffy_(n, &g[1], &gp[1], &u[1]);
	    agbfgs_(n, mc, &mcc, &inew, &ibfgs, &iflag, &g[1], &gp[1], &ga[1],
		     &u[1], &d__[1], &sm[1], &um[1], &rm[1], &c__[1], &umtum[
		    1], &alfn, &alfv, &gamma, &tmpmc1[1], &tmpmc2[1], &ic, &
		    c_b20);
	} else {
	    if (*method == 0) {
		copy_(n, &ga[1], &tmpn1[1]);
		aggsr1_(n, mc, &mcc, &inew, &iflag, &g[1], &gp[1], &ga[1], &
			d__[1], &alfn, &alfv, &tmpmat[1], &umtum[1], &rm[1], &
			gamma, &smtgp[1], &umtgp[1], &tmpmc1[1], &tmpmc2[1], &
			sm[1], &um[1], &tmpmc3[1], &tmpmc4[1], &tmpn2[1], &x[
			1], &u[1], &icn, &c_b20, &small);
		xdiffy_(n, &g[1], &gp[1], &u[1]);
	    } else {
		copy_(n, &ga[1], &tmpn1[1]);
		xdiffy_(n, &g[1], &gp[1], &u[1]);
		agskip_(n, mc, &mcc, &inew, &iflag, &g[1], &gp[1], &ga[1], &
			d__[1], &u[1], &alfn, &alfv, &umtum[1], &rm[1], &c__[
			1], &gamma, &smtgp[1], &umtgp[1], &tmpmc1[1], &tmpmc2[
			1], &sm[1], &um[1], &tmpmc3[1], &tmpmc4[1], &tmpn2[1],
			 &icn, &c_b20, &small);
	    }
	}
	copy_(n, &xo[1], &x[1]);
	*f = fo;
    } else {
	if (nnk != 0) {
	    copy_(n, &ga[1], &tmpn1[1]);
	} else {
	    copy_(n, &gp[1], &tmpn1[1]);
	}
	nnk = 0;
	xdiffy_(n, &g[1], &gp[1], &u[1]);
    }
    goto L100;
L900:

/*     Printout the final results */

    if (*iprint > 3) {
	if (*method == 0) {
	    s_wsfe(&io___66);
	    e_wsfe();
	}
	if (*method == 1) {
	    s_wsfe(&io___67);
	    e_wsfe();
	}
    }
    wprint_(iterm, iprint, &nout);
    d__1 = gnorm * .5 + alfv;
    rprint_(n, nit, nfe, &x[1], f, &xnorm, &d__1, iterm, iprint);
    return 0;
} /* lmbm_ */

