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

#ifndef __GEEX_CVT_CLIPPED_VD__
#define __GEEX_CVT_CLIPPED_VD__

#include <Geex/CVT/RVD.h>
#include <Geex/CVT/symbolic_vertex.h>
#include <map>
#include <vector>

namespace Geex {

    /**
     * Given a TopoPolyMesh and a Delaunay,
     * ClippedVoronoiDiagram computes the
     * intersections between the interior of
     * the surface and the cells of the Voronoi
     * diagram.
     *
     *   TODO: - debug (we still have some bad configurations)
     */
    class ClippedVoronoiDiagram {
    public:

        enum FaceType {CVD_NONE, CVD_RVD, CVD_WALL, CVD_CLOSE, CVD_INSIDE} ;
        FaceType current_face_type() const { return current_face_type_ ; }

        struct Vertex : public vec3Sym {
        public:
            Vertex(int cell_id_in = -1) : visited(false), cell_id(cell_id_in) {}
            Vertex(
                const TopoPolyVertexEdge& v, int cell_id_in = -1
            ) : vec3Sym(v), visited(false), cell_id(cell_id_in) {
            }
            Vertex(const vec3& v, const SymbolicVertex& k_in, int cell_id_in = -1) : 
                vec3Sym(v,k_in), visited(false), cell_id(cell_id_in) { 
            }
            bool visited ;
			int cell_id; //if cell_id > -1, it means Vertex is the circumcenter of the cell_id-th finite cell.
        } ;

    protected:
        // Used to store the borders of the 
        // Restricted Voronoi Cells.
        typedef std::map<SymbolicVertex, Vertex> VertexMap ;

        // Used as an argument of RestrictedVoronoiDiagram::for_each_facet(),
        // stores the borders of the Restricted Voronoi Cells in an array
        // and applies the user-specified action to the facets of the restricted
        // Voronoi cells.
        template <class ACTION> class GetCellsBoundary {
        public:
            GetCellsBoundary(
                std::vector<VertexMap>& cells, const ACTION& action
            ) : cells_(cells), action_(action) {}
            void operator()(unsigned int v, TopoPolyMesh* M) const {
                if(action_.get_restricted_cells()) {
                    for(unsigned int f=0; f<M->nb_facets(); f++) {
                        for(unsigned int i1=M->facet_begin(f); 
                            i1<M->facet_end(f); i1++) {
                            if(M->vertex(i1).flags == TopoPolyVertexEdge::INTERSECT) {
                                unsigned int i2=M->next_around_facet(f,i1) ;
                                cells_[v][M->vertex(i1).sym] = Vertex(M->vertex(i2)) ;
                            }
                        }
                    }
                }
                if(action_.process_cell(v)) {
                    for(unsigned int f=0; f<M->nb_facets(); f++) {
                        const_cast<ACTION&>(action_).begin_facet(v, -1) ;
                        for(unsigned int i=M->facet_begin(f); i<M->facet_end(f); i++) {
                            const_cast<ACTION&>(action_).vertex(M->vertex(i)) ;
                        }
                        const_cast<ACTION&>(action_).end_facet() ;
                    }
                }
            }
        private:
            std::vector<VertexMap>& cells_ ;
            ACTION action_ ;
        } ;

