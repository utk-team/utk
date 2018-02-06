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
#ifndef VORONOI_FILTERING_H_
#define VORONOI_FILTERING_H_

#include <Geex/mathematics/glsl_linear.h>
#include <algorithm>
#include <list>

namespace Geex {

	class GEEX_API VF_Edge
	{
	public:
		VF_Edge()
		{
			indices[0] = indices[1] = 0;
		}
		VF_Edge(
			unsigned int i, 
			unsigned int j
			) {
				if(i < j) {
					indices[0] = i ;
					indices[1] = j ;
				} else {
					indices[0] = j ;
					indices[1] = i ;
				}
		}
		bool operator<(const VF_Edge& rhs) const {
			if(indices[0] < rhs.indices[0]) { return true ;  }
			if(indices[0] > rhs.indices[0]) { return false ; }
			if(indices[1] < rhs.indices[1]) { return true ;  }
			return false ;
		}
		inline VF_Edge& operator= (const VF_Edge& rhs)
		{
			indices[0] = rhs.indices[0]; indices[1] = rhs.indices[1];
			return *this;
		}
		VF_Edge (const VF_Edge& rhs)
		{
			indices[0] = rhs.indices[0]; indices[1] = rhs.indices[1];
		}
	public:
		unsigned int indices[2] ;
	};

    class GEEX_API VF_Face
    {
    public:
        VF_Face(
            unsigned int i,
            unsigned int j,
            unsigned int k
            ) {
                indices[0] = i ; indices[1] = j ; indices[2] = k ;
                std::sort(indices, indices+3) ;
                is_outside = true;
                visited = false;
				id = -1;
                valid = true;
        }
        bool operator<(const VF_Face& rhs) const {
            for(unsigned int i=0; i<3; i++) {
                if(indices[i] < rhs.indices[i]) { return true ;  }
                if(indices[i] > rhs.indices[i]) { return false ; }
            }
            return false ;
        }
        inline VF_Face& operator= (const VF_Face& rhs)
        {
            indices[0] = rhs.indices[0]; indices[1] = rhs.indices[1]; indices[2] = rhs.indices[2];
            is_outside = rhs.is_outside;
            visited = rhs.visited;
            return *this;
        }
        VF_Face (const VF_Face& rhs)
        {
            indices[0] = rhs.indices[0]; indices[1] = rhs.indices[1]; indices[2] = rhs.indices[2];
            is_outside = rhs.is_outside;
            visited = rhs.visited;
        }
    public:
        unsigned int indices[3] ;
		unsigned int id;
        bool is_outside;
        bool visited;
		VF_Edge adj_edge[3];
        vec3 normal;
        bool valid;
    };

 

    class GEEX_API Voronoi_Filtering
    { 
    public:
        Voronoi_Filtering(const std::vector<vec3>& vertices, double input_theta_degree);
        ~Voronoi_Filtering();
        inline const std::list<VF_Face*>& get_face_indice()
        {
            return face_indices;
        }
    protected:
        std::list<VF_Face*> face_indices;
    };

}

#endif
