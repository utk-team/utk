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
 
 
 
 
 
 
 

#ifndef __GEEX_BASICS_LINE_STREAM__
#define __GEEX_BASICS_LINE_STREAM__

#include <Geex/basics/types.h>
#include <Geex/basics/assert.h>

#include <iostream>
#include <sstream>

namespace Geex {

    /**
     * LineInputStream facilitates parsing 
     * ASCII files line by line.
     */
    class LineInputStream {
    public:
        LineInputStream(std::istream& in) : in_(in), line_in_(nil) {   }
        ~LineInputStream() { delete line_in_ ; line_in_ = nil ;  }
        bool eof() const { return in_.eof() ; }
        bool eol() const { return line_in_ == nil || line_in_->eof() ; }
        //bool ok() const { return in_ != nil; }
		bool ok() const { 
			if (in_) return true;
		}
        void get_line() { in_.getline(buffer_, 65536) ; delete line_in_ ; line_in_ = new std::istringstream(buffer_) ;   }
        std::istream& line() {  gx_assert(line_in_ != nil) ; return *line_in_ ;    }
        const char *current_line() const { return buffer_;  }
        template <class T> LineInputStream& operator>>(T& param) { *line_in_ >> param; return *this;  }
    private:
        std::istream& in_ ;
        std::istringstream* line_in_ ;
        char buffer_[65536] ;
    } ;

}

#endif
