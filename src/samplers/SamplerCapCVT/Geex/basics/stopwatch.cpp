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
 
 
 
 
 
 
 

#include <Geex/basics/stopwatch.h>
#include <iostream>

namespace Geex {

//_________________________________________________________


    SystemStopwatch::SystemStopwatch() {
#ifdef WIN32
        start_ = GetTickCount() ;
#else
        clock_t init_user = times(&start_) ;
        while((start_user_ = times(&start_)) == init_user) ;
#endif
    }

    double SystemStopwatch::elapsed_user_time() const {
#ifdef WIN32
        return double(GetTickCount() - start_) / 1000.0 ;
#else
        clock_t end_user ;
        tms end ;
        end_user = times(&end) ;
        return double(end_user - start_user_) / 100.0 ;
#endif
    }

    double SystemStopwatch::now() {
#ifdef WIN32
        return double(GetTickCount()) / 1000.0 ;
#else
        clock_t end_user ;
        tms now_tms ;
        return double(times(&now_tms)) / 100.0 ;
#endif
    }

    void SystemStopwatch::print_elapsed_time() {
#ifdef WIN32
        std::cerr << "---- Times (seconds) ----" << std::endl ;
        std::cerr << "  Elapsed time: " << elapsed_user_time() << std::endl ;
#else
        clock_t end_user ;
        tms end ;
        end_user = times(&end) ;
    
        std::cerr << "---- Times (seconds) ----" << std::endl ;
        std::cerr << "  Real time: " 
                  << double(end_user - start_user_) / 100.0 << std::endl ;

        std::cerr << "  User time: " 
                  << double(end.tms_utime - start_.tms_utime) / 100.0 
                  << std::endl ;

        std::cerr << "  Syst time: " 
                  << double(end.tms_stime - start_.tms_stime) / 100.0 
                  << std::endl ;    
#endif    
    }

//_________________________________________________________

    Numeric::uint64 ProcessorStopwatch::now() {
        // Note: should be OGF_INTEL instead of GX_SSE
        // (but GX_SSE does the job for the moment ...)
#ifdef GX_SSE
#ifdef WIN32
        __int64 clock;
        __asm
            {
                    rdtsc                                             // Read the RDTSC Timer
                    mov    dword ptr[clock], eax        // Store the value in EAX and EDX Registers
                    mov    dword ptr[clock+4], edx
            }
        return clock;
#else
        unsigned long low, high ;
        __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high)) ;
        unsigned long long result = (unsigned long long) high;
        result <<= 32;
        result |= (unsigned long long) low;
        return result ;
#endif       
#else
        return Numeric::uint64(SystemStopwatch::now() * 1000.0) ;
#endif
    }

//_________________________________________________________

}

