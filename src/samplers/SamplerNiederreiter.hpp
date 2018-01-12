#ifndef _UTK_SAMPLER_NIEDERRETEIR_
#define _UTK_SAMPLER_NIEDERRETEIR_

#include <cmath>
#include <iostream>
#include <fstream>
#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"

namespace neider
{

//Code from https://people.sc.fsu.edu/~jburkardt/cpp_src/niederreiter2/niederreiter2.html
//{

//
//  GLOBAL DATA "/FIELD/"
//
//    The following GLOBAL data, used by many functions,
//    gives the order Q of a field, its characteristic P, and its
//    addition, multiplication, and subtraction tables.
//
//    Global, int DEG_MAX, the maximum degree of the polynomials
//    to be considered.
//
//    Global, int P, the characteristic of the field.
//
//    Global, int Q, the order of the field.
//
//    Global, int Q_MAX, the order of the largest field to
//    be handled.
//
//    Global, int ADD[Q_MAX][Q_MAX], the field addition table. 
//
//    Global, int MUL[Q_MAX][Q_MAX], the field multiplication table. 
//
//    Global, int SUB[Q_MAX][Q_MAX], the field subtraction table.
//
const int DEG_MAX = 50;
int P;
int Q;
const int Q_MAX = 50;

int add[Q_MAX][Q_MAX];
int mul[Q_MAX][Q_MAX];
int sub[Q_MAX][Q_MAX];
//
//  GLOBAL DATA "/COMM/"
//
//    Global, int DIM_MAX, the maximum dimension that will 
//    be used.
//
//    Global, int FIG_MAX, the maximum number of base-Q digits 
//    we can handle.  BASE^FIG_MAX - 1 must be representable in C++.
//    For base 2, this implies that FIG_MAX could be as high as 31.
//    In the original version of the program, FIG_MAX was set to 20.
//
//    Global, int C[DIM_MAX,FIG_MAX,0:FIG_MAX-1], the values of 
//    Niederreiter's C(I,J,R).
//
//    Global, int COUNT[0:FIG_MAX-1], the index of the current item 
//    in the sequence, expressed as an array of base-Q digits.  COUNT(R)
//    is the same as Niederreiter's AR(N) (page 54) except that N is implicit.
//
//    Global, int D[DIM_MAX][FIG_MAX].
//
//    Global, int DIMEN, the dimension of the sequence to be generated.
//
//    Global, int NEXTQ[DIM_MAX], the numerators of the next item in 
//    the series.  These are like Niederreiter's XI(N) (page 54) except that
//    N is implicit, and the NEXTQ are integers.  To obtain the values of 
//    XI(N), multiply by RECIP.
//
//    Global, int NFIGS, the number of base Q digits we are using.
//
//    Global, int QPOW[FIG_MAX], to speed things up a bit. 
//    QPOW(I) = Q ** (NFIGS-I).
//
//    Global, double RECIP = 1.0 / Q^NFIGS.
//
const static int DIM_MAX = 50;
//const int FIG_MAX = 20;
const static int FIG_MAX = 31;

static int C[DIM_MAX][FIG_MAX][FIG_MAX];
static int COUNT[FIG_MAX];
static int D[DIM_MAX][FIG_MAX];
static int DIMEN;
static int NEXTQ[DIM_MAX];
static int NFIGS;
static int QPOW[FIG_MAX];
static double RECIP;



static int i4_max ( int i1, int i2 )

//
//  Purpose:
//
//    I4_MAX returns the maximum of two I4's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    13 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I1, I2, are two integers to be compared.
//
//    Output, int I4_MAX, the larger of I1 and I2.
//
{
  int value;

  if ( i2 < i1 )
  {
    value = i1;
  }
  else
  {
    value = i2;
  }
  return value;
}


static int i4_min ( int i1, int i2 )

//
//  Purpose:
//
//    I4_MIN returns the minimum of two I4's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    13 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I1, I2, two integers to be compared.
//
//    Output, int I4_MIN, the smaller of I1 and I2.
//
{
  int value;

  if ( i1 < i2 )
  {
    value = i1;
  }
  else
  {
    value = i2;
  }
  return value;
}


static int *plymul ( int pa[], int pb[] )

//
//  Purpose:
//
//    PLYMUL multiplies one polynomial by another.
//
//  Discussion:
//
//    Polynomial coefficients are elements of the field of order Q.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    06 September 2007
//
//  Author:
//
//    Orginal FORTRAN77 version by Paul Bratley, Bennet Fox, 
//    Harald Niederreiter.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Harald Niederreiter,
//    Algorithm 738: 
//    Programs to Generate Niederreiter's Low-Discrepancy Sequences,
//    ACM Transactions on Mathematical Software,
//    Volume 20, Number 4, 1994, pages 494-495.
//
//  Parameters:
//
//    Input, int PA[DEG_MAX+2], the first polynomial.
//
//    Input, int PB[DEG_MAX+2], the second polynomial.
//
//    Output, int PLYMUL[DEG_MAX+2], the product polynomial.
//
{
  int dega;
  int degb;
  int degc;
  int i;
  int j;
  //int p;
  int *pc;
  int term;

  pc = new int[DEG_MAX+2];
  
  dega = pa[0];
  degb = pb[0];

  if ( dega == -1 || degb == -1 )
  {
    degc = -1;
  }
  else
  {
    degc = dega + degb;
  }

  if ( DEG_MAX < degc )
  {
    std::cerr << "\n";
    std::cerr << "PLYMUL - Fatal error!\n";
    std::cerr << "  The degree of the product exceeds DEG_MAX.\n";
    exit ( 1 );
  }

  for ( i = 0; i <= degc; i++ )
  {
    term = 0;
    for ( j = i4_max ( 0, i - dega ); j <= i4_min ( degb, i ); j++ )
	{
      term = add [ term ] [ mul [ pa[i-j+1] ] [ pb[j+1] ] ];
    }
    pc[i+1] = term;
  }

  pc[0] = degc;

  for ( i = degc + 1; i <= DEG_MAX; i++ )
  {
    pc[i+1] = 0;
  }

  return pc;
}



static void calcv ( int px[], int b[], int v[], int v_max )

//
//  Purpose:
//
//    CALCV calculates the constants V(J,R).
//
//  Discussion:
//
//    This program calculates the values of the constants V(J,R) as
//    described in Bratley, Fox and Niederreiter, section 3.3.  It 
//    is called from either CALCC or CALCC2.  The values transmitted 
//    through common /FIELD/ determine which field we are working in.
//
//    Polynomials stored as arrays have the coefficient of degree n 
//    in POLY(N), and the degree of the polynomial in POLY(-1).  The 
//    parameter DEG is just to remind us of this last fact.  A polynomial 
//    which is identically 0 is given degree -1.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    11 September 2007
//
//  Author:
//
//    Orginal FORTRAN77 version by Paul Bratley, Bennett Fox, 
//    Harald Niederreiter.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Harald Niederreiter,
//    Algorithm 738: 
//    Programs to Generate Niederreiter's Low-Discrepancy Sequences,
//    ACM Transactions on Mathematical Software,
//    Volume 20, Number 4, 1994, pages 494-495.
//
//  Parameters:
//
//    Input, int PX[MAXDEG+2], the appropriate irreducible polynomial 
//    for the dimension currently being considered.  The degree of PX will 
//    be called E.
//
//    Input/output, int B[DEG_MAX+2].  On input, B is the polynomial 
//    defined in section 2.3 of BFN.  The degree of B implicitly defines 
//    the parameter J of section 3.3, by degree(B) = E*(J-1).  On output, 
//    B has been multiplied by PX, so its degree is now E*J.
//
//    Input, int V[V_MAX+1], contains the values required.
//
//    Input, int V_MAX, the dimension of the array V.
//
//  Local Parameters:
//
//    Local, int ARBIT, indicates where the user can place
//    an arbitrary element of the field of order Q.  For the code,
//    this means 0 <= ARBIT < Q.  Within these limits, the user can 
//    do what he likes.  ARBIT could be declared as a function 
//    that returned a different arbitrary value each time it is referenced.
//
//    Local, int BIGM, is the M used in section 3.3.  It differs from 
//    the [little] m used in section 2.3, denoted here by M.
//
//    Local, int NONZER shows where the user must put an arbitrary 
//    non-zero element of the same field.  For the code, this means 
//    0 < NONZER < Q.  Within these limits, the user can do what he likes.  
//
{
  int arbit = 1;
  int *b2;
  int bigm;
  //int e;
  int h[DEG_MAX+2];
  int i;
  //int j;
  int kj;
  int m;
  int nonzer = 1;
  int r;
  int term;

  //e = px[0];
//
//  The polynomial H is PX^(J-1), which is the value of B on arrival.
//
//  In section 3.3, the values of Hi are defined with a minus sign:
//  don't forget this if you use them later//
//
  for ( i = 0; i < DEG_MAX + 2; i++ )
  {
    h[i] = b[i];
  }

  bigm = h[0];
//
//  Now multiply B by PX so B becomes PX^J.
//
//  In section 2.3, the values of Bi are defined with a minus sign:
//  don't forget this if you use them later!
//
  b2 = plymul ( px, b );
  
  for ( i = 0; i < DEG_MAX + 2; i++ )
  {
    b[i] = b2[i];
  }
  
  delete [] b2;

  m = b[0];
//
//  We don't use J explicitly anywhere, but here it is just in case.
//
  //j = m / e;
//
//  Now choose a value of Kj as defined in section 3.3.
//  We must have 0 <= Kj < E*J = M.
//  The limit condition on Kj does not seem very relevant
//  in this program.
//
  kj = bigm;
//
//  Now choose values of V in accordance with the conditions in
//  section 3.3
//
  for ( i = 0; i < kj; i++ )
  {
    v[i] = 0;
  }
  v[kj] = 1;

  if ( kj < bigm )
  {
    term = sub [0] [ h[kj+1] ];

	for ( r = kj + 1; r <= bigm - 1; r++ )
    {
      v[r] = arbit;
//
//  Check the condition of section 3.3,
//  remembering that the H's have the opposite sign.
//
      term = sub [ term ] [ mul [ h[r+1] ] [ v[r] ] ];
    }
//
//  Now V(BIGM) is anything but TERM.
//
    v[bigm] = add [ nonzer] [ term ];
	for ( i = bigm + 1; i <= m - 1; i++ )
	{
      v[i] = arbit;
    }
  }
  else
  {
    for ( i = kj + 1; i <= m - 1; i++ )
	{
      v[i] = arbit;
    }
  }
//
//  Calculate the remaining V's using the recursion of section 2.3,
//  remembering that the B's have the opposite sign.
//
  for ( r = 0; r <= v_max - m; r++ )
  {
    term = 0;
	for ( i = 0; i <= m - 1; i++ )
	{
      term = sub [ term ] [ mul [ b[i+1] ] [ v[r+i] ] ];
    }
    v[r+m] = term;
  }

  return;
}



static void calcc ( )

//
//  Purpose:
//
//    CALCC calculates the value of the constants C(I,J,R).
//
//  Discussion:
//
//    This routine calculates the values of the constants C(I,J,R).
//    As far as possible, we use Niederreiter's notation.
//    We calculate the values of C for each I in turn.
//    When all the values of C have been calculated, we return
//    this array to the calling program.
//
//    Irreducible polynomials are read from file "gfplys.txt"
//    This file should have been created earlier by running the
//    GFPLYS program.
//
//    Polynomials stored as arrays have the coefficient of degree n 
//    in POLY(N), and the degree of the polynomial in POLY(-1).  
//    The parameter DEG is just to remind us of this last fact.  
//    A polynomial which is identically 0 is given degree -1.
//
//    Thanks to Michael Baudin for pointing out that MAXE should
//    be increased from 5 to 7, since one of the irreducible polynomials
//    that must be stored in PX has degree 7, 07 June 2010.
//
//    In fact, the size of MAXE depends on the highest degree polynomial
//    computed by GFPLYS, which in turn depends in part on the 
//    value NPOL in that program.  To allow DIM_MAX = 50, we increased
//    NPOL to 50 in GFPLYS and here, and hence MAXE to 8, JVB, 07 June 2010.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    07 June 2010
//
//  Author:
//
//    Original FORTRAN77 version by Paul Bratley, Bennett Fox, 
//    Harald Niederreiter.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Harald Niederreiter,
//    Algorithm 738: 
//    Programs to Generate NiederreiteSoftware,
//    Volume 20, Number 4, 1994, pages 494-495.
//
//    Harald Niederreiter,
//    Low-discrepancy and low-dispersion sequences,
//    Journal of Number Theory,
//    Volume 30, 1988, pages 51-70.
//
//  Local Parameters:
//
//    Local, int MAXE; we need DIM_MAX irreducible polynomials over GF(Q).
//    MAXE is the highest degree among these.
//
//    Local, int V_MAX, the maximum index used in V.
//
//    Local, int NPOLS, the number of precalculated irreducible polynomials.
//
{
//const int maxe = 5;
  const int maxe = 8;

  const int v_max = FIG_MAX + maxe;

  int b[DEG_MAX+2];
  int e;

  int i;
  int j;
  int k;
//const int npols = 25;
  const int npols = 50;
  int px[maxe+2];
  int r;
  int u;
  int v[v_max+1];
//
//  Read the irreducible polynomials.
//
  std::ifstream input;
  std::string pf(UTK_DATA_PATH);
  input.open ( (pf + "/niederreiter/gfplys.txt").c_str(), std::ifstream::in );
  if(!(input.good()))
  {
	  std::cerr << "Failed to open " << (pf + "/niederreiter/gfplys.txt").c_str() << std::endl;
	  exit(-1);
  }

  for ( ; ; )
  {
    input >> i;

    if ( input.eof ( ) )
    {
      std::cerr << "\n";
      std::cerr << "CALCC - Fatal error!\n"; 
      std::cerr << "  Could not find tables for Q = " << Q << "\n";
      exit ( 1 );
    }

    if ( i == Q )
    {
      break;
    }

    for ( j = 1; j <= npols; j++ )
    {
      input >> e;
      for ( k = 0; k <= e; k++ )
      {
        input >> px[k+1];
	}
    }
  }

  for ( i = 0; i < DIMEN; i++ )
  {
//
//  For each dimension, we need to calculate powers of an
//  appropriate irreducible polynomial.  See Niederreiter
//  page 65, just below equation (19).
//
//  Read the appropriate irreducible polynomial into PX,
//  and its degree into E.  Set polynomial B = PX^0 = 1.
//  M is the degree of B.  Subsequently B will hold higher
//  powers of PX.
//
//  The polynomial PX is stored in 'gfplys.txt' in the format
//
//    n  a0  a1  a2  ... an
//
//  where n is the degree of the polynomial and the ai are
//  its coefficients.
//
    input >> e;
    for ( k = 0; k <= e; k++ )
    {
      input >> px[k+1];
    }

    px[0] = e;
    b[0] = 0;
    b[1] = 1;
//
//  Niederreiter (page 56, after equation (7), defines two variables 
//  Q and U.  We do not need Q explicitly, but we do need U.
//
    u = 0;

    for ( j = 0; j < NFIGS; j++ )
    {
//
//  If U = 0, we need to set B to the next power of PX
//  and recalculate V.  This is done by subroutine CALCV.
//
      if ( u == 0 )
      {
        calcv ( px, b, v, v_max );
      }
//
//  Now C is obtained from V.  Neiderreiter obtains A from V 
//  (page 65, near the bottom), and then gets C from A (page 56,
//  equation (7)).  However this can be done in one step.
//
      for ( r = 0; r < NFIGS; r++ )
      {
        C[i][j][r] = v[r+u];
      }
//
//  Increment U.  If U = E, then U = 0 and in Niederreiter's
//  paper Q = Q + 1.  Here, however, Q is not used explicitly.
//
      u = u + 1;
      if ( u == e )
      {
        u = 0;
      }
    }
  }

  input.close ( );

  return;
}


static void golo ( double quasi[] )

//
//  Purpose:
//
//    GOLO generates a new quasi-random vector on each call.
//
//  Discussion:
//
//    Before the first call to this routine, a call must be made
//    to subroutine INLO to carry out some initializations.
//
//    Polynomials stored as arrays have the coefficient of degree n 
//    in POLY(N), and the degree of the polynomial in POLY(-1).  
//    The parameter DEG is just to remind us of this last fact.  
//    A polynomial which is identically 0 is given degree -1.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    12 September 2007
//
//  Author:
//
//    Orginal FORTRAN77 version by Paul Bratley, Bennett Fox, 
//    Harald Niederreiter.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Harald Niederreiter,
//    Algorithm 738: 
//    Programs to Generate Niederreiter's Low-Discrepancy Sequences,
//    ACM Transactions on Mathematical Software,
//    Volume 20, Number 4, 1994, pages 494-495.
//
//    Harald Niederreiter,
//    Low-discrepancy and low-dispersion sequences,
//    Journal of Number Theory,
//    Volume 30, 1988, pages 51-70.
//
//  Parameters:
//
//    Output, double QUASI[], the next vector in the sequence.
//
{
  int diff;
  int i;
  int j;
  int nq;
  int oldcnt;
  int r;
//
//  Multiply the numerators in NEXTQ by RECIP to get the next
//  quasi-random vector.
//
  for ( i = 0; i < DIMEN; i++ )
  {
    quasi[i] = ( double ) ( NEXTQ[i] ) * RECIP;
  }
//
//  Update COUNT, treated as a base-Q integer.  Instead of
//  recalculating the values of D from scratch, we update
//  them for each digit of COUNT which changes.  In terms of
//  Niederreiter page 54, NEXTQ(I) corresponds to XI(N), with
//  N being implicit, and D(I,J) corresponds to XI(N,J), again
//  with N implicit.  Finally COUNT(R) corresponds to AR(N).
//
  r = 0;

  for ( ; ; )
  {
    if ( NFIGS <= r )
    {
      std::cerr << "\n";
      std::cerr << "GOLO - Fatal error!\n";
      std::cerr << "  Too many calls!\n";
      exit ( 1 );
    }

    oldcnt = COUNT[r];

    if ( COUNT[r] < Q - 1 )
    {
      COUNT[r] = COUNT[r] + 1;
    }
    else
    {
      COUNT[r] = 0;
    }

    diff = sub [ COUNT[r] ] [ oldcnt ];
//
//  Digit R has just changed.  DIFF says how much it changed
//  by.  We use this to update the values of array D.
//
    for ( i = 0; i < DIMEN; i++ )
    {
      for ( j = 0; j < NFIGS; j++ )
      {
        D[i][j] = add [ D[i][j] ] [ mul [ C[i][j][r] ] [ diff ] ];
      }
    }
//
//  If COUNT(R) is now zero, we must propagate the carry.
//
    if ( COUNT[r] != 0 )
    {
      break;
    }

    r = r + 1;
  }
//
//  Now use the updated values of D to calculate NEXTQ.
//  Array QPOW helps to speed things up a little:
//  QPOW(J) is Q^(NFIGS-J).
//
  for ( i = 0; i < DIMEN; i++ )
  {
    nq = 0;
    for ( j = 0; j < NFIGS; j++ )
    {
      nq = nq + D[i][j] * QPOW[j];
    }
    NEXTQ[i] = nq;
  }

  return;
}


static int i4_characteristic ( int q )

//
//  Purpose:
//
//    I4_CHARACTERISTIC gives the characteristic for an integer.
//
//  Discussion:
//
//    For any positive integer Q, the characteristic is:
//
//    Q, if Q is a prime;
//    P, if Q = P^N for some prime P and some integer N;
//    0, otherwise, that is, if Q is negative, 0, 1, or the product
//       of more than one distinct prime.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    06 September 2007
//
//  Author:
//
//    Orginal FORTRAN77 version by Paul Bratley, Bennet Fox, 
//    Harald Niederreiter.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Harald Niederreiter,
//    Algorithm 738: 
//    Programs to Generate Niederreiter's Low-Discrepancy Sequences,
//    ACM Transactions on Mathematical Software,
//    Volume 20, Number 4, 1994, pages 494-495.
//
//  Parameters:
//
//    Input, int Q, the value to be tested.
//
//    Output, int I4_CHARACTERISTIC, the characteristic of Q.
//
{
  int i;
  int i_max;
  int q_copy;
  int value;
  
  if ( q <= 1 )
  {
    value = 0;
    return value;
  }
//
//  If Q is not prime, then there is at least one prime factor
//  of Q no greater than SQRT(Q)+1.
//
//  A faster code would only consider prime values of I,
//  but that entails storing a table of primes and limiting the 
//  size of Q.  Simplicity and flexibility for now.
//
  i_max = ( int ) ( sqrt ( ( double ) ( q ) ) ) + 1;
  q_copy = q;

  for ( i = 2; i <= i_max; i++ )
  {
    if ( ( q_copy % i ) == 0 )
    {
      while ( ( q_copy % i ) == 0 )
	  {
        q_copy = q_copy / i;
      }

      if ( q_copy == 1 )
	  {
        value = i;
	  }
      else
	  {
        value = 0;
      }
      return value;
    }
  }
//
//  If no factor was found, then Q is prime.
//
  value = q;

  return value;
}





static int i4_power ( int i, int j )


//
//  Purpose:
//
//    I4_POWER returns the value of I^J.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    01 April 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I, J, the base and the power.  J should be nonnegative.
//
//    Output, int I4_POWER, the value of I^J.
//
{
  int k;
  int value;

  if ( j < 0 )
  {
    if ( i == 1 )
    {
      value = 1;
    }
    else if ( i == 0 )
    {
      std::cerr << "\n";
      std::cerr << "I4_POWER - Fatal error!\n";
      std::cerr << "  I^J requested, with I = 0 and J negative.\n";
      exit ( 1 );
    }
    else
    {
      value = 0;
    }
  }
  else if ( j == 0 )
  {
    if ( i == 0 )
    {
      std::cerr << "\n";
      std::cerr << "I4_POWER - Fatal error!\n";
      std::cerr << "  I^J requested, with I = 0 and J = 0.\n";
      exit ( 1 );
    }
    else
    {
      value = 1;
    }
  }
  else if ( j == 1 )
  {
    value = i;
  }
  else
  {
    value = 1;
    for ( k = 1; k <= j; k++ )
    {
      value = value * i;
    }
  }
  return value;
}


static void setfld ( int q_init )
//
//  Purpose: 
//
//    SETFLD sets up the arithmetic tables for a finite field.
//
//  Discussion:
//
//    This subroutine sets up addition, multiplication, and
//    subtraction tables for the finite field of order QIN.
//
//    A polynomial with coefficients A(*) in the field of order Q
//    can also be stored in an integer I, with
//
//      I = AN*Q^N + ... + A0.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    06 September 2007
//
//  Author:
//
//    Orginal FORTRAN77 version by Paul Bratley, Bennet Fox, 
//    Harald Niederreiter.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Harald Niederreiter,
//    Algorithm 738: 
//    Programs to Generate Niederreiter's Low-Discrepancy Sequences,
//    ACM Transactions on Mathematical Software,
//    Volume 20, Number 4, 1994, pages 494-495.
//
//  Parameters:
//
//    Input, int Q_INIT, the order of the field.
//
{
  int i;
  std::ifstream input;
  std::string pf(UTK_DATA_PATH);
  //const char *input_filename = (pf + "/neiderreiter/gfarit.txt").c_str();
  
  int j;
  //int n;

  if ( q_init <= 1 || Q_MAX < q_init )
  {
    std::cerr << "\n";
    std::cerr << "SETFLD - Fatal error!\n";
    std::cerr << "  Bad value of Q = " << q_init << "\n";
    exit ( 1 );
  }

  Q = q_init;
  P = i4_characteristic ( Q );

  if ( P == 0 )
  {
    std::cerr << "\n";
    std::cerr << "SETFLD - Fatal error!\n";
    std::cerr << "  There is no field of order Q = " << Q << "\n";
    exit ( 1 );
  }
//
//  Set up to handle a field of prime or prime-power order.
//  Calculate the addition and multiplication tables.
//
  for ( i = 0; i < Q; i++ )
  {
    for ( j = 0; j < Q; j++ )
    {
      add[i][j] = ( i + j ) % P;
    }
  }

  for ( i = 0; i < Q; i++ )
  {
    for ( j = 0; j < Q; j++ )
    {
      mul[i][j] = ( i * j ) % P;
    }
  }
//
//  Use the addition table to set the subtraction table.
//
  for ( i = 0; i < Q; i++ )
  {
    for ( j = 0; j < Q; j++ )
    {
      sub [ add[i][j] ] [i] = j;
    }
  }
  return;
}


static void inlo ( int dim, int base, int skip )
//
//  Purpose:
//
//    INLO calculates the values of C(I,J,R).
//
//  Discussion:
//
//    This subroutine calculates the values of Niederreiter's
//    C(I,J,R) and performs other initialization necessary
//    before calling GOLO.
//
//    Polynomials stored as arrays have the coefficient of degree n 
//    in POLY(N), and the degree of the polynomial in POLY(-1).  
//    The parameter DEG is just to remind us of this last fact.  
//    A polynomial which is identically 0 is given degree -1.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    12 September 2007
//
//  Author:
//
//    Orginal FORTRAN77 version by Paul Bratley, Bennett Fox, 
//    Harald Niederreiter.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Harald Niederreiter,
//    Algorithm 738: 
//    Programs to Generate Niederreiter's Low-Discrepancy Sequences,
//    ACM Transactions on Mathematical Software,
//    Volume 20, Number 4, 1994, pages 494-495.
//
//    Harald Niederreiter,
//    Low-discrepancy and low-dispersion sequences,
//    Journal of Number Theory,
//    Volume 30, 1988, pages 51-70.
//
//  Parameters:
//
//    Input, int DIM, the dimension of the sequence to be generated.
//    The value of DIM is copied into DIMEN in the common block.
//
//    Input, int BASE, the prime or prime-power base to be used.
//
//    Input, int SKIP, the number of values to throw away at the 
//    beginning of the sequence.
//
//  Local Parameters:
//
//    Local, int NBITS, the number of bits in a fixed-point integer, not
//    counting the sign.
//
{
  int i;
  int j;
  int nbits = 31;
  int nq;
  int r;
  double temp;

  DIMEN = dim;

  if ( DIMEN <= 0 || DIM_MAX < DIMEN )
  {
    std::cerr << "\n";
    std::cerr << "INLO - Fatal error!\n";
    std::cerr << "  Bad spatial dimension.\n";
    exit ( 1 );
  }

  if ( i4_characteristic ( base ) == 0 )
  {
    std::cerr << "\n";
    std::cerr << "INLO - Fatal error!\n";
    std::cerr << "  Base not prime power or out of range.\n";
    exit ( 1 );
  }

  setfld ( base );
//
//  Calculate how many figures to use in base Q = BASE
//
  temp = log ( pow ( 2.0, nbits ) - 1.0 ) / log ( ( double ) ( Q ) );

  NFIGS = i4_min ( FIG_MAX, ( int ) temp );
//
//  Calculate the C array.
//
  calcc ( );
//
//  Set RECIP.
//
  RECIP = 1.0 / ( double ) ( i4_power ( Q, NFIGS ) );
//
//  Set QPOW(I) = Q^(NFIGS-I).
//
  QPOW[NFIGS-1] = 1;
  for ( i = NFIGS - 1; 1 <= i; i-- )
  {
    QPOW[i-1] = Q * QPOW[i];
  }
//
//  Initialize COUNT.
//
  i = skip;

  for ( r = 0; r < NFIGS; r++ )
  {
    COUNT[r] = ( i % Q );
    i = i / Q;
  }

  if ( i != 0 )
  {
    std::cerr << "\n";
    std::cerr << "INLO - Fatal error!\n";
    std::cerr << "  SKIP is too long!\n";
    exit ( 1 );
  }
//
//  Initialize D.
//
  for ( i = 0; i < DIMEN; i++ )
  {
    for ( j = 0; j < NFIGS; j++ )
    {
      D[i][j] = 0;
    }
  }

  for ( r = 0; r < NFIGS; r++ )
  {
    if ( COUNT[r] != 0 )
    {
      for ( i = 0; i < DIMEN; i++ )
      {
        for ( j = 0; j < NFIGS; j++ )
        {
          D[i][j] = add [ D[i][j] ] [ mul [ C[i][j][r] ] [ COUNT[r] ] ];
        }
      }
    }
  }
//
//  Initialize NEXTQ.
//
  for ( i = 0; i < DIMEN; i++ )
  {
    nq = 0;
    for ( j = 0; j < NFIGS; j++ )
    {
      nq = nq + D[i][j] * QPOW[j];
    }
    NEXTQ[i] = nq;
  }

  return;
}


static void niederreiter ( int dim_num, int base, int *seed, double r[] )


//
//  Purpose:
//
//    NIEDERREITER returns an element of a Niederreiter sequence for base BASE.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    11 September 2007
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int DIM_NUM, the spatial dimension.
//
//    Input, int BASE, the base to use for the Niederreiter sequence.
//    The base should be a prime, or a power of a prime.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double R[DIM_NUM], the element of the sequence.
//
{
  static int dim_num_save = -1;
  int skip;

  if ( dim_num_save < 1 || dim_num != dim_num_save || *seed <= 0 )
  {
    skip = 1;

    inlo ( dim_num, base, skip );

    dim_num_save = dim_num;
  }

  golo ( r );

  *seed = *seed + 1;

  return;
}

} //end namespace neiderreiter

