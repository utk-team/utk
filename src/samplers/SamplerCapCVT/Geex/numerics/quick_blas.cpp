/*
 *  _____ _____ ________  _
 * /  __//  __//  __/\  \//
 * | |  _|  \  |  \   \  / 
 * | |_//|  /_ |  /_  /  \ 
 * \____\\____\\____\/__/\\
 *
 * Graphics Environment for EXperimentations.
 *  Copyright (C) 2006 INRIA - Project ALICE
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: 
 *
 *     ALICE Project - INRIA
 *     INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */
 
 
 
 
 
 
 

#include <Geex/numerics/quick_blas.h>
#include <Geex/basics/assert.h>
#include <iostream>

#ifdef WIN32
#define SSE_ALIGNED(x) __declspec(align(16)) x
#else
#define SSE_ALIGNED(x) x __attribute__ ((aligned (16)))
#endif

// Important node:
// Prefetch does not fault on invalid memory,
// so it is safe to prefetch past the end of arrays.
// http://www.cs.ccu.edu.tw/~pschen/course/software_optimization_2006spring/performance_issue_memory.pdf


#ifdef GX_SSE
#include <emmintrin.h>
#endif

namespace Geex {

    namespace QuickBLAS {

        static bool SSE2_supported = false ;

#ifdef GX_SSE
#ifdef GX_64
       static unsigned int cpu_flags() {
	  return 0x04000000 ;
       }
#else      
        static unsigned int cpu_flags() {
            unsigned int r ;
#ifdef WIN32
            _asm                                                         
                {                                                                                                                                                                          
                    mov     eax, 1                                                      
                        cpuid                                                               
                        mov     r,edx
                        }
#else
/* Note: we need to push and pop ebx, since it is used */
/* by PIC code (DLLs) */
            asm (
                "           push %%ebx                                                  \n\t"
                "           movl     $1,%%eax                                           \n\t" 
                "           cpuid                                                               \n\t" 
                "           mov     %%edx,%0                                           \n\t" 
                "           pop     %%ebx                                                \n\t"
                : "=r" (r) : : "eax","ecx","edx","cc" ) ;
#endif        
            return r ;
        }
#endif
#endif
       
        void initialize() {
#ifdef GX_SSE
            SSE2_supported = ((cpu_flags() & 0x04000000) != 0 ) ;
#else
            std::cerr << "compiled without SSE2 support" << std::endl ;
#endif
            std::cerr << "QuickBLAS SSE2: " << SSE2_supported << std::endl ;
        }

        Numeric::uint64 flops_counter = 0 ;

//______________________________________________________________________________________________________

        bool needs_alignment() {
            return SSE2_supported ;
        }

        unsigned int aligned_size(unsigned int N, unsigned int item_size) {
            if(!needs_alignment()) { return N ;  }
            unsigned int factor = 32 / item_size ;
            unsigned int result = N / factor ;
            if((N % factor) != 0) {
                result++ ;
            }
            result *= factor ;
            return result ;
        }

        void dzero(unsigned int N, double* y) {
#ifdef GX_SSE
            if(SSE2_supported) {
                __m128d Z = _mm_setzero_pd() ;
                for(unsigned int i=0; i<N; i+=4) {
                    _mm_stream_pd(&y[i], Z) ;
                    _mm_stream_pd(&y[i + 2], Z) ;
                }
                _mm_sfence() ;
                return ;
            }
#endif
            memset(y, 0, N*sizeof(double)) ;
        }
        
