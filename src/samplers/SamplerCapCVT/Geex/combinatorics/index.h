/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000-2009 INRIA - Project ALICE
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
 *  Contact: Bruno Levy - levy@loria.fr
 *
 *     Project ALICE
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 *
 * As an exception to the GPL, Graphite can be linked with the following (non-GPL) libraries:
 *     Qt, SuperLU, WildMagic and CGAL
 */
 

#ifndef __GEEX_COMBI_INDEX__
#define __GEEX_COMBI_INDEX__

//#include <OGF/GoodMesh/common/common.h>
#include <algorithm>
#include <iostream>

namespace Geex {
    
    //________________________________________________________________

    enum KeepOrderType { KeepOrder } ;

    struct bindex {
        bindex() {}
        bindex(
            int i, 
            int j
        ) {
            if(i < j) {
                indices[0] = i ;
                indices[1] = j ;
            } else {
                indices[0] = j ;
                indices[1] = i ;
            }
        }
        bindex(
            int i, 
            int j,
            KeepOrderType
        ) {
            indices[0] = i ;
            indices[1] = j ;
        }
        bool operator<(const bindex& rhs) const {
            if(indices[0] < rhs.indices[0]) { return true ;  }
            if(indices[0] > rhs.indices[0]) { return false ; }
            if(indices[1] < rhs.indices[1]) { return true ;  }
            return false ;
        }
	bool operator==(const bindex& rhs) const {
	    return (indices[0] == rhs.indices[0]) && (indices[1] == rhs.indices[1]) ;
	}
	bool operator!=(const bindex& rhs) const {
	    return (indices[0] != rhs.indices[0]) || (indices[1] != rhs.indices[1]) ;
	}

	bindex(const bindex& rhs) {
	    indices[0] = rhs.indices[0] ;
	    indices[1] = rhs.indices[1] ;
	}

	const bindex& operator=(const bindex& rhs) {
	    indices[0] = rhs.indices[0] ;
	    indices[1] = rhs.indices[1] ;
	    return *this ;
	}

        int indices[2] ;

	static bindex inverse(const bindex& b) {
	    return bindex(b.indices[1], b.indices[0], KeepOrder) ;
	}

    } ;

    inline std::ostream& operator<<(std::ostream& out, const bindex& B) {
	return out << B.indices[0] << " " << B.indices[1] ;
    }
    
    //________________________________________________________________

    struct trindex {
        trindex() {}
        trindex(
            int i,
            int j,
            int k
        ) {
            indices[0] = i ; indices[1] = j ; indices[2] = k ;
            std::sort(indices, indices+3) ;
        }
        trindex(
            int i, 
            int j,
            int k,
            KeepOrderType
        ) {
            indices[0] = i ;
            indices[1] = j ;
            indices[2] = k ;
        }
        bool operator<(const trindex& rhs) const {
            for(int i=0; i<3; i++) {
                if(indices[i] < rhs.indices[i]) { return true ;  }
                if(indices[i] > rhs.indices[i]) { return false ; }
            }
            return false ;
        }

	bool operator==(const trindex& rhs) const {
	    return 
	        (indices[0] == rhs.indices[0]) && 
	        (indices[1] == rhs.indices[1]) && 
	        (indices[2] == rhs.indices[2]) ;
	}

	bool operator!=(const trindex& rhs) const {
	    return 
		(indices[0] != rhs.indices[0]) || 
		(indices[1] != rhs.indices[1]) ||
		(indices[2] != rhs.indices[2]) ;
	}

	trindex(const trindex& rhs) {
	    indices[0] = rhs.indices[0] ;
	    indices[1] = rhs.indices[1] ;
	    indices[2] = rhs.indices[2] ;
	}

	const trindex& operator=(const trindex& rhs) {
	    indices[0] = rhs.indices[0] ;
	    indices[1] = rhs.indices[1] ;
	    indices[2] = rhs.indices[2] ;
	    return *this ;
	}

        int indices[3] ;

	static bool same_orientation(const trindex& t, int i, int j, int k) {
	    return 
		(t.indices[0] == i && t.indices[1] == j && t.indices[2] == k) ||
		(t.indices[1] == i && t.indices[2] == j && t.indices[0] == k) ||
		(t.indices[2] == i && t.indices[0] == j && t.indices[1] == k) ;
	}

	static bool same_orientation(const trindex& t1, const trindex& t2) {
	    return same_orientation(t1, t2.indices[0], t2.indices[1], t2.indices[2]) ;
	}

	static trindex inverse(const trindex& t) {
	    return trindex(t.indices[2], t.indices[1], t.indices[0], KeepOrder) ;
	}
    } ;

    inline std::ostream& operator<<(std::ostream& out, const trindex& T) {
	return out << T.indices[0] << " " << T.indices[1] << " " << T.indices[2] ;
    }

    //________________________________________________________________

    struct quadindex{
        quadindex() { }
        quadindex(
            int i,
            int j,
            int k,
            int l
        ) {
            indices[0] = i ; indices[1] = j ; indices[2] = k ; indices[3] = l;
            std::sort(indices, indices+4) ;
        }
        quadindex(
            int i,
            int j,
            int k,
            int l,
            KeepOrderType 
        ) {
            indices[0] = i ; indices[1] = j ; indices[2] = k ; indices[3] = l;
        }
        bool operator<(const quadindex& rhs) const {
            for(int i=0; i<4; i++) {
                if(indices[i] < rhs.indices[i]) { return true ;  }
                if(indices[i] > rhs.indices[i]) { return false ; }
            }
            return false ;
        }
	bool operator==(const quadindex& rhs) const {
	    return 
	        (indices[0] == rhs.indices[0]) && 
	        (indices[1] == rhs.indices[1]) && 
	        (indices[2] == rhs.indices[2]) && 
	        (indices[3] == rhs.indices[3]) ;
	}

	bool operator!=(const quadindex& rhs) const {
	    return 
		(indices[0] != rhs.indices[0]) || 
		(indices[1] != rhs.indices[1]) ||
		(indices[2] != rhs.indices[2]) ||
		(indices[3] != rhs.indices[3]) ;
	}

	quadindex(const quadindex& rhs) {
	    indices[0] = rhs.indices[0] ;
	    indices[1] = rhs.indices[1] ;
	    indices[2] = rhs.indices[2] ;
	    indices[3] = rhs.indices[3] ;
	}

	const quadindex& operator=(const quadindex& rhs) {
	    indices[0] = rhs.indices[0] ;
	    indices[1] = rhs.indices[1] ;
	    indices[2] = rhs.indices[2] ;
	    indices[3] = rhs.indices[3] ;
	    return *this ;
	}

        int indices[4] ;
    } ;

    inline std::ostream& operator<<(std::ostream& out, const quadindex& Q) {
	return out << Q.indices[0] << " " << Q.indices[1] << " " << Q.indices[2] << " " << Q.indices[3] ;
    }
    
    //________________________________________________________________
}


#endif


