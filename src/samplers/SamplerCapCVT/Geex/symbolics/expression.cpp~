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

#ifdef GX_GINAC

#include <stdio.h>
#include <Geex/symbolics/expression.h>

namespace Geex {

    namespace Symbolic {

        static int cur_var = 0 ;

        Expression new_var(const std::string& name, int id) {
            static char buff[300] ;
            sprintf(buff, "%s%d", name.c_str(), id) ;
            return symbol(buff) ;
        }
        
        Expression new_var() {
            cur_var++ ;
            return new_var("x",cur_var) ;
        }

        Expression new_var(const std::string& name, int id, const std::string& field) {
            static char buff[300] ;
            sprintf(buff, "%s[%d].%s", name.c_str(), id, field.c_str()) ;
            return symbol(buff) ;
        }

    }
    
}

#endif
