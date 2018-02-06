/* lmsub.f -- translated by f2c (version 20060506).
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
static integer c__0 = 0;
static doublereal c_b15 = 1.;
static integer c__3 = 3;
static doublereal c_b72 = .5;

/* *********************************************************************** */


/*     LMSUB includes the following subroutines */

/*     S   AGBFGS          Simplified subgradient aggregation. */
/*     S   AGGSR1          Subgradient aggregation. */
/*     S   AGSKIP          Subgradient aggregation using BFGS update. */
/*     S   DESTEP          Stepsize determination for descent steps. */
/*     S   DLBFGS          Computing the search direction by limited */
/*                           memory BFGS update. */
/*     S   DLSKIP          Skipping the updates and computing the search */
/*                           direction by limited memory BFGS update. */
/*     S   DLSR1           Computing the search direction by limited */
/*                           memory SR1 update. */
/*     S   DOBUN           Bundle selection. */
/*     S   GETIME          Execution time. */
/*     S   INDIC1          Initialization of indices. */
/*     S   LLS             Line search using function values and */
/*                           derivatives. */
/*     S   NULSTP          Stepsize determination for null steps. */
/*     S   QINT            Quadratic interpolation for line search */
/*                           with one directional derivative. */
/*     S   RESTAR          Initialization. */
/*     S   RPRINT          Printout the results. */
/*     S   WPRINT          Printout the error and warning messages. */
/*     S   TINIT           Calculation of initial step size. */

/*     RF  SCLPAR          Calculation of the scaling parameter. */


/* *********************************************************************** */

/*     * SUBROUTINE DLBFGS * */


/*     * Purpose * */

/*     Matrix update and computation of the search direction D = -DM*G */
/*     by the limited memory BFGS update. */


/*     * Calling sequence * */

/*     CALL DLBFGS(N,MC,MCC,INEW,IBFGS,IFLAG,D,G,GP,S,U,SM,UM,R, */
/*    &     UMTUM,C,SMTGP,UMTGP,GAMMA,TMPMC1,TMPMC2,TMPMC3,TMPMC4, */
/*    &     TMPN1,SMALL,METHOD,ISCALE) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     II  MC              Declared number of stored corrections. */
/*     IU  MCC             Current number of stored corrections. */
/*     IU  INEW            Index for circular arrays. */
/*     IO  IBFGS           Index of the type of BFGS update: */
/*                             1  - BFGS update: the corrections are */
/*                                    stored. */
/*                             2  - BFGS update: the corrections are not */
/*                                    stored. */
/*                             3  - BFGS update is skipped. */
/*     IU  IFLAG           Index for adaptive version: */
/*                             0  - Maximum number of stored corrections */
/*                                    has not been changed at previous */
/*                                    iteration. */
/*                             1  - Maximum number of stored corrections */
/*                                    has been changed at previous */
/*                                    iteration. */
/*     RO  D(N)            Search direction. */
/*     RI  G(N)            Current subgradient of the objective */
/*                           function. */
/*     RI  GP(N)           Previous subgradient of the objective */
/*                           function. */
/*     RI  S(N)            Difference of current and previous variables. */
/*     RI  U(N)            Difference of current and previous */
/*                           subgradients. */
/*     RU  SM(N*(MC+1))    Matrix whose columns are stored corrections. */
/*     RU  UM(N*(MC+1))    Matrix whose columns are stored subgradient */
/*                           differences. */
/*     RU  R((MC+2)*(MC+1)/2)  Upper triangular matrix stored columnwise */
/*                               in the one-dimensional array. */
/*     RU  UMTUM((MC+2)*(MC+1)/2)  Matrix UMTUM = TRANS(UM) * UM. */
/*     RU  C(MC+1)         Diagonal matrix. */
/*     RU  SMTGP(MC+1)     Vector SMTGP = TRANS(SM)*GP. */
/*     RU  UMTGP(MC+1)     Vector UMTGP = TRANS(UM)*GP. */
/*     RU  GAMMA           Scaling parameter. */
/*     RA  TMPMC#(MC+1)    Auxiliary arrays; # = 1,...,4. */
/*     RA  TMPN1(N)        Auxiliary array: */
/*                           On input: Previous aggregate subgradient. */
/*     RI  SMALL           Small positive value. */
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


/*     * Local variables * */

/*     I   MCNEW           Current size of vectors. */
/*     I   IOLD            Index of the oldest corrections. */
/*     R   STU             STU = TRANS(S)*U. */
/*     R   STS             STS = TRANS(S)*S. */
/*     I   IFLAG2          Index for adaptive version. */


/*     * Subprograms used * */

/*     S   COPY2           Copying of two vectors. */
/*     S   INDIC1          Initialization of indices. */
/*     S   XDIFFY          Difference of two vectors. */
/*     S   VXDIAG          Multiplication of a vector and a diagonal */
/*                           matrix. */
/*     S   VNEG            Copying of a vector with change of the sign */
/*     S   XSUMY           Sum of two vectors. */
/*     S   SCSUM           Sum of a vector and the scaled vector. */
/*     S   SCDIFF          Difference of the scaled vector and a vector. */
/*     S   CWMAXV          Multiplication of a vector by a dense */
/*                           rectangular matrix. */
/*     S   RWAXV2          Multiplication of two rowwise stored dense */
/*                           rectangular matrices A and B by vectors X */
/*                           and Y. */
/*     S   SYMAX           Multiplication of a dense symmetric matrix */
/*                           by a vector. */
/*     S   TRLIEQ          Solving X from linear equation L*X=Y or */
/*                           TRANS(L)*X=Y. */

/*     RF  VDOT            Dot product of two vectors. */
/*     RF  SCLPAR          Calculation of the scaling parameter. */


/*     The variable and subgradient differences and all the MC-vectors are */
/*     stored in a circular order controlled by the parameter point inew. */



/*     Napsu Karmitsa (2002,2003, last modified 2007) */

/* Subroutine */ int dlbfgs_(integer *n, integer *mc, integer *mcc, integer *
	inew, integer *ibfgs, integer *iflag, doublereal *d__, doublereal *g, 
	doublereal *gp, doublereal *s, doublereal *u, doublereal *sm, 
	doublereal *um, doublereal *r__, doublereal *umtum, doublereal *c__, 
	doublereal *smtgp, doublereal *umtgp, doublereal *gamma, doublereal *
	tmpmc1, doublereal *tmpmc2, doublereal *tmpmc3, doublereal *tmpmc4, 
	doublereal *tmpn1, doublereal *small, integer *method, integer *
	iscale)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    static integer i__, j, k;
    static doublereal sts, stu;
    static integer iold;
    extern /* Subroutine */ int vneg_(integer *, doublereal *, doublereal *);
    static integer ierr;
    extern doublereal vdot_(integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int copy2_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);
    static integer mcnew;
    extern /* Subroutine */ int scsum_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), symax_(integer *, integer *, integer 
	    *, doublereal *, doublereal *, doublereal *);
    static integer iflag2;
    extern /* Subroutine */ int indic1_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), xsumy_(
	    integer *, doublereal *, doublereal *, doublereal *), rwaxv2_(
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *), scdiff_(integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), vxdiag_(
	    integer *, doublereal *, doublereal *, doublereal *);
    extern doublereal sclpar_(integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    extern /* Subroutine */ int xdiffy_(integer *, doublereal *, doublereal *,
	     doublereal *), trlieq_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    cwmaxv_(integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Functions */
/*     Intrinsic Functions */
/*     External Subroutines */
    /* Parameter adjustments */
    --tmpn1;
    --tmpmc4;
    --tmpmc3;
    --tmpmc2;
    --tmpmc1;
    --umtgp;
    --smtgp;
    --c__;
    --umtum;
    --r__;
    --um;
    --sm;
    --u;
    --s;
    --gp;
    --g;
    --d__;

    /* Function Body */
    *ibfgs = 0;
    iflag2 = 0;
    stu = vdot_(n, &s[1], &u[1]);
    sts = vdot_(n, &s[1], &s[1]);

/*     Positive definiteness */

    if (stu > 0.) {
	if (-vdot_(n, &d__[1], &u[1]) - vdot_(n, &tmpn1[1], &s[1]) < -(*small)
		 || *method == 1) {

/*     Update matrices */

	    *ibfgs = 1;

/*     Initialization of indices. */

	    indic1_(mc, mcc, &mcnew, inew, &iold, iflag, &iflag2, ibfgs);

/*     Update SM and UM */

	    copy2_(n, &s[1], &sm[(*inew - 1) * *n + 1], &u[1], &um[(*inew - 1)
		     * *n + 1]);

/*     Computation of TRANS(SM)*G and TRANS(UM)*G */

	    if (*inew >= mcnew) {
		rwaxv2_(n, &mcnew, &sm[(*inew - mcnew) * *n + 1], &um[(*inew 
			- mcnew) * *n + 1], &g[1], &g[1], &tmpmc1[iold], &
			tmpmc2[iold]);
	    } else {
		rwaxv2_(n, inew, &sm[1], &um[1], &g[1], &g[1], &tmpmc1[1], &
			tmpmc2[1]);
		i__1 = mcnew - *inew;
		rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *
			n + 1], &g[1], &g[1], &tmpmc1[iold], &tmpmc2[iold]);
	    }

/*     Computation of TRANS(SM)*U and TRANS(UM)*U */

	    if (*inew >= mcnew) {
		i__1 = *inew - 1;
		for (i__ = iold; i__ <= i__1; ++i__) {
		    tmpmc3[i__] = tmpmc1[i__] - smtgp[i__];
		    smtgp[i__] = tmpmc1[i__];
		    tmpmc4[i__] = tmpmc2[i__] - umtgp[i__];
		    umtgp[i__] = tmpmc2[i__];
/* L30: */
		}
	    } else {
		i__1 = *inew - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    tmpmc3[i__] = tmpmc1[i__] - smtgp[i__];
		    smtgp[i__] = tmpmc1[i__];
		    tmpmc4[i__] = tmpmc2[i__] - umtgp[i__];
		    umtgp[i__] = tmpmc2[i__];
/* L10: */
		}
		i__1 = mcnew + 1;
		for (i__ = iold; i__ <= i__1; ++i__) {
		    tmpmc3[i__] = tmpmc1[i__] - smtgp[i__];
		    smtgp[i__] = tmpmc1[i__];
		    tmpmc4[i__] = tmpmc2[i__] - umtgp[i__];
		    umtgp[i__] = tmpmc2[i__];
/* L20: */
		}
	    }
	    tmpmc3[*inew] = tmpmc1[*inew] - vdot_(n, &s[1], &gp[1]);
	    smtgp[*inew] = tmpmc1[*inew];
	    tmpmc4[*inew] = tmpmc2[*inew] - vdot_(n, &u[1], &gp[1]);
	    umtgp[*inew] = tmpmc2[*inew];

/*     Update R and UMTUM */

	    if (*mcc >= *mc && iflag2 != 1) {
		i__1 = mcnew - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    j = (i__ - 1) * i__ / 2 + 1;
		    k = i__ * (i__ + 1) / 2 + 2;
		    copy2_(&i__, &r__[k], &r__[j], &umtum[k], &umtum[j]);
/* L40: */
		}
	    }
	    if (*inew >= mcnew) {
		copy2_(&mcnew, &tmpmc3[iold], &r__[(mcnew - 1) * mcnew / 2 + 
			1], &tmpmc4[iold], &umtum[(mcnew - 1) * mcnew / 2 + 1]
			);
	    } else {
		i__1 = mcnew - *inew;
		copy2_(&i__1, &tmpmc3[iold], &r__[(mcnew - 1) * mcnew / 2 + 1]
			, &tmpmc4[iold], &umtum[(mcnew - 1) * mcnew / 2 + 1]);
		copy2_(inew, &tmpmc3[1], &r__[(mcnew - 1) * mcnew / 2 + mcnew 
			- *inew + 1], &tmpmc4[1], &umtum[(mcnew - 1) * mcnew /
			 2 + mcnew - *inew + 1]);
	    }

/*     Update C */

	    c__[*inew] = stu;

/*     Computation of GAMMA */

	    *gamma = sclpar_(mcc, iscale, method, &sts, &stu, &tmpmc4[*inew], 
		    small);
	    ++(*inew);
	    if (*inew > *mc + 1) {
		*inew = 1;
	    }
	    if (*iflag == 0 && *mcc < *mc + 1) {
		++(*mcc);
	    }
	} else {

/*     BFGS update, corrections are not saved. */

	    *ibfgs = 2;

/*     Initialization of indices. */

	    indic1_(mc, mcc, &mcnew, inew, &iold, iflag, &iflag2, ibfgs);

/*     Update SM and UM */

	    copy2_(n, &s[1], &sm[(*inew - 1) * *n + 1], &u[1], &um[(*inew - 1)
		     * *n + 1]);

/*     Computation of TRANS(SM)*G and TRANS(UM)*G */

	    rwaxv2_(n, &mcnew, &sm[1], &um[1], &g[1], &g[1], &tmpmc1[1], &
		    tmpmc2[1]);

/*     Computation of TRANS(SM)*U and TRANS(UM)*U */

	    if (iold != 1) {
		i__1 = *inew - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    tmpmc3[i__] = tmpmc1[i__] - smtgp[i__];
		    smtgp[i__] = tmpmc1[i__];
		    tmpmc4[i__] = tmpmc2[i__] - umtgp[i__];
		    umtgp[i__] = tmpmc2[i__];
/* L50: */
		}
		i__1 = mcnew;
		for (i__ = iold; i__ <= i__1; ++i__) {
		    tmpmc3[i__] = tmpmc1[i__] - smtgp[i__];
		    smtgp[i__] = tmpmc1[i__];
		    tmpmc4[i__] = tmpmc2[i__] - umtgp[i__];
		    umtgp[i__] = tmpmc2[i__];
/* L60: */
		}
	    } else {
		i__1 = mcnew - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    tmpmc3[i__] = tmpmc1[i__] - smtgp[i__];
		    smtgp[i__] = tmpmc1[i__];
		    tmpmc4[i__] = tmpmc2[i__] - umtgp[i__];
		    umtgp[i__] = tmpmc2[i__];
/* L70: */
		}
	    }
	    tmpmc3[*inew] = tmpmc1[*inew] - vdot_(n, &s[1], &gp[1]);
	    smtgp[*inew] = tmpmc1[*inew];
	    tmpmc4[*inew] = tmpmc2[*inew] - vdot_(n, &u[1], &gp[1]);
	    umtgp[*inew] = tmpmc2[*inew];

/*     Update R and UMTUM */

	    if (iold != 1) {
		i__1 = mcnew - *inew;
		copy2_(&i__1, &tmpmc3[iold], &r__[(mcnew - 1) * mcnew / 2 + 1]
			, &tmpmc4[iold], &umtum[(mcnew - 1) * mcnew / 2 + 1]);
		copy2_(inew, &tmpmc3[1], &r__[(mcnew - 1) * mcnew / 2 + mcnew 
			- *inew + 1], &tmpmc4[1], &umtum[(mcnew - 1) * mcnew /
			 2 + mcnew - *inew + 1]);
	    } else {
		copy2_(&mcnew, &tmpmc3[1], &r__[(mcnew - 1) * mcnew / 2 + 1], 
			&tmpmc4[1], &umtum[(mcnew - 1) * mcnew / 2 + 1]);
	    }

/*     Update C */

	    c__[*inew] = stu;

/*     Computation of GAMMA */

	    *gamma = sclpar_(mcc, iscale, method, &sts, &stu, &tmpmc4[*inew], 
		    small);
	}
    } else {

/*     BFGS update is skipped */

	*ibfgs = 3;
	if (*mcc == 0) {
	    *iflag = 0;
	    vneg_(n, &g[1], &d__[1]);
	    return 0;
	}

/*     Initialization of indices. */

	indic1_(mc, mcc, &mcnew, inew, &iold, iflag, &iflag2, ibfgs);

/*     Computation of GAMMA */

	if (*iscale >= 4) {
	    *gamma = 1.;
	}

/*     Computation of TRANS(SM)*G and TRANS(UM)*G */

	if (iold == 1 || iold == 2) {
	    rwaxv2_(n, &mcnew, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n 
		    + 1], &g[1], &g[1], &smtgp[iold], &umtgp[iold]);
	} else {
	    i__1 = *inew - 1;
	    rwaxv2_(n, &i__1, &sm[1], &um[1], &g[1], &g[1], &smtgp[1], &umtgp[
		    1]);
	    i__1 = mcnew - *inew + 1;
	    rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 
		    1], &g[1], &g[1], &smtgp[iold], &umtgp[iold]);
	}
    }

