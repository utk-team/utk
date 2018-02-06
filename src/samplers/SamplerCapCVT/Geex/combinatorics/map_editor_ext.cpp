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

#include <Geex/combinatorics/map_editor_ext.h>

namespace Geex {

	bool MapEditorExt::edge_flip(Map::Halfedge* h)	{
		if(!is_flippable(h))
			return false;

		Map::Halfedge* hopp = h->opposite();

		Map::Halfedge* h00 = h->prev();
		Map::Halfedge* h01 = h->next();

		Map::Halfedge* h10 = hopp->next();
		Map::Halfedge* h11 = hopp->prev();

		Map::Facet* f0 = h->facet();
		Map::Facet* f1 = hopp->facet();


		link(h, h11, 1);
		link(h11, h01, 1);
		link(h01, h, 1);

		link(hopp, h00, 1);
		link(h00, h10, 1);
		link(h10, hopp, 1);

		set_facet_on_orbit(h, f0);
		make_facet_key(h);


		set_facet_on_orbit(hopp, f1);
		make_facet_key(hopp);

		make_vertex_key(h, h10->vertex());
		make_vertex_key(hopp, h01->vertex());

		make_vertex_key(h00);
		make_vertex_key(h10);
		make_vertex_key(h01);
		make_vertex_key(h11);

		return true;
	}
    
	bool MapEditorExt::is_flippable(Map::Halfedge* h)	{
		// the two edges involved in the flip

		vec3 plane_v0 = normalize(h->vertex()->point() - h->opposite()->vertex()->point());
		vec3 plane_v1 = normalize(h->opposite()->next()->vertex()->point() - h->next()->vertex()->point());		

		// the plane defined by the two edges

		vec3 plane_n = normalize(cross(plane_v1, plane_v0));


		// orthogonalize in-plane vectors

		plane_v0 = normalize(plane_v0 - dot(plane_v0, plane_v1) * plane_v1);

		vec3 plane_origin = h->next()->vertex()->point();


		// 2d coordinates in plane

		vec3 local_t = local_plane_coords(h->vertex()->point(), plane_v0, plane_v1, plane_n, plane_origin);
		vec3 local_b = local_plane_coords(h->opposite()->vertex()->point(), plane_v0, plane_v1, plane_n, plane_origin);
		vec3 local_l = local_plane_coords(h->next()->vertex()->point(), plane_v0, plane_v1, plane_n, plane_origin);
		vec3 local_r = local_plane_coords(h->opposite()->next()->vertex()->point(), plane_v0, plane_v1, plane_n, plane_origin);


		// check if edge intersections lies inside triangles pair (in plane)

		vec3 tb = local_t - local_b;
		vec3 lr = local_l - local_r;

		double ntb[2];
		double ctb;

		double nlr[2];
		double clr;

		ntb[0] = - tb[1];
		ntb[1] = tb[0];

		ctb = -(ntb[0] * local_t[0] + ntb[1] * local_t[1]);


		nlr[0] = - lr[1];
		nlr[1] = lr[0];

		clr = -(nlr[0] * local_l[0] + nlr[1] * local_l[1]);


		double det = ntb[0] * nlr[1] - nlr[0] * ntb[1];

		vec3 intersection(- (nlr[1] * ctb - ntb[1] * clr) / det, 
			- (-nlr[0] * ctb + ntb[0] * clr) / det, 
			0.0);


		double l0 = dot(intersection - local_r, lr) / dot(lr, lr);
		double l1 = dot(intersection - local_b, tb) / dot(tb, tb);

		return l0 > 0.0 && l0 < 1.0 && l1 > 0.0 && l1 < 1.0;
	}


    vec3 MapEditorExt::local_plane_coords(
        const vec3& p, const vec3& v0, const vec3& v1, const vec3& plane_n, const vec3& plane_origin
    ) {
        vec3 q(p + dot(plane_origin - p, plane_n) * plane_n);
        return vec3( dot(q - plane_origin, v0), dot(q - plane_origin, v1), 0.0);
    }

//_________________________________________________________

}
