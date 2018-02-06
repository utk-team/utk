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

#include <Geex/CVT/topo_poly_mesh.h>
#include <Geex/CVT/oriented_line.h>
#include <Geex/basics/line_stream.h>
#include <map>

// There is a problem with the definition of that function in METIS, probably needs to be fixed.
#define __log2 __xxlog2 
extern "C" {
#include <METIS/metis.h>
}

#include <Geex/basics/file_system.h>

#include <fstream>

namespace Geex {

    static bool has_edge(
        const std::vector<unsigned int>& facet_vertex_index, 
        unsigned int facet_begin, unsigned int facet_end,
        unsigned int v1, unsigned int v2
    ) {
        unsigned int facet_n = facet_end - facet_begin ;
        for(unsigned int i=0; i<facet_n; i++) {
            unsigned int w1 = facet_vertex_index[facet_begin + i] ;
            unsigned int w2 = facet_vertex_index[facet_begin + ((i + 1) % facet_n)] ;
            if(w1 == v1 && w2 == v2) {
                return true ;
            }
        }
        return false ;
    }


    unsigned int TopoPolyMesh::load(const std::string& filename, double gamma) {
        std::cerr << "TopoPolyMesh: Loading " << filename << std::endl ;

        std::vector<double> weight ;
        std::string weights_file_name = 
            FileSystem::dir_name(filename) + "/" + 
            FileSystem::base_name(filename) + ".cur" ;
        if(FileSystem::is_file(weights_file_name)) {
            std::cerr << "Found weights, reading them" << std::endl ;
            std::ifstream w_input(weights_file_name.c_str()) ;
            if(!w_input) {
                std::cerr << "Could not open weights file" << std::endl ;
            } else {
                unsigned int nb_v ;
                w_input >> nb_v ;
	        for(unsigned int i=0; i<nb_v; i++) {
                   double w ;
                   w_input >> w ;
                   w = ::pow(w, gamma) ;
                   weight.push_back(w) ;
		}
            }
        }

        std::ifstream input(filename.c_str()) ;
        if(!input) {
            std::cerr << "could not open file." << std::endl ;
            return 0 ;
        }
        clear() ;
        LineInputStream in(input) ;
        std::vector<vec3> vertex ;
        std::vector< std::vector<unsigned int> > star ;

        // Step 1: load vertices and triangles
        // (and keep track of stars, i.e. lists
        //  of triangles incident to each vertex).
        unsigned int v_offset = 0 ;
        while(!in.eof()) {
            in.get_line() ;
            std::string keyword ;
            
            in >> keyword ;
            
            if(keyword == "v") {
                vec3 p ;
                in >> p ;
                vertex.push_back(p) ;
                star.push_back( std::vector<unsigned int>() ) ;
            } else if(keyword == "HEADER") {
                v_offset = (unsigned int)vertex.size() ;
            } else if(keyword == "VRTX" || keyword == "PVRTX" || keyword == "ATOM") {
                int idx ;
                vec3 p ;
                in >> idx >> p ;
                vertex.push_back(p) ;
                star.push_back( std::vector<unsigned int>() ) ;
            } else if(keyword == "TRGL") {
                int i1,i2,i3 ;
                in >> i1 >> i2 >> i3 ;
                std::vector<unsigned int> cur_facet ;
                cur_facet.push_back(i1-1+v_offset) ;
                cur_facet.push_back(i2-1+v_offset) ;
                cur_facet.push_back(i3-1+v_offset) ;
                unsigned int f = nb_facets() ;
                begin_facet() ;
                for(unsigned int i=0; i<cur_facet.size(); i++) {
                    unsigned int v = cur_facet[i] ;
                    if(weight.size() != 0) {
                        add_vertex(TopoPolyVertexEdge(vertex[v], weight[v])) ;
                    } else {
                        add_vertex(TopoPolyVertexEdge(vertex[v])) ;
                    }
                    top_vertex().set_flag(TopoPolyVertexEdge::ORIGINAL) ;
                    vertex_index_.push_back(v) ;
                    star[v].push_back(f) ;
                }
                end_facet() ;
            } else if(keyword == "f") {
                std::vector<int> cur_facet ;
                while(!in.eol()) {
                    std::string s ;
                    in >> s ;
                    if(s.length() > 0) {
                        std::istringstream v_input(s) ;
                        unsigned int index ;
                        v_input >> index ;
                        if(index < 1 || index > vertex.size()) {
                            std::cerr << "Out of bounds vertex index" 
                                      << std::endl ;
                        } else {
                            cur_facet.push_back(index - 1) ;
                        }
                        char c ;
                        v_input >> c ;
                        // tex vertex, ignored
                        if(c == '/') {
                            v_input >> index ;
                        }
                    }
                }
                if(cur_facet.size() < 3) {
                    std::cerr << "facet with less than 3 vertices, ignoring" 
                              << std::endl ;
                } else {
                    unsigned int f = nb_facets() ;
                    begin_facet() ;
                    for(unsigned int i=0; i<cur_facet.size(); i++) {
                        unsigned int v = cur_facet[i] ;
                        if(weight.size() != 0) {
                            add_vertex(TopoPolyVertexEdge(vertex[v], weight[v])) ;
                        } else {
                            add_vertex(TopoPolyVertexEdge(vertex[v])) ;
                        }
                        top_vertex().set_flag(TopoPolyVertexEdge::ORIGINAL) ;
                        vertex_index_.push_back(v) ;
                        star[v].push_back(f) ;
                    }
                    end_facet() ;
                } 
            }
        }
    
        original_vertices_.resize(vertex.size());
        std::copy(vertex.begin(), vertex.end(), original_vertices_.begin());
        
        gx_assert(weight.size() == 0 || (weight.size() == vertex.size())) ;
                
        // Step 2: compute facet adjacencies
        for(unsigned int f=0; f<nb_facets(); f++) {
            unsigned int facet_base = facet_begin(f) ;
            unsigned int facet_n = facet_size(f) ;

            for(unsigned int i=0; i<facet_n; i++) {
                unsigned int v1 = facet_base + i ;
                unsigned int v2 = facet_base + ((i + 1) % facet_n) ;
                unsigned int gv1 = vertex_index_[v1] ;
                unsigned int gv2 = vertex_index_[v2] ;
                const std::vector<unsigned int>& S = star[gv1] ;
                for(unsigned int k=0; k<S.size(); k++) {
                    unsigned int g = S[k] ;
                    if(
                        g != f && has_edge(
                            vertex_index_, 
                            facet_begin(g), facet_end(g), gv2, gv1
                        )
                    ) {
                        this->vertex(v1).f = g ;
                        break ;
                    }
                }
            }
        }

        // Step 3: assign facet ids
        for(unsigned int f=0; f<nb_facets(); f++) {
            facet_info(f).id = f ;
        }

        // Step 4: initialize symbolic information
        init_symbolic_vertices() ;

        // Just for checking
        unsigned int nb_borders = 0 ;
        for(unsigned int i=0; i<nb_vertices(); i++) {
            if(this->vertex(i).f < 0) {
                nb_borders++ ;
            }
        }
        double vol = signed_volume() ;
        orientation_ = (vol > 0.0) ;
        std::cerr << "TopoPolyMesh loaded, nb_facets = " << nb_facets() 
                  << " nb_borders = " << nb_borders 
                  << " signed volume = " << vol
                  << std::endl ;
        if(!orientation_ && nb_borders == 0) {
            std::cerr << " WARNING ! orientation is negative"
                      << std::endl ;
        }
        return nb_borders ;
    }
    