        template <class TRIACTION> class TriangleAction {
        public:
            TriangleAction(
                const TRIACTION& action, bool recompute_restricted_cells = true
            ) : action_(action), recompute_rvc_(recompute_restricted_cells) {
            }
            bool get_restricted_cells() const             { return recompute_rvc_ ; }
            bool process_cell(unsigned int i) const       { return true ; }
            void begin_facet(int i, unsigned int j) const { vertices_.resize(0); i_ = i ; j_ = j ; }
            void vertex(const vec3Sym& v, int cell_id = -1) const           { vertices_.push_back(v) ; }
            void end_facet() const {  
                if(vertices_.size() > 2) {
                    //   Facets on the boundary, i.e. from restricted Voronoi cells,
                    // are traversed in reverse orientation, therefore we invert the
                    // triangles when traversing them.
                    if(j_ == -1) {
                        for(unsigned int i=1; i+1<vertices_.size(); i++) {
                            action_(i_, j_, vertices_[0], vertices_[i], vertices_[i+1]) ;
                        }
                    } else {
                        for(unsigned int i=1; i+1<vertices_.size(); i++) {
                            action_(i_, j_, vertices_[0], vertices_[i+1], vertices_[i]) ;
                        }
                    }
                }
            }
        protected:
            const TRIACTION& action_ ;
            bool recompute_rvc_ ;
            mutable std::vector<vec3Sym> vertices_ ;
            mutable unsigned int i_ ;
            mutable int j_ ;
        } ;
        

    public:
		ClippedVoronoiDiagram(Delaunay* delaunay, TopoPolyMesh* M) : RVD_(delaunay,M) {
			RVD_.set_symbolic(true) ;
			traverse_inner_cells_ = true ;
			geometry_ = true ;
			use_cell_id_ = false;
		}

        void set_traverse_inner_cells(bool x) {
            traverse_inner_cells_ = x ;
        }

        void set_geometry(bool x, bool use_cell_id = false) { 
            geometry_ = x ; 
			use_cell_id_ = use_cell_id;
        }

        /**
         * ACTION needs to define the following functions:
         * void operator()(
         *    unsigned int i, int j, const vec3Sym& v1, const vec3Sym& v2, const vec3Sym& v3
         * )
         */
        template <class TRIACTION> void for_each_triangle(
            const TRIACTION& action, bool recompute_rvc = true
        ) {
            for_each_facet(TriangleAction<TRIACTION>(action, recompute_rvc)) ;
        }

        /**
         * ACTION needs to define the following functions:
         * bool get_restricted_cells() -> returns true if restricted
         *                                cells need to be recomputed.
         * bool process_cell(unsigned int i) -> returns true if the cell
         *                                      associated with primal vertex i
         *                                      needs to be processed.
         * void begin_facet(unsigned int i, unsigned int j) -> starts the facet
         *                                      associated with bisector [i,j]
         *                                      j=-1 for facets on border.
         * void vertex(const vec3Sym& v, const int cell_id)
         * void end_facet()
         */
        template <class ACTION> void for_each_facet(const ACTION& action) {
            current_face_type_ = CVD_NONE ;
            if(action.get_restricted_cells()) {
                restricted_cells_.resize(RVD_.delaunay()->nb_vertices()) ;
                for(unsigned int i=0; i<restricted_cells_.size(); i++) {
                    restricted_cells_[i].clear() ;
                }
                current_face_type_ = CVD_RVD ;
                RVD_.for_each_facet(GetCellsBoundary<ACTION>(restricted_cells_, action)) ;
            }
            if(traverse_inner_cells_) {
                cell_visited_.resize(RVD_.delaunay()->nb_vertices()) ;
                cell_on_stack_.resize(RVD_.delaunay()->nb_vertices()) ;
                //std::fill(cell_visited_.begin(), cell_visited_.end(), false) ;
                //std::fill(cell_on_stack_.begin(), cell_on_stack_.end(), false) ;
				memset(&cell_visited_[0], 0, sizeof(int)*cell_visited_.size());
				memset(&cell_on_stack_[0], 0, sizeof(int)*cell_on_stack_.size());
            }
            for(unsigned int iv=0; iv<RVD_.delaunay()->nb_vertices(); iv++) {
                traverse_cell_ = action.process_cell(iv) ;
                if(traverse_cell_ || traverse_inner_cells_) {
                    current_face_type_ = CVD_WALL ;
                    begin_cell(iv) ;
                    for(VertexMap::iterator it = RVC_->begin() ; it != RVC_->end(); it++) {
                        if(!it->second.visited) { 
                            get_contour(it, action) ; 
                        }
                    }       
                    for(unsigned int i=0; i<cell_.nb_facets(); i++) {
                        close_facet(i, action) ;
                        if(facet_vertices_[i].size() > 0) {
                            unsigned int jv = cell_.facet_bisector(i) ;
                            facet_visited_.insert(jv) ;
                            if(traverse_cell_) {
                                const_cast<ACTION&>(action).begin_facet(iv_, jv) ;
                                for(unsigned int k=0; k<facet_vertices_[i].size(); k++) {
                                    const_cast<ACTION&>(action).vertex(facet_vertices_[i][k], facet_vertices_[i][k].cell_id) ;
                                }
                                const_cast<ACTION&>(action).end_facet() ;
                            }
                        }
                    }
                    if(facet_S_problem_) {
                        std::cerr << "Encountered problem with complicated cell" << std::endl ;
                        while(!facet_S_.empty()) { facet_S_.pop() ; }
                    } else {
                        current_face_type_ = CVD_CLOSE ;
                        propagate_to_adjacent_facets(action) ;
                    }
                    end_cell() ;
                    if(traverse_inner_cells_) {
						cell_visited_[iv_] = (RVC_->size() > 0) ? 1 : 0;
                    }
                }
            }
            if(traverse_inner_cells_) {
                current_face_type_ = CVD_INSIDE ;
                propagate_to_adjacent_cells(action) ;
            }
        }