/*     Computation of two intermediate values TMPMC1 and TMPMC2 */

    if (iold == 1 || *ibfgs == 2) {
	trlieq_(&mcnew, &mcnew, &iold, &r__[1], &tmpmc1[1], &smtgp[1], &c__1, 
		&ierr);
	symax_(&mcnew, &mcnew, &iold, &umtum[1], &tmpmc1[1], &tmpmc3[1]);
	vxdiag_(&mcnew, &c__[1], &tmpmc1[1], &tmpmc2[1]);
	scsum_(&mcnew, gamma, &tmpmc3[1], &tmpmc2[1], &tmpmc2[1]);
	d__1 = -(*gamma);
	scsum_(&mcnew, &d__1, &umtgp[1], &tmpmc2[1], &tmpmc3[1]);
	trlieq_(&mcnew, &mcnew, &iold, &r__[1], &tmpmc2[1], &tmpmc3[1], &c__0,
		 &ierr);
    } else if (*iflag == 0) {
	i__1 = *mc + 1;
	trlieq_(&mcnew, &i__1, &iold, &r__[1], &tmpmc1[1], &smtgp[1], &c__1, &
		ierr);
	i__1 = *mc + 1;
	symax_(&mcnew, &i__1, &iold, &umtum[1], &tmpmc1[1], &tmpmc3[1]);
	i__1 = *mc + 1;
	vxdiag_(&i__1, &c__[1], &tmpmc1[1], &tmpmc2[1]);
	i__1 = *mc + 1;
	scsum_(&i__1, gamma, &tmpmc3[1], &tmpmc2[1], &tmpmc2[1]);
	i__1 = *mc + 1;
	d__1 = -(*gamma);
	scsum_(&i__1, &d__1, &umtgp[1], &tmpmc2[1], &tmpmc3[1]);
	i__1 = *mc + 1;
	trlieq_(&mcnew, &i__1, &iold, &r__[1], &tmpmc2[1], &tmpmc3[1], &c__0, 
		&ierr);
    } else {
	trlieq_(&mcnew, mc, &iold, &r__[1], &tmpmc1[1], &smtgp[1], &c__1, &
		ierr);
	symax_(&mcnew, mc, &iold, &umtum[1], &tmpmc1[1], &tmpmc3[1]);
	vxdiag_(mc, &c__[1], &tmpmc1[1], &tmpmc2[1]);
	scsum_(mc, gamma, &tmpmc3[1], &tmpmc2[1], &tmpmc2[1]);
	d__1 = -(*gamma);
	scsum_(mc, &d__1, &umtgp[1], &tmpmc2[1], &tmpmc3[1]);
	trlieq_(&mcnew, mc, &iold, &r__[1], &tmpmc2[1], &tmpmc3[1], &c__0, &
		ierr);
    }

/*     Computation of the search direction D */

    if (iold == 1 || *ibfgs == 2) {
	cwmaxv_(n, &mcnew, &um[1], &tmpmc1[1], &d__[1], &c_b15);
    } else {
	i__1 = *inew - 1;
	cwmaxv_(n, &i__1, &um[1], &tmpmc1[1], &d__[1], &c_b15);
	i__1 = mcnew - *inew + 1;
	cwmaxv_(n, &i__1, &um[(iold - 1) * *n + 1], &tmpmc1[iold], &tmpn1[1], 
		&c_b15);
	xsumy_(n, &d__[1], &tmpn1[1], &d__[1]);
    }
    xdiffy_(n, &d__[1], &g[1], &d__[1]);
    if (iold == 1 || *ibfgs == 2) {
	cwmaxv_(n, &mcnew, &sm[1], &tmpmc2[1], &tmpn1[1], &c_b15);
	scdiff_(n, gamma, &d__[1], &tmpn1[1], &d__[1]);
    } else {
	i__1 = *inew - 1;
	cwmaxv_(n, &i__1, &sm[1], &tmpmc2[1], &tmpn1[1], &c_b15);
	scdiff_(n, gamma, &d__[1], &tmpn1[1], &d__[1]);
	i__1 = mcnew - *inew + 1;
	cwmaxv_(n, &i__1, &sm[(iold - 1) * *n + 1], &tmpmc2[iold], &tmpn1[1], 
		&c_b15);
	xdiffy_(n, &d__[1], &tmpn1[1], &d__[1]);
    }
    return 0;
} /* dlbfgs_ */

/* *********************************************************************** */

/*     * SUBROUTINE DLSKIP * */


/*     * Purpose * */

/*     Matrix skipping and computation of the search direction D = -DM*GA */
/*     by the limited memory BFGS update. */


/*     * Calling sequence * */

/*     CALL DLSKIP(N,MC,MCC,INEW,IBFGS,IFLAG,D,GA,SM,UM,R,UMTUM,C,TMPMC1, */
/*    &     TMPMC2,GAMMA,TMPMC3,TMPMC4,TMPMC5,TMPN1,ISCALE) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     II  MC              Declared number of stored corrections. */
/*     IU  MCC             Current number of stored corrections. */
/*     IU  INEW            Index for circular arrays. */
/*     IO  IBFGS           Index of the type of BFGS update: */
/*                             1  - BFGS update: the corrections are */
/*                                    stored. */
/*                             2  - BFGS update: the corrections are not */
/*                                    stored. */
/*                             3  - BFGS update is skipped. */
/*     II  IFLAG           Index for adaptive version: */
/*                             0  - Maximum number of stored corrections */
/*                                    has not been changed at previous */
/*                                    iteration. */
/*                             1  - Maximum number of stored corrections */
/*                                    has been changed at previous */
/*                                    iteration. */
/*     RO  D(N)            Search direction. */
/*     RI  GA(N)           Current aggregate subgradient. */
/*     RU  SM(N*(MC+1))    Matrix whose columns are stored corrections. */
/*     RU  UM(N*(MC+1))    Matrix whose columns are stored subgradient */
/*                           differences. */
/*     RU  R((MC+2)*(MC+1)/2)  Upper triangular matrix stored columnwise */
/*                               in the one-dimensional array. */
/*     RU  UMTUM((MC+2)*(MC+1)/2)  Matrix UMTUM = TRANS(UM) * UM. */
/*     RU  C(MC+1)         Diagonal matrix. */
/*     RU  GAMMA           Scaling parameter. */
/*     RA  TMPMC1(MC+1)    Auxiliary array: */
/*                           On output: TRANS(SM)*GA. */
/*     RA  TMPMC2(MC+1)    Auxiliary array: */
/*                           On output: TRANS(UM)*GA. */
/*     RA  TMPMC#(MC+1)    Auxiliary arrays; # = 3-5. */
/*     RA  TMPN1(N)        Auxiliary array: */
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


/*     * Local variables * */

/*     I   MCNEW           Current size of vectors. */
/*     I   IOLD            Index of the oldest corrections. */


/*     * Subprograms used * */

/*     S   XDIFFY          Difference of two vectors. */
/*     S   VXDIAG          Multiplication of a vector and a diagonal */
/*                           matrix. */
/*     S   VNEG            Copying of a vector with change of the sign */
/*     S   XSUMY           Sum of two vectors. */
/*     S   CWMAXV          Multiplication of a vector by a dense */
/*                           rectangular matrix. */
/*     S   RWAXV2          Multiplication of two rowwise stored dense */
/*                           rectangular matrices A and B by vectors X */
/*                           and Y. */
/*     S   SCSUM           Sum of a vector and the scaled vector. */
/*     S   SCDIFF          Difference of the scaled vector and a vector. */
/*     S   SYMAX           Multiplication of a dense symmetric matrix */
/*                           by a vector. */
/*     S   TRLIEQ          Solving X from linear equation L*X=Y or */
/*                           TRANS(L)*X=Y. */
/*     S   INDIC1          Initialization of indices. */


/*     The variable and subgradient differences and all the MC-vectors are */
/*     stored in a circular order controlled by the parameter point inew. */



/*     Napsu Karmitsa (2004, last modified 2007) */

/* Subroutine */ int dlskip_(integer *n, integer *mc, integer *mcc, integer *
	inew, integer *ibfgs, integer *iflag, doublereal *d__, doublereal *ga,
	 doublereal *sm, doublereal *um, doublereal *r__, doublereal *umtum, 
	doublereal *c__, doublereal *tmpmc1, doublereal *tmpmc2, doublereal *
	gamma, doublereal *tmpmc3, doublereal *tmpmc4, doublereal *tmpmc5, 
	doublereal *tmpn1, integer *iscale)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    static integer iold;
    extern /* Subroutine */ int vneg_(integer *, doublereal *, doublereal *);
    static integer ierr, mcnew;
    extern /* Subroutine */ int scsum_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), symax_(integer *, integer *, integer 
	    *, doublereal *, doublereal *, doublereal *);
    static integer iflag2;
    extern /* Subroutine */ int indic1_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), xsumy_(
	    integer *, doublereal *, doublereal *, doublereal *), rwaxv2_(
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *), scdiff_(integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), vxdiag_(
	    integer *, doublereal *, doublereal *, doublereal *), xdiffy_(
	    integer *, doublereal *, doublereal *, doublereal *), trlieq_(
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), cwmaxv_(integer *, integer *,
	     doublereal *, doublereal *, doublereal *, doublereal *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Subroutines */

/*     BFGS update is skipped */

    /* Parameter adjustments */
    --tmpn1;
    --tmpmc5;
    --tmpmc4;
    --tmpmc3;
    --tmpmc2;
    --tmpmc1;
    --c__;
    --umtum;
    --r__;
    --um;
    --sm;
    --ga;
    --d__;

    /* Function Body */
    *ibfgs = 3;
    if (*mcc == 0) {
	*iflag = 0;
	vneg_(n, &ga[1], &d__[1]);
	return 0;
    }

/*     Initialization of indices. */

    indic1_(mc, mcc, &mcnew, inew, &iold, iflag, &iflag2, ibfgs);

/*     Computation of TRANS(SM)*GA and TRANS(UM)*GA */

    if (iold == 1 || iold == 2) {
	rwaxv2_(n, &mcnew, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 1],
		 &ga[1], &ga[1], &tmpmc1[iold], &tmpmc2[iold]);
    } else {
	i__1 = *inew - 1;
	rwaxv2_(n, &i__1, &sm[1], &um[1], &ga[1], &ga[1], &tmpmc1[1], &tmpmc2[
		1]);
	i__1 = mcnew - *inew + 1;
	rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 1], 
		&ga[1], &ga[1], &tmpmc1[iold], &tmpmc2[iold]);
    }

/*     Computation of GAMMA */

    if (*iscale >= 4) {
	*gamma = 1.;
    }

/*     Computation of two intermediate values TMPMC3 and TMPMC4 */

    if (iold == 1) {
	trlieq_(&mcnew, &mcnew, &iold, &r__[1], &tmpmc3[1], &tmpmc1[1], &c__1,
		 &ierr);
	symax_(&mcnew, &mcnew, &iold, &umtum[1], &tmpmc3[1], &tmpmc5[1]);
	vxdiag_(&mcnew, &c__[1], &tmpmc3[1], &tmpmc4[1]);
	scsum_(&mcnew, gamma, &tmpmc5[1], &tmpmc4[1], &tmpmc4[1]);
	d__1 = -(*gamma);
	scsum_(&mcnew, &d__1, &tmpmc2[1], &tmpmc4[1], &tmpmc5[1]);
	trlieq_(&mcnew, &mcnew, &iold, &r__[1], &tmpmc4[1], &tmpmc5[1], &c__0,
		 &ierr);
    } else if (*iflag == 0) {
	i__1 = *mc + 1;
	trlieq_(&mcnew, &i__1, &iold, &r__[1], &tmpmc3[1], &tmpmc1[1], &c__1, 
		&ierr);
	i__1 = *mc + 1;
	symax_(&mcnew, &i__1, &iold, &umtum[1], &tmpmc3[1], &tmpmc5[1]);
	i__1 = *mc + 1;
	vxdiag_(&i__1, &c__[1], &tmpmc3[1], &tmpmc4[1]);
	i__1 = *mc + 1;
	scsum_(&i__1, gamma, &tmpmc5[1], &tmpmc4[1], &tmpmc4[1]);
	i__1 = *mc + 1;
	d__1 = -(*gamma);
	scsum_(&i__1, &d__1, &tmpmc2[1], &tmpmc4[1], &tmpmc5[1]);
	i__1 = *mc + 1;
	trlieq_(&mcnew, &i__1, &iold, &r__[1], &tmpmc4[1], &tmpmc5[1], &c__0, 
		&ierr);
    } else {
	trlieq_(&mcnew, mc, &iold, &r__[1], &tmpmc3[1], &tmpmc1[1], &c__1, &
		ierr);
	symax_(&mcnew, mc, &iold, &umtum[1], &tmpmc3[1], &tmpmc5[1]);
	vxdiag_(mc, &c__[1], &tmpmc3[1], &tmpmc4[1]);
	scsum_(mc, gamma, &tmpmc5[1], &tmpmc4[1], &tmpmc4[1]);
	d__1 = -(*gamma);
	scsum_(mc, &d__1, &tmpmc2[1], &tmpmc4[1], &tmpmc5[1]);
	trlieq_(&mcnew, mc, &iold, &r__[1], &tmpmc4[1], &tmpmc5[1], &c__0, &
		ierr);
    }

/*     Computation of the search direction D */

    if (iold == 1) {
	cwmaxv_(n, &mcnew, &um[1], &tmpmc3[1], &d__[1], &c_b15);
    } else {
	i__1 = *inew - 1;
	cwmaxv_(n, &i__1, &um[1], &tmpmc3[1], &d__[1], &c_b15);
	i__1 = mcnew - *inew + 1;
	cwmaxv_(n, &i__1, &um[(iold - 1) * *n + 1], &tmpmc3[iold], &tmpn1[1], 
		&c_b15);
	xsumy_(n, &d__[1], &tmpn1[1], &d__[1]);
    }
    xdiffy_(n, &d__[1], &ga[1], &d__[1]);
    if (iold == 1) {
	cwmaxv_(n, &mcnew, &sm[1], &tmpmc4[1], &tmpn1[1], &c_b15);
	scdiff_(n, gamma, &d__[1], &tmpn1[1], &d__[1]);
    } else {
	i__1 = *inew - 1;
	cwmaxv_(n, &i__1, &sm[1], &tmpmc4[1], &tmpn1[1], &c_b15);
	scdiff_(n, gamma, &d__[1], &tmpn1[1], &d__[1]);
	i__1 = mcnew - *inew + 1;
	cwmaxv_(n, &i__1, &sm[(iold - 1) * *n + 1], &tmpmc4[iold], &tmpn1[1], 
		&c_b15);
	xdiffy_(n, &d__[1], &tmpn1[1], &d__[1]);
    }
    return 0;
} /* dlskip_ */

/* *********************************************************************** */

/*     * SUBROUTINE DLSR1 * */


/*     * Purpose * */

/*     Matrix update and computation of the search direction D = -DM*GA */
/*     by the limited memory SR1 update. */


/*     * Calling sequence * */

/*     CALL DLSR1(N,MC,MCC,INEW,ISR1,IFLAG,D,GP,GA,S,U,SM,UM,R,UMTUM,C, */
/*    &     SMTGP,UMTGP,GAMMA,TMPMC1,TMPMC2,TMPMC3,TMPMC4,TMPMC5,TMPMC6, */
/*    &     TMPN1,TMPN2,TMPMAT,NNK,SMALL,IPRINT) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     II  MC              Declared number of stored corrections. */
/*     IU  MCC             Current number of stored corrections. */
/*     IU  INEW            Index for circular arrays. */
/*     IO  ISR1            Index of the type of SR1 update. */
/*                           1  - SR1 update: the corrections are stored. */
/*                           3  - SR1 update is skipped. */
/*     IU  IFLAG           Index for adaptive version: */
/*                             0  - Maximum number of stored corrections */
/*                                    has not been changed at previous */
/*                                    iteration. */
/*                             1  - Maximum number of stored corrections */
/*                                    has been changed at previous */
/*                                    iteration. */
/*     RO  D(N)            Search direction. */
/*     RI  GP(N)           Basic subgradient of the objective function. */
/*     RI  GA(N)           Current aggregate subgradient. */
/*     RI  S(N)            Difference of auxiliary and current variables. */
/*     RI  U(N)            Difference of auxiliary and current */
/*                           subgradients. */
/*     RU  SM(N*(MC+1))    Matrix whose columns are stored corrections. */
/*     RU  UM(N*(MC+1))    Matrix whose columns are stored subgradient */
/*                           differences. */
/*     RU  R((MC+2)*(MC+1)/2)  Upper triangular matrix stored columnwise */
/*                               in the one-dimensional array. */
/*     RU  UMTUM((MC+2)*(MC+1)/2)  Matrix UMTUM = TRANS(UM) * UM. */
/*     RU  C(MC+1)         Diagonal matrix. */
/*     RU  SMTGP(MC+1)     Vector SMTGP = TRANS(SM)*GP. */
/*     RU  UMTGP(MC+1)     Vector UMTGP = TRANS(UM)*GP. */
/*     RU  GAMMA           Scaling parameter. */
/*     RA  TMPMC1(MC+1)    Auxiliary array: */
/*                           On output: TRANS(SM)*GA. */
/*     RA  TMPMC2(MC+1)    Auxiliary array: */
/*                           On output: TRANS(UM)*GA. */
/*     RA  TMPMC#(MC+1)    Auxiliary arrays: # = 3,6. */
/*     RA  TMPN1(N)        Auxiliary array: */
/*                           On input: previous aggregate subgradient. */
/*     RA  TMPN2(N)        Auxiliary array. */
/*     RA  TMPMAT((MC+1)*(MC)/2)  Auxiliary matrix. */
/*     II  NNK             Consecutive null steps counter. */
/*     RI  SMALL           Small positive value. */
/*     II  IPRINT          Printout specification. */


/*     * Local variables * */

/*     I   MCNEW           Current size of vectors. */
/*     I   IOLD            Index of the oldest corrections. */
/*     R   STU             STU = TRANS(S)*U. */
/*     I   IFLAG2          Index for adaptive version. */
/*     R   A               TRANS(GA) DM_(K-1) GA. */
/*     R   B               TRANS(GA) DM_K GA. */


/*     * Subprograms used * */

/*     RF  VDOT            Dot product of two vectors. */
/*     S   COPY            Copying of a vector. */
/*     S   XDIFFY          Difference of two vectors. */
/*     S   VNEG            Copying of a vector with change of the sign */
/*     S   SCALEX          Scaling a vector. */
/*     S   XSUMY           Sum of two vectors. */
/*     S   CWMAXV          Multiplication of a vector by a dense */
/*                           rectangular matrix. */
/*     S   RWAXV2          Multiplication of two rowwise stored dense */
/*                           rectangular matrices A and B by vectors X */
/*                           and Y. */
/*     S   CALQ            Solving X from linear equation A*X=Y. */
/*     S   SCDIFF          Difference of the scaled vector and a vector. */
/*     S   COPY2           Copying of two vectors. */
/*     S   INDIC1          Initialization of indices. */


