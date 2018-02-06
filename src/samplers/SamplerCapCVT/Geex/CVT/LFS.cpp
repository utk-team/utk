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

#include <Geex/CVT/LFS.h>
#include <Geex/CVT/ann_kdtree.h>
#include <Geex/CVT/delaunay.h>

namespace Geex {
    LFS::LFS() {
        kdtree_ = nil ;
    }

    LFS::LFS(const std::vector<vec3>& vertices, 
		double sliver_quality /*= 0.01*/,
		const bool* feature_label /*= 0*/) {
        kdtree_ = nil;
        compute(vertices, sliver_quality, feature_label);
    }

    LFS::~LFS() {
        if(kdtree_!= nil) delete kdtree_ ;
    }

    void LFS::compute(const std::vector<vec3>& vertices, 
		double sliver_quality_degree /*= 0.01*/,
		const bool* feature_label /*= 0*/) {
        Delaunay* del = Delaunay::create("CGAL_spatial_sort") ;
        del->set_vertices(vertices) ;
        std::vector<int> tets ;
        del->get_tetras(tets, false) ;
        unsigned int nb_tets = (unsigned int)tets.size() / 4 ;

        //compute tets' circumcenter and radius
        std::vector<vec3> tet_circumcenter(nb_tets);
        std::vector<double> tet_radius(nb_tets);
        std::vector<bool> used_tet(nb_tets, false); // to avoid multiple insertion

        int nv = (int)vertices.size();
        poles.clear();
        poles.reserve(nv*2);
        std::vector<double> dis(nv, 0);
        std::vector<int> tetid(nv, -1);
        std::vector<bool> infinite_flag(nv, false);
        std::vector<bool> infinite_tet(nb_tets, false);
        N.clear();
        N.resize(nv);
        std::fill(N.begin(),N.end(), vec3(0,0,0)); // N x + d = 0 is a plane equation
        std::vector<double> d(nv, 0);
        std::vector<bool> good_tetra(nb_tets, true); 

        int start = 0;
        int vertex_id[4];
        vec3 normal;
        double quality[6];
        int bad_tetra = 0;
        const double mPI = 3.1415926535897932384626433832795;
        const double sliver_quality = sliver_quality_degree / 180. * mPI;
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
                        bool suc = tetra_circumcenter_squaredradius(
                            vertices[tets[start]], 
                            vertices[tets[start+1]], 
                            vertices[tets[start+2]],
                            vertices[tets[start+3]], 
                            tet_circumcenter[i], tet_radius[i], quality);
						if ( suc == false ||
							(
								(quality[0] <= sliver_quality ||  quality[0] >= mPI - sliver_quality) &&
								(quality[1] <= sliver_quality ||  quality[1] >= mPI - sliver_quality) &&
								(quality[2] <= sliver_quality ||  quality[2] >= mPI - sliver_quality) &&
								(quality[3] <= sliver_quality ||  quality[3] >= mPI - sliver_quality) &&
								(quality[4] <= sliver_quality ||  quality[4] >= mPI - sliver_quality) &&
								(quality[5] <= sliver_quality ||  quality[5] >= mPI - sliver_quality) 
							)
						   )
                        {
                            good_tetra[i] = false;
                            bad_tetra++;
                        }
                        

                        //set dis
                        if (good_tetra[i])
                        {
                            for (int j = 0; j < 4; j++)
                            {
                                if ( tet_radius[i] > dis[ vertex_id[j] ] )
                                {
                                    dis[ vertex_id[j] ] = tet_radius[i];
                                    tetid[ vertex_id[j] ] = i;
                                }
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

        std::cerr <<"Total tetrahedrons (including infinite tetras): " << nb_tets << std::endl;
        std::cerr <<"detected " << bad_tetra << " sliver tetrahedrons (min dihedral angle = " << sliver_quality_degree <<" degree" << std::endl;
        //set poles, first round
        for (int i = 0; i < nv; i++)
            {
                if (!infinite_flag[i])
                    {
                        if ( used_tet[tetid[i]] == false && 
							(feature_label == 0 || feature_label[i] == false))
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
                if (!infinite_tet[i]  && good_tetra[i])
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
                if ( tetid[i] >= 0  && used_tet[tetid[i]] == false &&
					(feature_label == 0 || feature_label[i] == false))
                    {
                        poles.push_back( tet_circumcenter[ tetid[i] ] );
                        used_tet[tetid[i]] = true;
                    }
            }
        delete del ;
        create_kdtree();
    }

    real LFS::squared_lfs(const vec3& p) {
		if (kdtree_)
			return kdtree_->queryNearestDist(&p.x);
		else
		{
			std::cerr <<"The kdtree does not exist !" << std::endl;
			return 0;
		}
	}

    real LFS::squared_lfs(const double* p) 
    {
		if (kdtree_)
			return kdtree_->queryNearestDist(p);
		else
		{
			std::cerr <<"The kdtree does not exist !" << std::endl;
			return 0;
		}
    }

    real LFS::squared_lfs(real x, real y, real z)
    {
        double squared_dis = 0;
		if (kdtree_)
	        kdtree_->queryNearestNeighbor(x, y, z, squared_dis);
		else
			std::cerr <<"The kdtree does not exist !" << std::endl;
        return squared_dis;
    }
    void LFS::create_kdtree()
    {
		if(kdtree_!= nil) 
			delete kdtree_ ;
		kdtree_ = nil;
        if (poles.size() >= 3) //!poles.empty())
            kdtree_ = new ANNKdTree_3(&poles[0].x, (int)poles.size()) ;
		else
			std::cerr <<"The kdtree is not created !" << std::endl;
    }
}
