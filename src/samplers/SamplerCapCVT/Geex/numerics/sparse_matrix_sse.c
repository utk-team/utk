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
 
#include <Geex/numerics/sparse_matrix_sse.h>
#include <stdio.h>

/*
* Notes:
* This is difficult to optimize sparse-matrix X vector products with SSE,
* due to irregular access patterns. Blocking has a great influence on 
* performances (and can be well written in SSE2). Streaming cache-control
* is more difficult (I only managed to optimize accesses to a[] with _mm_prefetch(),
* this results in a 25% performance gain).
*
* Optimizations that did not work (do not try that again...)
* - premultiplying colind by BN 
* - replacing multiplies by powers of two with shifts
* - in BCRS 2x2: unrolling main loop twice for grouping mm_stream_pd calls
* - in BCRS 4x2: mm_store_pd: 40s mm_stream_pd: 43s
* - in BCRS 2x2: tryed to gather x in a vector with same access pattern as a, it
*    did result in half performances ...
* Conclusions:
* * Seems that mm_stream_pd can only be efficient in *streaming-type* functions
*   (no big surprise there !!)
* * The access parttern to the coefficients vector a[] still allows efficient prefetching.
*
*  The double 4x2 version performs up to 30 MFlops even for systems that 
*   do not fit in the L2 cache (but the filling ratio is not always optimum).
*  For systems that fit in the cache, this reaches 0.5 GFlops.
*  The single-precision version reaches 1 GFlop (but requires much more iterations
*  to converge).
*
* Links: http://www.aceshardware.com/read.jsp?id=20000190
*            http://www.joryanick.com/memcpySGI.htm
* Prefetch does not fault on invalid memory,
* so it is safe to prefetch past the end of arrays.
* http://www.cs.ccu.edu.tw/~pschen/course/software_optimization_2006spring/performance_issue_memory.pdf
* 
*/

#ifdef GX_SSE
#include <emmintrin.h>
#ifdef GX_SSE3
#include <pmmintrin.h>
#endif
#endif

/* #ifdef WIN32 */
#define __v4sf __m128
/* #endif */

static int SSE2_supported = 0 ;

#ifdef GX_SSE
#ifdef GX_64
       static unsigned int cpu_flags() {
	  return 0x04000000 ;
       }
#else
static unsigned long cpu_flags() {
    unsigned long r ;
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

void sparse_matrix_SSE2_initialize() {
#ifdef GX_SSE
    SSE2_supported = ((cpu_flags() & 0x04000000) != 0 ) ;
#endif
    fprintf(stderr, "sparse matrix SSE2: %d\n", SSE2_supported) ;
}

/*
 * transpose_add_pd([a1,b1], [a2,b2]) = [a1+b1, a2+b2]
 * Note1: could be simpler with SSE3 (but my PentiumM does not have SSE3)
 * Anyway, since everything is done in the SSE registers, and since memory
 * access is the bottleneck (rather than SSE operations), I bet it would not
 * change performances.
 * Note2: tryed on a core2 duo, this adds 10-15% performances
 */

#ifdef GX_SSE3
#define transpose_add_pd(x1,x2) _mm_hadd_pd(x1,x2)
#else
#define transpose_add_pd(x1,x2) _mm_add_pd(             \
        _mm_shuffle_pd (x1, x2, _MM_SHUFFLE2 (0,0)),     \
        _mm_shuffle_pd (x1, x2, _MM_SHUFFLE2 (1,1))      \
    ) 
#endif

/*________________________________________________________________________*/

#ifdef BM
#undef BM
#endif
#ifdef BN
#undef BN
#endif
#ifdef BLOC_SIZE
#undef BLOC_SIZE
#endif

#define BM 2
#define BN 2
#define BLOC_SIZE 4

