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


#ifndef __LLOYD_ENERGY__
#define __LLOYD_ENERGY__

#include <Geex/CVT/geometry.h>

namespace Geex {

	template <class T> inline T Lloyd_energy(
		const vec3g<T>& p1, const vec3g<T>& p2, const vec3g<T>& p3, const vec3g<T>& p4
		) {
			vec3g<T> U = p2 - p1 ;
			vec3g<T> V = p3 - p1 ;
			vec3g<T> W = p4 - p1 ;
			return mixed_product(U,V,W) / T(60.) * (
				U.length2() + V.length2() + W.length2() + dot(U,V) + dot(V,W) + dot(W,U)
				) ;
	}

	template <class T> inline T Lloyd_energy(
		const vec3g<T>& p1, const vec3g<T>& p2, const vec3g<T>& p3
		) {
			vec3g<T> U = p2 - p1 ;
			vec3g<T> V = p3 - p1 ;
			return length(cross(U, V))/T(12)*(dot(U,U)+dot(U,V)+dot(V,V)) ;
	}

	// Lloyd energy of a triangle which belong to Voronoi region of seed
	template <class T> inline T Lloyd_energy(
		const vec3g<T>& seed, const vec3g<T>& p1, const vec3g<T>& p2, const vec3g<T>& p3,
		vec3g<T>& gradient, T& V
		) {
			vec3g<T> v0 = p1 - seed ;
			vec3g<T> v1 = p2 - seed ;
			vec3g<T> v2 = p3 - seed ;

			V = tri_area(p1, p2, p3);

			gradient = 2.0 * V *(seed - (1.0/3.0) * (p1+p2+p3));		

			vec3g<T> v_12 = v1+v2;
			vec3g<T> v_012 = v0+v_12;
			return V * (dot(v0,v_012)+dot(v1,v_12)+dot(v2,v2)) / 6.;
			//i.e return V * (dot(v0,v0)+dot(v0,v1)+dot(v0,v2)+dot(v1,v1)+dot(v1,v2)+dot(v2,v2)) / 6;
	}

	// weighted Lloyd energy of a triangle which belong to Voronoi region of seed
	// weight is defined at each vertex and linear interpolated within triangle
	template <class T> inline T Lloyd_energy(
		const vec3g<T>& seed, 
		const vec3g<T>& p1, const vec3g<T>& p2, const vec3g<T>& p3,
		T a, T b, T c, vec3g<T>& gradient, T& V
		) {

			T t_area = tri_area(p1, p2, p3);
			vec3g<T> sp[3];
			sp[0] = seed - p1;
			sp[1] = seed - p2;
			sp[2] = seed - p3;
			T rho[3];
			T alpha[3];
			T Sp = a + b + c;
			V  = t_area * Sp / 3.;
			rho[0] = a; rho[1] = b; rho[2] = c;
			for (int i =0; i < 3; i++)
			{
				alpha[i] = Sp + rho[i];
			}
			T f = 0;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j <= i; j++)
				{
					f += (alpha[i] + rho[j]) * dot(sp[i], sp[j]);
				}
			}
			gradient = (t_area/6.0) * (4.0*Sp*seed - (alpha[0]*p1+alpha[1]*p2+alpha[2]*p3));
			return t_area * f / 30.0;
	}


	// Lloyd energy of a tetrahedron which belong to Voronoi region of seed
	template <class T> inline T Lloyd_energy_tet(
		const vec3g<T>& seed, const vec3g<T>& p1, const vec3g<T>& p2, const vec3g<T>& p3, const vec3g<T>& p4,
		vec3g<T>& gradient, T& V
		) {
			vec3g<T> v0 = p1 - seed ;
			vec3g<T> v1 = p2 - seed ;
			vec3g<T> v2 = p3 - seed ;
			vec3g<T> v3 = p4 - seed ;

			V = tetra_signed_volume(p1, p2, p3, p4);

			gradient = 2.0 * V * ( seed - 0.25 * (p1+p2+p3+p4));		

			vec3g<T> v_23 = v2+v3;
			vec3g<T> v_123 = v1+v_23;
			vec3g<T> v_0123 = v0+v_123;
			return V * (dot(v0,v_0123)+dot(v1,v_123)+dot(v2,v_23)+dot(v3,v3)) / 10.;
			//i.e return V * (dot(v0,v0)+dot(v0,v1)+dot(v0,v2)++dot(v0,v3)+
			//dot(v1,v1)+dot(v1,v2)+dot(v1,v3)+dot(v2,v2)+dot(v2,v3)+dot(v3,v3)) / 10;
	}

	// weighted Lloyd energy of a tetrahedron which belong to Voronoi region of seed
	// weight is defined at each vertex and linear interpolated within tetrahedron
	template <class T> inline T Lloyd_energy_tet(
		const vec3g<T>& seed, 
		const vec3g<T>& p1, const vec3g<T>& p2, const vec3g<T>& p3, const vec3g<T>& p4,
		T a, T b, T c, T d, vec3g<T>& gradient, T& V
		) {
			T t_vol = tetra_signed_volume(p1, p2, p3, p4);
			vec3g<T> sp[4];
			sp[0] = seed - p1;  sp[1] = seed - p2;
			sp[2] = seed - p3;  sp[3] = seed - p4;
			T rho[4];
			T alpha[4];
			T Sp = a + b + c + d;
			V  = t_vol * Sp / 4.;
			rho[0] = a; rho[1] = b; rho[2] = c; rho[3] = d;
			for (int i =0; i < 4; i++)
			{
				alpha[i] = Sp + rho[i];
			}
			T f = 0;
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j <= i; j++)
				{
					f += (alpha[i] + rho[j]) * dot(sp[i], sp[j]);
				}
			}
			gradient = (t_vol/10.0) * (5.0*Sp*seed - (alpha[0]*p1+alpha[1]*p2+alpha[2]*p3+alpha[3]*p4));
			return t_vol * f / 60.0;
	}

}


#endif
