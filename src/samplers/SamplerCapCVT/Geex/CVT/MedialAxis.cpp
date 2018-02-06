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
#include <Geex/CVT/MedialAxis.h>
#include <Geex/CVT/ann_kdtree.h>
#include <Geex/CVT/geometry.h>

//use CGAL's efficient data structure
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <Geex/CVT/CVT_spatial_sort.h>

#include <set>
#include <fstream>

namespace Geex
{
    namespace MA_CGAL
    {
        //////////////////////////////////////////////////////////////////////////
        template < class GT, class Vb = CGAL::Triangulation_vertex_base_3<GT> >
        class My_vertex_base_3D
            : public Vb
        {
        public:
            int  id;
            bool is_on_convex_hull;
            bool has_positive_pole;
            Geex::vec3 positive_pole;
            Geex::vec3 pos_N;
        public:
            typedef typename Vb::Vertex_handle  Vertex_handle;
            typedef typename Vb::Cell_handle    Cell_handle;
            typedef typename Vb::Point          Point;

            template < class TDS2 >
            struct Rebind_TDS {
                typedef typename Vb::template Rebind_TDS<TDS2>::Other  Vb2;
                typedef My_vertex_base_3D<GT, Vb2>                        Other;
            };

            My_vertex_base_3D(): id(-1), is_on_convex_hull(false){pos_N = vec3(0,0,0); has_positive_pole = true; }

            My_vertex_base_3D(const Point& p)
                : Vb(p), id(-1), is_on_convex_hull(false){pos_N = vec3(0,0,0); has_positive_pole = true;}

            My_vertex_base_3D(const Point& p, Cell_handle c)
                : Vb(p, c), id(-1), is_on_convex_hull(false){pos_N = vec3(0,0,0); has_positive_pole = true;}
        }; 
        //////////////////////////////////////////////////////////////////////////
        template <typename GT, typename Cb = CGAL::Triangulation_cell_base_3<GT> >
        class My_cell_base
            : public Cb
        {
            typedef typename GT::Point_3						Point_3;
        public:
            int id;
            Geex::vec3 circumcenter;
            double radius;
            bool is_not_sliver;
            bool is_used;
        public:
            typedef typename Cb::Vertex_handle                   Vertex_handle;
            typedef typename Cb::Cell_handle                     Cell_handle;
            typedef GT                                           Geom_traits; 

            template < typename TDS2 >
            struct Rebind_TDS {
                typedef typename Cb::template Rebind_TDS<TDS2>::Other       Cb2;
                typedef My_cell_base<GT, Cb2>  Other;
            };

            My_cell_base()
                : Cb(), id(-1), is_not_sliver(true), is_used(false){}

            My_cell_base(Vertex_handle v0, Vertex_handle v1,
                Vertex_handle v2, Vertex_handle v3)
                : Cb(v0, v1, v2, v3), id(-1), is_not_sliver(true), is_used(false) {}

            My_cell_base(Vertex_handle v0, Vertex_handle v1,
                Vertex_handle v2, Vertex_handle v3,
                Cell_handle   n0, Cell_handle   n1,
                Cell_handle   n2, Cell_handle   n3)
                : Cb(v0, v1, v2, v3, n0, n1, n2, n3), id(-1), is_not_sliver(true) , is_used(false){}
        }; 

        typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
        typedef My_vertex_base_3D<K>               Vb3D;
        typedef My_cell_base<K>                 Fc3D;
        typedef CGAL::Triangulation_data_structure_3<Vb3D,Fc3D>   Tds3D;
        typedef CGAL::Delaunay_triangulation_3<K,Tds3D>			Triangulation3D;
        typedef Triangulation3D::Point                            Point3D;

        // Used by spatial sort.
        struct PointWithId {
            Point3D pt ;
            int id ;
        } ;

        static const double mPI = 3.1415926535897932384626433832795;