    protected:

        template <class ACTION> void propagate_to_adjacent_cells(const ACTION& action) {
            // This Commented-out version uses the interior test.
            /*
            while(!cell_S_.empty()) { cell_S_.pop() ; }
            for(iv_=0; iv_<RVD_.delaunay()->nb_vertices(); iv_++) {
                if(
                    !cell_visited_[iv_] && 
                    RVD_.mesh()->contains(RVD_.delaunay()->vertex(iv_)) &&
                    action.process_cell(iv_)
                ) {
                    RVD_.delaunay()->get_voronoi_cell(iv_, cell_) ;
                    for(unsigned int fi=0; fi<cell_.nb_facets(); fi++) {
                        apply_action_to_cell_facet(fi, action) ;
                    }
                }
            }
            */
            while(!cell_S_.empty()) {
                iv_ = cell_S_.top() ;
                cell_S_.pop() ;
                if(
                    //!cell_visited_[iv_] 
					cell_visited_[iv_] == 0
                ) {
                    cell_visited_[iv_] = 1; //true ;
                    if(action.process_cell(iv_)) {
                        RVD_.delaunay()->get_voronoi_cell(iv_, cell_, geometry_, use_cell_id_) ;
                        for(unsigned int fi=0; fi<cell_.nb_facets(); fi++) {
                            apply_action_to_cell_facet(fi, action) ;
                            if(cell_.facet_bisector(fi) >= 0) {
                                push_cell(cell_.facet_bisector(fi)) ;
                            }
                        }
                    }
                }
            }
        }

        template <class ACTION> void propagate_to_adjacent_facets(const ACTION& action) {
            while(!facet_S_.empty()) {
                unsigned int f = facet_S_.top() ; facet_S_.pop() ;
                if(facet_visited_.find(f) == facet_visited_.end()) {
                    facet_visited_.insert(f) ;
                    unsigned int fi = cell_.find_facet(f) ;
                    if(traverse_cell_) {
                        apply_action_to_cell_facet(fi, action) ;
                    }
                    if(traverse_inner_cells_) {
                        push_cell(f) ;
                    } 
                    for(unsigned int i = cell_.facet_begin(fi); i<cell_.facet_end(fi); i++) {
                        int g = cell_.edge_bisector(i) ;
                        if(g >= 0 && facet_visited_.find(g) == facet_visited_.end()) {
                            facet_S_.push(g) ;
                        }
                    }
                }
            }
        }

