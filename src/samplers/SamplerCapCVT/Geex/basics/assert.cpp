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
 
 
 
 
 
 
 

#include <Geex/basics/assert.h>
#include <stdlib.h>
#include <iostream>

#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#endif

namespace Geex {

    static void geex_abort() {
#ifdef WIN32
        abort() ;
#else
        int geex_pid = getpid() ;
        std::cerr << "Current pid is: " << geex_pid << std::endl ;
        std::cerr << "Going to bed" << std::endl ;
        std::cerr << "Use: \'gdb geex " << geex_pid << "\' and then \'where\' to see the stack trace" << std::endl ;

        kill(geex_pid, SIGSTOP) ;
#endif        
    }
    
    void gx_assertion_failed(
        const std::string& condition_string,
        const std::string& file, int line
    ) {
        std::cerr << "Assertion failed: " << condition_string << std::endl ;
        std::cerr << "File: " << file << std::endl ;
        std::cerr << "Line: " << line << std::endl ;
        geex_abort() ;
    }
    
    void gx_range_assertion_failed(
        double value, double min_value, double max_value, 
        const std::string& file, int line
    ) {
        std::cerr << "Range assertion failed: " 
                  << value << " in " 
                  << "[ " << min_value << " ... " << max_value << " ]"
                  << std::endl ;
        std::cerr << "File: " << file << std::endl ;
        std::cerr << "Line: " << line << std::endl ;
        geex_abort() ;
    }

    void gx_should_not_have_reached(
        const std::string& file, int line
    ) {
        std::cerr << "Control should not have reached this point:" << std::endl ;
        std::cerr << "File: " << file << std::endl ;
        std::cerr << "Line: " << line << std::endl ;
        geex_abort() ;
    }

}