        void  daxpy(unsigned int N, double a, const double* x, double* y) {
            flops_counter += (2*N) ;
#ifdef GX_SSE
            if(SSE2_supported) {
                __m128d Y1, Y2, X1, X2, AA ;
                SSE_ALIGNED(double temp[2]) ;
                temp[0] = a ; temp[1] = a ;
                AA = _mm_load_pd(temp) ;
                unsigned int i = 0 ;
                while(i<N) {
                    _mm_prefetch((const char*)(&x[i] + 128), _MM_HINT_NTA) ;
                    _mm_prefetch((const char*)(&y[i] + 128), _MM_HINT_NTA) ;
                    X1 = _mm_load_pd(&x[i]) ;
                    Y1 = _mm_load_pd(&y[i]) ;
                    Y1 = _mm_add_pd(Y1, _mm_mul_pd(X1, AA)) ;
                    i += 2 ;
                    X2 = _mm_load_pd(&x[i]) ;
                    Y2 = _mm_load_pd(&y[i]) ;
                    Y2 = _mm_add_pd(Y2, _mm_mul_pd(X2, AA)) ;
                    i += 2 ;
                    _mm_stream_pd(&y[i - 4], Y1) ;
                    _mm_stream_pd(&y[i - 2], Y2) ;
                }
                _mm_sfence() ;
                return ;
            }
#endif
            for(unsigned int i=0; i<N; i++) {
                y[i] += a * x[i] ;
            }
        }
        
        void dcopy(unsigned int N, const double* x, double* y) {
#ifdef GX_SSE
            if(SSE2_supported) {
                __m128d X1,X2 ;
                unsigned int i = 0 ;
                while(i<N) {
                    _mm_prefetch((const char*)(&y[i] + 128), _MM_HINT_NTA) ;
                    X1 = _mm_load_pd(&x[i]) ;
                    i+=2 ;
                    X2 = _mm_load_pd(&x[i]) ;
                    i+=2 ;
                    _mm_stream_pd(&y[i - 4], X1) ;
                    _mm_stream_pd(&y[i - 2], X2) ;
                }
                _mm_sfence() ;
                return ;
            }
#endif
            memcpy(y, x, N * sizeof(double)) ;
        }
        
        double  ddot(unsigned int N, const double* x, const double* y)  {
            flops_counter += (2*N) ;
#ifdef GX_SSE
            if(SSE2_supported) {
                __m128d X1, Y1, X2, Y2 ;
                __m128d acc1 = _mm_setzero_pd() ;
                __m128d acc2 = _mm_setzero_pd() ;
                SSE_ALIGNED(double temp[2]) ;
                unsigned int i = 0 ;
                while(i<N) {
                    _mm_prefetch((const char*)(&x[i] + 128), _MM_HINT_NTA) ;
                    _mm_prefetch((const char*)(&y[i] + 128), _MM_HINT_NTA) ;
                    X1 = _mm_load_pd(&x[i]) ;
                    Y1 = _mm_load_pd(&y[i]) ;
                    acc1 = _mm_add_pd(acc1, _mm_mul_pd(X1,Y1)) ;
                    i += 2 ;
                    X2 = _mm_load_pd(&x[i]) ;
                    Y2 = _mm_load_pd(&y[i]) ;
                    acc2 = _mm_add_pd(acc2, _mm_mul_pd(X2,Y2)) ;
                    i += 2 ;
                }
                acc1 = _mm_add_pd(acc1, acc2) ;
                _mm_store_pd(temp, acc1)  ;
                return temp[0] + temp[1] ;
            }
#endif
            double result = 0.0 ;
            for(unsigned int i=0; i<N; i++) {
                result += x[i]*y[i] ;
            }
            return result ;
        }
        
        double  dsquared_nrm2(unsigned int N, const double *x) {
            flops_counter += (2*N) ;
#ifdef GX_SSE
            if(SSE2_supported) {
                __m128d X1, X2;
                __m128d acc1 = _mm_setzero_pd() ;
                __m128d acc2 = _mm_setzero_pd() ;
                SSE_ALIGNED(double temp[2]) ;
                unsigned int i = 0 ;
                while(i<N) {
                    _mm_prefetch((const char*)(&x[i] + 128), _MM_HINT_NTA) ;
                    X1 = _mm_load_pd(&x[i]) ;
                    acc1 = _mm_add_pd(acc1, _mm_mul_pd(X1,X1)) ;
                    i += 2 ;
                    X2 = _mm_load_pd(&x[i]) ;
                    acc2 = _mm_add_pd(acc2, _mm_mul_pd(X2,X2)) ;
                    i += 2 ;
                }
                acc1 = _mm_add_pd(acc1, acc2) ;
                _mm_store_pd(temp, acc1)  ;
                return temp[0] + temp[1] ;
            }
#endif
            double result = 0.0 ;
            for(unsigned int i=0; i<N; i++) {
                result += x[i]*x[i] ;
            }
            return result ;
        }
        
