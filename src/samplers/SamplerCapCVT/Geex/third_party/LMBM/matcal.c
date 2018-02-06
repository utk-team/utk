/* matcal.f -- translated by f2c (version 20060506).
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

/* *********************************************************************** */


/*     MATCAL includes the following subroutines */

/*     S   CALQ            Solving X from linear equation A*X=Y. */
/*     S   COPY            Copying of vector. */
/*     S   COPY2           Copying of two vectors. */
/*     S   CWMAXV          Multiplication of a columnwise stored dense */
/*                           rectangular matrix by a vector. */
/*     S   LINEQ           Solver for linear equation. */
/*     S   MXDPGF          Gill-Murray decomposition of a dense symmetric */
/*                           matrix. Coded by L. Luksan. */
/*     S   RWAXV2          Multiplication of two rowwise stored dense */
/*                           rectangular matrices A and B by vectors X */
/*                           and Y. */
/*     S   SCALEX          Scaling a vector. */
/*     S   SCDIFF          Difference of the scaled vector and a vector. */
/*     S   SCSUM           Sum of a vector and the scaled vector. */
/*     S   SYMAX           Multiplication of a dense symmetric matrix */
/*                           by a vector. */
/*     S   TRLIEQ          Solving X from linear equation L*X=Y or */
/*                           L'*X=Y. */
/*     S   VNEG            Change the signs of vector elements. */
/*     S   VXDIAG          Multiplication of a vector and a diagonal */
/*                           matrix. */
/*     S   XDIFFY          Difference of two vectors. */
/*     S   XSUMY           Sum of two vectors. */
/*     RF  EPS0            The smallest positive number such that */
/*                           1.0 + EPS0 > 1.0. */
/*     RF  VDOT            Dot product of two vectors. */


/* *********************************************************************** */

/*     * SUBROUTINE SYMAX * */


/*     * Purpose * */

/*     Multiplication of a dense symmetric matrix A by a vector X. */


/*     * Calling sequence * */

/*     CALL SYMAX(N,M,IOLD,A,X,Y) */


/*     * Parameters * */

/*     II  N               Order of matrix A. */
/*     RI  A(N*(N+1)/2)    Dense symmetric matrix stored in the packed */
/*                           form. */
/*     II  M               Length of vector X, M >= N, note that only N */
/*                           components from vector X are used. */
/*     RI  X(M)            Input vector stored in a circular order. */
/*     II  IOLD            Index, which controlls the circular order of */
/*                           the vector X. */
/*     RO  Y(M)            Output vector equal to A*X. The vector Y has */
/*                           the same circular order than X. */


/*     Napsu Haarala (2002). */

/* Subroutine */ int symax_(integer *n, integer *m, integer *iold, doublereal 
	*a, doublereal *x, doublereal *y)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, k, l;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --y;
    --x;
    --a;

    /* Function Body */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	l = j + *iold - 1;
	if (l > *m) {
	    l -= *m;
	}
	y[l] = 0.;
	k = l;
	i__2 = *n;
	for (i__ = j; i__ <= i__2; ++i__) {
	    y[l] = a[(i__ - 1) * i__ / 2 + j] * x[k] + y[l];
	    ++k;
	    if (k > *m) {
		k -= *m;
	    }
/* L10: */
	}
/* L20: */
    }
    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
	l = j + *iold - 1;
	if (l > *m) {
	    l -= *m;
	}
	k = *iold;
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (k > *m) {
		k -= *m;
	    }
	    y[l] = a[(j - 1) * j / 2 + i__] * x[k] + y[l];
	    ++k;
/* L30: */
	}
/* L40: */
    }
    return 0;
} /* symax_ */

/* *********************************************************************** */

/*     * SUBROUTINE CWMAXV * */


/*     * Purpose * */

/*     Multiplication of a columnwise stored dense rectangular matrix A */
/*     by a vector X with a possibility of scaling. */