	unsigned int TopoPolyMesh::load(const std::vector<vec3>& surf_vertices, const std::vector<int>& surf_facets) {
        clear() ;
        std::vector<vec3> vertex ;
        std::vector< std::vector<unsigned int> > star ;

        // Step 1: load vertices and triangles
        // (and keep track of stars, i.e. lists
        //  of triangles incident to each vertex).
		for(unsigned int j=0; j<surf_vertices.size(); j++){
			vertex.push_back(surf_vertices[j]) ;
            star.push_back( std::vector<unsigned int>() ) ;
		}
		for(unsigned int i=0; i<=surf_facets.size()-3; i+=3){
			std::vector<int> cur_facet ;
			for(int k=0; k<3; k++){
				cur_facet.push_back(surf_facets[i+k]) ;
			}
			unsigned int f = nb_facets() ;
			begin_facet() ;
			for(unsigned int i=0; i<cur_facet.size(); i++) {
				unsigned int v = cur_facet[i] ;
				add_vertex(TopoPolyVertexEdge(vertex[v])) ;
				top_vertex().set_flag(TopoPolyVertexEdge::ORIGINAL) ;
				vertex_index_.push_back(v) ;
				star[v].push_back(f) ;
			}
			end_facet() ;
		}
      
        original_vertices_.resize(vertex.size());
        std::copy(vertex.begin(), vertex.end(), original_vertices_.begin());
        
        // Step 2: compute facet adjacencies
        for(unsigned int f=0; f<nb_facets(); f++) {
            unsigned int facet_base = facet_begin(f) ;
            unsigned int facet_n = facet_size(f) ;

            for(unsigned int i=0; i<facet_n; i++) {
                unsigned int v1 = facet_base + i ;
                unsigned int v2 = facet_base + ((i + 1) % facet_n) ;
                unsigned int gv1 = vertex_index_[v1] ;
                unsigned int gv2 = vertex_index_[v2] ;
                const std::vector<unsigned int>& S = star[gv1] ;
                for(unsigned int k=0; k<S.size(); k++) {
                    unsigned int g = S[k] ;
                    if(
                        g != f && has_edge(
                            vertex_index_, 
                            facet_begin(g), facet_end(g), gv2, gv1
                        )
                    ) {
                        this->vertex(v1).f = g ;
                        break ;
                    }
                }
            }
        }

        // Step 3: assign facet ids
        for(unsigned int f=0; f<nb_facets(); f++) {
            facet_info(f).id = f ;
        }

        // Step 4: initialize symbolic information
        init_symbolic_vertices() ;

        // Just for checking
        unsigned int nb_borders = 0 ;
        for(unsigned int i=0; i<nb_vertices(); i++) {
            if(this->vertex(i).f < 0) {
                nb_borders++ ;
            }
        }
        double vol = signed_volume() ;
        orientation_ = (vol > 0.0) ;
        return nb_borders ;
	}