/*     The variable and subgradient differences and all the MC-vectors are */
/*     stored in a circular order controlled by the parameter point inew. */



/*     Napsu Karmitsa (2002 - 2004, last modified 2007) */

/* Subroutine */ int dlsr1_(integer *n, integer *mc, integer *mcc, integer *
	inew, integer *isr1, integer *iflag, doublereal *d__, doublereal *gp, 
	doublereal *ga, doublereal *s, doublereal *u, doublereal *sm, 
	doublereal *um, doublereal *r__, doublereal *umtum, doublereal *c__, 
	doublereal *smtgp, doublereal *umtgp, doublereal *gamma, doublereal *
	tmpmc1, doublereal *tmpmc2, doublereal *tmpmc3, doublereal *tmpmc4, 
	doublereal *tmpmc5, doublereal *tmpmc6, doublereal *tmpn1, doublereal 
	*tmpn2, doublereal *tmpmat, integer *nnk, doublereal *small, integer *
	iprint)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    static doublereal a, b;
    static integer i__, j, k;
    static doublereal stu;
    extern /* Subroutine */ int calq_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *)
	    ;
    static integer iold;
    extern /* Subroutine */ int vneg_(integer *, doublereal *, doublereal *), 
	    copy_(integer *, doublereal *, doublereal *);
    extern doublereal vdot_(integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int copy2_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);
    static integer mcnew, iflag2;
    extern /* Subroutine */ int indic1_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), xsumy_(
	    integer *, doublereal *, doublereal *, doublereal *), rwaxv2_(
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *), scdiff_(integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), scalex_(
	    integer *, doublereal *, doublereal *, doublereal *), xdiffy_(
	    integer *, doublereal *, doublereal *, doublereal *), cwmaxv_(
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Functions */
/*     External Subroutines */
    /* Parameter adjustments */
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
    --r__;
    --um;
    --sm;
    --u;
    --s;
    --ga;
    --gp;
    --d__;

    /* Function Body */
    iflag2 = 0;
    *isr1 = 0;

/*     Initialization of indices */

    indic1_(mc, mcc, &mcnew, inew, &iold, iflag, &iflag2, &c__3);

/*     Computation of GAMMA */

    *gamma = 1.;

/*     Computation of TRANS(SM)*GA and TRANS(UM)*GA */

    if (iold == 1 || iold == 2) {
	rwaxv2_(n, &mcnew, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 1],
		 &ga[1], &ga[1], &tmpmc1[iold], &tmpmc2[iold]);
    } else {
	i__1 = *inew - 1;
	rwaxv2_(n, &i__1, &sm[1], &um[1], &ga[1], &ga[1], &tmpmc1[1], &tmpmc2[
		1]);
	i__1 = mcnew - *inew + 1;
	rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 1], 
		&ga[1], &ga[1], &tmpmc1[iold], &tmpmc2[iold]);
    }

/*     Positive definiteness */

    if (-vdot_(n, &d__[1], &u[1]) - vdot_(n, &tmpn1[1], &s[1]) >= -(*small)) {

/*     SR1 update is skipped */

	*isr1 = 3;
	if (*mcc == 0) {
	    *iflag = 0;
	    vneg_(n, &ga[1], &d__[1]);
	    return 0;
	}
	goto L200;
    }
    stu = vdot_(n, &s[1], &u[1]);
    tmpmc1[*inew] = vdot_(n, &s[1], &ga[1]);
    tmpmc2[*inew] = vdot_(n, &u[1], &ga[1]);

/*     Convergence conditions */

    if (*nnk == 1 || *mcc < *mc) {
	goto L100;
    }
    if (*iflag == 1 && (*inew == 1 || *inew == *mc)) {
	goto L100;
    }

/*     Calculation of matrix (UMTUM-R-TRANS(R)+C) from previous iteration */

    i__1 = mcnew * (mcnew + 1) / 2;
    for (i__ = 1; i__ <= i__1; ++i__) {
	tmpmat[i__] = *gamma * umtum[i__] - r__[i__];
/* L10: */
    }

/*     Computation of TMPMAT*TMPMC4 = GAMMA*TRANS(UM)*GA-TRANS(SM)*GA */

    if (iold == 1) {
	scdiff_(&mcnew, gamma, &tmpmc2[1], &tmpmc1[1], &tmpmc5[1]);
	calq_(&mcnew, &mcnew, &iold, &tmpmat[1], &tmpmc4[1], &tmpmc5[1], 
		small, &c__0);
    } else if (*iflag == 0) {
	i__1 = *mc + 1;
	scdiff_(&i__1, gamma, &tmpmc2[1], &tmpmc1[1], &tmpmc5[1]);
	i__1 = *mc + 1;
	calq_(&mcnew, &i__1, &iold, &tmpmat[1], &tmpmc4[1], &tmpmc5[1], small,
		 &c__0);
    } else {
	scdiff_(mc, gamma, &tmpmc2[1], &tmpmc1[1], &tmpmc5[1]);
	calq_(&mcnew, mc, &iold, &tmpmat[1], &tmpmc4[1], &tmpmc5[1], small, &
		c__0);
    }

/*     Computation of A = -TRANS(GA)*DM_(K-1)*GA */

    if (iold == 1 || iold == 2) {
	scalex_(&mcnew, gamma, &tmpmc4[iold], &tmpmc3[iold]);
	cwmaxv_(n, &mcnew, &sm[(iold - 1) * *n + 1], &tmpmc4[iold], &tmpn1[1],
		 &c_b15);
	d__1 = -(*gamma);
	scdiff_(n, &d__1, &ga[1], &tmpn1[1], &tmpn2[1]);
	cwmaxv_(n, &mcnew, &um[(iold - 1) * *n + 1], &tmpmc3[iold], &tmpn1[1],
		 &c_b15);
	xsumy_(n, &tmpn2[1], &tmpn1[1], &tmpn2[1]);
    } else {
	scalex_(mcc, gamma, &tmpmc4[1], &tmpmc3[1]);
	i__1 = *inew - 1;
	cwmaxv_(n, &i__1, &sm[1], &tmpmc4[1], &tmpn1[1], &c_b15);
	d__1 = -(*gamma);
	scdiff_(n, &d__1, &ga[1], &tmpn1[1], &tmpn2[1]);
	i__1 = mcnew - *inew + 1;
	cwmaxv_(n, &i__1, &sm[(iold - 1) * *n + 1], &tmpmc4[iold], &tmpn1[1], 
		&c_b15);
	xdiffy_(n, &tmpn2[1], &tmpn1[1], &tmpn2[1]);
	i__1 = *inew - 1;
	cwmaxv_(n, &i__1, &um[1], &tmpmc3[1], &tmpn1[1], &c_b15);
	xsumy_(n, &tmpn2[1], &tmpn1[1], &tmpn2[1]);
	i__1 = mcnew - *inew + 1;
	cwmaxv_(n, &i__1, &um[(iold - 1) * *n + 1], &tmpmc3[iold], &tmpn1[1], 
		&c_b15);
	xsumy_(n, &tmpn2[1], &tmpn1[1], &tmpn2[1]);
    }
    a = vdot_(n, &ga[1], &tmpn2[1]);
    if (*iflag == 0) {
	mcnew = *mc;
	iold = *inew + 2;
	if (iold > *mc + 1) {
	    iold = iold - *mc - 1;
	}
    } else {
	mcnew = *mc - 1;
	iold = *inew + 2;
	if (iold > *mc) {
	    iold -= *mc;
	}
    }

/*     Calculation of the new canditate for search direction */
/*     Updates are not necessarily saved */


/*     Update SM and UM */

    copy2_(n, &s[1], &sm[(*inew - 1) * *n + 1], &u[1], &um[(*inew - 1) * *n + 
	    1]);

/*     Computation of TRANS(SM)*U and TRANS(UM)*U */

    if (iold == 1 || iold == 2) {
	i__1 = mcnew - 1;
	rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 1], 
		&u[1], &u[1], &tmpmc3[iold], &tmpmc4[iold]);
    } else {
	i__1 = *inew - 1;
	rwaxv2_(n, &i__1, &sm[1], &um[1], &u[1], &u[1], &tmpmc3[1], &tmpmc4[1]
		);
	i__1 = mcnew - *inew;
	rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 1], 
		&u[1], &u[1], &tmpmc3[iold], &tmpmc4[iold]);
    }
    tmpmc3[*inew] = stu;
    tmpmc4[*inew] = vdot_(n, &u[1], &u[1]);

/*     Calculation of matrix (UMTUM-R-TRANS(R)+C) without updating */
/*     matrices R, UMTUM and C */

    i__1 = mcnew * (mcnew + 1) / 2;
    for (i__ = 1; i__ <= i__1; ++i__) {
	tmpmat[i__] = *gamma * umtum[i__] - r__[i__];
/* L20: */
    }
    i__1 = mcnew - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	j = (i__ - 1) * i__ / 2 + 1;
	k = i__ * (i__ + 1) / 2 + 2;
	copy_(&i__, &tmpmat[k], &tmpmat[j]);
/* L30: */
    }
    i__1 = mcnew + 1;
    scdiff_(&i__1, gamma, &tmpmc4[1], &tmpmc3[1], &tmpmc5[1]);
    if (*inew >= mcnew) {
	copy_(&mcnew, &tmpmc5[iold], &tmpmat[(mcnew - 1) * mcnew / 2 + 1]);
    } else {
	i__1 = mcnew - *inew;
	copy_(&i__1, &tmpmc5[iold], &tmpmat[(mcnew - 1) * mcnew / 2 + 1]);
	copy_(inew, &tmpmc5[1], &tmpmat[(mcnew - 1) * mcnew / 2 + mcnew - *
		inew + 1]);
    }
    if (*iflag == 0) {
	i__1 = *mc + 1;
	scdiff_(&i__1, gamma, &tmpmc2[1], &tmpmc1[1], &tmpmc5[1]);
	i__1 = *mc + 1;
	calq_(&mcnew, &i__1, &iold, &tmpmat[1], &tmpmc5[1], &tmpmc5[1], small,
		 iprint);
    } else {
	scdiff_(mc, gamma, &tmpmc2[1], &tmpmc1[1], &tmpmc5[1]);
	calq_(&mcnew, mc, &iold, &tmpmat[1], &tmpmc5[1], &tmpmc5[1], small, 
		iprint);
    }

/*     Calculation of the new canditate for search direction D = -DM_K*GA */
/*     and computation of B = -TRANS(GA)*DM_K*GA */

    if (iold == 1 || iold == 2) {
	scalex_(&mcnew, gamma, &tmpmc5[iold], &tmpmc6[iold]);
	cwmaxv_(n, &mcnew, &sm[(iold - 1) * *n + 1], &tmpmc5[iold], &tmpn1[1],
		 &c_b15);
	d__1 = -(*gamma);
	scdiff_(n, &d__1, &ga[1], &tmpn1[1], &d__[1]);
	cwmaxv_(n, &mcnew, &um[(iold - 1) * *n + 1], &tmpmc6[iold], &tmpn1[1],
		 &c_b15);
	xsumy_(n, &d__[1], &tmpn1[1], &d__[1]);
    } else {
	i__1 = mcnew + 1;
	scalex_(&i__1, gamma, &tmpmc5[1], &tmpmc6[1]);
	cwmaxv_(n, inew, &sm[1], &tmpmc5[1], &tmpn1[1], &c_b15);
	d__1 = -(*gamma);
	scdiff_(n, &d__1, &ga[1], &tmpn1[1], &d__[1]);
	i__1 = mcnew - *inew;
	cwmaxv_(n, &i__1, &sm[(iold - 1) * *n + 1], &tmpmc5[iold], &tmpn1[1], 
		&c_b15);
	xdiffy_(n, &d__[1], &tmpn1[1], &d__[1]);
	cwmaxv_(n, inew, &um[1], &tmpmc6[1], &tmpn1[1], &c_b15);
	xsumy_(n, &d__[1], &tmpn1[1], &d__[1]);
	i__1 = mcnew - *inew;
	cwmaxv_(n, &i__1, &um[(iold - 1) * *n + 1], &tmpmc6[iold], &tmpn1[1], 
		&c_b15);
	xsumy_(n, &d__[1], &tmpn1[1], &d__[1]);
    }
    b = vdot_(n, &ga[1], &d__[1]);

/*     Checking the convergence conditions */

    if (b - a < 0.) {
	*isr1 = 3;
	copy_(n, &tmpn2[1], &d__[1]);
    } else {
	*isr1 = 1;

/*     Update TRANS(SM)*GP and TRANS(UM)*GP */

	smtgp[*inew] = vdot_(n, &s[1], &gp[1]);
	umtgp[*inew] = vdot_(n, &u[1], &gp[1]);

/*     Update R and UMTUM */

	i__1 = mcnew - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = (i__ - 1) * i__ / 2 + 1;
	    k = i__ * (i__ + 1) / 2 + 2;
	    copy2_(&i__, &r__[k], &r__[j], &umtum[k], &umtum[j]);
/* L40: */
	}
	if (*inew >= mcnew) {
	    copy2_(&mcnew, &tmpmc3[iold], &r__[(mcnew - 1) * mcnew / 2 + 1], &
		    tmpmc4[iold], &umtum[(mcnew - 1) * mcnew / 2 + 1]);
	} else {
	    i__1 = mcnew - *inew;
	    copy2_(&i__1, &tmpmc3[iold], &r__[(mcnew - 1) * mcnew / 2 + 1], &
		    tmpmc4[iold], &umtum[(mcnew - 1) * mcnew / 2 + 1]);
	    copy2_(inew, &tmpmc3[1], &r__[(mcnew - 1) * mcnew / 2 + mcnew - *
		    inew + 1], &tmpmc4[1], &umtum[(mcnew - 1) * mcnew / 2 + 
		    mcnew - *inew + 1]);
	}

/*     Update C */

	c__[*inew] = stu;
	++(*inew);
	if (*inew > *mc + 1) {
	    *inew = 1;
	}
	if (*iflag == 0 && *mcc < *mc + 1) {
	    ++(*mcc);
	}
    }
    goto L300;
L100:
    *isr1 = 1;

/*     Initialization of indices */

    indic1_(mc, mcc, &mcnew, inew, &iold, iflag, &iflag2, &c__1);
    if (iflag2 == 1 && iold == 2) {
	tmpmc1[*inew] = tmpmc1[1];
	tmpmc2[*inew] = tmpmc2[1];
    }

/*     Update SM and UM */

    copy2_(n, &s[1], &sm[(*inew - 1) * *n + 1], &u[1], &um[(*inew - 1) * *n + 
	    1]);

/*     Update TRANS(SM)*GP and TRANS(UM)*GP */

    smtgp[*inew] = vdot_(n, &s[1], &gp[1]);
    umtgp[*inew] = vdot_(n, &u[1], &gp[1]);

/*     COMPUTATION OF TRANS(SM)*U and TRANS(UM)*U */

    if (iold == 1 || iold == 2) {
	i__1 = mcnew - 1;
	rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 1], 
		&u[1], &u[1], &tmpmc3[iold], &tmpmc4[iold]);
    } else {
	i__1 = *inew - 1;
	rwaxv2_(n, &i__1, &sm[1], &um[1], &u[1], &u[1], &tmpmc3[1], &tmpmc4[1]
		);
	i__1 = mcnew - *inew;
	rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 1], 
		&u[1], &u[1], &tmpmc3[iold], &tmpmc4[iold]);
    }
    tmpmc3[*inew] = stu;
    tmpmc4[*inew] = vdot_(n, &u[1], &u[1]);

/*     Update R and UMTUM */

    if (*mcc >= *mc && iflag2 != 1) {
	i__1 = mcnew - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = (i__ - 1) * i__ / 2 + 1;
	    k = i__ * (i__ + 1) / 2 + 2;
	    copy2_(&i__, &r__[k], &r__[j], &umtum[k], &umtum[j]);
/* L110: */
	}
    }
    if (*inew >= mcnew) {
	copy2_(&mcnew, &tmpmc3[iold], &r__[(mcnew - 1) * mcnew / 2 + 1], &
		tmpmc4[iold], &umtum[(mcnew - 1) * mcnew / 2 + 1]);
    } else {
	i__1 = mcnew - *inew;
	copy2_(&i__1, &tmpmc3[iold], &r__[(mcnew - 1) * mcnew / 2 + 1], &
		tmpmc4[iold], &umtum[(mcnew - 1) * mcnew / 2 + 1]);
	copy2_(inew, &tmpmc3[1], &r__[(mcnew - 1) * mcnew / 2 + mcnew - *inew 
		+ 1], &tmpmc4[1], &umtum[(mcnew - 1) * mcnew / 2 + mcnew - *
		inew + 1]);
    }

/*     Update C */

    c__[*inew] = stu;
    ++(*inew);
    if (*inew > *mc + 1) {
	*inew = 1;
    }
    if (*iflag == 0 && *mcc < *mc + 1) {
	++(*mcc);
    }
L200:
    i__1 = mcnew * (mcnew + 1) / 2;
    for (i__ = 1; i__ <= i__1; ++i__) {
	tmpmat[i__] = *gamma * umtum[i__] - r__[i__];
/* L210: */
    }