/*     * Calling sequence * */

/*     CALL CWMAXV(N,M,A,X,Y,S) */


/*     * Parameters * */

/*     II  N             Number of rows of the matrix A. */
/*     II  M             Number of columns of the matrix A. */
/*     RI  A(N*M)        Rectangular matrix stored columnwise in the */
/*                       one-dimensional array. */
/*     RI  X(M)          Input vector. */
/*     RI  S             Scaling parameter. */
/*     RO  Y(N)          Output vector equal to S*A*X. If M = 0 Y is */
/*                         a zero vector. */


/*     * Subprograms used * */

/*     S   SCSUM         Sum of a vector and the scaled vector. */


/*     * Napsu Haarala, 2006 */

/* Subroutine */ int cwmaxv_(integer *n, integer *m, doublereal *a, 
	doublereal *x, doublereal *y, doublereal *s)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    static integer i__, j, k;
    extern /* Subroutine */ int scsum_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     External Subroutines */
    /* Parameter adjustments */
    --y;
    --x;
    --a;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	y[i__] = 0.;
/* L10: */
    }
    k = 0;
    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	d__1 = *s * x[j];
	scsum_(n, &d__1, &a[k + 1], &y[1], &y[1]);
	k += *n;
/* L20: */
    }
    return 0;
} /* cwmaxv_ */

/* *********************************************************************** */

/*     * SUBROUTINE RWAXV2 * */


/*     * Purpose * */

/*     Multiplication of two rowwise stored dense rectangular matrices A */
/*     and B by vectors X and Y. */


/*     * Calling sequence * */

/*     CALL RWAXV2(N,M,A,B,X,Y,V,W) */


/*     * Parameters * */

/*     II  N             Number of columns of the matrices A and B. */
/*     II  M             Number of rows of the matrices A and B. */
/*     RI  A(N*M)        Rectangular matrix stored rowwise in the */
/*                       one-dimensional array. */
/*     RI  B(N*M)        Rectangular matrix stored rowwise in the */
/*                       one-dimensional array. */
/*     RI  X(N)          Input vector. */
/*     RI  Y(N)          Input vector. */
/*     RO  V(M)          Output vector equal to A*X. */
/*     RO  W(M)          Output vector equal to B*Y. */


/*     Napsu Haarala, 2006 */


/* Subroutine */ int rwaxv2_(integer *n, integer *m, doublereal *a, 
	doublereal *b, doublereal *x, doublereal *y, doublereal *v, 
	doublereal *w)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, k;
    static doublereal tmp1, tmp2;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --w;
    --v;
    --y;
    --x;
    --b;
    --a;

    /* Function Body */
    k = 0;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	tmp1 = 0.;
	tmp2 = 0.;
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    tmp1 += a[k + j] * x[j];
	    tmp2 += b[k + j] * y[j];
/* L20: */
	}
	v[i__] = tmp1;
	w[i__] = tmp2;
	k += *n;
/* L10: */
    }
    return 0;
} /* rwaxv2_ */

/* *********************************************************************** */

/*     * SUBROUTINE MXDPGF * */


/*     * Purpose * */

/*     Factorization A+E=L*D*trans(L) of a dense symmetric positive */
/*     definite matrix A+E, where D and E are diagonal positive definite */
/*     matrices and L is a lower triangular matrix. If A is sufficiently */
/*     positive definite then E=0. */


/*     * CALLING SEQUENCE * */

/*     CALL MXDPGF(N,A,INF,ALF,TAU) */


/*     * PARAMETERS * */