	class faceindex {
	public:
		faceindex(
			unsigned int i,
			unsigned int j,
			unsigned int k
			) {
				indices[0] = indices_unsort[0] = i ; indices[1] = indices_unsort[1] = j ; indices[2] = indices_unsort[2] = k ;
				std::sort(indices, indices+3) ;
		}
		bool operator<(const faceindex& rhs) const {
			for(unsigned int i=0; i<3; i++) {
				if(indices[i] < rhs.indices[i]) { return true ;  }
				if(indices[i] > rhs.indices[i]) { return false ; }
			}
			return false ;
		}
		unsigned int indices[3] ;
		unsigned int indices_unsort[3];
	} ;

	void TopoPolyMesh::load_tet(const std::string& filename, std::vector<int>& tetrahedra_indice){
		std::ifstream input(filename.c_str()) ;
		if(!input) {
			std::cerr << "could not open file." << std::endl ;
			return ;
		}
		clear();
		int num_vertices = 0, num_cells = 0;
		std::string str;
		input >> num_vertices >> str >> num_cells >> str;
		std::vector<vec3> vertices;
		vertices.resize(num_vertices);
		for (int i = 0; i < num_vertices; i++)
		{
			input >> vertices[i].x >> vertices[i].y >> vertices[i].z;
		}
		tetrahedra_indice.clear();
        tetrahedra_indice.resize(num_cells*4);
		int four;
		for (int i = 0; i < num_cells; i++)
		{
			input >> four >> tetrahedra_indice[4*i] >> tetrahedra_indice[4*i+1] 
			>> tetrahedra_indice[4*i+2] >> tetrahedra_indice[4*i+3];
		}
		input.close();
		load_tet(vertices, tetrahedra_indice);
		// Just for checking
		unsigned int nb_borders = 0 ;
		for(unsigned int i=0; i<nb_vertices(); i++) {
			if(this->vertex(i).f < 0) {
				nb_borders++ ;
			}
		}
		double vol = signed_volume() ;
		orientation_ = (vol > 0.0) ;
		std::cerr << "TopoPolyMesh loaded, nb_facets = " << nb_facets() 
			<< " nb_borders = " << nb_borders 
			<< " signed volume = " << vol
			<< std::endl ;
		if(!orientation_ && nb_borders == 0) {
			std::cerr << " WARNING ! orientation is negative"
				<< std::endl ;
		}
	}

