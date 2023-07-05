void exact(float **A, float *b, int n) {

  double v ;
  for(int i = 1 ; i <= n ; i++) {

    v = A[i][i] ;

    if(fabs(v) < 0.001) {
      cout << "VVVV ERR" << endl ;
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