/*     II  N             Order of the matrix A. */
/*     RU  A(N*(N+1)/2)  On input a given dense symmetric (usually */
/*                       positive definite) matrix A stored in the packed */
/*                       form. On output the computed factorization */
/*                       A+E=L*D*trans(L). */
/*     IO  INF           An information obtained in the factorization */
/*                       process: */
/*                         INF=0  - A is sufficiently positive definite */
/*                                  and E=0. */
/*                         INF<0  - A is not sufficiently positive */
/*                                  definite and E>0. */
/*                         INF>0  - A is indefinite and INF is an index */
/*                                  of the most negative diagonal element */
/*                                  used in the factorization process. */
/*     RU  ALF           On input a desired tolerance for positive */
/*                       definiteness. On output the most negative */
/*                       diagonal element used in the factorization */
/*                       process (if INF>0). */
/*     RO  TAU           Maximum diagonal element of the matrix E. */


/*     * Method * */

/*     P.E.Gill, W.Murray : Newton type methods for unconstrained and */
/*     linearly constrained optimization, Math. Programming 28 (1974) */
/*     pp. 311-350. */


/*     * Original version * */

/*     L.Luksan */


/* Subroutine */ int mxdpgf_(integer *n, doublereal *a, integer *inf, 
	doublereal *alf, doublereal *tau)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal d__1, d__2, d__3;

    /* Local variables */
    static integer i__, j, k, l, ij, ik, kj, kk;
    static doublereal del, gam, bet, sig, rho, tol;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
/*     Intrinsic Functions */
    /* Parameter adjustments */
    --a;

    /* Function Body */
    l = 0;
    *inf = 0;
    tol = *alf;

/*     Estimation of the matrix norm */

    *alf = 0.;
    bet = 0.;
    gam = 0.;
    *tau = 0.;
    kk = 0;
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	kk += k;
/* Computing MAX */
	d__2 = bet, d__3 = (d__1 = a[kk], abs(d__1));
	bet = max(d__2,d__3);
	kj = kk;
	i__2 = *n;
	for (j = k + 1; j <= i__2; ++j) {
	    kj = kj + j - 1;
/* Computing MAX */
	    d__2 = gam, d__3 = (d__1 = a[kj], abs(d__1));
	    gam = max(d__2,d__3);
/* L10: */
	}
/* L20: */
    }
/* Computing MAX */
    d__1 = max(tol,bet), d__2 = gam / *n;
    bet = max(d__1,d__2);
    del = tol * max(bet,1.);
    kk = 0;
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	kk += k;

/*     Determination of a diagonal correction */

	sig = a[kk];
	if (*alf > sig) {
	    *alf = sig;
	    l = k;
	}
	gam = 0.;
	kj = kk;
	i__2 = *n;
	for (j = k + 1; j <= i__2; ++j) {
	    kj = kj + j - 1;
/* Computing MAX */
	    d__2 = gam, d__3 = (d__1 = a[kj], abs(d__1));
	    gam = max(d__2,d__3);
/* L30: */
	}
	gam *= gam;
/* Computing MAX */
	d__1 = abs(sig), d__2 = gam / bet, d__1 = max(d__1,d__2);
	rho = max(d__1,del);
	if (*tau < rho - sig) {
	    *tau = rho - sig;
	    *inf = -1;
	}

/*     Gaussian elimination */

	a[kk] = rho;
	kj = kk;
	i__2 = *n;
	for (j = k + 1; j <= i__2; ++j) {
	    kj = kj + j - 1;
	    gam = a[kj];
	    a[kj] = gam / rho;
	    ik = kk;
	    ij = kj;
	    i__3 = j;
	    for (i__ = k + 1; i__ <= i__3; ++i__) {
		ik = ik + i__ - 1;
		++ij;
		a[ij] -= a[ik] * gam;
/* L40: */
	    }
/* L50: */
	}
/* L60: */
    }
    if (l > 0 && abs(*alf) > del) {
	*inf = l;
    }
    return 0;
} /* mxdpgf_ */

/* *********************************************************************** */

/*     * SUBROUTINE CALQ * */


/*     * Purpose * */

/*     Solving X from linear equation A*X=Y. */


/*     * Calling sequence * */