        template <class ACTION> void apply_action_to_cell_facet(unsigned int fi, const ACTION& action) {
            unsigned int f = cell_.facet_bisector(fi) ;
            const_cast<ACTION&>(action).begin_facet(iv_, f) ;
            for(unsigned int i = cell_.facet_begin(fi); i<cell_.facet_end(fi); i++) {
                SymbolicVertex k ; 
                k.insert(cell_.edge_bisector(i)+1) ;
                k.insert(cell_.edge_bisector(cell_.prev_around_facet(fi, i))+1) ;
                k.insert(f+1) ;
                if(geometry_) {
                    const_cast<ACTION&>(action).vertex(Vertex(cell_.vertex(i),k), cell_.cell_id(i)) ;
                } else {
                    const_cast<ACTION&>(action).vertex(Vertex(vec3(0,0,0),k), cell_.cell_id(i)) ;
                }
            }
            const_cast<ACTION&>(action).end_facet() ;
        }

        // initiates computation of cell associated with primal vertex iv.
        void begin_cell(unsigned int iv) {
            iv_ = iv ;
            RVC_ = &(restricted_cells_[iv]) ;
            RVD_.delaunay()->get_voronoi_cell(iv, cell_, true, use_cell_id_) ;
            facet_vertices_.resize(cell_.nb_facets()) ;
            for(unsigned int i=0; i<facet_vertices_.size(); i++) {
                facet_vertices_[i].resize(0) ;
            }
            facet_first_.resize(cell_.nb_facets()) ;
            facet_cur_.resize(cell_.nb_facets()) ;
            facet_enter_.resize(cell_.nb_facets()) ;
            facet_visited_.clear() ;
            //std::fill(facet_first_.begin(), facet_first_.end(), -1) ;
            //std::fill(facet_cur_.begin(), facet_cur_.end(), -1) ;
            //std::fill(facet_enter_.begin(), facet_enter_.end(), -1) ;
			memset(&facet_first_[0], 0, sizeof(unsigned int)*facet_first_.size());
			memset(&facet_cur_[0], 0, sizeof(unsigned int)*facet_cur_.size());
			memset(&facet_enter_[0], 0, sizeof(unsigned int)*facet_enter_.size());
            facet_S_problem_ = false ;
        }

        void end_cell() {
            for(VertexMap::iterator it = RVC_->begin(); it != RVC_->end(); it++) {
                it->second.visited = false ;
            }
        }
        
        bool has_several_contours() {
            VertexMap::iterator it = RVC_->begin() ;
            unsigned int count = 0 ;
            do {
                count++ ;
                it = next(it) ;
            } while(it != RVC_->begin()) ;
            return (count < RVC_->size()) ;
        }

        unsigned int nb_contours() {
            unsigned int result = 0 ;
            for(VertexMap::iterator it = RVC_->begin(); it != RVC_->end(); it++) {
                if(!it->second.visited) {
                    result++ ;
                    VertexMap::iterator jt = it ;
                    do {
                        jt->second.visited = true ;
                        jt = next(jt) ;
                    } while(jt != it) ;
                }
            }
            for(VertexMap::iterator it = RVC_->begin(); it != RVC_->end(); it++) {
                it->second.visited = false ;
            }
            return result ;
        }


