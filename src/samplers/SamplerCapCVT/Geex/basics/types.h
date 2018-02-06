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
 
 
#ifndef __GEEX_BASICS_TYPES__
#define __GEEX_BASICS_TYPES__

#include <Geex/basics/linkage.h>

#include <string>
#include <vector>

#include <math.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Geex {

//____________________________________________________________________________

#ifndef nil
#define nil 0
#endif

    void GEEX_API initialize() ;
    void GEEX_API terminate() ;


    namespace Memory {
        typedef unsigned char  byte ;
        typedef unsigned char  word8 ;
        typedef unsigned short word16 ;
        typedef unsigned int   word32 ;
        typedef byte* pointer ;
        inline void clear(void* addr, size_t size) { ::memset(addr, 0, size) ; }
        template <class T> inline void clear(std::vector<T>& v) { ::memset(&v[0], 0, v.size() * sizeof(T)) ; }
        inline void copy(void* to, const void* from, size_t size) { ::memcpy(to, from, size) ; }
        long GEEX_API used_memory() ;
    } 

//_______________________________________________________________________

/**
 * A namespace gathering typedefs
 * corresponding to numbers. These types
 * names have the form (u)int<size> or float<size>,
 * where the (optional) u denotes an unsigned type,
 * and the size is in bits.
 */

    namespace Numeric {
        typedef char          int8 ;
        typedef short         int16 ;
        typedef int           int32 ;
#ifdef WIN32
        typedef __int64 int64 ;
#else
        typedef long long int int64 ;	
#endif
        typedef unsigned char  uint8 ;
        typedef unsigned short uint16 ;
        typedef unsigned int   uint32 ;
#ifdef WIN32
        typedef unsigned __int64 uint64 ;
#else
        typedef unsigned long long int uint64 ;
#endif    

        typedef float  float32 ;
        typedef double float64 ;

        extern GEEX_API const float big_float ;
        extern GEEX_API const float small_float ;
        extern GEEX_API const double big_double ;
        extern GEEX_API const double small_double ;

        int32   GEEX_API random_int32() ;
        float32 GEEX_API random_float32() ;
        float64 GEEX_API random_float64() ;
        bool GEEX_API is_nan(float32 x) ;
        bool GEEX_API is_nan(float64 x) ;
    } 

    // These ones can be changed according to
    // precision and number of bits in target machine.
    // Note: if moving index_t, signed_index_t and size_t 
    // to 64 bits, properties/record_id.h needs to be
    // changed accordingly (to enable 64bits properties
    // management).
    typedef Numeric::float64 real ;
    typedef Numeric::uint32 index_t ;
    typedef Numeric::int32 signed_index_t ;
    typedef Numeric::uint32 size_t ;

    enum { alignment_for_SSE2 = 16} ;

//_______________________________________________________________________

    namespace String {

        void GEEX_API split_string(
            const std::string& in, 
            char separator,
            std::vector<std::string>& out,
            bool skip_empty_fields = true
        ) ;

        void GEEX_API join_strings(
            const std::vector<std::string>& in,
            char separator,
            std::string& out
        ) ;

        void GEEX_API join_strings(
            const std::vector<std::string>& in,
            const std::string& separator,
            std::string& out
        ) ;


        std::string GEEX_API join_strings(
            const std::vector<std::string>& in,
            char separator
        ) ;

        std::string GEEX_API join_strings(
            const std::vector<std::string>& in,
            const std::string& separator
        ) ;


        void GEEX_API to_lowercase(std::string& in) ;
        void GEEX_API to_uppercase(std::string& in) ;

        inline std::string char_to_string(char c) {
            char s[2] ;
            s[0] = c ;
            s[1] = '\0' ;
            return std::string(s) ;
        }

        std::string GEEX_API quote(const std::string& s, char quotes = '\"') ;
        
        bool GEEX_API string_starts_with(const std::string& haystack, const std::string& needle) ;
        bool GEEX_API string_ends_with(const std::string& haystack, const std::string& needle) ;
    } 

//_______________________________________________________________________

    template <class T> inline T gx_max(T x1, T x2) {
        return x1 > x2 ? x1 : x2;
    }

    template <class T> inline T gx_min(T x1, T x2) {
        return x1 < x2 ? x1 : x2;
    }

    enum Sign { NEGATIVE=-1, ZERO=0, POSITIVE=1 } ;

    template <class T> inline Sign gx_sgn(T x) {
        return (x > 0) ? POSITIVE : (
            (x < 0) ? NEGATIVE : ZERO
        );
    }

    template <class T> inline T gx_abs(T x)  {
        return (x > 0) ? x : -x;
    }

    template <class T> inline T gx_sqr(T x)  {
        return x*x;
    }

    template <class T> inline void gx_clamp(T& x, T min, T max) {
        if(x < min) {
            x = min ;
        } else if(x > max) {
            x = max ;
        }
    }

    template <class T> inline void gx_swap(T& x, T& y) {
        T z = x ;
        x = y ;
        y = z ;
    }

}

#endif