/*     CALL CALQ(N,M,IOLD,A,X,Y,SMALL,IPRINT) */


/*     * Parameters * */

/*     II  N               Order of matrix A. */
/*     RU  A(N*(N+1)/2)    On input: Dense symmetric matrix stored in */
/*                           the packed form. On output: factorization */
/*                           A+E=L*D*TRANS(L). */
/*     II  M               Length of vector Y, M >= N, note that only N */
/*                           components from vector Y are used */
/*     RI  Y(M)            Input vector stored in a circular order. */
/*     II  IOLD            Index, which controlls the circular order of */
/*                           the vector Y. */
/*     RO  X(M)            Output vector equal to the solution of A*X=Y. */
/*                           The vector X has the same circular order */
/*                           than Y. Note that X may be equal to Y in */
/*                           calling sequence. */
/*     RI  SMALL           Small positive value. */
/*     II  IPRINT          Printout specification. */


/*     * Local variables * */

/*     I   INF */
/*     R   ETA */
/*     R   BET */
/*     I   IERR            Error indicador: */
/*                             0  - Everything is ok. */
/*                            -2  - Error in LINEQ. */


/*     * Subprograms used * */

/*     S   MXDPGF          Gill-Murray decomposition of a dense symmetric */
/*                           matrix. */
/*     S   LINEQ           Solver for linear equation. */


/*     Napsu Haarala (2003) */

/* Subroutine */ int calq_(integer *n, integer *m, integer *iold, doublereal *
	a, doublereal *x, doublereal *y, doublereal *small, integer *iprint)
{
    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(void);

    /* Local variables */
    static doublereal eta, bet;
    static integer inf, ierr;
    extern /* Subroutine */ int lineq_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), mxdpgf_(
	    integer *, doublereal *, integer *, doublereal *, doublereal *);

    /* Fortran I/O blocks */
    static cilist io___30 = { 0, 6, 0, "(1X,'Warning: Insufficiently positiv"
	    "e'                 ' definite matrix detected. ')", 0 };
    static cilist io___31 = { 0, 6, 0, "(1X,'Correction added.')", 0 };
    static cilist io___32 = { 0, 6, 0, "(1X,'Warning: Indefinite'           "
	    "' matrix detected. '                                            "
	    "'Correction added.')", 0 };
    static cilist io___34 = { 0, 6, 0, "(1X,'Warning: '                     "
	    "                   ' Indefinite matrix detected. ')", 0 };


/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Variables */
/*     External Subroutines */
    /* Parameter adjustments */
    --y;
    --x;
    --a;

    /* Function Body */
    eta = *small;
    mxdpgf_(n, &a[1], &inf, &eta, &bet);
    if (inf < 0) {
	if (*iprint == 2) {
	    s_wsfe(&io___30);
	    e_wsfe();
	    s_wsfe(&io___31);
	    e_wsfe();
	}
    } else if (inf > 0) {
	if (*iprint == 2) {
	    s_wsfe(&io___32);
	    e_wsfe();
	}
    }
    lineq_(n, m, iold, &a[1], &x[1], &y[1], &ierr);
    if (ierr != 0) {
	if (*iprint == 2) {
	    s_wsfe(&io___34);
	    e_wsfe();
	}
    }
    return 0;
} /* calq_ */

/* *********************************************************************** */

/*     * SUBROUTINE LINEQ * */


/*     * Purpose * */

/*     Solving X from linear equation A*X=Y. Positive definite matrix A+E */
/*     is given using the factorization A+E=L*D*L' obtained by the */
/*     subroutine MXDPGF. */


/*     * Calling sequence * */

/*     CALL LINEQ(N,M,IOLD,A,X,Y,IERR) */


/*     * Parameters * */