int sparse_matrix_bcrs_double_2_2_mult_SSE2(
    unsigned int M,
    const double* a, const unsigned int* colind, const unsigned int* rowptr,
    const double* x, double* y
) {
#ifdef GX_SSE
    unsigned int y_base = 0 ;
    unsigned int I,JJ ;
    if(!SSE2_supported) { return 0 ; }
    for(I=0; I<M; I++) {
        __m128d sum1 = _mm_setzero_pd() ;
        __m128d sum2 = sum1 ;

        for(JJ=rowptr[I]; JJ<rowptr[I+1]; JJ++) {
            unsigned int J = colind[JJ] ;
            unsigned int a_base = JJ * BLOC_SIZE ;
            unsigned int x_base = J * BN ;
            {
                __m128d x_01 = _mm_load_pd(&x[x_base]) ;
                {
                    __m128d a_00_01 = _mm_load_pd(&a[a_base]) ;
                    sum1 = _mm_add_pd(sum1, _mm_mul_pd(a_00_01, x_01)) ;
                }
/*                _mm_prefetch((const char*)(&a[a_base + 56]), _MM_HINT_NTA) ;  */
                {
                    __m128d a_10_11 = _mm_load_pd(&a[a_base + 2]) ;
                    sum2 = _mm_add_pd(sum2, _mm_mul_pd(a_10_11, x_01)) ;
                }
            }
        }


        /*
         * Note: tryed mm_stream_pd() but was 25% slower
         * tryed also to unroll loop twice to pair mm_stream_pd() calls,
         * but was also 25% slower
         */
        _mm_store_pd(&y[y_base], transpose_add_pd(sum1, sum2)) ;
        y_base += 2 ;
    }
    return 1 ;
#else
    return 0 ;
#endif
}

/*________________________________________________________________________*/

#ifdef BM
#undef BM
#endif
#ifdef BN
#undef BN
#endif
#ifdef BLOC_SIZE
#undef BLOC_SIZE
#endif

#define BM 4
#define BN 2
#define BLOC_SIZE 8

int sparse_matrix_bcrs_double_4_2_mult_SSE2(
    unsigned int M,
    const double* a, const unsigned int* colind, const unsigned int* rowptr,
    const double* x, double* y
) {
#ifdef GX_SSE
    unsigned int I,JJ ;
    unsigned int y_base = 0 ;
    if(!SSE2_supported) { return 0 ; }
    for(I=0; I<M; I++) {
        __m128d sum1 = _mm_setzero_pd() ;
        __m128d sum2 = sum1 ;
        __m128d sum3 = sum1 ;
        __m128d sum4 = sum1 ;
        for(JJ=rowptr[I]; JJ<rowptr[I+1]; JJ++) {
            unsigned int J = colind[JJ] ;
            unsigned int a_base = JJ * BLOC_SIZE ;
            unsigned int x_base = J * BN ;

            /* Note: x's access pattern is not regular enough for doing prefetch */
/*
            _mm_prefetch((const char*)(&a[a_base]) + 4096, _MM_HINT_NTA) ;
            _mm_prefetch((const char*)(&a[a_base]) + 4096 + 32, _MM_HINT_NTA) ;
            _mm_prefetch((const char*)(&colind[JJ]) + 4096, _MM_HINT_NTA) ;            
            _mm_prefetch((const char*)(&colind[JJ]) + 4096 + 32, _MM_HINT_NTA) ;            
*/

            {
                __m128d x_01 = _mm_load_pd(&x[x_base]) ;

                {
                    __m128d a_00_01 = _mm_load_pd(&a[a_base]) ;
                    sum1 = _mm_add_pd(sum1, _mm_mul_pd(a_00_01, x_01)) ;
                }
                {
                    __m128d a_10_11 = _mm_load_pd(&a[a_base + 2]) ;
                    sum2 = _mm_add_pd(sum2, _mm_mul_pd(a_10_11, x_01)) ;
                }
                {
                    __m128d a_20_21 = _mm_load_pd(&a[a_base + 4]) ;
                    sum3 = _mm_add_pd(sum3, _mm_mul_pd(a_20_21, x_01)) ;
                }
                {
                    __m128d a_30_31 = _mm_load_pd(&a[a_base + 6]) ;
                    sum4 = _mm_add_pd(sum4, _mm_mul_pd(a_30_31, x_01)) ;
                }
            }
        }

        _mm_store_pd(&y[y_base], transpose_add_pd(sum1, sum2)) ;
        _mm_store_pd(&y[y_base+2], transpose_add_pd(sum3, sum4)) ;
        y_base += 4 ;
    }
    return 1 ;
#else
    return 0 ;
#endif
}

