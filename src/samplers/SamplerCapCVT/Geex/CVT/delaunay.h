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

#ifndef __GEEX_CVT_DELAUNAY__
#define __GEEX_CVT_DELAUNAY__

#include <Geex/CVT/delaunay_skel.h>
#include <Geex/CVT/voronoi_cell.h>
#include <Geex/CVT/ann_kdtree.h>

namespace Geex {

    /**
     * Abstract API for Delaunay triangulation in 3D.
     * Input: set of vertices. Output: Delaunay skeleton.
     */
    class GEEX_API Delaunay {
    public:
        Delaunay() ;
        virtual ~Delaunay() ;

        virtual void set_vertices(unsigned int n, const double* xyz) = 0 ;
		virtual void insert_vertices(unsigned int pos, unsigned int n, const double* xyz) { } ;
		virtual void remove_vertices(std::vector<vec3>& V, std::vector<double>& weight, std::set<int>& to_remove) { } ;
        void set_vertices(unsigned int n, const vec3* V) { 
            set_vertices(n, &(V[0].x)) ;   
            destroy_kdtree();
        }       
        void set_vertices(const std::vector<vec3>& V)    {
            set_vertices((unsigned int)V.size(), &(V[0])) ; 
            destroy_kdtree();
        }
		// insert last nb vertices of V
		void insert_vertices(const std::vector<vec3>& V, unsigned int pos, unsigned int n) {
			insert_vertices(pos, n, &(V[0][0])) ;
			destroy_kdtree() ;
		}

        void create_kdtree();
        void destroy_kdtree();

        virtual void set_anisotropy(unsigned int n, const double* aniso, int aniso_metric = 0) ;
        virtual void set_weight(unsigned int n, const double* weight) ;

        unsigned int nb_vertices() const { return nb_vertices_ ; }
        const double* vertices() const { return vertices_ ; }
        vec3 vertex(unsigned int i) const { 
            gx_debug_assert(i < nb_vertices_) ; 
            return vec3(vertices_[3*i], vertices_[3*i+1], vertices_[3*i+2]); 
        }

        virtual unsigned int nearest_vertex_id(double x, double y, double z) const = 0 ;
        unsigned int nearest_vertex_id(const vec3& p) const { 
            if (m_kdtree_ ) {
                return m_kdtree_->queryNearestNeighbor(p.x, p.y, p.z);
            } else {
                return nearest_vertex_id(p.x, p.y, p.z) ; 
            }
        }

        const DelaunaySkeleton* skeleton() const {
            if(skeleton_dirty_) { 
                update_skeleton() ; 
            }
            return &skeleton_ ;
        }

        /**
         * Entry point to factory.
         * For the moment, name = "CGAL" is supported.
         */
        static Delaunay* create(const std::string& name) ;

        virtual void get_tetras(std::vector<int>& tetras, bool finite_only) ;
		virtual void get_tetras(std::vector<int>& tetras, std::vector<int>& neighbors, std::vector<vec3>& duals) ;
        virtual void get_facets(std::vector<int>& facets, bool finite_only) ;
        virtual void get_voronoi_cell(unsigned int i, VoronoiCell& cell, bool geometry = true, bool use_cell_id = false) ;

		virtual unsigned int get_number_of_finite_vertices() const;
		virtual unsigned int get_number_of_finite_cells() const;

		//************extend*************
		virtual void insert_vertex(double* xyz, double* weight, vec3 V, double weightv, unsigned int pos) { };
		virtual void remove_vertex(double* xyz, double* weight, unsigned int index) { };
		//*******************************

    protected:
        virtual void update_skeleton() const = 0 ;

    protected:
        mutable bool skeleton_dirty_ ;
        mutable DelaunaySkeleton skeleton_ ;
        const double* vertices_ ;
		//double* vertices_ ;
        const double* aniso_ ;
        int aniso_metric_;
        unsigned int nb_vertices_ ;
        ANNKdTree_3* m_kdtree_;
        const double* weight_;
		//double* weight_;
    } ;


    //===========================================================================================

    /**
     * A Delaunay without Delaunay, just to be used with ANN.
     */
    class GEEX_API AnnDelaunay : public Delaunay {
    public:
        virtual void set_vertices(unsigned int n, const double* xyz) ;
        virtual void set_anisotropy(unsigned int n, const double* aniso, int aniso_metric = 0) ;
        virtual void set_weight(unsigned int n, const double* weight) ;
        virtual unsigned int nearest_vertex_id(double x, double y, double z) const ;
    protected:
        virtual void update_skeleton() const ;
    } ;

}

#endif