/*     II  N               Order of matrix A. */
/*     RI  A(N*(N+1)/2)    Factorization A+E=L*D*L' obtained by the */
/*                           subroutine MXDPGF. */
/*     II  M               Length of vector Y, M >= N. Note that only N */
/*                           components from vector Y are used */
/*     RI  Y(M)            Input vector stored in a circular order. */
/*     II  IOLD            Index, which controlls the circular order of */
/*                           the vector Y. */
/*     RO  X(M)            Output vector equal to the solution of A*X=Y. */
/*                           The vector X has the same circular order */
/*                           than Y. Note that X may be equal to Y in */
/*                           calling sequence. */
/*     IO  IERR            Error indicador: */
/*                             0  - Everything is ok. */
/*                            -2  - Error; 0 at diagonal. */


/*     * Method * */

/*     Forward and backward substitution */


/*     Napsu Haarala (2003) */

/* Subroutine */ int lineq_(integer *n, integer *m, integer *iold, doublereal 
	*a, doublereal *x, doublereal *y, integer *ierr)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, k, l, ii, ij;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Variables */
    /* Parameter adjustments */
    --y;
    --x;
    --a;

    /* Function Body */
    *ierr = -2;

/*     Phase 1: X=L**(-1)*X */

    ij = 0;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	l = i__ + *iold - 1;
	if (l > *m) {
	    l -= *m;
	}
	x[l] = y[l];
/*        II = II + I */
	i__2 = i__ - 1;
	for (j = 1; j <= i__2; ++j) {
	    ++ij;
	    k = j + *iold - 1;
	    if (k > *m) {
		k -= *m;
	    }
	    x[l] -= a[ij] * x[k];
/* L20: */
	}
	++ij;
/* L10: */
    }

/*     Phase 2 : X:=D**(-1)*X */

    ii = 0;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ii += i__;
	if (a[ii] == 0.) {
	    goto L999;
	}
	l = i__ + *iold - 1;
	if (l > *m) {
	    l -= *m;
	}
	x[l] /= a[ii];
/* L30: */
    }

/*     Phase 3 : X:=TRANS(L)**(-1)*X */

    ii = *n * (*n - 1) / 2;
    for (i__ = *n - 1; i__ >= 1; --i__) {
	ij = ii;
	l = i__ + *iold - 1;
	if (l > *m) {
	    l -= *m;
	}
	i__1 = *n;
	for (j = i__ + 1; j <= i__1; ++j) {
	    k = j + *iold - 1;
	    if (k > *m) {
		k -= *m;
	    }
	    ij = ij + j - 1;
	    x[l] -= a[ij] * x[k];
/* L40: */
	}
	ii -= i__;
/* L50: */
    }
/*     Phase 2 and 3 */
/*      II = N * (N+1)/2 */
/*      DO 30 I = N,1,-1 */
/*         L=I+IOLD-1 */
/*         IF (L .GT. M) L=L-M */
/*         X(L) = X(L)/A(II) */
/*         IJ = II */
/*         DO 40 J = I + 1,N */
/*            K=J+IOLD-1 */
/*            IF (K .GT. M) K=K-M */
/*            IJ = IJ + J - 1 */
/*            X(L) = X(L) - A(IJ)*X(K) */
/* 40      CONTINUE */
/*         II = II - I */
/* 30   CONTINUE */
    *ierr = 0;
L999:
    return 0;
} /* lineq_ */

/* *********************************************************************** */

/*     * SUBROUTINE TRLIEQ * */


/*     * Purpose * */

/*     Solving X from linear equation U*X=Y or U'*X=Y, where U is */
/*     an upper triangular matrix. */


/*     * Calling sequence * */

/*     CALL TRLIEQ(N,M,IOLD,U,X,Y,JOB,SMALL,IERR) */


/*     * Parameters * */