/*________________________________________________________________________*/

#ifdef BM
#undef BM
#endif
#ifdef BN
#undef BN
#endif
#ifdef BLOC_SIZE
#undef BLOC_SIZE
#endif

#define BM 4
#define BN 4
#define BLOC_SIZE 16

int sparse_matrix_bcrs_double_4_4_mult_SSE2(
    unsigned int M,
    const double* a, const unsigned int* colind, const unsigned int* rowptr,
    const double* x, double* y
) {
#ifdef GX_SSE
    unsigned int I,JJ ;
    unsigned int y_base = 0 ;
    if(!SSE2_supported) { return 0 ; }
    for(I=0; I<M; I++) {
        __m128d sum1 = _mm_setzero_pd() ;
        __m128d sum2 = sum1 ;
        __m128d sum3 = sum1 ;
        __m128d sum4 = sum1 ;
        for(JJ=rowptr[I]; JJ<rowptr[I+1]; JJ++) {
            unsigned int J = colind[JJ] ;
            unsigned int a_base = JJ * BLOC_SIZE ;
            unsigned int x_base = J * BN ;
            
            /* Note: x's access pattern is not regular enough for doing prefetch */
/*
            _mm_prefetch((const char*)(&a[a_base]) + 4096, _MM_HINT_NTA) ;
            _mm_prefetch((const char*)(&a[a_base]) + 4096 + 32, _MM_HINT_NTA) ;
            _mm_prefetch((const char*)(&a[a_base]) + 4096 + 64, _MM_HINT_NTA) ;
            _mm_prefetch((const char*)(&a[a_base]) + 4096 + 92, _MM_HINT_NTA) ;

            _mm_prefetch((const char*)(&colind[JJ]) + 4096, _MM_HINT_NTA) ;            
            _mm_prefetch((const char*)(&colind[JJ]) + 4096 + 32, _MM_HINT_NTA) ;            
*/

            {
                __m128d x_01 = _mm_load_pd(&x[x_base]) ;
                __m128d x_23 = _mm_load_pd(&x[x_base + 2]) ;

                {
                    __m128d a_00_01 = _mm_load_pd(&a[a_base]) ;
                    sum1 = _mm_add_pd(sum1, _mm_mul_pd(a_00_01, x_01)) ;
                }
                {
                    __m128d a_02_03 = _mm_load_pd(&a[a_base + 2]) ;
                    sum1 = _mm_add_pd(sum1, _mm_mul_pd(a_02_03, x_23)) ;
                }
                {
                    __m128d a_10_11 = _mm_load_pd(&a[a_base + 4]) ;
                    sum2 = _mm_add_pd(sum2, _mm_mul_pd(a_10_11, x_01)) ;
                }
                {
                    __m128d a_12_13 = _mm_load_pd(&a[a_base + 6]) ;
                    sum2 = _mm_add_pd(sum2, _mm_mul_pd(a_12_13, x_23)) ;
                }
                {
                    __m128d a_20_21 = _mm_load_pd(&a[a_base + 8]) ;
                    sum3 = _mm_add_pd(sum3, _mm_mul_pd(a_20_21, x_01)) ;
                }
                {
                    __m128d a_22_23 = _mm_load_pd(&a[a_base + 10]) ;
                    sum3 = _mm_add_pd(sum3, _mm_mul_pd(a_22_23, x_23)) ;
                }
                {
                    __m128d a_30_31 = _mm_load_pd(&a[a_base + 12]) ;
                    sum4 = _mm_add_pd(sum4, _mm_mul_pd(a_30_31, x_01)) ;            
                }
                {
                    __m128d a_32_33 = _mm_load_pd(&a[a_base + 14]) ;
                    sum4 = _mm_add_pd(sum4, _mm_mul_pd(a_32_33, x_23)) ;            
                }
            }
        }
        _mm_store_pd(&y[y_base], transpose_add_pd(sum1, sum2)) ;
        _mm_store_pd(&y[y_base+2], transpose_add_pd(sum3, sum4)) ;
        y_base += 4 ;
    }
    return 1 ;
#else
    return 0 ;
#endif
}