	void TopoPolyMesh::load_tet(const std::vector<vec3>& tet_vertices, const std::vector<int>& cell_indice){
		clear();
		original_vertices_.resize(tet_vertices.size());
		std::copy(tet_vertices.begin(), tet_vertices.end(), original_vertices_.begin());
		int num_vertices = (int)tet_vertices.size(), num_cells = (int)cell_indice.size()/4;
		std::map<faceindex, bool> facemap;
		static const int sortid[4][3] = {{0,2,1},{2,3,1},{1,3,0},{0,3,2}};
		unsigned int tetra[4];
		for (int i = 0; i < num_cells; i++)
		{
			tetra[0] = cell_indice[4*i]; tetra[1] = cell_indice[4*i+1];
			tetra[2] = cell_indice[4*i+2]; tetra[3] = cell_indice[4*i+3];
			for (int j = 0; j < 4; j++)
			{
				faceindex findex(tetra[sortid[j][0]],tetra[sortid[j][1]],tetra[sortid[j][2]]);
				std::map<faceindex, bool>::iterator fiter = facemap.find(findex);
				if (fiter != facemap.end())
				{
					fiter->second = false;
				}
				else
				{
					facemap[findex] = true;
				}
			}
		}
		std::vector< std::vector<unsigned int> > star(num_vertices) ;
		for (std::map<faceindex, bool>::iterator fiter = facemap.begin();
			fiter != facemap.end(); fiter++)
		{
			if (fiter->second) // the face is a boundary face
			{
				unsigned int f = nb_facets() ;
				begin_facet() ;
				for(unsigned int i=0; i<3; i++) {
					unsigned int v = fiter->first.indices_unsort[i] ;
					add_vertex(TopoPolyVertexEdge(original_vertices_[v])) ;
					top_vertex().set_flag(TopoPolyVertexEdge::ORIGINAL) ;
					vertex_index_.push_back(v) ;
					star[v].push_back(f) ;
				}
				end_facet() ;
			}
		}

		// Step 2: compute facet adjacencies
		for(unsigned int f=0; f<nb_facets(); f++) {
			unsigned int facet_base = facet_begin(f) ;
			unsigned int facet_n = facet_size(f) ;

			for(unsigned int i=0; i<facet_n; i++) {
				unsigned int v1 = facet_base + i ;
				unsigned int v2 = facet_base + ((i + 1) % facet_n) ;
				unsigned int gv1 = vertex_index_[v1] ;
				unsigned int gv2 = vertex_index_[v2] ;
				const std::vector<unsigned int>& S = star[gv1] ;
				for(unsigned int k=0; k<S.size(); k++) {
					unsigned int g = S[k] ;
					if(
						g != f && has_edge(
						vertex_index_, 
						facet_begin(g), facet_end(g), gv2, gv1
						)
						) {
							this->vertex(v1).f = g ;
							break ;
					}
				}
			}
		}

		// Step 3: assign facet ids
		for(unsigned int f=0; f<nb_facets(); f++) {
			facet_info(f).id = f ;
		}

		// Step 4: initialize symbolic information
		init_symbolic_vertices() ;
	}
	void TopoPolyMesh::init_symbolic_vertices() {
        for(unsigned int f=0; f<nb_facets(); f++) {
            for(unsigned int i1=facet_begin(f); i1<facet_end(f); i1++) {
                unsigned int i2=i1+1 ;
                if(i2 == facet_end(f)) { i2 = facet_begin(f) ; }
                // Note: Here we compute vertex(i2).sym 
                // (and we do not touch vertex(i1).sym).
                vertex(i2).sym.clear() ;
                vertex(i2).sym.add_boundary_facet(f) ;
                if(vertex(i1).f >= 0) {
                    vertex(i2).sym.add_boundary_facet(vertex(i1).f) ;
                } else {
                    // "Virtual" boundary facet: indicates edge (i1,i2 = i1 \oplus 1)
                    vertex(i2).sym.add_boundary_facet(
                        (i1 + nb_facets()) - facet_begin(f)
                    ) ;
                }
                if(vertex(i2).f >= 0) {
                    vertex(i2).sym.add_boundary_facet(vertex(i2).f) ;
                } else {
                    // "Virtual" boundary facet: indicates edge (i2,i2 \oplus 1)
                    vertex(i2).sym.add_boundary_facet(
                        (i2 + nb_facets()) - facet_begin(f) 
                    ) ;
                }
            }
        }        
    }

