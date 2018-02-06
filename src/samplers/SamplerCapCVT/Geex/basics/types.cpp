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
 
#include <Geex/basics/types.h>
#include <Geex/basics/processor.h>
#include <Geex/mathematics/glsl_linear.h>
#include <Geex/mathematics/shewchuk.h>
#include <Geex/numerics/quick_blas.h>
#include <Geex/numerics/sparse_matrix_sse.h>
#include <Geex/properties/property_serializer.h>
#include <Geex/properties/property_interpolator.h>


#ifdef WIN32
#include <float.h> // for ::_isnan()
#else
#include <stdlib.h>
#endif

#include <ctype.h>

namespace Geex {

    namespace Numeric {

        const float big_float = 1e10f ;
        const float small_float = 1e-10f ;
        const double big_double = 1e20 ;
        const double small_double = 1e-20 ;


        bool is_nan(float32 x) {
#ifdef WIN32
            return (_isnan(x) != 0) || (_finite(x) == 0) ;
#else
            return isnan(x) || !finite(x);
#endif
        }
    
        bool is_nan(float64 x) {
#ifdef WIN32
            return (_isnan(x) != 0) || (_finite(x) == 0) ;
#else
            return isnan(x) || !finite(x);
#endif
        }    


        int32 random_int32() {
#ifdef WIN32
            return rand() ;
#else
            return random() ;
#endif        
        }

        float64 random_float64() {
#ifdef WIN32
            return float64(rand()) / double(RAND_MAX);
#else
            return float64(drand48()) ;
#endif        
        }


        float32 random_float32() {
#ifdef WIN32
            return (float32)(float32(rand()) / double(RAND_MAX));
#else
            return float32(drand48()) ;
#endif        
        }


    }

    namespace Memory {

        long used_memory() {
#ifdef WIN32
            // Note: this returns the amount of virtual memory used
            // by the system (we should replace that by a function
            // that returns the amount of virtual mem used by the
            // *process* ...)
            MEMORYSTATUS stat ;
            GlobalMemoryStatus(&stat) ;
            return (long)(stat.dwTotalVirtual - stat.dwAvailVirtual) ;
#else
            return (long)sbrk(0) ;            
#endif            
        }

    }

    namespace String {

        void split_string(
            const std::string& in, 
            char separator,
            std::vector<std::string>& out,
            bool skip_empty_fields
        ) {
            int length = (int)in.length() ;
            int start = 0 ;
            int end = 0 ;
            
            while(start < length) {
                
                end = (int)in.find(separator,start) ;
                if(end < 0) {
                    end = length ;
                }
                
                if(!skip_empty_fields || (end - start > 0)) {
                    out.push_back(in.substr(start, end - start)) ;
                }
                start = end + 1 ;
            }
        }
        

        void join_strings(
            const std::vector<std::string>& in,
            char separator,
            std::string& out
        ) {
            out = "" ;
            for(unsigned int i=0; i<in.size(); i++) {
                if(out.length() != 0) {
                    out += separator ;
                }
                out += in[i] ;
            }
        }

        void join_strings(
            const std::vector<std::string>& in,
            const std::string& separator,
            std::string& out
        ) {
            out = "" ;
            for(unsigned int i=0; i<in.size(); i++) {
                if(out.length() != 0) {
                    out += separator ;
                }
                out += in[i] ;
            }
        }

        std::string join_strings(
            const std::vector<std::string>& in,
            char separator
        ) {
            std::string result ;
            join_strings(in, separator, result) ;
            return result ;
        }

        std::string join_strings(
            const std::vector<std::string>& in,
            const std::string& separator
        ) {
            std::string result ;
            join_strings(in, separator, result) ;
            return result ;
        }

        void to_lowercase(std::string& in) {
            for(unsigned int i=0; i<in.length(); i++) {
                if(isupper(in[i])) {
                    in[i] = tolower(in[i]) ;
                }
            }
        }

        void to_uppercase(std::string& in) {
            for(unsigned int i=0; i<in.length(); i++) {
                if(islower(in[i])) {
                    in[i] = toupper(in[i]) ;
                }
            }
        }        

        std::string quote(const std::string& s, char quotes) {
            return char_to_string(quotes) + s + char_to_string(quotes) ;
        }
        
        
        bool string_starts_with(const std::string& haystack, const std::string& needle) {
            unsigned int l1 = haystack.length() ;
            unsigned int l2 = needle.length() ;
            if(l2 > l1) { return false ; }
            return (haystack.substr(0, l2) == needle) ;
        }
        
        bool string_ends_with(const std::string& haystack, const std::string& needle) {
            unsigned int l1 = haystack.length() ;
            unsigned int l2 = needle.length() ;
            if(l2 > l1) { return false ; }
            return (haystack.substr(l1 - l2, l2) == needle) ;
        }

    }
}

namespace Geex {

    void initialize() {

        exactinit() ;
//      Processor::enable_FPE() ;

        QuickBLAS::initialize() ;
        sparse_matrix_SSE2_initialize() ;

        PropertySerializer::initialize() ;
        PropertyInterpolator::initialize() ;

        gx_register_property_type<int>("integer") ;
        gx_register_property_type<bool>("boolean") ;
        gx_register_property_type<std::string>("string") ;
        gx_register_property_type<real>("real") ;
        gx_register_numeric_property_interpolator<real>() ;

        gx_register_property_type<vec2>("vec2") ;
        gx_register_property_type<vec3>("vec3") ;
        gx_register_property_type<vec4>("vec4") ;

        gx_register_numeric_property_interpolator<vec2>() ;
        gx_register_numeric_property_interpolator<vec3>() ;
        gx_register_numeric_property_interpolator<vec4>() ;

        gx_register_property_type_alias("Point2d", "vec2") ;
        gx_register_property_type_alias("Vector2d", "vec2") ;
        gx_register_property_type_alias("Complex", "vec2") ;

        gx_register_property_type_alias("Point3d", "vec3") ;
        gx_register_property_type_alias("Vector3d", "vec3") ;

        gx_register_property_type_alias("Color", "vec4") ;
    }

    void terminate() {
        PropertySerializer::terminate() ;
        PropertyInterpolator::terminate() ;
    }


}