/*________________________________________________________________________*/

#ifdef BM
#undef BM
#endif
#ifdef BN
#undef BN
#endif
#ifdef BLOC_SIZE
#undef BLOC_SIZE
#endif

#define BM 4
#define BN 4
#define BLOC_SIZE 16

int sparse_matrix_bcrs_float_4_4_mult_SSE2(
    unsigned int M,
    const float* a, const unsigned int* colind, const unsigned int* rowptr,
    const float* x, float* y
) {

#ifdef GX_SSE
    unsigned int I,JJ ;
    unsigned int y_base = 0 ;
    if(!SSE2_supported) { return 0 ; }
    for(I=0; I<M; I++) {
        __v4sf sum1 = _mm_setzero_ps() ;
        __v4sf sum2 = sum1 ;
        __v4sf sum3 = sum1 ;
        __v4sf sum4 = sum1 ;
        for(JJ=rowptr[I]; JJ<rowptr[I+1]; JJ++) {
            unsigned int J = colind[JJ] ;
            unsigned int a_base = JJ * BLOC_SIZE ;
            unsigned int x_base = J * BN ;
            
            {
                __v4sf x_0123 = _mm_load_ps(&x[x_base]) ;
                {
                    __v4sf a_00_01_02_03 = _mm_load_ps(&a[a_base]) ;
                    sum1 = _mm_add_ps(sum1, _mm_mul_ps(a_00_01_02_03, x_0123)) ;
                }
                {
                    __v4sf a_10_11_12_13 = _mm_load_ps(&a[a_base + 4]) ;
                    sum2 = _mm_add_ps(sum2, _mm_mul_ps(a_10_11_12_13, x_0123)) ;
                }
/*                _mm_prefetch((const char*)(&a[a_base + 56]), _MM_HINT_NTA) ; */
                {
                    __v4sf a_20_21_22_23 = _mm_load_ps(&a[a_base + 8]) ;
                    sum3 = _mm_add_ps(sum3, _mm_mul_ps(a_20_21_22_23, x_0123)) ;
                }
                {
                    __v4sf a_30_31_32_33 = _mm_load_ps(&a[a_base + 12]) ;
                    sum4 = _mm_add_ps(sum4, _mm_mul_ps(a_30_31_32_33, x_0123)) ;
                }
            }
        }
        
        /*
         * Note: could be simpler with SSE3 (but my PentiumM does not have SSE3)
         * Anyway, since everything is done in the SSE registers, and since memory
         * access is the bottleneck (rather than SSE operations), I bet it would not
         * change performances.
         */

        _MM_TRANSPOSE4_PS(sum1,sum2,sum3,sum4) ;                            
        _mm_store_ps(
            &y[y_base], 
            _mm_add_ps(
                _mm_add_ps(sum1,sum2), 
                _mm_add_ps(sum3,sum4)
            )
        ) ; 
        y_base += 4 ;
    }
    return 1 ;
#else
    return 0 ;
#endif
}

/*----------------------------------------------------------------------------------------------------------------*/