/*     Computation of TMPMAT*TMPMC4 = GAMMA*TRANS(UM)*GA-TRANS(SM)*GA */

    if (iold == 1) {
	scdiff_(&mcnew, gamma, &tmpmc2[1], &tmpmc1[1], &tmpmc4[1]);
	calq_(&mcnew, &mcnew, &iold, &tmpmat[1], &tmpmc4[1], &tmpmc4[1], 
		small, iprint);
    } else if (*iflag == 0) {
	i__1 = *mc + 1;
	scdiff_(&i__1, gamma, &tmpmc2[1], &tmpmc1[1], &tmpmc4[1]);
	i__1 = *mc + 1;
	calq_(&mcnew, &i__1, &iold, &tmpmat[1], &tmpmc4[1], &tmpmc4[1], small,
		 iprint);
    } else {
	scdiff_(mc, gamma, &tmpmc2[1], &tmpmc1[1], &tmpmc4[1]);
	calq_(&mcnew, mc, &iold, &tmpmat[1], &tmpmc4[1], &tmpmc4[1], small, 
		iprint);
    }

/*     Computation of the search direction D */

    if (iold == 1 || iold == 2) {
	scalex_(&mcnew, gamma, &tmpmc4[iold], &tmpmc3[iold]);
	cwmaxv_(n, &mcnew, &sm[(iold - 1) * *n + 1], &tmpmc4[iold], &tmpn1[1],
		 &c_b15);
	d__1 = -(*gamma);
	scdiff_(n, &d__1, &ga[1], &tmpn1[1], &d__[1]);
	cwmaxv_(n, &mcnew, &um[(iold - 1) * *n + 1], &tmpmc3[iold], &tmpn1[1],
		 &c_b15);
	xsumy_(n, &d__[1], &tmpn1[1], &d__[1]);
    } else {
	scalex_(mcc, gamma, &tmpmc4[1], &tmpmc3[1]);
	i__1 = *inew - 1;
	cwmaxv_(n, &i__1, &sm[1], &tmpmc4[1], &tmpn1[1], &c_b15);
	d__1 = -(*gamma);
	scdiff_(n, &d__1, &ga[1], &tmpn1[1], &d__[1]);
	i__1 = mcnew - *inew + 1;
	cwmaxv_(n, &i__1, &sm[(iold - 1) * *n + 1], &tmpmc4[iold], &tmpn1[1], 
		&c_b15);
	xdiffy_(n, &d__[1], &tmpn1[1], &d__[1]);
	i__1 = *inew - 1;
	cwmaxv_(n, &i__1, &um[1], &tmpmc3[1], &tmpn1[1], &c_b15);
	xsumy_(n, &d__[1], &tmpn1[1], &d__[1]);
	i__1 = mcnew - *inew + 1;
	cwmaxv_(n, &i__1, &um[(iold - 1) * *n + 1], &tmpmc3[iold], &tmpn1[1], 
		&c_b15);
	xsumy_(n, &d__[1], &tmpn1[1], &d__[1]);
    }
L300:
    return 0;
} /* dlsr1_ */

/* *********************************************************************** */

/*     * SUBROUTINE AGBFGS * */


/*     * Purpose * */

/*     Computation of aggregate values by the limited memory BFGS update. */


/*     * Calling sequence * */

/*     CALL AGBFGS(N,MC,MCC,INEW,IBFGS,IFLAG,G,GP,GA,U,D,SM,UM,R,C,UMTUM, */
/*     &      ALFN,ALFV,GAMMA,TMPMC1,TMPMC2,IC,RHO) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     II  MC              Declared number of stored corrections. */
/*     II  MCC             Current number of stored corrections. */
/*     II  INEW            Index for circular arrays. */
/*     II  IBFGS           Index of the type of BFGS update. */
/*     IU  IFLAG           Index for adaptive version: */
/*                             0  - Maximum number of stored corrections */
/*                                    has not been changed at this */
/*                                    iteration. */
/*                             1  - Maximum number of stored corrections */
/*                                    has been changed at this iteration. */
/*     RI  ALFN            Locality measure. */
/*     RO  ALFV            Aggregate locality measure. */
/*     RI  D(N)            Search direction. */
/*     RI  G(N)            Current (auxiliary) subgradient of the */
/*                           objective function. */
/*     RI  GP(N)           Previous subgradient and also aggregate */
/*                           subradient of the objective function */
/*     RO  GA(N)           Next aggregate subgradient of the objective */
/*                           function. */
/*     RI  U(N)            Difference of trial and aggregate gradients. */
/*     RI  SM(N*(MC+1))    Matrix whose columns are stored corrections. */
/*     RI  UM(N*(MC+1))    Matrix whose columns are stored subgradient */
/*                           differences. */
/*     RI  R((MC+2)*(MC+1)/2)  Upper triangular matrix. */
/*     RI  UMTUM((MC+2)*(MC+1)/2)  Matrix UMTUM = TRANS(UM) * UM. */
/*     RI  C(MC+1)         Diagonal matrix. */
/*     RI  GAMMA           Scaling parameter. */
/*     RA  TMPMC#(MC+1)    Auxiliary arrays; # = 1,2. */
/*     II  IC              Correction indicator. */
/*     RI  RHO             Correction parameter. */


/*     * Local variables * */

/*     I   MCNEW           Current size of vectors. */
/*     I   IOLD            Index of the oldest corrections. */
/*     R   P               P = TRANS(D)*U - ALFN. */
/*     R   Q               Q = TRANS(U)*DM*U, where DM is the inverse */
/*                            approximation of the hessian calculated */
/*                            by using the L-BFGS formula. */
/*     R   LAM             Multiplier used to calculate aggregate */
/*                            values. */
/*     R   W               Correction. */


/*     * Subprograms used * */

/*     RF  VDOT            Dot product of two vectors. */
/*     S   SYMAX           Multiplication of a dense symmetric matrix */
/*                           by a vector. */
/*     S   RWAXV2          Multiplication of two rowwise stored dense */
/*                           rectangular matrices A and B by vectors X */
/*                           and Y. */
/*     S   TRLIEQ          Solving X from linear equation L*X=Y or */
/*                           TRANS(L)*X=Y. */


/*     The variable and subgradient differences and all the MC-vectors are */
/*     stored in a circular order controlled by the parameter point inew. */



/*     Napsu Karmitsa (2002,2003, last modified 2007) */

/* Subroutine */ int agbfgs_(integer *n, integer *mc, integer *mcc, integer *
	inew, integer *ibfgs, integer *iflag, doublereal *g, doublereal *gp, 
	doublereal *ga, doublereal *u, doublereal *d__, doublereal *sm, 
	doublereal *um, doublereal *r__, doublereal *c__, doublereal *umtum, 
	doublereal *alfn, doublereal *alfv, doublereal *gamma, doublereal *
	tmpmc1, doublereal *tmpmc2, integer *ic, doublereal *rho)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *);

    /* Local variables */
    static integer i__;
    static doublereal p, q, w, lam;
    static integer iold, ierr;
    extern doublereal vdot_(integer *, doublereal *, doublereal *);
    static integer mcnew;
    extern /* Subroutine */ int symax_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *), rwaxv2_(integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *), trlieq_(integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    integer *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Functions */
/*     Intrinsic Functions */
/*     External Subroutines */
    /* Parameter adjustments */
    --tmpmc2;
    --tmpmc1;
    --umtum;
    --c__;
    --r__;
    --um;
    --sm;
    --d__;
    --u;
    --ga;
    --gp;
    --g;

    /* Function Body */
    if (*mcc < *mc) {
	if (*ibfgs == 2) {
	    mcnew = *mcc + 1;
	} else {
	    mcnew = *mcc;
	}
	iold = 1;
    } else {
	if (*iflag == 0) {
	    if (*ibfgs == 2) {
		mcnew = *mc + 1;
	    } else {
		mcnew = *mc;
	    }
	    iold = *inew + 1;
	    if (iold > *mc + 1) {
		iold = 1;
	    }
	} else {
	    if (*ibfgs == 2) {
		mcnew = *mc;
	    } else {
		mcnew = *mc - 1;
	    }
	    iold = *inew + 1;
	    if (iold > *mc) {
		iold = 1;
	    }
	}
    }

/*     Computation of TRANS(D) * U - ALFN */

    p = vdot_(n, &d__[1], &u[1]) - *alfn;
    q = vdot_(n, &u[1], &u[1]);
    if (*ic == 1) {
	w = *rho * q;
    } else {
	w = 0.;
    }

/*     Computation of the product TRANS(U)*DM*U */

    if (*mcc > 0 || *ibfgs == 2) {
	if (iold == 1 || *ibfgs == 2) {
	    rwaxv2_(n, &mcnew, &sm[1], &um[1], &u[1], &u[1], &tmpmc1[1], &
		    tmpmc2[1]);
	    trlieq_(&mcnew, &mcnew, &iold, &r__[1], &tmpmc1[1], &tmpmc1[1], &
		    c__1, &ierr);
	    q -= vdot_(&mcnew, &tmpmc2[1], &tmpmc1[1]) * 2;
	    q = *gamma * q;
	    i__1 = mcnew;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc2[i__] = c__[i__] * tmpmc1[i__];
/* L10: */
	    }
	    q += vdot_(&mcnew, &tmpmc1[1], &tmpmc2[1]);
	    symax_(&mcnew, &mcnew, &iold, &umtum[1], &tmpmc1[1], &tmpmc2[1]);
	    q += *gamma * vdot_(&mcnew, &tmpmc1[1], &tmpmc2[1]);
	} else {
	    i__1 = *inew - 1;
	    rwaxv2_(n, &i__1, &sm[1], &um[1], &u[1], &u[1], &tmpmc1[1], &
		    tmpmc2[1]);
	    i__1 = *mcc - *inew;
	    rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 
		    1], &u[1], &u[1], &tmpmc1[iold], &tmpmc2[iold]);
	    trlieq_(&mcnew, mcc, &iold, &r__[1], &tmpmc1[1], &tmpmc1[1], &
		    c__1, &ierr);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    q -= (vdot_(&i__1, &tmpmc2[iold], &tmpmc1[iold]) + vdot_(&i__2, &
		    tmpmc2[1], &tmpmc1[1])) * 2;
	    q = *gamma * q;
	    i__1 = *mcc;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc2[i__] = c__[i__] * tmpmc1[i__];
/* L20: */
	    }
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    q = q + vdot_(&i__1, &tmpmc1[iold], &tmpmc2[iold]) + vdot_(&i__2, 
		    &tmpmc1[1], &tmpmc2[1]);
	    symax_(&mcnew, mcc, &iold, &umtum[1], &tmpmc1[1], &tmpmc2[1]);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    q += *gamma * (vdot_(&i__1, &tmpmc1[iold], &tmpmc2[iold]) + vdot_(
		    &i__2, &tmpmc1[1], &tmpmc2[1]));
	}
    }
    q += w;
    lam = d_sign(&c_b72, &p) + .5;
    if (q > 0.) {
/* Computing MIN */
/* Computing MAX */
	d__3 = 0., d__4 = p / q;
	d__1 = 1., d__2 = max(d__3,d__4);
	lam = min(d__1,d__2);
    }

/*     Computation of the aggregate values */

    p = 1. - lam;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ga[i__] = lam * g[i__] + p * gp[i__];
/* L30: */
    }
    *alfv = lam * *alfn;
    return 0;
} /* agbfgs_ */

/* *********************************************************************** */

/*     * SUBROUTINE AGGSR1 * */


/*     * Purpose * */

/*     Computation of aggregate values by the limited memory SR1 update. */


/*     * Calling sequence * */

/*     CALL AGGSR1(N,MC,MCC,INEW,IFLAG,G,GP,GA,D,ALFN,ALFV,TMPMAT,UMTUM, */
/*    &     R,GAMMA,SMTGP,UMTGP,SMTGA,UMTGA,SM,UM,TMPMC3,TMPMC4,TMPN2, */
/*    &     TMPN3,TMPN4,ICN,RHO,SMALL) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     II  MC              Declared number of stored corrections. */
/*     II  MCC             Current number of stored corrections. */
/*     II  INEW            Index for circular arrays. */
/*     IU  IFLAG           Index for adaptive version: */
/*                             0  - Maximum number of stored corrections */
/*                                    has not been changed at this */
/*                                    iteration. */
/*                             1  - Maximum number of stored corrections */
/*                                    has been changed at this iteration. */
/*     RI  G(N)            Current (auxiliary) subgradient of the */
/*                           objective function. */
/*     RI  GP(N)           Basic subgradient of the objective function. */
/*     RU  GA(N)           Current aggregate subgradient. */
/*     RI  D(N)            Search direction. */
/*     RI  ALFN            Locality measure. */
/*     RO  ALFV            Aggregate locality measure. */
/*     RI  SM(N*(MC+1))    Matrix whose columns are stored corrections. */
/*     RI  UM(N*(MC+1))    Matrix whose columns are stored subgradient. */
/*     RI  UMTUM((MC+2)*(MC+1)/2)  Matrix UMTUM = TRANS(UM) * UM. */
/*     RI  R((MC+2)*(MC+1)/2)   Upper triangular matrix. */
/*     RI  SMTGP(MC+1)     Vector SMTGP = TRANS(SM)*GP. */
/*     RI  SMTGA(MC+1)     Vector SMTGA = TRANS(SM)*GA. */
/*     RI  UMTGP(MC+1)     Vector UMTGP = TRANS(UM)*GP. */
/*     RI  UMTGA(MC+1)     Vector UMTGA = TRANS(UM)*GA. */
/*     RI  GAMMA           Scaling parameter. */
/*     RA  TMPMC#(MC+1)    Auxiliary arrays; # = 3,4. */
/*     RA  TMPN#(N)        Auxiliary arrays; # = 2,...,4. */
/*     RA  TMPMAT((MC+1)*(MC)/2)  Auxiliary matrix. */
/*     II  ICN             Correction indicator. */
/*     RI  RHO             Correction parameter. */
/*     RI  SMALL           Small positive value. */


/*     * Local variables * */

/*     I   MCNEW           Current size of vectors. */
/*     I   IOLD            Index of the oldest corrections. */
/*     R   LAM#            Multipliers: # = 1,2. */
/*     R   PR              PR = TRANS(GP-GA) DM (GP-GA), where DM */
/*                           presents the L-SR1- approximation of Hessian. */
/*     R   RRP             RRP = TRANS(GP-GA) DM GA - ALFV. */
/*     R   PRQR            PRQR = TRANS(GP-GA) DM (G-GA). */
/*     R   RRQ             RRQ = TRANS(G-GA) DM GA - ALFV + ALFN. */
/*     R   QR              QR = TRANS(G-GA) DM (G-GA). */
/*     R   PQ              PQ = TRANS(G-GP) DM (G-GP). */
/*     R   QQP             QQP = TRANS(G-GP) DM G + ALFN. */
/*     R   TMP1            Auxiliary scalar. */
/*     R   TMP2            Auxiliary scalar. */
/*     R   W               Correction. */


/*     * Subprograms used * */

/*     RF  VDOT            Dot product of two vectors. */
/*     S   XSUMY           Sum of two vectors. */
/*     S   RWAXV2          Multiplication of two rowwise stored dense */
/*                           rectangular matrices A and B by vectors X */
/*                           and Y. */
/*     S   CWMAXV          Multiplication of a vector by a dense */
/*                           rectangular matrix. */
/*     S   SCALEX          Scaling a vector. */
/*     S   XDIFFY          Difference of two vectors. */
/*     S   SCSUM           Sum of a vector and the scaled vector. */
/*     S   SCDIFF          Difference of the scaled vector and a vector. */
/*     S   CALQ            Solving X from linear equation A*X=Y. */
/*     S   LINEQ           Solver for linear equation. */


/*     The variable and subgradient differences and all the MC-vectors are */
/*     stored in a circular order controlled by the parameter point inew. */



/*     Napsu Karmitsa (2002 - 2004, last modified 2007) */