/*     II  N               Order of matrix U. */
/*     RI  U(N*(N+1)/2)    Triangular matrix. */
/*     II  M               Length of vector Y, M >= N. Note that only N */
/*                           components from vector Y are used */
/*     RI  Y(M)            Input vector stored in a circular order. */
/*     II  IOLD            Index, which controlls the circular order of */
/*                           the vector Y. */
/*     RO  X(M)            Output vector equal to the solution of U*X=Y */
/*                           or U'*X=Y. The vector X has the same */
/*                           circular order than Y. Note that X may be */
/*                           equal to Y in calling sequence. */
/*     II  JOB             Option: */
/*                             0  - X:=(U')**(-1)*Y, U upper */
/*                                  triangular. */
/*                             1  - X:=U**(-1)*Y, U upper triangular. */
/*     RI  SMALL           Small positive value. */
/*     IO  IERR            Error indicador: */
/*                             0  - Everything is ok. */
/*                            -3  - Error; 0 at diagonal. */



/*     * Method * */

/*     Forward and backward substitution */


/*     Napsu Haarala (2003) */

/* Subroutine */ int trlieq_(integer *n, integer *m, integer *iold, 
	doublereal *u, doublereal *x, doublereal *y, integer *job, integer *
	ierr)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, k, l, ii, ij, ji;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Variables */
/*     Intrinsic Functions */
    /* Parameter adjustments */
    --y;
    --x;
    --u;

    /* Function Body */
    *ierr = -3;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = y[i__];
    }
    if (*job == 0) {

/*     X=U'**(-1)*Y, U' = [u1         ] is lower triangular. */
/*                        [u2 u3      ] */
/*                        [u4 u5 u6   ] */
/*                        [.  .  .  . ] */

	ii = 0;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ii += i__;
	    l = i__ + *iold - 1;
	    if (l > *m) {
		l -= *m;
	    }
	    if (u[ii] == 0.) {
		goto L999;
	    }
	    x[l] /= u[ii];
	    i__2 = *n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		ji = (j - 1) * j / 2 + i__;
		k = j + *iold - 1;
		if (k > *m) {
		    k -= *m;
		}
		x[k] -= u[ji] * x[l];
/* L20: */
	    }
/* L10: */
	}
    } else if (*job == 1) {

/*     X=U**(-1)*Y, U = [u1 u2 u4 . ] is upper triangular. */
/*                      [   u3 u5 . ] */
/*                      [      u6 . ] */
/*                      [         . ] */

	ii = *n * (*n + 1) / 2;
	for (i__ = *n; i__ >= 1; --i__) {
	    l = i__ + *iold - 1;
	    if (l > *m) {
		l -= *m;
	    }
	    if (u[ii] == 0.) {
		goto L999;
	    }
	    ij = ii;
	    i__1 = *n;
	    for (j = i__ + 1; j <= i__1; ++j) {
		k = j + *iold - 1;
		if (k > *m) {
		    k -= *m;
		}
		ij = ij + j - 1;
		x[l] -= u[ij] * x[k];
/* L60: */
	    }
	    x[l] /= u[ii];
	    ii -= i__;
/* L50: */
	}
    } else {
	goto L999;
    }
    *ierr = 0;
L999:
    return 0;
} /* trlieq_ */

/* *********************************************************************** */

/*     * SUBROUTINE SCALEX * */


/*     * Purpose * */

/*     Scaling a vector. */


/*     * Calling sequence * */

/*     CALL SCALEX(N,A,X,Y) */


/*     * Parameters * */

/*     II  N             Vector dimension. */
/*     RI  X(N)          Input vector. */
/*     RI  A             Scaling parameter. */
/*     RO  Y(N)          Output vector, where Y:= A*X. */


/*     Napsu Haarala (2006) */

/* Subroutine */ int scalex_(integer *n, doublereal *a, doublereal *x, 
	doublereal *y)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	y[i__] = *a * x[i__];
/* L10: */
    }
    return 0;
} /* scalex_ */

/* *********************************************************************** */

/*     * SUBROUTINE SCDIFF * */


/*     * Purpose * */

/*     Difference of the scaled vector and a vector. */


