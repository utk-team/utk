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
#ifndef LFS_H
#define LFS_H


#include <Geex/mathematics/glsl_linear.h>

namespace Geex {

    class ANNKdTree_3 ;
    
    class GEEX_API LFS { 
    public:
        LFS() ;
        LFS(const std::vector<vec3>& pts, double sliver_quality = 0.01, const bool* feature_label = 0) ;
        ~LFS() ;
        
        void compute(const std::vector<vec3>& pts, double sliver_quality_degree = 0.01, const bool* feature_label = 0) ;
        real squared_lfs(const vec3& p) ;
        real squared_lfs(const double* p) ;
        real squared_lfs(real x, real y, real z);
        inline const std::vector<vec3>& get_poles()
        {
            return poles;
        }
        inline const std::vector<vec3>& get_positive_normal()
        {
            return N;
        }
		inline ANNKdTree_3* get_kdtree() 
		{
			return kdtree_;
		}
    protected:
        void create_kdtree();
    private:
        ANNKdTree_3 * kdtree_ ;
        std::vector<vec3> poles;
        std::vector<vec3> N;
    } ;

}

#endif