/* Subroutine */ int aggsr1_(integer *n, integer *mc, integer *mcc, integer *
	inew, integer *iflag, doublereal *g, doublereal *gp, doublereal *ga, 
	doublereal *d__, doublereal *alfn, doublereal *alfv, doublereal *
	tmpmat, doublereal *umtum, doublereal *r__, doublereal *gamma, 
	doublereal *smtgp, doublereal *umtgp, doublereal *smtga, doublereal *
	umtga, doublereal *sm, doublereal *um, doublereal *tmpmc3, doublereal 
	*tmpmc4, doublereal *tmpn2, doublereal *tmpn3, doublereal *tmpn4, 
	integer *icn, doublereal *rho, doublereal *small)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2, d__3, d__4;

    /* Local variables */
    static integer i__;
    static doublereal w, pq, pr, qr, qqp, rrp, rrq, lam1, lam2, tmp1, tmp2;
    extern /* Subroutine */ int calq_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *)
	    ;
    static integer iold, ierr;
    extern doublereal vdot_(integer *, doublereal *, doublereal *);
    static doublereal prqr;
    static integer mcnew;
    extern /* Subroutine */ int lineq_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), scsum_(
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *)
	    , xsumy_(integer *, doublereal *, doublereal *, doublereal *), 
	    rwaxv2_(integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), scdiff_(
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *)
	    , scalex_(integer *, doublereal *, doublereal *, doublereal *), 
	    xdiffy_(integer *, doublereal *, doublereal *, doublereal *), 
	    cwmaxv_(integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Functions */
/*     External Subroutines */
/*     Intrinsic Functions */
    /* Parameter adjustments */
    --tmpn4;
    --tmpn3;
    --tmpn2;
    --tmpmc4;
    --tmpmc3;
    --um;
    --sm;
    --umtga;
    --smtga;
    --umtgp;
    --smtgp;
    --r__;
    --umtum;
    --tmpmat;
    --d__;
    --ga;
    --gp;
    --g;

    /* Function Body */
    ierr = 0;
    if (*mcc < *mc) {
	iold = 1;
	mcnew = *mcc;
    } else if (*iflag == 0) {
	mcnew = *mc;
	iold = *inew + 1;
	if (iold > *mc + 1) {
	    iold = 1;
	}
    } else {
	mcnew = *mc - 1;
	iold = *inew + 1;
	if (iold > *mc) {
	    iold = 1;
	}
    }
    xdiffy_(n, &gp[1], &ga[1], &tmpn2[1]);

/*     Calculation of TMPN3 = TRANS(GP - GA)DM */

    if (*mcc > 0) {
	i__1 = mcnew * (mcnew + 1) / 2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    tmpmat[i__] = *gamma * umtum[i__] - r__[i__];
/* L10: */
	}
	if (iold == 1) {
	    xdiffy_(&mcnew, &umtgp[1], &umtga[1], &tmpmc4[1]);
	    scdiff_(&mcnew, gamma, &tmpmc4[1], &smtgp[1], &tmpmc4[1]);
	    xsumy_(&mcnew, &tmpmc4[1], &smtga[1], &tmpmc4[1]);
	    calq_(&mcnew, &mcnew, &iold, &tmpmat[1], &tmpmc3[1], &tmpmc4[1], 
		    small, &c__0);
	    scalex_(&mcnew, gamma, &tmpmc3[1], &tmpmc4[1]);
	    cwmaxv_(n, &mcnew, &sm[1], &tmpmc3[1], &tmpn4[1], &c_b15);
	    scsum_(n, gamma, &tmpn2[1], &tmpn4[1], &tmpn3[1]);
	    cwmaxv_(n, &mcnew, &um[1], &tmpmc4[1], &tmpn4[1], &c_b15);
	    xdiffy_(n, &tmpn3[1], &tmpn4[1], &tmpn3[1]);
	} else {
	    xdiffy_(mcc, &umtgp[1], &umtga[1], &tmpmc4[1]);
	    scdiff_(mcc, gamma, &tmpmc4[1], &smtgp[1], &tmpmc4[1]);
	    xsumy_(mcc, &tmpmc4[1], &smtga[1], &tmpmc4[1]);
	    calq_(&mcnew, mcc, &iold, &tmpmat[1], &tmpmc3[1], &tmpmc4[1], 
		    small, &c__0);
	    scalex_(mcc, gamma, &tmpmc3[1], &tmpmc4[1]);
	    i__1 = *inew - 1;
	    cwmaxv_(n, &i__1, &sm[1], &tmpmc3[1], &tmpn4[1], &c_b15);
	    scsum_(n, gamma, &tmpn2[1], &tmpn4[1], &tmpn3[1]);
	    i__1 = mcnew - *inew + 1;
	    cwmaxv_(n, &i__1, &sm[(iold - 1) * *n + 1], &tmpmc3[iold], &tmpn4[
		    1], &c_b15);
	    xsumy_(n, &tmpn3[1], &tmpn4[1], &tmpn3[1]);
	    i__1 = *inew - 1;
	    cwmaxv_(n, &i__1, &um[1], &tmpmc4[1], &tmpn4[1], &c_b15);
	    xdiffy_(n, &tmpn3[1], &tmpn4[1], &tmpn3[1]);
	    i__1 = mcnew - *inew + 1;
	    cwmaxv_(n, &i__1, &um[(iold - 1) * *n + 1], &tmpmc4[iold], &tmpn4[
		    1], &c_b15);
	    xdiffy_(n, &tmpn3[1], &tmpn4[1], &tmpn3[1]);
	}
	if (*icn == 1) {
	    scsum_(n, rho, &tmpn2[1], &tmpn3[1], &tmpn3[1]);
	}
	pr = vdot_(n, &tmpn3[1], &tmpn2[1]);
	rrp = vdot_(n, &tmpn3[1], &ga[1]);
	xdiffy_(n, &g[1], &ga[1], &tmpn4[1]);
	prqr = vdot_(n, &tmpn3[1], &tmpn4[1]);
	rrq = -vdot_(n, &tmpn4[1], &d__[1]);
    } else {
	pr = vdot_(n, &tmpn2[1], &tmpn2[1]);
	rrp = vdot_(n, &tmpn2[1], &ga[1]);
	xdiffy_(n, &g[1], &ga[1], &tmpn4[1]);
	prqr = vdot_(n, &tmpn2[1], &tmpn4[1]);
	rrq = -vdot_(n, &tmpn4[1], &d__[1]);
    }

/*     Calculation of QR = TRANS(G - GA) DM (G - GA) */

    qr = vdot_(n, &tmpn4[1], &tmpn4[1]);
    if (*icn == 1) {
	w = *rho * qr;
    } else {
	w = 0.;
    }
    if (*mcc > 0) {
	qr = *gamma * qr;
	if (iold == 1) {
	    rwaxv2_(n, &mcnew, &sm[1], &um[1], &tmpn4[1], &tmpn4[1], &tmpmc4[
		    1], &tmpmc3[1]);
	    d__1 = -(*gamma);
	    scsum_(&mcnew, &d__1, &tmpmc3[1], &tmpmc4[1], &tmpmc4[1]);
	    lineq_(&mcnew, &mcnew, &iold, &tmpmat[1], &tmpmc3[1], &tmpmc4[1], 
		    &ierr);
	    qr = qr - vdot_(&mcnew, &tmpmc4[1], &tmpmc3[1]) + w;
	} else {
	    i__1 = *inew - 1;
	    rwaxv2_(n, &i__1, &sm[1], &um[1], &tmpn4[1], &tmpn4[1], &tmpmc4[1]
		    , &tmpmc3[1]);
	    i__1 = mcnew - *inew + 1;
	    rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 
		    1], &tmpn4[1], &tmpn4[1], &tmpmc4[iold], &tmpmc3[iold]);
	    d__1 = -(*gamma);
	    scsum_(mcc, &d__1, &tmpmc3[1], &tmpmc4[1], &tmpmc4[1]);
	    lineq_(&mcnew, mcc, &iold, &tmpmat[1], &tmpmc3[1], &tmpmc4[1], &
		    ierr);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    qr = qr - vdot_(&i__1, &tmpmc4[iold], &tmpmc3[iold]) - vdot_(&
		    i__2, &tmpmc4[1], &tmpmc3[1]) + w;
	}
    }
    pq = qr - prqr * 2 + pr;
    qqp = pq + prqr + rrq - pr - rrp + *alfn;
    rrp -= *alfv;
    rrq = rrq + *alfn - *alfv;

/*     Computation of multipliers LAM1 and LAM2 */

    if (pr <= 0. || qr <= 0.) {
	goto L100;
    }
    tmp1 = rrq / qr;
    tmp2 = prqr / qr;
    w = pr - prqr * tmp2;
    if (w == 0.) {
	goto L100;
    }
    lam1 = (tmp1 * prqr - rrp) / w;
    lam2 = -tmp1 - lam1 * tmp2;
    if (lam1 * (lam1 - 1.) < 0. && lam2 * (lam1 + lam2 - 1.) < 0.) {
	goto L200;
    }

/*     Minimum on the boundary */

L100:
    lam1 = 0.;
    lam2 = 0.;
    if (*alfn <= *alfv) {
	lam2 = 1.;
    }
    if (qr > 0.) {
/* Computing MIN */
/* Computing MAX */
	d__3 = 0., d__4 = -rrq / qr;
	d__1 = 1., d__2 = max(d__3,d__4);
	lam2 = min(d__1,d__2);
    }
    w = (lam2 * qr + rrq * 2.) * lam2;
    tmp1 = 0.;
    if (*alfv >= 0.) {
	tmp1 = 1.;
    }
    if (pr > 0.) {
/* Computing MIN */
/* Computing MAX */
	d__3 = 0., d__4 = -rrp / pr;
	d__1 = 1., d__2 = max(d__3,d__4);
	tmp1 = min(d__1,d__2);
    }
    tmp2 = (tmp1 * pr + rrp * 2.) * tmp1;
    if (tmp2 < w) {
	w = tmp2;
	lam1 = tmp1;
	lam2 = 0.;
    }
    if (qqp * (qqp - pq) >= 0.) {
	goto L200;
    }
    if (qr + rrq * 2. - qqp * qqp / pq >= w) {
	goto L200;
    }
    lam1 = qqp / pq;
    lam2 = 1. - lam1;
L200:
    if (lam1 == 0. && lam2 * (lam2 - 1.) < 0. && -rrp - lam2 * prqr > 0. && 
	    pr > 0.) {
/* Computing MIN */
	d__1 = 1. - lam2, d__2 = (-rrp - lam2 * prqr) / pr;
	lam1 = min(d__1,d__2);
    }

/*     Computation of the aggregate values */

    tmp1 = 1. - lam1 - lam2;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ga[i__] = lam1 * gp[i__] + lam2 * g[i__] + tmp1 * ga[i__];
/* L30: */
    }
    *alfv = lam2 * *alfn + tmp1 * *alfv;
    return 0;
} /* aggsr1_ */

/* *********************************************************************** */

/*     * SUBROUTINE AGSKIP * */


/*     * Purpose * */

/*     Computation of aggregate values after consecutive null steps */
/*     by the limited memory BFGS update. */


/*     * Calling sequence * */

/*     CALL AGSKIP(N,MC,MCC,INEW,IFLAG,G,GP,GA,D,U,ALFN,ALFV,UMTUM,R,C, */
/*    &     GAMMA,SMTGP,UMTGP,SMTGA,UMTGA,SM,UM,TMPMC3,TMPMC4,TMPN2,ICN, */
/*    &     RHO,SMALL) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     II  MC              Declared number of stored corrections. */
/*     II  MCC             Current number of stored corrections. */
/*     II  INEW            Index for circular arrays. */
/*     IU  IFLAG           Index for adaptive version: */
/*                             0  - Maximum number of stored corrections */
/*                                    has not been changed at this */
/*                                    iteration. */
/*                             1  - Maximum number of stored corrections */
/*                                    has been changed at this iteration. */
/*     RI  G(N)            Current (auxiliary) subgradient of the */
/*                           objective function. */
/*     RI  GP(N)           Basic subgradient of the objective function. */
/*     RU  GA(N)           Current aggregate subgradient. */
/*     RI  D(N)            Search direction. */
/*     RI  U(N)            Difference of trial and aggregate gradients. */
/*     RI  ALFN            Locality measure. */
/*     RO  ALFV            Aggregate locality measure. */
/*     RI  SM(N*(MC+1))    Matrix whose columns are stored corrections. */
/*     RI  UM(N*(MC+1))    Matrix whose columns are stored subgradient. */
/*     RI  UMTUM((MC+2)*(MC+1)/2)  Matrix UMTUM = TRANS(UM) * UM. */
/*     RI  R((MC+2)*(MC+1)/2)   Upper triangular matrix. */
/*     RI  C(MC+1)         Diagonal matrix. */
/*     RI  SMTGP(MC+1)     Vector SMTGP = TRANS(SM)*GP. */
/*     RI  SMTGA(MC+1)     Vector SMTGA = TRANS(SM)*GA. */
/*     RI  UMTGP(MC+1)     Vector UMTGP = TRANS(UM)*GP. */
/*     RI  UMTGA(MC+1)     Vector UMTGA = TRANS(UM)*GA. */
/*     RI  GAMMA           Scaling parameter. */
/*     RA  TMPMC#(MC+1)    Auxiliary arrays; # = 3,4. */
/*     RA  TMPN2(N)        Auxiliary array. */
/*     II  ICN             Correction indicator. */
/*     RI  RHO             Correction parameter. */
/*     RI  SMALL           Small positive value. */


/*     * Local variables * */

/*     I   MCNEW           Current size of vectors. */
/*     I   IOLD            Index of the oldest corrections. */
/*     R   LAM#            Multipliers: # = 1,2. */
/*     R   PR              PR = TRANS(GP-GA) DM (GP-GA), where DM */
/*                           presents the L-BFGS- approximation of Hessian. */
/*     R   RRP             RRP = TRANS(GP-GA) DM GA - ALFV. */
/*     R   PRQR            PRQR = TRANS(GP-GA) DM (G-GA). */
/*     R   RRQ             RRQ = TRANS(G-GA) DM GA - ALFV + ALFN. */
/*     R   QR              QR = TRANS(G-GA) DM (G-GA). */
/*     R   PQ              PQ = TRANS(G-GP) DM (G-GP). */
/*     R   QQP             QQP = TRANS(G-GP) DM G + ALFN. */
/*     R   TMP1            Auxiliary scalar. */
/*     R   TMP2            Auxiliary scalar. */
/*     R   W               Correction. */


/*     * Subprograms used * */

/*     RF  VDOT            Dot product of two vectors. */
/*     S   RWAXV2          Multiplication of two rowwise stored dense */
/*                           rectangular matrices A and B by vectors X */
/*                           and Y. */
/*     S   XDIFFY          Difference of two vectors. */
/*     S   SYMAX           Multiplication of a dense symmetric matrix */
/*                           by a vector. */
/*     S   TRLIEQ          Solving X from linear equation L*X=Y or */
/*                           TRANS(L)*X=Y. */


/*     The variable and subgradient differences and all the MC-vectors are */
/*     stored in a circular order controlled by the parameter point inew. */



/*     Napsu Karmitsa (2002,2003, last modified 2007) */

/* Subroutine */ int agskip_(integer *n, integer *mc, integer *mcc, integer *
	inew, integer *iflag, doublereal *g, doublereal *gp, doublereal *ga, 
	doublereal *d__, doublereal *u, doublereal *alfn, doublereal *alfv, 
	doublereal *umtum, doublereal *r__, doublereal *c__, doublereal *
	gamma, doublereal *smtgp, doublereal *umtgp, doublereal *smtga, 
	doublereal *umtga, doublereal *sm, doublereal *um, doublereal *tmpmc3,
	 doublereal *tmpmc4, doublereal *tmpn2, integer *icn, doublereal *rho,
	 doublereal *small)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2, d__3, d__4;

    /* Local variables */
    static integer i__;
    static doublereal w, pq, pr, qr, qqp, rrp, rrq, lam1, lam2, tmp1, tmp2;
    static integer iold, ierr;
    extern doublereal vdot_(integer *, doublereal *, doublereal *);
    static doublereal prqr;
    static integer mcnew;
    extern /* Subroutine */ int symax_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *), rwaxv2_(integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *), xdiffy_(integer *, doublereal *, 
	    doublereal *, doublereal *), trlieq_(integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    integer *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Functions */
/*     External Subroutines */
/*     Intrinsic Functions */
    /* Parameter adjustments */
    --tmpn2;
    --tmpmc4;
    --tmpmc3;
    --um;
    --sm;
    --umtga;
    --smtga;
    --umtgp;
    --smtgp;
    --c__;
    --r__;
    --umtum;
    --u;
    --d__;
    --ga;
    --gp;
    --g;

    /* Function Body */
    if (*mcc < *mc) {
	iold = 1;
	mcnew = *mcc;
    } else {
	if (*iflag == 0) {
	    mcnew = *mc;
	    iold = *inew + 1;
	    if (iold > *mc + 1) {
		iold = 1;
	    }
	} else {
	    mcnew = *mc - 1;
	    iold = *inew + 1;
	    if (iold > *mc) {
		iold = 1;
	    }
	}
    }

/*     Calculation of PQ = TRANS(G-GP) DM (G-GP) = TRANS(U) DM U. */

    pq = vdot_(n, &u[1], &u[1]);
    if (*icn == 1) {
	w = *rho * pq;
    } else {
	w = 0.;
    }
    if (*mcc > 0) {
	if (iold == 1) {
	    rwaxv2_(n, &mcnew, &sm[1], &um[1], &u[1], &u[1], &tmpmc3[1], &
		    tmpmc4[1]);
	    trlieq_(&mcnew, &mcnew, &iold, &r__[1], &tmpmc3[1], &tmpmc3[1], &
		    c__1, &ierr);
	    pq -= vdot_(&mcnew, &tmpmc4[1], &tmpmc3[1]) * 2;
	    pq = *gamma * pq;
	    i__1 = mcnew;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc4[i__] = c__[i__] * tmpmc3[i__];
/* L10: */
	    }
	    pq += vdot_(&mcnew, &tmpmc3[1], &tmpmc4[1]);
	    symax_(&mcnew, &mcnew, &iold, &umtum[1], &tmpmc3[1], &tmpmc4[1]);
	    pq += *gamma * vdot_(&mcnew, &tmpmc3[1], &tmpmc4[1]);
	} else {
	    i__1 = *inew - 1;
	    rwaxv2_(n, &i__1, &sm[1], &um[1], &u[1], &u[1], &tmpmc3[1], &
		    tmpmc4[1]);
	    i__1 = *mcc - *inew;
	    rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 
		    1], &u[1], &u[1], &tmpmc3[iold], &tmpmc4[iold]);
	    trlieq_(&mcnew, mcc, &iold, &r__[1], &tmpmc3[1], &tmpmc3[1], &
		    c__1, &ierr);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    pq -= (vdot_(&i__1, &tmpmc4[iold], &tmpmc3[iold]) + vdot_(&i__2, &
		    tmpmc4[1], &tmpmc3[1])) * 2;
	    pq = *gamma * pq;
	    i__1 = *mcc;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc4[i__] = c__[i__] * tmpmc3[i__];
/* L20: */
	    }
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    pq = pq + vdot_(&i__1, &tmpmc3[iold], &tmpmc4[iold]) + vdot_(&
		    i__2, &tmpmc3[1], &tmpmc4[1]);
	    symax_(&mcnew, mcc, &iold, &umtum[1], &tmpmc3[1], &tmpmc4[1]);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    pq += *gamma * (vdot_(&i__1, &tmpmc3[iold], &tmpmc4[iold]) + 
		    vdot_(&i__2, &tmpmc3[1], &tmpmc4[1]));
	}
    }
    pq += w;