/*     * Calling sequence * */

/*     CALL SCDIFF(N,A,X,Y,Z) */


/*     * Parameters * */

/*     II  N             Vector dimension. */
/*     RI  A             Scaling factor. */
/*     RI  X(N)          Input vector. */
/*     RI  Y(N)          Input vector. */
/*     RO  Z(N)          Output vector, where Z:= A*X - Y. */


/*     Napsu Haarala (2004) */

/* Subroutine */ int scdiff_(integer *n, doublereal *a, doublereal *x, 
	doublereal *y, doublereal *z__)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --z__;
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	z__[i__] = *a * x[i__] - y[i__];
/* L10: */
    }
    return 0;
} /* scdiff_ */

/* *********************************************************************** */

/*     * SUBROUTINE SCSUM * */


/*     * Purpose * */

/*     Sum of a vector and the scaled vector. */


/*     * Calling sequence * */

/*     CALL SCSUM(N,A,X,Y,Z) */


/*     * Parameters * */

/*     II  N             Vector dimension. */
/*     RI  A             Scaling factor. */
/*     RI  X(N)          Input vector. */
/*     RI  Y(N)          Input vector. */
/*     RO  Z(N)          Output vector, where Z:= Y + A*X. */


/*     Napsu Haarala (2004) */

/* Subroutine */ int scsum_(integer *n, doublereal *a, doublereal *x, 
	doublereal *y, doublereal *z__)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --z__;
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	z__[i__] = y[i__] + *a * x[i__];
/* L10: */
    }
    return 0;
} /* scsum_ */

/* *********************************************************************** */

/*     * SUBROUTINE COPY * */


/*     * Purpose * */

/*     Copying of vector. */


/*     * Calling sequence * */

/*     CALL COPY(N,X,Y) */


/*     * Parameters * */

/*     II  N               Vectors dimension. */
/*     RI  X(N)            Input vector. */
/*     RO  Y(N)            Output vector where Y:= X. */


/*     Napsu Haarala (2004) */


/* Subroutine */ int copy_(integer *n, doublereal *x, doublereal *y)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	y[i__] = x[i__];
/* L10: */
    }
    return 0;
} /* copy_ */

/* *********************************************************************** */

/*     * SUBROUTINE COPY2 * */


/*     * Purpose * */

/*     Copying of two vectors. */


/*     * Calling sequence * */

/*     CALL COPY2(N,X,Y,Z,V) */


/*     * Parameters * */

/*     II  N             Vectors dimension. */
/*     RI  X(N)          Input vector. */
/*     RO  Y(N)          Output vector where Y:= X. */
/*     RI  Z(N)          Input vector. */
/*     RO  V(N)          Output vector where V:= Z. */


/*     Napsu Haarala (2004) */


/* Subroutine */ int copy2_(integer *n, doublereal *x, doublereal *y, 
	doublereal *z__, doublereal *v)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --v;
    --z__;
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	y[i__] = x[i__];
	v[i__] = z__[i__];
/* L10: */
    }
    return 0;
} /* copy2_ */

/* *********************************************************************** */

/*     * SUBROUTINE VNEG * */


/*     * Purpose * */

/*     Change the signs of vector elements. */


/*     * Calling sequence * */

/*     CALL VNEG(N,X,Y) */


/*     * Parameters * */

/*     II  N               Vector dimension. */
/*     RI  X(N)            Input vector. */
/*     RO  Y(N)            Output vector, where Y:= -X. */


/*     Napsu Haarala (2006) */

/* Subroutine */ int vneg_(integer *n, doublereal *x, doublereal *y)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	y[i__] = -x[i__];
/* L10: */
    }
    return 0;
} /* vneg_ */

/* *********************************************************************** */

/*     * SUBROUTINE VXDIAG * */


/*     * Purpose * */

/*     Vector is multiplied by a diagonal matrix. */