        // finds a contour (i.e. a closed loop) in the border of
        // the current Restricted Voronoi Cell.
        template <class ACTION> void get_contour(
            VertexMap::iterator from, const ACTION& action
        ) {
            from = contour_start(from) ;
            if(!is_corner(from)) {
                // Case 1: contour does not have corners, all in same bisector
                //   The contour is directly processed.
                unsigned int jv = from->second.sym[2]-1;
                facet_visited_.insert(jv) ;
                if(traverse_cell_) {
                    const_cast<ACTION&>(action).begin_facet(iv_, jv) ;
                    VertexMap::iterator it = from ;
                    do {
                        const_cast<ACTION&>(action).vertex(it->second, it->second.cell_id) ;
                        it->second.visited = true ;
                        it = next(it) ;
                    } while(it != from) ;
                    const_cast<ACTION&>(action).end_facet() ;
                }
            } else {
                // Case 2: contour has corners
                //   The contour is stored.
                VertexMap::iterator it = from ;
                int cur_edge_bisector = edge_bisector(from) ;
                gx_assert(cur_edge_bisector >= 0) ;
                unsigned int cur_facet = cell_.find_facet(cur_edge_bisector) ;
                enter_facet(cur_facet, it, action) ;
                do {
                    it->second.visited = true ;
                    // Detect whether we changed facet in the 
                    // current Voronoi cell.
                    unsigned int it_edge_bisector = edge_bisector(it) ;
                    if(it_edge_bisector != cur_edge_bisector) {
                        leave_facet(cur_facet, it, action) ;
                        cur_edge_bisector = it_edge_bisector ;
                        cur_facet = cell_.find_facet(cur_edge_bisector) ;
                        enter_facet(cur_facet, it, action) ;
                    } else {
                        facet_vertices_[cur_facet].push_back(it->second) ;
                    }
                    it = next(it) ;
                } while(it != from) ;
                leave_facet(cur_facet, it, action) ;
            }
        }

        template <class ACTION> 
        void enter_facet(unsigned int f, VertexMap::iterator it, const ACTION& action) {
            unsigned int eb = prev_edge_bisector(it) ;
            //if(facet_first_[f] == -1) {
			if(facet_first_[f] == 0) {
                facet_first_[f] = eb + 1;
                facet_enter_[f] = eb + 1;
            } else {
                connect(f, facet_cur_[f] - 1, eb, action) ;
                facet_enter_[f] = eb + 1;
            }
            facet_vertices_[f].push_back(it->second) ;
        }

        template <class ACTION>
        void leave_facet(unsigned int f, VertexMap::iterator it, const ACTION& action) {
            facet_cur_[f] = edge_bisector(it) + 1;
            facet_vertices_[f].push_back(it->second) ;
        }

        template <class ACTION>
        void close_facet(unsigned int f, const ACTION& action) {
            //if(facet_first_[f] != -1) {
			if(facet_first_[f] != 0) {
                //gx_debug_assert(facet_cur_[f] != -1) ;
				gx_debug_assert(facet_cur_[f] != 0) ;
                connect(f, facet_cur_[f] - 1, facet_first_[f] - 1, action) ;
            } 
        }

        template <class ACTION>
        void facet_component(unsigned int f, const ACTION& action) {
            if(facet_vertices_[f].size() > 0) {
                unsigned int jv = cell_.facet_bisector(f) ;
                facet_visited_.insert(jv) ;
                if(traverse_cell_) {
                    const_cast<ACTION&>(action).begin_facet(iv_, jv) ;
                    for(unsigned int k=0; k<facet_vertices_[f].size(); k++) {
                        const_cast<ACTION&>(action).vertex(facet_vertices_[f][k], facet_vertices_[f][k].cell_id) ;
                    }
                    const_cast<ACTION&>(action).end_facet() ;
                }
            }
            facet_vertices_[f].resize(0) ;
            facet_first_[f] = 0; //-1 ;
            facet_cur_[f] = 0; //-1 ;
            facet_enter_[f] = 0; //-1 ;
        }

        // Insert the vertices of the Voronoi facet that are met between
        // bisectors eb1 and eb2 when turning around the facet.
        template <class ACTION>
        void connect(unsigned int f, unsigned int eb1, unsigned int eb2, const ACTION& action) {
            unsigned int eb0 = facet_enter_[f] - 1;
            if(eb1 != eb2 && eb1 != eb0) {
                unsigned int ivv = cell_.facet_begin(f) ;
                while(cell_.edge_bisector(ivv) != eb1) {
                    ivv++ ;
                    gx_debug_assert(ivv < cell_.facet_end(f)) ;
                }
                unsigned int f_bisector = cell_.facet_bisector(f) ;
                int eb ;
                do {
                    unsigned int prev_bisector = cell_.edge_bisector(ivv) ;
                    ivv = cell_.next_around_facet(f, ivv);
                    {
                        SymbolicVertex k ; 
                        k.insert(f_bisector+1) ;
                        k.insert(prev_bisector+1) ;
                        prev_bisector = cell_.edge_bisector(ivv) ;
                        k.insert(prev_bisector+1) ;
                        facet_vertices_[f].push_back(Vertex(cell_.vertex(ivv), k, cell_.cell_id(ivv))) ; 
                    }
                    eb = cell_.edge_bisector(ivv) ;
                    if(eb != eb2 && eb != eb0) { 
                        if(eb != -1) {
                            facet_S_.push(eb) ; 
                        } else {
                            facet_S_problem_ = true ;
                        }
                    }
                } while(eb != eb2 && eb != eb0) ;
                if(eb == eb0) {
                    facet_component(f, action) ;
                }
            } else if(eb1 == eb0) {
                facet_component(f, action) ;
            }
        }