        void dscal(unsigned int N, double a, double* y) {
            flops_counter += N ;
#ifdef GX_SSE
            if(SSE2_supported) {
                __m128d Y1, Y2, AA ;
                SSE_ALIGNED(double temp[2]) ;
                temp[0] = a ; temp[1] = a ;
                AA = _mm_load_pd(temp) ;
                unsigned int i = 0 ;
                while(i<N) {
                    _mm_prefetch((const char*)(&y[i] + 128), _MM_HINT_NTA) ;
                    Y1 = _mm_load_pd(&y[i]) ;
                    Y1 = _mm_mul_pd(Y1, AA) ;
                    i += 2 ;
                    Y2 = _mm_load_pd(&y[i]) ;
                    Y2 = _mm_mul_pd(Y2, AA) ;
                    i += 2 ;
                    _mm_stream_pd(&y[i - 4], Y1) ;
                    _mm_stream_pd(&y[i - 2], Y2) ;
                }
                _mm_sfence() ;
                return ; 
            }
#endif
            for(unsigned int i=0; i<N; i++) {
                y[i] *= a ;
            }
        }
        
        
        void dmul(unsigned int N, const double* a, const double* b, double* y) {
            flops_counter += N ;
#ifdef GX_SSE
            if(SSE2_supported) {
                __m128d Y1, Y2, A1, A2, B1, B2 ;
                unsigned int i = 0 ;
                while(i<N) {
                    _mm_prefetch((const char*)(&a[i] + 256), _MM_HINT_NTA) ;
                    _mm_prefetch((const char*)(&b[i] + 256), _MM_HINT_NTA) ;
                    A1 = _mm_load_pd(&a[i]) ;
                    B1 = _mm_load_pd(&b[i]) ;
                    Y1 = _mm_mul_pd(A1,B1) ;
                    i += 2 ;
                    A2 = _mm_load_pd(&a[i]) ;
                    B2 = _mm_load_pd(&b[i]) ;
                    Y2 = _mm_mul_pd(A2,B2) ;
                    i += 2 ;
                    _mm_stream_pd(&y[i - 4], Y1) ;
                    _mm_stream_pd(&y[i - 2], Y2) ;
                }
                _mm_sfence() ;
                return ;
            }
#endif
            for(unsigned int i=0; i<N; i++) {
                y[i] = a[i] * b[i] ;
            }
        }

//______________________________________________________________________________________________________

        /** y <- a*x+y */
        void saxpy(unsigned int N, float a, const float* x, float* y) {
            flops_counter += (2*N) ;
            for(unsigned int i=0; i<N; i++) {
                y[i] += a * x[i] ;
            }
        }
        
        /** y <- x */
        void scopy(unsigned int N, const float* x, float* y) {
            Memory::copy(y, x, N * sizeof(float)) ;
        }

        /** returns x^T*y */
        float  sdot(unsigned int N, const float* x, const float* y)  {
            flops_counter += (2*N) ;
            float result = 0.0 ;
            for(unsigned int i=0; i<N; i++) {
                result += x[i] * y[i] ;
            }
            return result ;
        }
        
        /** returns |x|^2 */
        float  ssquared_nrm2(unsigned int N, const float *x) {
            flops_counter += (2*N) ;
            float result = 0.0 ;
            for(unsigned int i=0; i<N; i++) {
                result += x[i] * x[i] ;
            }
            return result ;
        }
        
        /** x <- alpha * x */
        void sscal(unsigned int N, float alpha, float* x) {
            flops_counter += N ;
            for(unsigned int i=0; i<N; i++) {
                x[i] *= alpha ;
            }
        }
    
        /** y <- 0 */
        void szero(unsigned int N, float* y) {
            Memory::clear(y, N * sizeof(float)) ;
        }

        void smul(unsigned int N, const float* a, const float* b, float* y) {
            flops_counter += N ;
            for(unsigned int i=0; i<N; i++) {
                y[i] = a[i] * b[i] ;
            }
        }

//______________________________________________________________________________________________________

    }


}