/*     * Calling sequence * */

/*     CALL VXDIAG(N,D,X,Y) */


/*     * Parameters * */

/*     II  N               Vector dimension. */
/*     RI  D(N)            Diagonal matrix stored as a vector with N elements. */
/*     RI  X(N)            Input vector. */
/*     RO  Y(N)            Output vector where Y:=D*X. */


/*     Napsu Haarala (2005) */

/* Subroutine */ int vxdiag_(integer *n, doublereal *d__, doublereal *x, 
	doublereal *y)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --y;
    --x;
    --d__;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	y[i__] = x[i__] * d__[i__];
/* L10: */
    }
    return 0;
} /* vxdiag_ */

/* *********************************************************************** */

/*     * SUBROUTINE XDIFFY * */


/*     * Purpose * */

/*     Difference of two vectors. */


/*     * Calling sequence * */

/*     CALL XDIFFY(N,X,Y,Z) */


/*     * Parameters * */

/*     II  N               Vector dimension. */
/*     RI  X(N)            Input vector. */
/*     RI  Y(N)            Input vector. */
/*     RO  Z(N)            Output vector, where Z:= X - Y. */


/*     Napsu Haarala (2006) */

/* Subroutine */ int xdiffy_(integer *n, doublereal *x, doublereal *y, 
	doublereal *z__)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --z__;
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	z__[i__] = x[i__] - y[i__];
/* L10: */
    }
    return 0;
} /* xdiffy_ */

/* *********************************************************************** */

/*     * SUBROUTINE XSUMY * */


/*     * Purpose * */

/*     Sum of two vectors. */


/*     * Calling sequence * */

/*     CALL XSUMY(N,X,Y,Z) */


/*     * Parameters * */

/*     II  N               Vector dimension. */
/*     RI  X(N)            Input vector. */
/*     RI  Y(N)            Input vector. */
/*     RO  Z(N)            Output vector, where Z:= Y + X. */


/*     Napsu Haarala (2006) */

/* Subroutine */ int xsumy_(integer *n, doublereal *x, doublereal *y, 
	doublereal *z__)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --z__;
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	z__[i__] = y[i__] + x[i__];
/* L10: */
    }
    return 0;
} /* xsumy_ */

/* *********************************************************************** */

/*     * DOUBLE PRECISION FUNCTION VDOT * */


/*     * Purpose * */

/*     Dot product of two vectors. */


/*     * Calling sequence * */

/*     XTY = VDOT(N,X,Y) */


/*     * Parameters * */

/*     II  N               Vectors dimension. */
/*     RI  X(N)            Input vector. */
/*     RI  Y(N)            Input vector. */
/*     RO  VDOT            Value of dot product VDOT=trans(X)*Y. */


/*     Napsu Haarala (2006) */


doublereal vdot_(integer *n, doublereal *x, doublereal *y)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Local variables */
    static integer i__;
    static doublereal tmp;

/*     Scalar Arguments */
/*     Array Arguments */
/*     Local Scalars */
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    tmp = 0.;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	tmp += x[i__] * y[i__];
/* L10: */
    }
    ret_val = tmp;
    return ret_val;
} /* vdot_ */

/* *********************************************************************** */

/*     * DOUBLE PRECISION FUNCTION EPS0 * */


/*     * Purpose * */

/*     Computation of the smallest positive number such that */
/*     1.0 + EPS0 > 1.0. */


/*     * Calling sequence * */

/*     SMALL = EPS0() */


/*     Napsu Haarala (2003) */


doublereal eps0_(void)
{
    /* System generated locals */
    doublereal ret_val;

    /* Local variables */
    static doublereal epsnew;

/*     Local scalars */
    ret_val = 1.;
L100:
    epsnew = ret_val / 2.;
    if (epsnew + 1. == 1.) {
	return ret_val;
    }
    ret_val = epsnew;
    goto L100;
} /* eps0_ */