    void TopoPolyMesh::save(const std::string& filename) {
        std::ofstream out(filename.c_str()) ;
        for(unsigned int i=0; i<nb_vertices(); i++) {
            out << "v " << vec3(vertex(i)) << std::endl ;
        }
        for(unsigned int f=0; f<nb_facets(); f++) {
            out << "f " ;
            for(unsigned int i=facet_begin(f); i<facet_end(f); i++) {
                out << (i+1) << " " ;
            }
            out << std::endl ;
        }
    }

    void TopoPolyMesh::normalize() {
        vec3 g(0.0, 0.0, 0.0) ;
        double R = 0.0 ;
        for(unsigned int i=0; i<nb_vertices(); i++) {
            g += vertex(i) ;
        }
        if(nb_vertices() != 0) {
            g /= double(nb_vertices()) ;
        }
        for(unsigned int i=0; i<nb_vertices(); i++) {
            R = gx_max(R, length2(vertex(i) - g)) ;
        }
        R = ::sqrt(R) ;
        normalize(g, R) ;
    }

    void TopoPolyMesh::normalize(const vec3& g, double R, bool disable_normalize_) {
        if (disable_normalize_)
            return;
        
        double scal = (R != 0.0) ? (1.0 / R) : 0.0 ;
        for(unsigned int i=0; i<nb_vertices(); i++) {
            vec3 new_pos = scal * (vertex(i) - g) ;
            new_pos += vec3(1.0, 1.0, 1.0) ;
            new_pos *= 0.5 ;                                
            vertex(i).set_point(new_pos) ;
        }
    }

    void TopoPolyMesh::get_bbox(
        real& x_min, real& y_min, real& z_min,
        real& x_max, real& y_max, real& z_max
    ) {
        x_min = y_min = z_min =  1e30 ;
        x_max = y_max = z_max = -1e30 ;
        for(unsigned int i=0; i<vertex_.size(); i++) {
            x_min = gx_min(x_min, vertex_[i].x) ;
            y_min = gx_min(y_min, vertex_[i].y) ;
            z_min = gx_min(z_min, vertex_[i].z) ;
            x_max = gx_max(x_max, vertex_[i].x) ;
            y_max = gx_max(y_max, vertex_[i].y) ;
            z_max = gx_max(z_max, vertex_[i].z) ;
        }
    }