        //   For contours with corners (i.e. vertices that are on two bisectors),
        // we systematically start from a corner (makes the code simpler !)
        //   Note that all contours do not necessarily have corners (e.g. tubular
        // zones that are undersampled).
        VertexMap::iterator contour_start(VertexMap::iterator from) {
            VertexMap::iterator it = from ;
            do {
                if(is_corner(it)) { return it ; }
                it = next(it) ;
            } while(it != from) ;
            return it ;
        }

        VertexMap::iterator next(VertexMap::iterator from) {
            VertexMap::iterator result = RVC_->find(from->second.sym) ;
            gx_debug_assert(result != RVC_->end()) ;        
            return result ;
        }

        // Corners are vertices that are on two bisectors.
        bool is_corner(VertexMap::iterator it) {
            const SymbolicVertex& s = it->second.sym ;
            return (s.size() == 3) && (s[1] > 0) && (s[2] > 0) ;
        }

        unsigned int edge_bisector(VertexMap::iterator it) {
            VertexMap::iterator jt = next(it) ;
            return edge_bisector(it->second.sym, jt->second.sym) ;
        }

        unsigned int prev_edge_bisector(VertexMap::iterator it) {
            return edge_bisector(it->first, it->second.sym) ;
        }

        // finds the bisector common to two vertices.
        static unsigned int edge_bisector(const SymbolicVertex& s1, const SymbolicVertex& s2) {
            SymbolicVertex I ;
            sets_intersect(s1, s2, I) ;
            // I is supposed to have one negative and one positive value,
            // the positive one is the last one
            gx_debug_assert(I.size() == 2) ;
            gx_debug_assert(I[0] < 0) ;
            gx_debug_assert(I[1] > 0) ;                            
            return I[1]-1 ;
        }

        void push_cell(unsigned int i) {
            //if(!cell_on_stack_[i] && !cell_visited_[i]) {
			  if(cell_on_stack_[i] == 0 && cell_visited_[i] == 0) {
                cell_on_stack_[i] =  1; //true ;
                cell_S_.push(i) ;
            }
        }

    private:
        unsigned int iv_ ;
        RestrictedVoronoiDiagram_poly RVD_ ;
        bool traverse_cell_ ;
        VoronoiCell cell_ ;
        std::vector<VertexMap> restricted_cells_ ;
        VertexMap* RVC_ ;
        std::vector<unsigned int> facet_first_ ;
        std::vector<unsigned int> facet_cur_ ;
        std::vector<unsigned int> facet_enter_ ;
        std::vector< std::vector<Vertex> > facet_vertices_ ;
        std::set<unsigned int> facet_visited_ ;
        std::stack<unsigned int> facet_S_ ;
        bool facet_S_problem_ ;
        bool traverse_inner_cells_ ;

        std::stack<unsigned int> cell_S_ ;
        //std::vector<bool> cell_visited_ ;
        //std::vector<bool> cell_on_stack_ ;
        std::vector<unsigned int> cell_visited_ ;
        std::vector<unsigned int> cell_on_stack_ ;
        bool geometry_, use_cell_id_;
        FaceType current_face_type_ ;
    } ;

}

#endif