        struct bindex {
            bindex(
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
            bool operator<(const bindex& rhs) const {
                if(indices[0] < rhs.indices[0]) { return true ;  }
                if(indices[0] > rhs.indices[0]) { return false ; }
                if(indices[1] < rhs.indices[1]) { return true ;  }
                return false ;
            }
            unsigned int indices[2] ;
        } ;
    }
    class MedialAxis::Implementation : public ::Geex::MA_CGAL::Triangulation3D{
    } ;
    //////////////////////////////////////////////////////////////////////////
    MedialAxis::MedialAxis(const std::vector<vec3>& vertices, 
        const double min_dihedral_angle /* = 0.01  */, 
        const double angle_condition /* = 22.5 */ ,
        const double ratio_condition /* = 8.0 */ )
        :vertices_(vertices), m_kdtree(0)
    {
        min_dihedral_angle_ = min_dihedral_angle * MA_CGAL::mPI / 180.0;
        angle_condition_ = angle_condition * MA_CGAL::mPI / 180.0;
        ratio_condition_ = ratio_condition;
        if (vertices_.size() > 3)
            compute();
    }
    //////////////////////////////////////////////////////////////////////////
    MedialAxis::~MedialAxis()
    {
        if (m_kdtree)
            delete m_kdtree;
    }
    //////////////////////////////////////////////////////////////////////////
    void MedialAxis::compute()
    {
        //std::cerr <<"====== building Medial Axis /*(using "<< vertices_.size() << " input points)*/ ... ======" << std::endl;
		std::cerr <<"====== Creating density map ... ======" << std::endl;
        Implementation *dt = build_dt();
        compute_poles(dt);
        construct_medial_axis(dt);
        delete dt;
    }
    //////////////////////////////////////////////////////////////////////////
    MedialAxis::Implementation* MedialAxis::build_dt()
    {
        unsigned int n = (unsigned int)vertices_.size();

        Implementation* impl_ = new Implementation;

        std::vector<MA_CGAL::PointWithId> sorted_points_(n);

        for(unsigned int i=0; i<n; i++) 
        {
            sorted_points_[i].pt = MA_CGAL::Point3D(vertices_[i].x, vertices_[i].y, vertices_[i].z);
            sorted_points_[i].id = i ;
        }
        std::random_shuffle(sorted_points_.begin(), sorted_points_.end());

        CGAL::CVT_spatial_sort_3( sorted_points_.begin(), sorted_points_.end(), MA_CGAL::K()  ) ;

        Implementation::Cell_handle ch;
        Implementation::Vertex_handle v;

        for (unsigned int i=0; i<n; i++) 
        {
            int id = sorted_points_[i].id ; 
            v = impl_->insert(sorted_points_[i].pt, ch);
            ch = v->cell();
            v->id = id ;
        }
        int count = 0;
        for (Implementation::Finite_cells_iterator citer = impl_->finite_cells_begin();
            citer != impl_->finite_cells_end(); citer++)
        {
            citer->id = count++;
        }
        return impl_;
    }
    //////////////////////////////////////////////////////////////////////////
    void MedialAxis::compute_poles(Implementation* dt)
    {
        unsigned int nb_vert = (unsigned int) vertices_.size();

        // compute positive poles
        std::vector<Implementation::Cell_handle> vert_cell(nb_vert);
        std::vector<double> dis(nb_vert, 0.0);
        int vid[4];
        vec3 normal;
        double quality[6];

        for (Implementation::Cell_iterator citer = dt->cells_begin();
            citer != dt->cells_end(); citer++)
        {
            vid[0] = citer->vertex(0)->id;
            vid[1] = citer->vertex(1)->id;
            vid[2] = citer->vertex(2)->id;
            vid[3] = citer->vertex(3)->id;

            if (dt->is_infinite(citer))
            {
                int inf_id = -1;
                for (int j = 0; j < 4; j++)
                {
                    if (vid[j] != -1)
                    {
                        citer->vertex(j)->is_on_convex_hull = true;
                    }
                    else
                    {
                        inf_id = j;
                    }
                }

                const int &id1 = vid[(inf_id+1)%4];
                const int &id2 = vid[(inf_id+2)%4];
                const int &id3 = vid[(inf_id+3)%4];

                normal = inf_id%2==0? 
                    normalize(cross(vertices_[id3]-vertices_[id1], vertices_[id2]-vertices_[id1])):
                normalize(cross(vertices_[id2]-vertices_[id1], vertices_[id3]-vertices_[id1]));
                citer->vertex((inf_id+1)%4)->pos_N += normal;
                citer->vertex((inf_id+2)%4)->pos_N += normal;
                citer->vertex((inf_id+3)%4)->pos_N += normal;
            }
            else
            {
                bool suc = tetra_circumcenter_squaredradius(
                    vertices_[vid[0]], vertices_[vid[1]], 
                    vertices_[vid[2]], vertices_[vid[3]], 
                    citer->circumcenter, citer->radius, quality);

                if ( suc == false ||
                    (
                    (quality[0] <= min_dihedral_angle_ ||  quality[0] >= MA_CGAL::mPI - min_dihedral_angle_) &&
                    (quality[1] <= min_dihedral_angle_ ||  quality[1] >= MA_CGAL::mPI - min_dihedral_angle_) &&
                    (quality[2] <= min_dihedral_angle_ ||  quality[2] >= MA_CGAL::mPI - min_dihedral_angle_) &&
                    (quality[3] <= min_dihedral_angle_ ||  quality[3] >= MA_CGAL::mPI - min_dihedral_angle_) &&
                    (quality[4] <= min_dihedral_angle_ ||  quality[4] >= MA_CGAL::mPI - min_dihedral_angle_) &&
                    (quality[5] <= min_dihedral_angle_ ||  quality[5] >= MA_CGAL::mPI - min_dihedral_angle_) 
                    )
                    )
                {
                    citer->is_not_sliver = false;
                }
                else
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if ( citer->radius > dis[ vid[j] ] )
                        {
                            dis[ vid[j] ] = citer->radius;
                            vert_cell[ vid[j] ] = citer;
                        }
                    } // end for
                } // end if
            } // end if
        } // end for