    void TopoPolyMesh::partition(
        unsigned int nb_parts_in, Array1d<TopoPolyMesh>& parts
    ) const {

        // Step 0: allocate arrays for METIS
        //   (note: if the mesh has borders, this allocates
        //     slightly more memory than necessary, 
        //     but this is not really a problem since
        //     this is deallocated right after...)
        int n = nb_facets() ;
        int nnz = nb_vertices() ;
        int* xadj = new int[n+1] ;
        int* adjncy = new int[nnz] ;
        int* part = new int[n] ;
        unsigned int* old2new = new unsigned int[n] ;

        // Step 1: copy facet graph and convert into METIS format
        //  (and skip borders !)
        int count = 0 ;
        for(int f=0; f<n; f++) {
            xadj[f] = count ;
            for(unsigned int i=facet_begin(f) ; i<facet_end(f); i++) {
                if(vertex(i).f >= 0) {
                    adjncy[count] = vertex(i).f ;
                    count++ ;
                }
            }
        }
        xadj[n] = count ;

        // Step 2: call METIS
        int options[5] ;
        options[0] = 0 ; // use default values
        int zero = 0;
        int edgecut = 0;
        int nb_parts = nb_parts_in ;

        METIS_PartGraphRecursive(
            &n, xadj, adjncy,  // The matrix
            nil, nil,          // No vertex weight, no edge weight
            &zero,             // No weights
            &zero,             // C-style indexing
            &nb_parts,
            options,
            &edgecut,
            part
        ) ;

        // Step 3: create parts
        parts.allocate(nb_parts) ;
        for(unsigned int f=0; f<nb_facets(); f++) {
            int p = part[f] ;
            old2new[f] = parts[p].nb_facets() ;
            parts[p].begin_facet() ;
            for(unsigned int i=facet_begin(f); i<facet_end(f); i++) {
                parts[p].add_vertex(vertex(i)) ;
            }
            parts[p].end_facet() ;
        }

        // Step 4: translate adjacencies
        for(int p=0; p<nb_parts; p++) {
            for(unsigned int v=0; v<parts[p].nb_vertices(); v++) {
                TopoPolyVertexEdge& ve = parts[p].vertex(v) ;
                if(ve.f >= 0) {
                    if(part[ve.f] == p) {
                        ve.f = old2new[ve.f] ;
                    } else {
                        ve.f = -1 ;
                    }
                }
            }
            // Assign facet ids
            for(unsigned int f=0; f<parts[p].nb_facets(); f++) {
                parts[p].facet_info(f).id = f ;
            }
            //   We do not call init_symbolic_vertices() on part[p],
            // since the original symbolic vertices are copied.
            // This means the symbolic vertices have the global 
            // facet IDs (this is what we need).
            //   In contrast, facet_info(f).id is a local index
            // (relative to this part), this is what the RVD algorithm
            // needs.
        }


        // Cleanup
        delete[] xadj ;
        delete[] adjncy ;
        delete[] part ;
        delete[] old2new ;
    }

  
    void TopoPolyMesh::partition_volume(const std::vector<int>& tetrahedra_indice,
		unsigned int nb_parts, Array1d<TopoPolyMesh>& parts
		)  {
            int ne = (int)tetrahedra_indice.size()/4;
            int nn = (int)original_vertices_.size();
            int etype = 2; 
            int numflag = 0;
            int nparts = nb_parts;
            int edgecut;
            int *epart = new int[ne];
            int *npart = new int[nn];
            int *indice = new int[tetrahedra_indice.size()];
            std::copy(tetrahedra_indice.begin(), tetrahedra_indice.end(), &indice[0]);
            METIS_PartMeshDual(&ne, &nn, indice, &etype, &numflag, &nparts, &edgecut, epart, npart);

            //process partitioned tets
            std::vector< std::map<int, int> > vertice_map(nb_parts);
            std::vector< std::vector<int> > vertice_indice(nb_parts);
            std::vector< unsigned int > counter(nb_parts);
            std::fill(counter.begin(), counter.end(), 0);
            std::vector<std::vector<int> > new_parts(nb_parts);
            for (unsigned int i = 0; i < nb_parts; i++)
            {
                new_parts[i].reserve(ne);
                vertice_indice[i].reserve(nn);
            }
            int current_part = -1;
            for (int i = 0; i < ne; i++)
            {
                current_part = epart[i];
                new_parts[current_part].push_back(i);
                for (int j = 0; j < 4; j++)
                {
                    int id = tetrahedra_indice[4*i+j];
                    if (vertice_map[current_part].find(id) == vertice_map[current_part].end() )

                    {
                        vertice_map[current_part][id] = counter[current_part];
                        counter[current_part]++;
                        vertice_indice[current_part].push_back(id);
                    }
                }
            }

            delete[] indice;
            delete[] epart;
            delete[] npart;


            //prepare tet_vertices and indices
            std::vector<vec3> tet_vertices;
            std::vector<int> tet_indices;
            parts.allocate(nb_parts) ;
            for (unsigned int i = 0; i < nb_parts; i++)
            {
                tet_vertices.resize(vertice_indice[i].size());
                tet_indices.resize(4*new_parts[i].size());

                for (int j = 0; j < (int)vertice_indice[i].size(); j++)
                {
                    tet_vertices[j] = original_vertices_[vertice_indice[i][j]];
                }
                for (int j = 0; j < (int)new_parts[i].size(); j++)
                {
                    int cur_cell = new_parts[i][j];
                    for (int k = 0; k < 4; k++)
                    {
                        tet_indices[4*j+k] = vertice_map[i][tetrahedra_indice[4*cur_cell+k]];

                    }
                }
                parts[i].load_tet(tet_vertices, tet_indices);
            }
    }