/*     Calculation of PR = TRANS(GP-GA) DM (GP-GA). */

    xdiffy_(n, &gp[1], &ga[1], &tmpn2[1]);
    pr = vdot_(n, &tmpn2[1], &tmpn2[1]);
    if (*icn == 1) {
	w = *rho * pr;
    } else {
	w = 0.;
    }
    if (*mcc > 0) {
	if (iold == 1) {
	    i__1 = mcnew;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc3[i__] = smtgp[i__] - smtga[i__];
		tmpmc4[i__] = umtgp[i__] - umtga[i__];
/* L301: */
	    }
	    trlieq_(&mcnew, &mcnew, &iold, &r__[1], &tmpmc3[1], &tmpmc3[1], &
		    c__1, &ierr);
	    pr -= vdot_(&mcnew, &tmpmc4[1], &tmpmc3[1]) * 2;
	    pr = *gamma * pr;
	    i__1 = mcnew;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc4[i__] = c__[i__] * tmpmc3[i__];
/* L30: */
	    }
	    pr += vdot_(&mcnew, &tmpmc3[1], &tmpmc4[1]);
	    symax_(&mcnew, &mcnew, &iold, &umtum[1], &tmpmc3[1], &tmpmc4[1]);
	    pr += *gamma * vdot_(&mcnew, &tmpmc3[1], &tmpmc4[1]);
	} else {
	    i__1 = *mcc;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc3[i__] = smtgp[i__] - smtga[i__];
		tmpmc4[i__] = umtgp[i__] - umtga[i__];
/* L401: */
	    }
	    trlieq_(&mcnew, mcc, &iold, &r__[1], &tmpmc3[1], &tmpmc3[1], &
		    c__1, &ierr);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    pr -= (vdot_(&i__1, &tmpmc4[iold], &tmpmc3[iold]) + vdot_(&i__2, &
		    tmpmc4[1], &tmpmc3[1])) * 2;
	    pr = *gamma * pr;
	    i__1 = *mcc;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc4[i__] = c__[i__] * tmpmc3[i__];
/* L40: */
	    }
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    pr = pr + vdot_(&i__1, &tmpmc3[iold], &tmpmc4[iold]) + vdot_(&
		    i__2, &tmpmc3[1], &tmpmc4[1]);
	    symax_(&mcnew, mcc, &iold, &umtum[1], &tmpmc3[1], &tmpmc4[1]);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    pr += *gamma * (vdot_(&i__1, &tmpmc3[iold], &tmpmc4[iold]) + 
		    vdot_(&i__2, &tmpmc3[1], &tmpmc4[1]));
	}
    }
    pr += w;

/*     Calculation of RRP = TRANS(GP-GA) DM GA - ALFV. */

    rrp = -vdot_(n, &tmpn2[1], &d__[1]) - *alfv;

/*     Calculation of QR = TRANS(G-GA) DM (G-GA). */

    xdiffy_(n, &g[1], &ga[1], &tmpn2[1]);
    qr = vdot_(n, &tmpn2[1], &tmpn2[1]);
    if (*icn == 1) {
	w = *rho * qr;
    } else {
	w = 0.;
    }
    if (*mcc > 0) {
	if (iold == 1) {
	    rwaxv2_(n, &mcnew, &sm[1], &um[1], &tmpn2[1], &tmpn2[1], &tmpmc3[
		    1], &tmpmc4[1]);
	    trlieq_(&mcnew, &mcnew, &iold, &r__[1], &tmpmc3[1], &tmpmc3[1], &
		    c__1, &ierr);
	    qr -= vdot_(&mcnew, &tmpmc4[1], &tmpmc3[1]) * 2;
	    qr = *gamma * qr;
	    i__1 = mcnew;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc4[i__] = c__[i__] * tmpmc3[i__];
/* L50: */
	    }
	    qr += vdot_(&mcnew, &tmpmc3[1], &tmpmc4[1]);
	    symax_(&mcnew, &mcnew, &iold, &umtum[1], &tmpmc3[1], &tmpmc4[1]);
	    qr += *gamma * vdot_(&mcnew, &tmpmc3[1], &tmpmc4[1]);
	} else {
	    i__1 = *inew - 1;
	    rwaxv2_(n, &i__1, &sm[1], &um[1], &tmpn2[1], &tmpn2[1], &tmpmc3[1]
		    , &tmpmc4[1]);
	    i__1 = *mcc - *inew;
	    rwaxv2_(n, &i__1, &sm[(iold - 1) * *n + 1], &um[(iold - 1) * *n + 
		    1], &tmpn2[1], &tmpn2[1], &tmpmc3[iold], &tmpmc4[iold]);
	    trlieq_(&mcnew, mcc, &iold, &r__[1], &tmpmc3[1], &tmpmc3[1], &
		    c__1, &ierr);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    qr -= (vdot_(&i__1, &tmpmc4[iold], &tmpmc3[iold]) + vdot_(&i__2, &
		    tmpmc4[1], &tmpmc3[1])) * 2;
	    qr = *gamma * qr;
	    i__1 = *mcc;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		tmpmc4[i__] = c__[i__] * tmpmc3[i__];
/* L60: */
	    }
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    qr = qr + vdot_(&i__1, &tmpmc3[iold], &tmpmc4[iold]) + vdot_(&
		    i__2, &tmpmc3[1], &tmpmc4[1]);
	    symax_(&mcnew, mcc, &iold, &umtum[1], &tmpmc3[1], &tmpmc4[1]);
	    i__1 = *mcc - *inew;
	    i__2 = *inew - 1;
	    qr += *gamma * (vdot_(&i__1, &tmpmc3[iold], &tmpmc4[iold]) + 
		    vdot_(&i__2, &tmpmc3[1], &tmpmc4[1]));
	}
    }
    qr += w;

/*     Calculation of RRQ = TRANS(G-GA) DM GA - ALFV + ALFN. */

    rrq = -vdot_(n, &tmpn2[1], &d__[1]) - *alfv + *alfn;

/*     Calculation of PRQR = TRANS(GP-GA) DM (G-GA). */

    prqr = (qr - pq + pr) / 2.;

/*     Calculation of QQP = TRANS(G-GP) DM G + ALFN. */

    qqp = pq + prqr + rrq - pr - rrp;

/*     Computation of multipliers LAM1 and LAM2 */

    if (pr <= 0. || qr <= 0.) {
	goto L100;
    }
    tmp1 = rrq / qr;
    tmp2 = prqr / qr;
    w = pr - prqr * tmp2;
    if (w == 0.) {
	goto L100;
    }
    lam1 = (tmp1 * prqr - rrp) / w;
    lam2 = -tmp1 - lam1 * tmp2;
    if (lam1 * (lam1 - 1.) < 0. && lam2 * (lam1 + lam2 - 1.) < 0.) {
	goto L200;
    }

/*     Minimum on the boundary */

L100:
    lam1 = 0.;
    lam2 = 0.;
    if (*alfn <= *alfv) {
	lam2 = 1.;
    }
    if (qr > 0.) {
/* Computing MIN */
/* Computing MAX */
	d__3 = 0., d__4 = -rrq / qr;
	d__1 = 1., d__2 = max(d__3,d__4);
	lam2 = min(d__1,d__2);
    }
    w = (lam2 * qr + rrq * 2.) * lam2;
    tmp1 = 0.;
    if (*alfv >= 0.) {
	tmp1 = 1.;
    }
    if (pr > 0.) {
/* Computing MIN */
/* Computing MAX */
	d__3 = 0., d__4 = -rrp / pr;
	d__1 = 1., d__2 = max(d__3,d__4);
	tmp1 = min(d__1,d__2);
    }
    tmp2 = (tmp1 * pr + rrp * 2.) * tmp1;
    if (tmp2 < w) {
	w = tmp2;
	lam1 = tmp1;
	lam2 = 0.;
    }
    if (qqp * (qqp - pq) >= 0.) {
	goto L200;
    }
    if (qr + rrq * 2. - qqp * qqp / pq >= w) {
	goto L200;
    }
    lam1 = qqp / pq;
    lam2 = 1. - lam1;
L200:
    if (lam1 == 0. && lam2 * (lam2 - 1.) < 0. && -rrp - lam2 * prqr > 0. && 
	    pr > 0.) {
/* Computing MIN */
	d__1 = 1. - lam2, d__2 = (-rrp - lam2 * prqr) / pr;
	lam1 = min(d__1,d__2);
    }

/*     Computation of the aggregate values */

    tmp1 = 1. - lam1 - lam2;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ga[i__] = lam1 * gp[i__] + lam2 * g[i__] + tmp1 * ga[i__];
/* L210: */
    }
    *alfv = lam2 * *alfn + tmp1 * *alfv;
    return 0;
} /* agskip_ */

/* *********************************************************************** */

/*     * SUBROUTINE LLS * */


/*     * Purpose * */

/*     Special line search for limited memory bundle method */


/*     * Calling sequence * */

/*     CALL LLS(N,X,G,D,XO,T,FO,F,P,ALFN,TMIN,DNORM,WK,THETA,EPSL, */
/*    &     EPSR,ETA,MOS,ITERS,NFE,NNK,ITERM) */


/*     * PARAMETERS * */

/*     II  N             Number of variables. */
/*     RU  X(N)          Vector of variables. */
/*     RI  XO(N)         Previous vector of variables. */
/*     RO  G(N)          Subgradient of the objective function. */
/*     RI  D(N)          Direction vector. */
/*     RU  T             Stepsize. */
/*     RO  F             Value of the objective function. */
/*     RI  FO            Previous value of the objective function. */
/*     RU  P             Directional derivative. */
/*     RI  TMIN          Minimum value of the stepsize. */
/*     RI  DNORM         Euclidean norm of the direction vector. */
/*     RI  WK            Stopping parameter. */
/*     RI  EPSL          Termination tolerance for line search (in test */
/*                         on the change of the function value). */
/*     RI  EPSR          Termination tolerance for line search (in test */
/*                         on the directional derivative). */
/*     RI  ETA           Distance measure parameter. */
/*     RO  ALFN          Locality measure. */
/*     RI  THETA         Scaling parameter. */
/*     II  MOS           Locality measure parameter. */
/*     IO  ITERS         Null step indicator. */
/*                            0  - Null step. */
/*                            1  - Serious step. */
/*     II  NNK           Number of consequtive null steps. */
/*     IU  NFE           Number of function evaluations. */
/*     IO  ITERM         Cause of termination: */
/*                          0  - Everything is ok. */
/*                         -3  - Failure in function or subgradient */
/*                               calculations. */


/*     * Local parameters * */

/*     I   MAXINT        Maximum number of interpolations. */


/*     * Local variables * */

/*     I   NIN           Number of interpolations. */
/*     R   TL,TU         Lower and upper limits for T used in */
/*                         interpolation. */
/*     R   FL            Value of the objective function for T=TL. */
/*     R   FU            Value of the objective function for T=TU. */
/*     R   EPSA          Line search parameter. */
/*     R   EPST          Line search parameter. */
/*     R   EPSLK         Line search parameter. */
/*     R   EPSRK         Line search parameter. */


/*     * Subprograms used * */

/*     RF  VDOT          Dot product of two vectors. */
/*     S   QINT          Quadratic interpolation for line search */
/*                         with one directional derivative. */
/*     S   SCSUM         Sum of a vector and the scaled vector. */


/*     * EXTERNAL SUBROUTINES * */

/*     SE  FUNDER        Computation of the value and the subgradient of */
/*                       the objective function. Calling sequence: */
/*                       CALL FUNDER(N,X,F,G,ITERM), where N is a number of */
/*                       variables, X(N) is a vector of variables, F is */
/*                       the value of the objective function and G(N) is */
/*                       the subgradient of the objective function and */
/*                       ITERM is the error indicator. */



/*     * Original method * */

/*     Special line search for nonsmooth nonconvex variable metric */
/*     method (PS1L08) by J.Vlcek (1999) */


/*     * Limited memory version * */

/*     Napsu Karmitsa (2002 - 2003, last modified 2007). */

/* Subroutine */ int lls_(integer *n, doublereal *x, doublereal *g, 
	doublereal *d__, doublereal *xo, doublereal *t, doublereal *fo, 
	doublereal *f, doublereal *p, doublereal *alfn, doublereal *tmin, 
	doublereal *dnorm, doublereal *wk, doublereal *theta, doublereal *
	epsl, doublereal *epsr, doublereal *eta, integer *mos, integer *iters,
	 integer *nfe, integer *nnk, integer *iterm)
{
    /* System generated locals */
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double pow_di(doublereal *, integer *);

    /* Local variables */
    static doublereal fl, fu, tl, tu;
    static integer nin;
    static doublereal epsa, epst;
    extern doublereal vdot_(integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int qint_(doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *);
    static doublereal epslk, epsrk;
    extern /* Subroutine */ int scsum_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), funder_(integer *, doublereal *, 
	    doublereal *, doublereal *, integer *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     Parameters */
/*     External Functions */
/*     Intrinsic Functions */
/*     External Subroutines */

/*     Initialization */

    /* Parameter adjustments */
    --xo;
    --d__;
    --g;
    --x;

    /* Function Body */
    nin = 0;
    epst = *epsl * 2.;
    epsa = (*epsr - epst) / 2.;
    tl = 0.;
    tu = *t;
    fl = *fo;
    if (*theta < 1.) {
	epst = *theta * epst;
	epsa = *theta * epsa;
	epslk = *epsl;
	*epsl = *theta * *epsl;
	epsrk = *epsr;
	*epsr = *theta * *epsr;
    }

/*     Function and gradient evaluation at a new point */

L10:
    d__1 = *theta * *t;
    scsum_(n, &d__1, &d__[1], &xo[1], &x[1]);
    funder_(n, &x[1], f, &g[1], iterm);
    ++(*nfe);
    if (*iterm != 0) {
	return 0;
    }
    *p = *theta * vdot_(n, &g[1], &d__[1]);
/* Computing MAX */
    d__4 = *t * *theta * *dnorm;
    d__2 = (d__1 = *fo - *f + *p * *t, abs(d__1)), d__3 = *eta * pow_di(&d__4,
	     mos);
    *alfn = max(d__2,d__3);

/*     Null/descent step test (ITERS=0/1) */

    *iters = 1;
    if (*f <= *fo - *t * epst * *wk) {
	tl = *t;
	fl = *f;
    } else {
	tu = *t;
	fu = *f;
    }

/*     Serious step */

    if (*f <= *fo - *t * *epsl * *wk && (*t >= *tmin || *alfn > epsa * *wk)) {
	goto L40;
    }

/*     Additional interpolation */

    if (*f > *fo && tu - tl >= *tmin * .1 && *nnk >= 1 && nin < 200) {
	goto L20;
    }

/*     Null step */

    if (*p - *alfn >= -(*epsr) * *wk || tu - tl < *tmin * .1 || nin >= 200) {
	goto L30;
    }

/*     Interpolation */

L20:
    ++nin;
    if (tl == 0. && *wk > 0.) {
	d__1 = 1. - .5 / (1. - epst);
	qint_(&tu, &fl, &fu, wk, t, &d__1);
    } else {
	*t = (tu + tl) * .5;
    }
    goto L10;
L30:
    *iters = 0;
L40:
    if (*theta != 1.) {
	*epsl = epslk;
	*epsr = epsrk;
    }
    return 0;
} /* lls_ */

/* *********************************************************************** */

/*     * SUBROUTINE QINT * */


/*     * Purpose * */

/*     Quadratic interpolation for line search with one directional */
/*     derivative. */


/*     * Calling sequence * */

/*     CALL QINT(TU,FL,FU,WK,T,KAPPA) */


/*     * Parameters * */

/*     RI  TU            Upper value of the stepsize parameter. */
/*     RI  FL            Value of the objective function. */
/*     RI  FU            Value of the objective function for T=TU. */
/*     RI  WK            Directional derivative. */
/*     RO  T             Stepsize parameter. */
/*     RI  KAPPA         Interpolation parameter. */



/*     Napsu Haarala (2004). */

/* Subroutine */ int qint_(doublereal *tu, doublereal *fl, doublereal *fu, 
	doublereal *wk, doublereal *t, doublereal *kappa)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Local variables */
    static doublereal tmp1, tmp2;

/*     Scalar Arguments */
/*     Local Scalars */
/*     Intrinsic Functions */
    tmp1 = (*fu - *fl) / (-(*wk) * *tu);

/*     Quadratic interpolation with one directional derivative */

    tmp2 = (1. - tmp1) * 2.;
    if (tmp2 > 1.) {

/*     Interpolation accepted */

/* Computing MAX */
	d__1 = *kappa * *tu, d__2 = *tu / tmp2;
	*t = max(d__1,d__2);
	return 0;
    }

/*     Bisection */

    *t = *tu * .5;
    return 0;
} /* qint_ */

/* *********************************************************************** */

/*     * SUBROUTINE TINIT * */


/*     * Purpose * */

/*     Initial stepsize selection for limited memory bundle method */


/*     * Calling sequence * */

/*     CALL TINIT(N,NA,MAL,X,AF,AG,AY,IBUN,D,F,P,T,TMAX,TMIN,ETA,ETA9, */
/*    &     MOS,ITERS) */


/*     * Parameters * */

/*     II  N             Number of variables. */
/*     II  NA            Maximum size of the bundle. */
/*     II  MAL           Current size of the bundle. */
/*     RU  X(N)          Vector of variables. */
/*     RU  AF(4*NA)      Vector of values of bundle functions. */
/*     RU  AG(N*NA)      Matrix whose columns are bundle subgradients. */
/*     RU  AY(N*NA)      Matrix whose columns are bundle points. */
/*     II  IBUN          Index for the circular arrays in bundle. */
/*     RI  D(N)          Direction vector. */
/*     RI  F             Value of the objective function. */
/*     RI  DF            Directional derivative. */
/*     RO  T             Value of the stepsize parameter. */
/*     RI  TMIN          Lower limit for stepsize parameter. */
/*     RI  TMAX          Upper limit for stepsize parameter. */
/*     RI  ETA           Distance measure parameter. */
/*     RI  ETA9          Maximum for real numbers. */
/*     RI  MOS           Locality measure parameter. */
/*     II  ITERS         Null step indicator. */
/*                            0  - Null step. */
/*                            1  - Serious step. */