        //set positive pole and Normal
        for (Implementation::Finite_vertices_iterator viter = dt->finite_vertices_begin();
            viter != dt->finite_vertices_end(); viter++)
        {
            if (viter->is_on_convex_hull)
            {
                viter->pos_N = normalize(viter->pos_N);
                viter->has_positive_pole = false; // it just means that viter has no finite positive pole.
            }
            else
            {
                if (vert_cell[viter->id]->id != -1)
                {
                    viter->positive_pole = vert_cell[viter->id]->circumcenter;
                    viter->pos_N = normalize(viter->positive_pole - vertices_[viter->id]);
                }
                else
                {
                    viter->has_positive_pole = false;
                    std::cerr <<"this vertex has no positive pole, please check whether minimal dihedral angle is too big !" << std::endl;
                }
            }
        }
    }

	//void MedialAxis::filter_poles(double scale, std::vector<vec3>& filtered_poles, std::vector<vec3>& filtered_radius)
	//	) {
 //       unsigned int n = (unsigned int)used_poles.size();

 //       Implementation* impl_ = new Implementation;

 //       std::vector<MA_CGAL::PointWithId> sorted_points_(n);

 //       for(unsigned int i=0; i<n; i++) 
 //       {
 //           sorted_points_[i].pt = MA_CGAL::Point3D(used_poles[i].x, used_poles[i].y, used_poles[i].z);
 //           sorted_points_[i].id = i ;
 //       }
 //       std::random_shuffle(sorted_points_.begin(), sorted_points_.end());

 //       CGAL::CVT_spatial_sort_3( sorted_points_.begin(), sorted_points_.end(), MA_CGAL::K()  ) ;

 //       Implementation::Cell_handle ch;
 //       Implementation::Vertex_handle v;

 //       for (unsigned int i=0; i<n; i++) 
 //       {
 //           int id = sorted_points_[i].id ; 
 //           v = impl_->insert(sorted_points_[i].pt, ch);
 //           ch = v->cell();
 //           v->id = id ;
 //       }
 //       int count = 0;
 //       for (Implementation::Finite_cells_iterator citer = impl_->finite_cells_begin();
 //           citer != impl_->finite_cells_end(); citer++)
 //       {
 //           citer->id = count++;
 //       }

	//	for(Implementation::Finite_vertices_iterator vit=impl_->finite_vertices_begin(); vit!=impl_->finite_vertices_end(); ++vit) {
	//		int  id = vit->id ;
	//		bool valid = true ; 
	//		Implementation::Vertex_circulator cir = vit->incident_vertices() ;
	//		do {
	//			int cid = cir->id ;
	//			if(distance2(to_geex(vit->point()), to_geex(cir->point()) < pole_radius[cid]*scale) {
	//				valid = false ;
	//				break ;
	//			}
	//		} while(cir!= vit->incident_vertices()) ;

	//		if(valid) {
	//			filtered_poles.push_back(used_poles[id]) ;
	//			filtered_radius.push_back(used_pole_radius[id]) ;
	//		}
	//	}
	//}

    //////////////////////////////////////////////////////////////////////////
    void MedialAxis::construct_medial_axis(Implementation* dt)
    {
        medialaxis_voronoi_facets.clear();
        vertice_to_voronoi_faces.clear();
        vertice_to_voronoi_faces.resize(dt->number_of_finite_cells());

        std::set<MA_CGAL::bindex> edgeset;

        int id[2];
        int valid_edge_count = 0;
        for (Implementation::Finite_edges_iterator eiter = dt->finite_edges_begin();
            eiter != dt->finite_edges_end(); eiter++)
        {
            const Implementation::Cell_handle ch = eiter->first;
            id[0] = eiter->second;
            id[1] = eiter->third;

            //skip the boundary edge
            if (ch->vertex(id[0])->is_on_convex_hull && ch->vertex(id[1])->is_on_convex_hull)
            {
                continue;
            }
            //skip the existing valid edge
            if (edgeset.find(MA_CGAL::bindex(ch->vertex(id[0])->id, ch->vertex(id[1])->id)) != 
                edgeset.end() )
            {
                continue;
            }

            vec3 edge_ = vertices_[ch->vertex(id[0])->id] - vertices_[ch->vertex(id[1])->id];
            double edge_len = edge_.length();
            vec3 edge_direction = (1./ edge_len) * edge_ ;

            bool is_valid_edge = false;

            double cos_angle = cos(MA_CGAL::mPI/2 - angle_condition_);

            for (int j = 0; j < 2; j++)
            {
                Implementation::Vertex_handle v = ch->vertex(id[j]);
                if (!v->has_positive_pole)
                    continue;

                std::list<Implementation::Facet> facets;
                dt->incident_facets(v, std::back_inserter(facets));
                double min_cos_angle = DBL_MAX;
                double min_ratio = DBL_MAX;
                bool has_valid_face = false;
                for (std::list<Implementation::Facet>::iterator fiter = facets.begin();
                    fiter != facets.end(); fiter++)
                {
                    const Implementation::Facet& f = *fiter;
                    const Implementation::Cell_handle cch = f.first;
                    const Implementation::Cell_handle rch = dt->mirror_facet(f).first;
                    //skip boundary and infinite faces
                    if (dt->is_infinite(cch) || dt->is_infinite(rch))
                    {
                        continue;
                    }
                    //check whether the face belongs to the umbrella 
                    if ( dot(cch->circumcenter - vertices_[v->id], v->pos_N) * 
                        dot(rch->circumcenter - vertices_[v->id], v->pos_N) > 0 )
                    {
                        continue;
                    }

                    const Implementation::Vertex_handle v1 = cch->vertex((f.second+1)%4);
                    const Implementation::Vertex_handle v2 = cch->vertex((f.second+2)%4);
                    const Implementation::Vertex_handle v3 = cch->vertex((f.second+3)%4);
                    //check angle condition
                    vec3 face_normal = normalize(
                        cross(vertices_[v1->id]-vertices_[v2->id], vertices_[v1->id]-vertices_[v3->id]));
                    min_cos_angle = gx_min(min_cos_angle, fabs(dot(face_normal, edge_direction)) );
                    //check ratio condition
                    min_ratio = gx_min(min_ratio, edge_len /(vertices_[v1->id] - 
                        tri_circumcenter(vertices_[v1->id], vertices_[v2->id], vertices_[v3->id]) ).length());
                    has_valid_face = true;
                }

                if (has_valid_face && (min_cos_angle >= cos_angle || min_ratio > ratio_condition_))
                {
                    is_valid_edge = true;
                    break;
                }
            }
            //add the voronoi cells
            if (is_valid_edge)
            {
                edgeset.insert(MA_CGAL::bindex(ch->vertex(id[0])->id, ch->vertex(id[1])->id));
                //add voronoi cells
                Implementation::Cell_circulator cc = dt->incident_cells(eiter->first, eiter->second, eiter->third);
                Implementation::Cell_circulator cstart = cc;
                std::list<int> vface;
                do 
                {
                    vface.push_back(cc->id);
                    vertice_to_voronoi_faces[cc->id].push_back(valid_edge_count);
                    cc->is_used = true;
                    cc++;
                } while (cc != cstart);
                medialaxis_voronoi_facets.push_back(vface);
                valid_edge_count++;
            }
        }



        //store poles and change id.
        used_poles.clear();
		used_pole_radius.clear() ;
        std::vector<int> vertexmap(dt->number_of_finite_cells(), -1);
        int start = 0;
        for (Implementation::Finite_cells_iterator citer = dt->finite_cells_begin();
            citer != dt->finite_cells_end(); citer++)
        {
            if (citer->is_used)
            {
                used_poles.push_back(citer->circumcenter);
				used_pole_radius.push_back(sqrt(citer->radius)) ;
                vertexmap[citer->id] = start++;
            }
        }

        for (std::vector< std::list<int> >::iterator liter = medialaxis_voronoi_facets.begin();
            liter != medialaxis_voronoi_facets.end(); liter++)
        {
            for (std::list<int>::iterator miter = liter->begin(); 
                miter != liter->end(); miter++)
            {
                *miter = vertexmap[*miter];
            }
        }

        //create_kdtree
        if (m_kdtree)
            delete m_kdtree;
        if (!used_poles.empty())
        {
            m_kdtree = new ANNKdTree_3(&used_poles[0].x, (int)used_poles.size());
            //std::cerr << used_poles.size() <<" poles are used " << std::endl;
            //std::cerr << "The medial axis has " << valid_edge_count <<" Voronoi faces" << std::endl;
            //std::cerr << "===========================" << std::endl;
        }
        else
        {
            m_kdtree = 0;
            std::cerr << "Kdtree is not created. The input points may be co-spherical" << std::endl;
        }

    }
    //////////////////////////////////////////////////////////////////////////
    void MedialAxis::export_medial_axis_as_OBJ(const char filename[])
    {
        std::ofstream mout(filename);
        for (int i = 0; i < (int)used_poles.size(); i++)
        {
            mout <<"v " << used_poles[i] << std::endl;
        }
        for (std::vector< std::list<int> >::iterator liter = medialaxis_voronoi_facets.begin();
            liter != medialaxis_voronoi_facets.end(); liter++)
        {
            mout <<"f";
            for (std::list<int>::iterator miter = liter->begin(); 
                miter != liter->end(); miter++)
            {
                mout << ' ' << (*miter)+1;
            }
            mout << std::endl;
        }
        mout.close();
    }
    //////////////////////////////////////////////////////////////////////////
    real MedialAxis::squared_lfs(const vec3& p)
    {
        vec3 foot;
        return squared_lfs(p, foot);
    }
    //////////////////////////////////////////////////////////////////////////
    real MedialAxis::squared_lfs(const vec3& p, vec3& foot)
    {
        if (m_kdtree == 0)
        {
            return 1;
        }
        double dist = 0;
        int nearest_id = m_kdtree->queryNearestNeighbor(p.x, p.y, p.z, dist);
        foot = used_poles[nearest_id];

        if (vertice_to_voronoi_faces[nearest_id].empty())
        {
            return dist;
        }
        else
        {
            vec3 normal;
            for(std::list<int>::iterator liter = vertice_to_voronoi_faces[nearest_id].begin();
                liter != vertice_to_voronoi_faces[nearest_id].end(); liter++)
            {
                //find the nearest point on the voronoi face
                std::vector<vec3> points;
                int psize = (int)medialaxis_voronoi_facets[*liter].size();
                points.reserve(psize);

                for (std::list<int>::iterator fiter = medialaxis_voronoi_facets[*liter].begin();
                    fiter != medialaxis_voronoi_facets[*liter].end(); fiter++)
                {
                    points.push_back(used_poles[*fiter]);
                }
                normal = cross(points[1]-points[2],points[0]-points[2]);
                double normal_len = normal.length2();
                if (normal_len == 0)
                    continue; //degenerate case
                else
                    normal = (1./normal_len) * normal;
                double ortho_dis = dot(normal, p) - dot(normal, points[0]);
                double squared_ortho_dis = ortho_dis * ortho_dis;

                if (squared_ortho_dis > dist)
                {
                    continue;
                }

                double last_area = 1;
                bool is_inside = true;
                for (int i = 0; i <= psize; i++)
                {
                    double area = dot(normal, cross(points[(i+1)%psize]-p, points[i%psize]-p));
                    if (i > 0 && last_area * area < 0)
                    {
                        //is outside
                        is_inside = false;
                        break;
                    }
                    else
                    {
                        last_area = area;
                    }
                }

                if (is_inside)
                {
                    if (squared_ortho_dis < dist)
                    {
                        dist = squared_ortho_dis;
                        foot = p - ortho_dis * normal;
                    }
                }
                else
                {
                    //search nearest points at edges
                    vec3 tmp_foot;
                    for (int i = 0; i < psize; i++)
                    {
                        vec3 e = points[(i+1)%psize] - points[i];
                        double t = - dot(e, points[i] - p) / e.length2();
                        if ( t < 0)
                        {
                            t = 0;
                        }
                        else if (t > 1)
                        {
                            t = 1;
                        }
                        tmp_foot = t*e + points[i];
                        double mdis = (tmp_foot - p).length2();
                        if (mdis < dist)
                        {
                            dist = mdis;
                            foot = tmp_foot;
                        }
                    }
                }
            }
            return dist;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MedialAxis::export_medial_axis(const char filename[])
    {
        std::ofstream mout(filename);
        mout << used_poles.size() << ' ' << medialaxis_voronoi_facets.size() << std::endl;

        for (int i = 0; i < (int)used_poles.size(); i++)
        {
            mout << used_poles[i] << std::endl;
        }
        for (std::vector< std::list<int> >::iterator liter = medialaxis_voronoi_facets.begin();
            liter != medialaxis_voronoi_facets.end(); liter++)
        {
            mout << liter->size();
            for (std::list<int>::iterator miter = liter->begin(); 
                miter != liter->end(); miter++)
            {
                mout << ' ' << *miter;
            }
            mout << std::endl;
        }
        mout.close();
    }
    //////////////////////////////////////////////////////////////////////////
    void MedialAxis::import_medial_axis(const char filename[])
    {
        std::ifstream minfile(filename);
        int nv, nf;
        minfile >> nv >> nf;
        if (nv > 0)
        {
            used_poles.resize(nv);
            for (int i = 0; i < nv; i++)
            {
                minfile >> used_poles[i];
            }

            if (m_kdtree)
                delete m_kdtree;

            m_kdtree = new ANNKdTree_3(&used_poles[0].x, (int)used_poles.size());

            medialaxis_voronoi_facets.clear();
            vertice_to_voronoi_faces.clear();
            vertice_to_voronoi_faces.resize(nv);
            int np, tmp;
            for (int i = 0; i < nf; i++)
            {
                minfile >> np;
                std::list<int> vface;
                for (int j = 0; j < np; j++)
                {
                    minfile >> tmp;
                    vface.push_back(tmp); 
                    vertice_to_voronoi_faces[tmp].push_back(i);
                }
                medialaxis_voronoi_facets.push_back(vface);
            }
            std::cerr << used_poles.size() <<" poles are used " << std::endl;
            std::cerr << "The medial axis has " << medialaxis_voronoi_facets.size() <<" Voronoi faces" << std::endl;
        }
        minfile.close();
    }
    //////////////////////////////////////////////////////////////////////////
}