    class ComputeVolume {
    public:
        ComputeVolume(double& result) : result_(result) {
            result = 0.0 ;
        }
        void operator()(const vec3& p1, const vec3& p2, const vec3& p3) const {
            result_ += mixed_product(p1, p2, p3) / 6.0 ;
        }
    private:
        double& result_ ;
    } ;



    double TopoPolyMesh::signed_volume() const {
        double result = 0.0 ;
        for_each_triangle(ComputeVolume(result)) ;
        return result ;
    }

    class ComputeArea {
    public:
        ComputeArea(double& result) : result_(result) {
            result = 0.0 ;
        }
        void operator()(const vec3& p1, const vec3& p2, const vec3& p3) const {
            result_ += length(cross(p2-p1,p3-p1)) / 2.0 ;
        }
    private:
        double& result_ ;
    } ;

    double TopoPolyMesh::area() const {
        double result = 0.0 ;
        for_each_triangle(ComputeArea(result)) ;
        return result ;
    }

    inline bool line_intersects_triangle(
        const OrientedLine<real>& L, const vec3& q1, const vec3& q2,
        const vec3& p1, const vec3& p2, const vec3& p3
    ) {
        vec3 N = cross(p2-p1, p3-p1) ;
        double side1 = dot(q1-p1,N) ;
        double side2 = dot(q2-p1,N) ;
        if(
            side1 > 0 && side2 > 0 ||
            side1 < 0 && side2 < 0
        ) { 
            return false ; 
        }

        OrientedLine<real> e1(p1,p2) ;
        OrientedLine<real> e2(p2,p3) ;
        OrientedLine<real> e3(p3,p1) ;
        double s1 = side(e1, L) ;
        double s2 = side(e2, L) ;
        double s3 = side(e3, L) ;
        return (
            (s1 > 0 && s2 > 0 && s3 > 0) ||
            (s1 < 0 && s2 < 0 && s3 < 0) 
        ) ;
    }

    bool TopoPolyMesh::contains(const vec3& P) const {
        int nb_isect = 0 ;
        vec3 Q = P ;
        Q.z += 1e3 ;
        OrientedLine<real> L(P,Q) ;
        for(unsigned int f=0; f<nb_facets(); f++) {
            int i1 = facet_ptr_[f] ;
            int i2 = i1 + 1 ;
            do {
                //vec3 I ;
                if(
                    line_intersects_triangle(
                        L, P, Q, vertex_[i1], vertex_[i2], vertex_[i2+1]
                    )
                ) { nb_isect++ ; }
                i2++ ;
            } while(i2+1 != facet_ptr_[f+1]) ;
        }
        return ((nb_isect & 1) != 0) ;
    }

}