/*     * Subprograms used * */

/*     S   DESTEP        Stepsize determination for descent steps. */
/*     S   NULSTP        Stepsize determination for null steps. */



/*     Napsu Haarala (2003) */

/* Subroutine */ int tinit_(integer *n, integer *na, integer *mal, doublereal 
	*x, doublereal *af, doublereal *ag, doublereal *ay, integer *ibun, 
	doublereal *d__, doublereal *f, doublereal *p, doublereal *t, 
	doublereal *tmax, doublereal *tmin, doublereal *eta, doublereal *eta9,
	 integer *mos, integer *iters)
{
    /* System generated locals */
    doublereal d__1;

    /* Local variables */
    extern /* Subroutine */ int destep_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *), nulstp_(integer *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, integer *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     External Subroutines */
/*     Intrinsic Functions */
    /* Parameter adjustments */
    --d__;
    --ay;
    --ag;
    --af;
    --x;

    /* Function Body */
    *t = min(1.,*tmax);
    if (*p == 0.) {
	goto L10;
    }
    if (*iters == 1) {
	destep_(n, na, mal, &x[1], &af[1], &ag[1], &ay[1], ibun, &d__[1], f, 
		p, t, eta, eta9, mos);
    } else {
	nulstp_(n, na, mal, &x[1], &af[1], &ag[1], &ay[1], ibun, &d__[1], f, 
		p, t, eta, eta9, mos);
    }
L10:
/* Computing MIN */
    d__1 = max(*t,*tmin);
    *t = min(d__1,*tmax);
    return 0;
} /* tinit_ */

/* *********************************************************************** */

/*     * SUBROUTINE DESTEP * */


/*     * Purpose * */

/*     Stepsize selection using polyhedral approximation for descent step */
/*     in limited memory bundle method. */


/*     * Calling sequence * */

/*     CALL DESTEP(N,NA,MAL,X,AF,AG,AY,IBUN,D,F,DF,T,ETA,ETA9,MOS) */


/*     * Parameters * */

/*     II  N             Number of variables. */
/*     II  NA            Maximum size of the bundle. */
/*     II  MAL           Current size of the bundle. */
/*     RU  X(N)          Vector of variables. */
/*     RU  AF(4*NA)      Vector of values of bundle functions. */
/*     RU  AG(N*NA)      Matrix whose columns are bundle subgradients. */
/*     RU  AY(N*NA)      Matrix whose columns are bundle points. */
/*     II  IBUN          Index for the circular arrays in bundle. */
/*     RI  D(N)          Direction vector. */
/*     RI  F             Value of the objective function. */
/*     RI  DF            Directional derivative. */
/*     RO  T             Value of the stepsize parameter. */
/*     RI  ETA           Distance measure parameter. */
/*     RI  ETA9          Maximum for real numbers. */
/*     RI  MOS           Locality measure parameter. */



/*     * Original method * */

/*     PNSTP4 by J. Vleck (1999) */


/*     * Limited memory version * */

/*     Napsu Haarala (2002,2003) */

/* Subroutine */ int destep_(integer *n, integer *ma, integer *mal, 
	doublereal *x, doublereal *af, doublereal *ag, doublereal *ay, 
	integer *ibun, doublereal *d__, doublereal *f, doublereal *df, 
	doublereal *t, doublereal *eta, doublereal *eta9, integer *mos)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal);

    /* Local variables */
    static integer i__, j, k, l;
    static doublereal q, r__, w;
    static integer ib, jn;
    static doublereal dx;
    static integer lq;
    static doublereal alf, bet, alfl, alfr, betl, betr;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     Intrinsic Functions */
    /* Parameter adjustments */
    --d__;
    --ay;
    --ag;
    --af;
    --x;

    /* Function Body */
    alfl = 0.;
    betl = 0.;
    w = *df * *t * (1. - *t * .5);

/*     Initial choice of possibly active lines */

    k = 0;
    l = -1;
    jn = (*ibun - 1) * *n;
    betr = -(*eta9);
    i__1 = *mal - 1;
    for (j = 1; j <= i__1; ++j) {
	ib = *ibun - 1 + j;
	if (ib > *mal) {
	    ib -= *mal;
	}
	if (jn >= *mal * *n) {
	    jn -= *mal * *n;
	}
	r__ = 0.;
	bet = 0.;
	alfl = af[ib] - *f;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    dx = x[i__] - ay[jn + i__];
	    q = ag[jn + i__];
	    r__ += dx * dx;
	    alfl += dx * q;
	    bet += d__[i__] * q;
/* L10: */
	}
	if (*mos != 2) {
	    d__1 = (doublereal) (*mos) * .5;
	    r__ = pow_dd(&r__, &d__1);
	}
/* Computing MAX */
	d__1 = abs(alfl), d__2 = *eta * r__;
	alf = max(d__1,d__2);
	r__ = 1. - bet / *df;
	if (r__ * r__ + (alf + alf) / *df > 1e-6) {
	    ++k;
	    af[*ma + k] = alf;
	    af[*ma + *ma + k] = bet;
	    r__ = *t * bet - alf;
	    if (r__ > w) {
		w = r__;
		l = k;
	    }
	}
/* Computing MAX */
	d__1 = betr, d__2 = bet - alf;
	betr = max(d__1,d__2);
	jn += *n;
/* L20: */
    }
    lq = -1;
    if (betr <= *df * .5) {
	return 0;
    }
    lq = 1;
    if (l < 0) {
	return 0;
    }
    betr = af[*ma + *ma + l];
    if (betr <= 0.) {
	if (*t < 1. || betr == 0.) {
	    return 0;
	}
	lq = 2;
    }
    alfr = af[*ma + l];

/*     Iteration loop */

L30:
    if (lq >= 1) {
	q = 1. - betr / *df;
	r__ = q + sqrt(q * q + (alfr + alfr) / *df);
	if (betr >= 0.) {
	    r__ = -(alfr + alfr) / (*df * r__);
	}
/* Computing MIN */
	d__1 = 1.95, d__2 = max(0.,r__);
	r__ = min(d__1,d__2);
    } else {
	if ((d__1 = betr - betl, abs(d__1)) + (d__2 = alfr - alfl, abs(d__2)) 
		< *df * -1e-4) {
	    return 0;
	}
	r__ = (alfr - alfl) / (betr - betl);
    }
    if ((d__1 = *t - r__, abs(d__1)) < 1e-4) {
	return 0;
    }
    *t = r__;
    af[*ma + l] = -1.;
    w = *t * betr - alfr;
    l = -1;
    i__1 = k;
    for (j = 1; j <= i__1; ++j) {
	alf = af[*ma + j];
	if (alf < 0.) {
	    goto L40;
	}
	bet = af[*ma + *ma + j];
	r__ = *t * bet - alf;
	if (r__ > w) {
	    w = r__;
	    l = j;
	}
L40:
	;
    }
    if (l < 0) {
	return 0;
    }
    bet = af[*ma + *ma + l];
    if (bet == 0.) {
	return 0;
    }

/*     New interval selection */

    alf = af[*ma + l];
    if (bet < 0.) {
	if (lq == 2) {
	    alfr = alf;
	    betr = bet;
	} else {
	    alfl = alf;
	    betl = bet;
	    lq = 0;
	}
    } else {
	if (lq == 2) {
	    alfl = alfr;
	    betl = betr;
	    lq = 0;
	}
	alfr = alf;
	betr = bet;
    }
    goto L30;
} /* destep_ */

/* *********************************************************************** */

/*     * SUBROUTINE NULSTP * */


/*     * Purpose * */

/*     Stepsize selection using polyhedral approximation for null step */
/*     in limited memory bundle method. */


/*     * Calling sequence * */

/*     CALL NULSTP(N,NA,MAL,X,AF,AG,AY,IBUN,D,F,DF,T,ETA,ETA9,MOS) */


/*     * Parameters * */

/*     II  N             Number of variables. */
/*     II  NA            Maximum size of the bundle. */
/*     II  MAL           Current size of the bundle. */
/*     RU  X(N)          Vector of variables. */
/*     RU  AF(4*NA)      Vector of values of bundle functions. */
/*     RU  AG(N*NA)      Matrix whose columns are bundle subgradients. */
/*     RU  AY(N*NA)      Matrix whose columns are bundle points. */
/*     II  IBUN          Index for the circular arrays in bundle. */
/*     RI  D(N)          Direction vector. */
/*     RI  F             Value of the objective function. */
/*     RI  DF            Directional derivative. */
/*     RO  T             Value of the stepsize parameter. */
/*     RI  ETA           Distance measure parameter. */
/*     RI  ETA9          Maximum for real numbers. */
/*     RI  MOS           Locality measure parameter. */


/*     * Original method * */

/*     PNSTP5 by J. Vleck (1999) */


/*     * Limited memory version * */

/*     Napsu Haarala (2002,2003) */

/* Subroutine */ int nulstp_(integer *n, integer *ma, integer *mal, 
	doublereal *x, doublereal *af, doublereal *ag, doublereal *ay, 
	integer *ibun, doublereal *d__, doublereal *f, doublereal *df, 
	doublereal *t, doublereal *eta, doublereal *eta9, integer *mos)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *);
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static integer i__, j, k, l;
    static doublereal q, r__, w;
    static integer ib, jn;
    static doublereal dx, alf, bet, alfl, alfr, betl, betr;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     Intrinsic Functions */
    /* Parameter adjustments */
    --d__;
    --ay;
    --ag;
    --af;
    --x;

    /* Function Body */
    w = *df * *t;

/*     Initial choice of possibly active parabolas */

    k = 0;
    l = -1;
    jn = (*ibun - 1) * *n;
    betr = -(*eta9);
    i__1 = *mal - 1;
    for (j = 1; j <= i__1; ++j) {
	ib = *ibun - 1 + j;
	if (ib > *mal) {
	    ib -= *mal;
	}
	if (jn >= *mal * *n) {
	    jn -= *mal * *n;
	}
	bet = 0.;
	r__ = 0.;
	alfl = af[ib] - *f;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    dx = x[i__] - ay[jn + i__];
	    r__ += dx * dx;
	    q = ag[jn + i__];
	    alfl += dx * q;
	    bet += d__[i__] * q;
/* L10: */
	}
	if (*mos != 2) {
	    d__1 = (doublereal) (*mos) * .5;
	    r__ = pow_dd(&r__, &d__1);
	}
/* Computing MAX */
	d__1 = abs(alfl), d__2 = *eta * r__;
	alf = max(d__1,d__2);
/* Computing MAX */
	d__1 = betr, d__2 = bet - alf;
	betr = max(d__1,d__2);
	if (alf < bet - *df) {
	    ++k;
	    r__ = *t * bet - alf;
	    af[*ma + k] = alf;
	    af[*ma + *ma + k] = bet;
	    if (r__ > w) {
		w = r__;
		l = k;
	    }
	}
	jn += *n;
/* L20: */
    }
    if (l < 0) {
	return 0;
    }
    betr = af[*ma + *ma + l];
    alfr = af[*ma + l];
    alf = alfr;
    bet = betr;
    alfl = 0.;
    betl = *df;

/*     Iteration loop */

L30:
    w = bet / *df;
    if ((d__1 = betr - betl, abs(d__1)) + (d__2 = alfr - alfl, abs(d__2)) < *
	    df * -1e-4) {
	return 0;
    }
    if (betr - betl == 0.) {
	s_stop("11", (ftnlen)2);
    }
    r__ = (alfr - alfl) / (betr - betl);
    if ((d__1 = *t - w, abs(d__1)) < (d__2 = *t - r__, abs(d__2))) {
	r__ = w;
    }
    q = *t;
    *t = r__;
    if ((d__1 = *t - q, abs(d__1)) < .001) {
	return 0;
    }
    af[*ma + l] = -1.;
    w = *t * bet - alf;
    l = -1;
    i__1 = k;
    for (j = 1; j <= i__1; ++j) {
	alf = af[*ma + j];
	if (alf < 0.) {
	    goto L40;
	}
	bet = af[*ma + *ma + j];
	r__ = *t * bet - alf;
	if (r__ > w) {
	    w = r__;
	    l = j;
	}
L40:
	;
    }
    if (l < 0) {
	return 0;
    }
    bet = af[*ma + *ma + l];
    q = bet - *t * *df;
    if (q == 0.) {
	return 0;
    }

/*     New interval selection */

    alf = af[*ma + l];
    if (q < 0.) {
	alfl = alf;
	betl = bet;
    } else {
	alfr = alf;
	betr = bet;
    }
    goto L30;
} /* nulstp_ */

/* *********************************************************************** */

/*     * SUBROUTINE DOBUN * */


/*     * Purpose * */

/*     Bundle construction for limited memory bundle method */


/*     * Calling sequence * */

/*     CALL DOBUN(N,NA,MAL,X,G,F,AY,AG,AF,ITERS,IBUN) */


/*     * Parameters * */

/*     II  N             Number of variables. */
/*     II  NA            Maximum size of the bundle. */
/*     II  MAL           Current size of the bundle. */
/*     RI  X(N)          Vector of variables. */
/*     RI  G(N)          Subgradient of the objective function. */
/*     RI  F             Value of the objective function. */
/*     RU  AY(N*NA)      Matrix whose columns are bundle points. */
/*     RU  AG(N*NA)      Matrix whose columns are bundle subgradients. */
/*     RU  AF(4*NA)      Vector of values of bundle functions. */
/*     IU  IBUN          Index for the circular arrays. */
/*     II  ITERS         Null step indicator. */
/*                            0  - Null step. */
/*                            1  - Serious step. */


/*     * Subprograms used * */

/*     S   COPY2         Copying of two vectors. */


/*      Napsu Haarala (2003) */

/* Subroutine */ int dobun_(integer *n, integer *ma, integer *mal, doublereal 
	*x, doublereal *g, doublereal *f, doublereal *ay, doublereal *ag, 
	doublereal *af, integer *iters, integer *ibun)
{
    static integer i__, j;
    extern /* Subroutine */ int copy2_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Subroutines */
    /* Parameter adjustments */
    --af;
    --ag;
    --ay;
    --g;
    --x;

    /* Function Body */
    if (*iters == 1) {

/*     Serious step */

	af[*ibun] = *f;
	i__ = (*ibun - 1) * *n + 1;
	copy2_(n, &g[1], &ag[i__], &x[1], &ay[i__]);
    } else {

/*     Null step */

	if (*mal < *ma) {
	    af[*ibun] = af[*mal];
	    af[*mal] = *f;
	    i__ = *mal * *n + 1;
	    copy2_(n, &ag[i__ - *n], &ag[i__], &ay[i__ - *n], &ay[i__]);
	    copy2_(n, &g[1], &ag[i__ - *n], &x[1], &ay[i__ - *n]);
	} else {
	    i__ = *ibun - 1;
	    if (i__ < 1) {
		i__ = *mal;
	    }
	    af[*ibun] = af[i__];
	    af[i__] = *f;
	    i__ = (*ibun - 2) * *n + 1;
	    if (i__ < 1) {
		i__ = (*mal - 1) * *n + 1;
	    }
	    j = (*ibun - 1) * *n + 1;
	    copy2_(n, &ag[i__], &ag[j], &ay[i__], &ay[j]);
	    copy2_(n, &g[1], &ag[i__], &x[1], &ay[i__]);
	}
    }
    ++(*mal);
    if (*mal > *ma) {
	*mal = *ma;
    }
    ++(*ibun);
    if (*ibun > *ma) {
	*ibun = 1;
    }
    return 0;
} /* dobun_ */

/* *********************************************************************** */

/*     * SUBROUTINE RESTAR * */


/*     * Purpose * */

/*     Initialization. */


/*     * Calling sequence * */

/*     CALL RESTAR(N,MC,MCC,MCINIT,INEW,IBUN,IBFGS,ITERS,GP,G,NNK,ALFV, */
/*    &     ALFN,GAMMA,D,IC,ICN,MAL,NCRES,IFLAG) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     IO  MC              Current maximum number of stored corrections. */
/*     IO  MCC             Current number of stored corrections. */
/*     II  MCINIT          Initial maximum number of stored corrections. */
/*     IO  INEW            Index for the circular arrays. */
/*     IO  IBUN            Index for the circular arrays in bundle */
/*                           updating. */
/*     IO  IBFGS           Index of the type of BFGS update. */
/*     IU  ITERS           Null step indicator. */
/*                              0  - Null step. */
/*                              1  - Serious step. */
/*     IO  NNK             Consecutive null steps counter. */
/*     RI  GP(N)           Basic subgradient of the objective function. */
/*     RO  G(N)            Current (auxiliary) subgradient of the */
/*                           objective function. */
/*     RO  ALFN            Locality measure. */
/*     RO  ALFV            Aggregate locality measure. */
/*     RO  D(N)            Search direction. */
/*     RO  GAMMA           Scaling parameter. */
/*     IO  IC              Correction indicator. */
/*     IO  ICN             Correction indicator for null steps. */
/*     IO  MAL             Current size of the bundle. */
/*     IO  NCRES           Number of restarts. */
/*     IO  IFLAG           Index for adaptive version. */


/*     * Subprograms used * */

/*     S   COPY            Copying of a vector. */
/*     S   VNEG            Copying of a vector with change of the sign. */


/*     Napsu Karmitsa (2004, last modified 2007) */

