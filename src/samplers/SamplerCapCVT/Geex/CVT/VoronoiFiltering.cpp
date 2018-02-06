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

#include "VoronoiFiltering.h"
#include <Geex/CVT/delaunay.h>
#include <map>
#include <stack>


namespace Geex
{
    Voronoi_Filtering::Voronoi_Filtering(const std::vector<vec3>& vertices, double input_theta_degree)
    {
        std::cerr <<"========= Start Voronoi Filtering =========" << std::endl;
        std::cerr <<"The filtering angle is " << input_theta_degree <<" degree" << std::endl;
        //1. Compute dt of sample points
        std::cerr <<"1. Compute Dt of samples points" << std::endl;
        Delaunay* del = Delaunay::create("CGAL_spatial_sort") ;
        del->set_vertices(vertices) ;
        std::vector<int> tets ;
        del->get_tetras(tets, false) ;
        unsigned int nb_tets = (unsigned int)tets.size() / 4 ;
        
        //2. Compute poles
        std::cerr <<"2. Compute poles" << std::endl;
        //compute tets' circumcenter and radius
        std::vector<vec3> tet_circumcenter(nb_tets);
        std::vector<double> tet_radius(nb_tets);
        std::vector<bool> used_tet(nb_tets, false); // to avoid multiple insertion
        int nv = (int)vertices.size();
        std::vector<vec3> poles;
        poles.reserve(3*nv);
        std::vector<double> dis(nv, 0);
        std::vector<int> tetid(nv, -1);
        std::vector<bool> infinite_flag(nv, false);
        std::vector<bool> infinite_tet(nb_tets, false);
        std::vector<vec3> N(nv, vec3(0,0,0)); // N x + d = 0 is a plane equation
        std::vector<double> d(nv, 0);
        std::vector<vec3> neg_N(nv);

        int start = 0;
        int vertex_id[4];
        vec3 normal;
        for(unsigned int i = 0; i < nb_tets; i++) 
        {
            //set id
            int inf_id = -1;
            for (int j = 0; j < 4; j++)
            {
                vertex_id[j] = tets[start+j];
                if (vertex_id[j] == -1)
                {
                    infinite_tet[i] = true;
                    inf_id = j;
                }
            }

            if (!infinite_tet[i])
            {
                tetra_circumcenter_squaredradius(
                    vertices[tets[start]], 
                    vertices[tets[start+1]], 
                    vertices[tets[start+2]],
                    vertices[tets[start+3]], 
                    tet_circumcenter[i], tet_radius[i]);

                //set dis
                for (int j = 0; j < 4; j++)
                {
                    if ( tet_radius[i] > dis[ vertex_id[j] ] )
                    {
                        dis[ vertex_id[j] ] = tet_radius[i];
                        tetid[ vertex_id[j] ] = i;
                    }
                }

            }
            else
            {
                const int &id1 = vertex_id[(inf_id+1)%4];
                const int &id2 = vertex_id[(inf_id+2)%4];
                const int &id3 = vertex_id[(inf_id+3)%4];

                infinite_flag[ id1  ] = true;
                infinite_flag[ id2  ] = true;
                infinite_flag[ id3  ] = true;
                normal = inf_id%2==0? 
                    normalize(cross(vertices[id3]-vertices[id1], vertices[id2]-vertices[id1])):
                normalize(cross(vertices[id2]-vertices[id1], vertices[id3]-vertices[id1]));
                N[id1] += normal;
                N[id2] += normal;
                N[id3] += normal;
            }

            start += 4;
        }

        //set poles, first round
        for (int i = 0; i < nv; i++)
        {
            if (!infinite_flag[i])
            {
                if ( used_tet[tetid[i]] == false)
                {
                    poles.push_back( tet_circumcenter[ tetid[i] ] );
                    used_tet[tetid[i]] = true;
                }

                N[i] = tet_circumcenter[ tetid[i] ] - vertices[i];
            }
            d[i] = -Geex::dot(N[i], vertices[i]);

        }

        //set poles, second round
        std::fill(dis.begin(),dis.end(), 0);
        std::fill(tetid.begin(),tetid.end(), -1);
        start = 0;
        for (unsigned int i = 0; i < nb_tets; i++)
        {
            if (!infinite_tet[i])
            {
                for (int j = 0; j < 4; j++)
                {
                    const int& vid = tets[start+j];
                    if ( tet_radius[i] > dis[ vid ] &&  Geex::dot(N[vid], tet_circumcenter[i])+ d[vid] < 0 )
                    {
                        dis[vid] = tet_radius[i];
                        tetid[vid] = i;
                    }
                }
            }
            start += 4;
        }
        for (int i = 0; i < nv; i++)
        {
            if ( tetid[i] >= 0  && used_tet[tetid[i]] == false)
            {
                poles.push_back( tet_circumcenter[ tetid[i] ] );
                used_tet[tetid[i]] = true;
            }
            neg_N[i] = tet_circumcenter[tetid[i]] - vertices[i];
        }
        delete del ;

        std::cerr <<"3. Compute Dt of samples points and poles\n";
        //3. computer dt of poles and vertices
        poles.insert(poles.begin(), vertices.begin(), vertices.end());
         del =  Delaunay::create("CGAL_spatial_sort");
        del->set_vertices(poles);

        std::cerr <<"4. Voronoi filtering and Filtering by Normal" << std::endl;
        //4. oronoi filtering and Filtering by Normal
        std::vector<int> facets;
        del->get_facets(facets, true);
        int nb_facets = (int)facets.size() / 3;
        delete del;

        static double m_PI = 3.1415926535897932384626433832795;
        const double filter_theta = input_theta_degree * m_PI / 180.;

        vec3 tmp_N;
        double angles[3];
        int filter_count = 0;
        std::map<VF_Edge, std::list<VF_Face*> > my_edges;
        int vid[3];
        vec3 p0, p1, p2;
        start = 0;

        int start_vertex = -1;
        VF_Edge start_edge;

        for (int i = 0; i < nb_facets; i++)
        {
            vid[0] = facets[start]; vid[1] = facets[start+1]; vid[2] = facets[start+2];
            start += 3;
            if (vid[0] >= nv || vid[1] >= nv || vid[2] >= nv )
            {
                continue;
            }
            tmp_N = normalize(cross(vertices[vid[1]]-vertices[vid[0]], vertices[vid[2]]-vertices[vid[0]]));
            angles[0] = acos(dot(tmp_N, normalize(N[vid[0]])));
            angles[1] = acos(dot(tmp_N, normalize(N[vid[1]])));
            angles[2] = acos(dot(tmp_N, normalize(N[vid[2]])));
            for (int j = 0; j < 3; j++)
            {
                if (angles[j] > m_PI/2)
                {
                    angles[j] = m_PI - angles[j];
                }
            }
            std::sort(angles, angles+3);
            if ( angles[2] >= filter_theta && angles[1] >= 2.2 * filter_theta && angles[0] >= 2.2 * filter_theta)
            {
                filter_count++;
                continue;
            }
            angles[0] = acos(dot(tmp_N, normalize(neg_N[vid[0]])));
            angles[1] = acos(dot(tmp_N, normalize(neg_N[vid[1]])));
            angles[2] = acos(dot(tmp_N, normalize(neg_N[vid[2]])));
            for (int j = 0; j < 3; j++)
            {
                if (angles[j] > m_PI/2)
                {
                    angles[j] = m_PI - angles[j];
                }
            }
            std::sort(angles, angles+3);
            if ( angles[2] >= filter_theta && angles[1] >= 2.2 * filter_theta && angles[0] >= 2.2 * filter_theta)
            {
                filter_count++;
                continue;
            }
 
            VF_Face *cur_face = new VF_Face(vid[0], vid[1], vid[2]);
			cur_face->id = (int)face_indices.size();
            face_indices.push_back(cur_face);
			
            my_edges[VF_Edge(vid[1],vid[2])].push_back(cur_face);
            my_edges[VF_Edge(vid[0],vid[2])].push_back(cur_face);
            my_edges[VF_Edge(vid[0],vid[1])].push_back(cur_face);
			cur_face->adj_edge[0] = VF_Edge(vid[1],vid[2]);
			cur_face->adj_edge[1] = VF_Edge(vid[0],vid[2]);
			cur_face->adj_edge[2] = VF_Edge(vid[0],vid[1]);
            cur_face->normal = cross( vertices[cur_face->indices[1]] - vertices[cur_face->indices[0]],
                vertices[cur_face->indices[2]] - vertices[cur_face->indices[0]]);

            for (int j = 0; j < 3; j++)
            {
                if (infinite_flag[vid[j]])
                {
                    start_vertex = vid[j];
                    start_edge = VF_Edge(vid[j], vid[(j+1)%3]);
                    break;
                }
            }
           
        }
        std::cerr << " >>>>>>> " << filter_count <<" triangles have been filtered by the input angle" << std::endl;
        std::cerr <<"5. Trimming (this part is not implemented correctly, skip it)" << std::endl;
       
        std::cerr <<"=============== Done ==================" << std::endl << std::endl;

        return;

        //5. Trimming
		std::vector<bool> visited_vertex(nv, false);
		
		//handle the starting vertex
		visited_vertex[start_vertex] = true;
	
		std::stack<VF_Face*> face_stack;
		face_stack.push(*my_edges[start_edge].begin());
        //start to propagate
		while(!face_stack.empty())
		{
			VF_Face* m_face = face_stack.top();
			face_stack.pop();

			m_face->visited = true;

			int bvertex = -1;
			for (int i = 0; i < 3; i++)
			{
				if (visited_vertex[m_face->indices[i]])
				{
					bvertex = m_face->indices[i];
					break;
				}
			}

			//check face normal
			if( dot(m_face->normal, N[bvertex]) < 0)
			{
				m_face->is_outside = false;
				m_face->normal = -m_face->normal;
			}
			//check other two vertices
			for (int i = 0; i < 3; i++)
			{
				const int &f_vid = m_face->indices[i];
				if (!visited_vertex[f_vid])
				{
					if (dot(m_face->normal, N[f_vid]) < 0)
					{
						N[f_vid] = -N[f_vid];
					}

					visited_vertex[f_vid] = true;
				}
			}
		
			for (int i = 0; i < 3; i++)
			{
				std::list<VF_Face*>& facelist = my_edges[m_face->adj_edge[i]];
				for(std::list<VF_Face*>::iterator fiter = facelist.begin();
					fiter != facelist.end(); fiter++)
				{
					VF_Face* vf = *fiter;
					if (!vf->visited)
					{
						face_stack.push(vf);
					}
				}
			}
		}
        
        
        for (std::list<VF_Face*>::iterator fiter = face_indices.begin();
            fiter != face_indices.end(); fiter++)
        {
            if ( !(*fiter)->visited )
            {
                std::cerr <<" some facets are not visited (bug issue) \n";
                break;
            }
        }
        for (int i = 0; i < (int) visited_vertex.size(); i++)
        {
            if (!visited_vertex[i])
            {
                std::cerr <<" some vertices are not visited (bug issue) \n";
                break;
            }
        }

        filter_count = 0;
		for (std::map<VF_Edge, std::list<VF_Face*> >::iterator miter = my_edges.begin();
			miter != my_edges.end(); miter++)
		{
			std::list<VF_Face*>& flist = miter->second;
			
			bool find = false;
			for (std::list<VF_Face*>::iterator fiter = flist.begin();
				fiter != flist.end(); fiter++)
			{
				VF_Face* vf = *fiter;
                std::list<VF_Face*>::iterator fiter2 = fiter;
                fiter2++;
                for (; fiter2 != flist.end(); fiter2++)
                {
                    VF_Face* vf2 = *fiter;
                    if ( dot( vf->normal, vf2->normal) < -0.5 ) // > 145 degree
                    {
                        find = true;
                        break;
                    }
                }
			}
			if (find)
			{
				for (std::list<VF_Face*>::iterator fiter = flist.begin();
					fiter != flist.end(); fiter++)
				{
					if ((*fiter)->valid)
					{
                        (*fiter)->valid = false;
                        filter_count++;
					}
				}
			}
		}
        
        std::cerr << " >>>>>>> " << filter_count <<" triangles have been filtered by trimming" << std::endl;

        std::cerr <<"=============== Done ==================" << std::endl << std::endl;
    }

    Voronoi_Filtering::~Voronoi_Filtering()
    {
        for (std::list<VF_Face*>::iterator fiter = face_indices.begin();
            fiter != face_indices.end(); fiter++)
        {
            delete *fiter;
        }
    }
}
