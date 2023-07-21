#ifndef _FFT
#define _FFT

namespace fattal
{
	
class cmplx {
 public :
  double a, b ;

  cmplx(double pa, double pb) { a = pa; b = pb ; }
  cmplx() { a = b = 0 ; }
  
  double abs2() { return a * a + b * b ; } 
  
  void operator/=(cmplx& B) { 
    double c = B.abs2() ; 
    double ta = (a*B.a+b*B.b)/c;
    double tb = (b*B.a-a*B.b)/c ; a = ta ; b = tb ; 
  }
  
  void operator/=(int i) { a /= i ; b /= i ; }
  
  friend cmplx operator+(cmplx& A, cmplx& B) { return cmplx(A.a+B.a,A.b+B.b) ; }
  friend cmplx operator-(cmplx& A, cmplx& B) { return cmplx(A.a-B.a,A.b-B.b) ; }
  friend cmplx operator*(cmplx& A, cmplx& B) { return cmplx(A.a*B.a-A.b*B.b, A.a*B.b+A.b*B.a) ; }
  friend cmplx operator/(cmplx& A, cmplx& B) { 
    double c = B.abs2() ;
    return cmplx((A.a*B.a+A.b*B.b)/c, (A.b*B.a-A.a*B.b)/c) ; 
  }
} ;


void cfft(cmplx *data, int size, int direction)
{
  int i,j,m,mmax,istep;
  double theta;
  cmplx temp, w;

  /* reorder data by bit reversal of index */

  j=1;
  for(i=1;i<=size;i++)
    {
      if(i<j)
	{
	  temp = data[j-1];
	  data[j-1] = data[i-1];
	  data[i-1] = temp;
	}
      m=size/2;
      while(j>m)
	{
	  j-=m;
	  m=(m+1)/2;
	}
      j+=m;
    }

  /* calculate the sub transforms */

  mmax=1;
  while(size>mmax)
    {
      istep=2*mmax;
      for(m=1;m<=mmax;m++)
	{
	  theta=M_PI*direction*(m-1)/mmax;
	  w=cmplx(cos(theta),sin(theta));
	  for(i=m-1;i<=size-1;i+=istep)
	    {
	      j=i+mmax;
	      temp = data[j]*w;
	      data[j] = data[i] - temp;
	      data[i] = data[i] + temp;
	    }
	}
      mmax=istep;
    }
  for (i=0;i<size;i++) data[i] /= (size/2);
}

int FFT(int dir,int m,double *x,double *y)
{
  long nn,i,i1,j,k,i2,l,l1,l2;
  double c1,c2,tx,ty,t1,t2,u1,u2,z;

  /* Calculate the number of points */
  nn = 1;
  for (i=0;i<m;i++)
    nn *= 2;

  /* Do the bit reversal */
  i2 = nn >> 1;
  j = 0;
  for (i=0;i<nn-1;i++) {
    if (i < j) {
      tx = x[i];
      ty = y[i];
      x[i] = x[j];
      y[i] = y[j];
      x[j] = tx;
      y[j] = ty;
    }
    k = i2;
    while (k <= j) {
      j -= k;
      k >>= 1;
    }
    j += k;
  }

  /* Compute the FFT */
  c1 = -1.0;
  c2 = 0.0;
  l2 = 1;
  for (l=0;l<m;l++) {
    l1 = l2;
    l2 <<= 1;
    u1 = 1.0;
    u2 = 0.0;
    for (j=0;j<l1;j++) {
      for (i=j;i<nn;i+=l2) {
	i1 = i + l1;
	t1 = u1 * x[i1] - u2 * y[i1];
	t2 = u1 * y[i1] + u2 * x[i1];
	x[i1] = x[i] - t1;
	y[i1] = y[i] - t2;
	x[i] += t1;
	y[i] += t2;
      }
      z =  u1 * c1 - u2 * c2;
      u2 = u1 * c2 + u2 * c1;
      u1 = z;
    }
    c2 = sqrt((1.0 - c1) / 2.0);
    if (dir == 1)
      c2 = -c2;
    c1 = sqrt((1.0 + c1) / 2.0);
  }

  /* Scaling for forward transform */
  if (dir == 1) {
    for (i=0;i<nn;i++) {
      x[i] /= (double)nn;
      y[i] /= (double)nn;
    }
  }

  return 1 ;
}


int FFT2D(cmplx **c,int nx,int ny,int dir)
{
  int i,j;
  cmplx *work ;

  /* Transform the rows */
  work = (cmplx *)malloc(nx * sizeof(cmplx));
  if (work == NULL) return 0 ;
  for (j=0;j<ny;j++) {
    for (i=0;i<nx;i++) work[i] = c[i][j]; 
    cfft(work,nx,dir);
    for (i=0;i<nx;i++) c[i][j] = work[i];
  }
  free(work);

  /* Transform the columns */
  work = (cmplx *)malloc(ny * sizeof(cmplx));
  if (work == NULL) return 0 ;
  for (i=0;i<nx;i++) {
    for (j=0;j<ny;j++) work[j] = c[i][j];
    cfft(work,ny,dir);
    for (j=0;j<ny;j++) c[i][j] = work[j];
  }
  free(work);
  return 1 ;
}

}

#endif /* _FFT */