/* Subroutine */ int restar_(integer *n, integer *mc, integer *mcc, integer *
	mcinit, integer *inew, integer *ibun, integer *ibfgs, integer *iters, 
	doublereal *gp, doublereal *g, integer *nnk, doublereal *alfv, 
	doublereal *alfn, doublereal *gamma, doublereal *d__, integer *ic, 
	integer *icn, integer *mal, integer *ncres, integer *iflag)
{
    extern /* Subroutine */ int vneg_(integer *, doublereal *, doublereal *), 
	    copy_(integer *, doublereal *, doublereal *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     External Subroutines */
    /* Parameter adjustments */
    --d__;
    --g;
    --gp;

    /* Function Body */
    *mc = *mcinit;
    *mcc = 0;
    *inew = 1;
    *ibun = 1;
    *ibfgs = 0;
    *ic = 0;
    *icn = 0;
    *mal = 0;
    ++(*ncres);
    *iflag = 0;
    if (*iters == 0) {
	copy_(n, &gp[1], &g[1]);
	*iters = 1;
	*nnk = 0;
	*alfv = 0.;
	*alfn = 0.;
    }
    *gamma = 1.;
    vneg_(n, &g[1], &d__[1]);
    return 0;
} /* restar_ */

/* *********************************************************************** */

/*     * SUBROUTINE RPRINT * */


/*     * Purpose * */

/*     Printout the (final) results. */


/*     * Calling sequence * */

/*     SUBROUTINE RPRINT(N,NIT,NFE,X,F,WK,QK,ITERM,IPRINT,NOUT) */


/*     * Parameters * */

/*     II  N               Number of variables. */
/*     II  NIT             Number of used iterations. */
/*     II  NFE             Number of used function evaluations. */
/*     RI  X(N)            Vector of variables. */
/*     RI  F               Value of the objective function. */
/*     RI  WK              Value of the first stopping criterion. */
/*     RI  QK              Value of the second stopping criterion. */
/*     II  ITERM           Cause of termination: */
/*                             1  - The problem has been solved. */
/*                                  with desired accuracy. */
/*                             2  - (F - FO) < TOLF in MTESF */
/*                                  subsequent iterations. */
/*                             3  - (F - FO) < TOLF*SMALL*MAX(|F|,|FO|,1). */
/*                             4  - Number of function calls > MFV. */
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



/*     Napsu Karmitsa (2004, last modified 2007) */

/* Subroutine */ int rprint_(integer *n, integer *nit, integer *nfe, 
	doublereal *x, doublereal *f, doublereal *wk, doublereal *qk, integer 
	*iterm, integer *iprint)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    static integer i__;

    /* Fortran I/O blocks */
    static cilist io___109 = { 0, 6, 0, "(1X,'NIT=',I5,2X,                  "
	    " 'NFE=',I5,2X,'F=',D15.8,2X,'WK=',D11.4,2X,                  'QK"
	    "=',D11.4,2X)", 0 };
    static cilist io___110 = { 0, 6, 0, "(1X,'X=',                          "
	    " 5D15.7:/(4X,5D15.7))", 0 };
    static cilist io___112 = { 0, 6, 0, "(1X,'NIT=',I5,2X,                  "
	    " 'NFE=',I5,2X,'F=',D15.8,2X,'WK=',D11.4,2X,                  'QK"
	    "=',D11.4,2X,'ITERM=',I3)", 0 };
    static cilist io___113 = { 0, 6, 0, "(1X,'X=',5D15.7:/(4X,5D15.7))", 0 };


/*     Scalar Arguments */
/*     Array Arguments */

/*     Intermediate results */

    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*iterm == 0) {
	if (*iprint > 3) {
	    s_wsfe(&io___109);
	    do_fio(&c__1, (char *)&(*nit), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*nfe), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*f), (ftnlen)sizeof(doublereal));
	    do_fio(&c__1, (char *)&(*wk), (ftnlen)sizeof(doublereal));
	    do_fio(&c__1, (char *)&(*qk), (ftnlen)sizeof(doublereal));
	    e_wsfe();
	}
	if (*iprint == 5) {
	    s_wsfe(&io___110);
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&x[i__], (ftnlen)sizeof(doublereal));
	    }
	    e_wsfe();
	}
	return 0;
    }

/*     Printout the final results */

    if (*iprint > 0) {
	s_wsfe(&io___112);
	do_fio(&c__1, (char *)&(*nit), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*nfe), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*f), (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&(*wk), (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&(*qk), (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&(*iterm), (ftnlen)sizeof(integer));
	e_wsfe();
    }
    if (*iprint == 3 || *iprint == 5) {
	s_wsfe(&io___113);
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&x[i__], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
    }
    return 0;
} /* rprint_ */

/* *********************************************************************** */

/*     * SUBROUTINE WPRINT * */


/*     * Purpose * */

/*     Printout the warning and error messages. */


/*     * Calling sequence * */

/*     SUBROUTINE WPRINT(ITERM,IPRINT,NOUT) */


/*     * Parameters * */

/*     II  ITERM           Cause of termination: */
/*                             1  - The problem has been solved. */
/*                                  with desired accuracy. */
/*                             2  - (F - FO) < TOLF*MAX(|F|,1) in MTESF */
/*                                  subsequent iterations. */
/*                             3  - (F - FO) < TOLF*SMALL*MAX(|F|,|FO|,1). */
/*                             4  - Number of function calls > MFV. */
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
/*     II  NOUT            Auxilary printout specification. */



/*     Napsu Karmitsa (2004, last modified 2007) */

/* Subroutine */ int wprint_(integer *iterm, integer *iprint, integer *nout)
{
    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Fortran I/O blocks */
    static cilist io___114 = { 0, 6, 0, "(1X,'Error: '                      "
	    "   'Number of variables (N) is too small. ITERM=',I3)           "
	    "   ", 0 };
    static cilist io___115 = { 0, 6, 0, "(1X,'Error: '                      "
	    "   'The maximum number of stored corrections (MCU) '            "
	    "   'is too small. ITERM=',I3)", 0 };
    static cilist io___116 = { 0, 6, 0, "(1X,'Error: '                      "
	    "   'The size of the bundle (NA) is too small. ITERM='           "
	    "   ,I3)", 0 };
    static cilist io___117 = { 0, 6, 0, "(1X,'Error: '                      "
	    "   'Line search parameter RPAR(6) >= 0.25. ITERM='              "
	    "   ,I3)", 0 };
    static cilist io___118 = { 0, 6, 0, "(1X,'Error: '                      "
	    "                    'Not enough working space. ITERM=',I3)", 0 };
    static cilist io___119 = { 0, 6, 0, "(1X,'Warning: '                    "
	    "  'MC > MCU. Assigned MC = MCU.')", 0 };
    static cilist io___120 = { 0, 6, 0, "(1X,'Warning: '                    "
	    "  'A line search parameter EPSR >= 0.5.')", 0 };
    static cilist io___121 = { 0, 6, 0, "(1X,'Warning: '                    "
	    "  'A nondescent search direction occured. Restart.')            "
	    "  ", 0 };
    static cilist io___122 = { 0, 6, 0, "(1X,'Warning: '                    "
	    "  'Does not converge.')", 0 };
    static cilist io___123 = { 0, 6, 0, "(1X,'Warning: '                    "
	    "  'TMAX < TMIN. Restart.')", 0 };
    static cilist io___124 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "  'Time is up.')", 0 };
    static cilist io___125 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "  'F < TOLB.')", 0 };
    static cilist io___126 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "  'Too many steps without significant progress.')               "
	    "  ", 0 };
    static cilist io___127 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "  'The value of the function does not change.')", 0 };
    static cilist io___128 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "  'Number of iterations > ',I5)", 0 };
    static cilist io___129 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "  'Number of function evaluations > ',I5)", 0 };
    static cilist io___130 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "                    'Two consecutive restarts.')", 0 };
    static cilist io___131 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "              'TMAX < TMIN in two subsequent iterations.')", 0 };
    static cilist io___132 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    "       'Number of restarts > ',I5'.')", 0 };
    static cilist io___133 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    " 'Failure in function or subgradient calculations.')", 0 };
    static cilist io___134 = { 0, 6, 0, "(1X,'Abnormal exit: '              "
	    " 'Failure in attaining the demanded accuracy.')", 0 };


/*     Scalar Arguments */
    if (*iprint >= 0) {

/*     Initial error messages */

	if (*iterm <= -5) {
	    if (*iterm == -5) {
		if (*nout == 1) {
		    s_wsfe(&io___114);
		    do_fio(&c__1, (char *)&(*iterm), (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		if (*nout == 2) {
		    s_wsfe(&io___115);
		    do_fio(&c__1, (char *)&(*iterm), (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		if (*nout == 3) {
		    s_wsfe(&io___116);
		    do_fio(&c__1, (char *)&(*iterm), (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		if (*nout == 4) {
		    s_wsfe(&io___117);
		    do_fio(&c__1, (char *)&(*iterm), (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    } else if (*iterm == -6) {
		s_wsfe(&io___118);
		do_fio(&c__1, (char *)&(*iterm), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    return 0;
	}

/*     Warning messages */

	if (*iprint >= 2) {
	    if (*iterm == 0) {
		if (*nout == -1) {
		    s_wsfe(&io___119);
		    e_wsfe();
		}
		if (*nout == -2) {
		    s_wsfe(&io___120);
		    e_wsfe();
		}
		if (*nout == -3) {
		    s_wsfe(&io___121);
		    e_wsfe();
		}
		if (*nout == -4) {
		    s_wsfe(&io___122);
		    e_wsfe();
		}
		if (*nout == -5) {
		    s_wsfe(&io___123);
		    e_wsfe();
		}
		return 0;
	    }

/*     Printout the final results */

	    if (*iterm == 6) {
		s_wsfe(&io___124);
		e_wsfe();
	    }
	    if (*iterm == 7) {
		s_wsfe(&io___125);
		e_wsfe();
	    }
	    if (*iterm == 2) {
		s_wsfe(&io___126);
		e_wsfe();
	    }
	    if (*iterm == 3) {
		s_wsfe(&io___127);
		e_wsfe();
	    }
	    if (*iterm == 5) {
		s_wsfe(&io___128);
		do_fio(&c__1, (char *)&(*nout), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*iterm == 4) {
		s_wsfe(&io___129);
		do_fio(&c__1, (char *)&(*nout), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*iterm == -1) {
		if (*nout == -1) {
		    s_wsfe(&io___130);
		    e_wsfe();
		} else {
		    s_wsfe(&io___131);
		    e_wsfe();
		}
	    }
	    if (*iterm == -2) {
		s_wsfe(&io___132);
		do_fio(&c__1, (char *)&(*nout), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*iterm == -3) {
		s_wsfe(&io___133);
		e_wsfe();
	    }
	    if (*iterm == -4) {
		s_wsfe(&io___134);
		e_wsfe();
	    }
	}
    }
    return 0;
} /* wprint_ */

/* *********************************************************************** */

/*     * SUBROUTINE INDIC1 * */


/*     * Purpose * */

/*     Initialization of indices. */


/*     * Calling sequence * */

/*     CALL INDIC1(MC,MCC,MCNEW,INEW,IOLD,IFLAG,IFLAG2,ITYPE) */


/*     * Parameters * */

/*     II  MC              Declared number of stored corrections. */
/*     II  MCC             Current number of depositories used. */
/*     IO  MCNEW           Current size of vectors. */
/*     II  ITYPE           Type of Initialization: */
/*                             1  - corrections are stored, */
/*                             2  - corrections are not stored, */
/*                             3  - update is skipped. */
/*     IU  INEW            Index for circular arrays. */
/*     IO  IOLD            Index of the oldest corrections. */
/*     IU  IFLAG           Index for adaptive version: */
/*                             0  - Maximum number of stored corrections */
/*                                    has not been changed at previous */
/*                                    iteration. */
/*                             1  - Maximum number of stored corrections */
/*                                    has been changed at previous */
/*                                    iteration. */
/*     IU  IFLAG2          Index for adaptive version: */
/*                             0  - IFLAG has not been changed. */
/*                             1  - IFLAG has been changed. */



/*     Napsu Haarala (2002,2003, Last modified 2005) */

/* Subroutine */ int indic1_(integer *mc, integer *mcc, integer *mcnew, 
	integer *inew, integer *iold, integer *iflag, integer *iflag2, 
	integer *itype)
{
/*     Scalar Arguments */
    if (*itype == 1) {
	if (*mcc < *mc) {
	    *mcnew = *mcc + 1;
	    *iold = 1;
	    *iflag = 0;
	} else {
	    if (*iflag == 0) {
		*mcnew = *mc;
		*iold = *inew + 2;
		if (*iold > *mc + 1) {
		    *iold = *iold - *mc - 1;
		}
	    } else {
		if (*inew == 1) {
		    *inew = *mc + 1;
		    *mcnew = *mc;
		    *iold = 2;
		    *iflag = 0;
		    *iflag2 = 1;
		} else if (*inew == *mc) {
		    *mcnew = *mc;
		    *iold = 1;
		    *iflag = 0;
		    *iflag2 = 1;
		} else {
		    *mcnew = *mc - 1;
		    *iold = *inew + 2;
		    if (*iold > *mc) {
			*iold -= *mc;
		    }
		}
	    }
	}
    } else if (*itype == 2) {
	if (*mcc < *mc) {
	    *mcnew = *mcc + 1;
	    *iold = 1;
	    *iflag = 0;
	} else {
	    if (*iflag == 0) {
		*mcnew = *mc + 1;
		*iold = *inew + 1;
		if (*iold > *mc + 1) {
		    *iold = 1;
		}
	    } else {
		*mcnew = *mc;
		*iold = *inew + 1;
		if (*iold > *mc) {
		    *iold = 1;
		}
	    }
	}
    } else {
	if (*mcc < *mc) {
	    *mcnew = *mcc;
	    *iold = 1;
	    *iflag = 0;
	} else {
	    if (*iflag == 0) {
		*mcnew = *mc;
		*iold = *inew + 1;
		if (*iold > *mc + 1) {
		    *iold = 1;
		}
	    } else {
		*mcnew = *mc - 1;
		*iold = *inew + 1;
		if (*iold > *mc) {
		    *iold = 1;
		}
	    }
	}
    }
    return 0;
} /* indic1_ */

/* *********************************************************************** */

/*     * DOUBLE PRECISION FUNCTION SCLPAR * */


/*     * Purpose * */

/*     Calculation of the scaling parameter appointed by parameter ISCALE. */


/*     * Calling sequence * */

/*      GAMMA = SCLPAR(MCC,ISCALE,METHOD,STS,STU,UTU,SMALL) */



/*     * Parameters * */

/*     II  MCC             Current number of depositories used. */
/*     RI  STS             STS = TRANS(S)*S. */
/*     RI  STU             STU = TRANS(S)*U. */
/*     RI  UTU             UTU = TRANS(U)*U. */
/*     RI  SMALL           Small positive value. */
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


/*     Napsu Karmitsa (2004, Last modified 2007) */

doublereal sclpar_(integer *mcc, integer *iscale, integer *method, doublereal 
	*sts, doublereal *stu, doublereal *utu, doublereal *small)
{
    /* System generated locals */
    doublereal ret_val;

    /* Builtin functions */
    double sqrt(doublereal);

/*     Scalar Arguments */
/*     Intrinsic Functions */

/*     Computation of scaling parameter. */

    ret_val = 1.;

/*     Scaling parameter = STU/UTU */

    if (*iscale == 0 || *iscale == 2 || *iscale == 4) {
	if (*utu < sqrt(*small)) {
	    ret_val = 1.;
	    goto L80;
	} else {
	    ret_val = *stu / *utu;
	}

/*     Scaling parameter = STS/STU */

    } else if (*iscale == 1 || *iscale == 3 || *iscale == 5) {
	if (*stu < sqrt(*small)) {
	    ret_val = 1.;
	    goto L80;
	} else {
	    ret_val = *sts / *stu;
	}
    } else {

/*     No scaling */

	ret_val = 1.;
	goto L80;
    }

/*     Scaling */

    if (*mcc == 0) {
	if (ret_val < .01) {
	    ret_val = .01;
	}
	if (ret_val > 100.) {
	    ret_val = 100.;
	}

/*     Interval scaling */

    } else if (*iscale == 2) {
	if (*method == 0) {
	    if (ret_val < .6 || ret_val > 6.) {
		ret_val = 1.;
	    }
	} else {
	    if (ret_val < .01 || ret_val > 100.) {
		ret_val = 1.;
	    }
	}
    } else if (*iscale == 3) {
	if (ret_val < .5 || ret_val > 5.) {
	    ret_val = 1.;
	}

/*     Preliminary scaling */

    } else if (*iscale == 4 || *iscale == 5) {
	ret_val = 1.;

/*     Scaling at every iteration */

    } else {
    }
L80:
    if (ret_val <= *small * 1e3) {
	ret_val = *small * 1e3;
    }
    return ret_val;
} /* sclpar_ */

/* *********************************************************************** */

/*     * SUBROUTINE GETIME * */


/*     * Purpose * */

/*     Execution time. */


/*     * Calling sequence * */

/*     CALL GETIME(CTIM,RTIM) */


/*     * Parameters * */

/*     RO  CTIM          Current time. REAL argument */
/*     RA  RTIM(2)       Auxiliary array. REAL array. */


/*     * Subprograms used * */

/*     RF  ETIME         Execution time. */


/*     Napsu Karmitsa (2007) */

/* Subroutine */ int getime_(real *ctim, real *rtim)
{
    extern doublereal etime_(real *);

/*     Scalar Arguments */
/*     Array arguments */
/*     Intrinsic Functions */
/*     REAL ETIME */
/*     INTRINSIC ETIME */
    /* Parameter adjustments */
    --rtim;

    /* Function Body */
    *ctim = etime_(&rtim[1]);
    *ctim = rtim[1];
    return 0;
} /* getime_ */