//}
//Code from https://people.sc.fsu.edu/~jburkardt/cpp_src/niederreiter2/niederreiter2.html

namespace utk
{

class SamplerNiederreiter
{
protected:

public:

	SamplerNiederreiter() { m_basis=3; }
	
	void setBasis(uint arg_basis) { m_basis = arg_basis; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerNiederreiter::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		//load domain & toricity
		if(isFloatingType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
		}
		else
		{
			uint power = ceil(log(arg_points)/log(m_basis));
			
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = pow(m_basis, power);
			}
		}
		
		arg_pts.toricity = 1;

		arg_pts.resize(arg_points);
		
		int seed = 0;
        for ( int j = 0; j < arg_points; j++ )
        {
          double pos[D] = {0};
          neider::niederreiter ( D, m_basis, &seed, pos );
		  
		  if(isFloatingType<T>())
		  {
			Point<D, T> pt;
			for(int d=0; d<D; d++)
				pt.pos()[d] = pos[d];
			arg_pts[j] = pt;
		  }
		  else
		  {
			uint power = ceil(log(arg_points)/log(m_basis));
			
			Point<D, T> pt;
			for(int d=0; d<D; d++)
				pt.pos()[d] = pos[d]*pow(m_basis, power);
			arg_pts[j] = pt;
		  }
        }
		return true;
	};

protected:
    int m_basis;
};

} //end namespace utk


#endif
