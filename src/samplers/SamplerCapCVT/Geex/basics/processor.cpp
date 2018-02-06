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

#include <Geex/basics/processor.h>

#ifdef WIN32
#else
#include <fenv.h> 
#endif 

namespace Geex {
    namespace Processor {

        unsigned int GEEX_API number_of_cores() {
            static unsigned int result = 0 ;
            if(result == 0) {
#ifdef WIN32
            SYSTEM_INFO si ;
            GetSystemInfo(&si) ;
            result = si.dwNumberOfProcessors ;
#else
            result = sysconf(_SC_NPROCESSORS_ONLN) ;
#endif                
            }
            return result ;
        }

        void GEEX_API enable_FPE() {
#ifdef WIN32
#else
            int excepts= 0
              //| FE_INEXACT           // inexact result
                | FE_DIVBYZERO         // division by zero
              //| FE_UNDERFLOW         // result not representable due to underflow
                | FE_OVERFLOW          // result not representable due to overflow
                | FE_INVALID           // invalid operation
                ;
            feenableexcept(excepts);
#endif
        }


    }


}
