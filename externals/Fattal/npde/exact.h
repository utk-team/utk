#include <iostream>
#include <math.h>
#include <algorithm>
#define NRANSI
#define TINY 1.0e-20;

using namespace std;

void lubksb(float **a, int n, int *indx, float b[])
{
  int i,ii=0,ip,j;
  float sum;

  for (i=1;i<=n;i++) {
    ip=indx[i];
    sum=b[ip];
    b[ip]=b[i];
    if (ii)
      for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
    else if (sum) ii=i;
    b[i]=sum;
  }
  for (i=n;i>=1;i--) {
    sum=b[i];
    for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j];
    b[i]=sum/a[i][i];
  }
}



void ludcmp(float **a, int n, int *indx, float *d)
{
  int i,imax,j,k;
  float big,dum,sum,temp;
  float *vv = new float[n+1] ;

  *d=1.0;
  for (i=1;i<=n;i++) {
    big=0.0;
    for (j=1;j<=n;j++)
      if ((temp=fabs(a[i][j])) > big) big=temp;
    //if (big == 0.0) cout << "Singular matrix in routine ludcmp" << endl ;
    vv[i]=1.0/big;
  }
  for (j=1;j<=n;j++) {
    for (i=1;i<j;i++) {
      sum=a[i][j];
      for (k=1;k<i;k++) sum -= a[i][k]*a[k][j];
      a[i][j]=sum;
    }
    big=0.0;
    for (i=j;i<=n;i++) {
      sum=a[i][j];
      for (k=1;k<j;k++)
	sum -= a[i][k]*a[k][j];
      a[i][j]=sum;
      if ( (dum=vv[i]*fabs(sum)) >= big) {
	big=dum;
	imax=i;
      }
    }
    if (j != imax) {
      for (k=1;k<=n;k++) {
	dum=a[imax][k];
	a[imax][k]=a[j][k];
	a[j][k]=dum;
      }

      *d = -(*d);
      vv[imax]=vv[j];
    }
    indx[j]=imax;
    if (a[j][j] == 0.0) a[j][j]=TINY;
    if (j != n) {
      dum=1.0/(a[j][j]);
      for (i=j+1;i<=n;i++) a[i][j] *= dum;
    }
  }

  delete[] vv ;
}

void exact(float **A, float *b, int n) {
  //cout << endl << endl ;
  double v ;
  for(int i = 1 ; i <= n ; i++) {
    /*
    for(int g = 1 ; g <= n ; g++) {
      for(int f = 1 ; f <= n ; f++)
	cout << A[g][f] << " " ;
      cout << endl ;
    }
    */
    v = A[i][i] ;

    if(fabs(v) < 0.001) {
      //cout << "VVVV ERR" << endl ;
      while(1) ;
    }
    
    for(int k = 1 ; k <= n ; k++)
      A[i][k] /= v ;
    
    b[i] /= v ;
    
    for(int j = 1 ; j <= n ; j++) {

      v = A[j][i] ;

      if(j==i || fabs(v) < 0.001)
	continue ;
                  
      for(int k = 1 ; k <= n ; k++)
	A[j][k] -= A[i][k] * v ;

      b[j] -= b[i] * v ;
    }
  }
}
