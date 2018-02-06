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
 
 
 
 
 
 
 

#ifndef __GEEX_BASICS_STOPWATCH__
#define __GEEX_BASICS_STOPWATCH__

#include <Geex/basics/types.h>

//_________________________________________________________

#ifdef WIN32
#else
#include <sys/types.h>
#include <sys/times.h> 
#endif

//______________________________________________________________________

namespace Geex {



/**
 * A utility class to precisely compute the time taken
 * by an algorithm. SystemStopwatch prints to cerr the elapsed time
 * between the line where it is declared and the end
 * of the scope where it is declared. The times computed by
 * SystemStopwatch are expressed as system ticks, which is a system
 * dependant unit. SystemStopwatch prints three different times:
 *
 * real time: the really elapsed time (depends on the load of the
 *   machine, i.e. on the others programs that are executed at the
 *   same time).
 *
 * system time: the time spent in system calls.
 *
 * user time: the time really spent in the process.
 *
 * example:
 * <pre>
 *   {
 *      SystemStopwatch clock ;
 *        do_something() ;
 *        clock.print_elapsed_time() ;
 *   } 
 * </pre>
 *
 */

    class GEEX_API SystemStopwatch {
    public :
        SystemStopwatch() ;

        /**
         * prints real, user and system times since the
         * construction of this SystemStopWatch (in seconds).
         */
        void print_elapsed_time() ;

        /**
         * returns user elapsed time since the construction
         * of this SystemStopWatch (in seconds).
         */
        double elapsed_user_time() const ;


        /**
         * returns current time (in seconds).
         */
        static double now() ;
        
    private:
#ifdef WIN32
        long start_ ;
#else
        tms start_ ;
        clock_t start_user_ ;
#endif    
    } ;

//_________________________________________________________

    class GEEX_API ProcessorStopwatch {
    public:
        ProcessorStopwatch() { start_ = now() ; }
        static Numeric::uint64 now() ;
        Numeric::uint64 elapsed_time() const { return now() - start_ ; }
    private:
        Numeric::uint64 start_ ;
    } ;

//_________________________________________________________

}
#endif

