
#include "polygons.h"
#include <Geex/basics/line_stream.h>
#include <Geex/basics/file_system.h>
#include <iostream>
#include <fstream>

namespace Geex {


    //====================================================================================================

    void Polygon::load(const std::string& filename) {
        clear() ;
        std::ifstream in(filename.c_str()) ;
        if(!in) {
            std::cerr << "could not open " << filename << std::endl ;
            return ;
        }
        vec2 p ;
        while(in) {
            in >> p ;
            if(in) { push_back(p) ; }
        }
        std::cerr << "Loaded " << size() << " vertices" << std::endl ;
        for(int i=0; i<int(size()); i++) {
            int j = i-1 ;
            if(j < 0) { j = size() - 1 ; }
            (*this)[i].boundary_edges.insert(i) ;
            (*this)[i].boundary_edges.insert(j) ;
        }
    }


    bool Polygon::contains(const vec2& p1) {
        int count = 0 ;
        vec2 p2 = p1 ; p2.x = 1e3 ;
        for(unsigned int i=0; i<size(); i++) {
            unsigned int j = i+1 ;
            if(j == size()) { j = 0 ; }
            if(seg_seg_intersect(p1, p2, (*this)[i], (*this)[j])) {
                count++ ;
            }
        }
        return ((count & 1) != 0) ; 
    }


    Line<real> Polygon::edge_line(unsigned int i) const {
        gx_assert(i < size()) ;
        int j = (i + 1) ; 
        if(j == size()) { j = 0 ; }
        const vec2& p1 = (*this)[i] ;
        const vec2& p2 = (*this)[j] ;
        vec2 V = p2 - p1 ;
        return Line<real>(p1, vec2(-V.y, V.x)) ;
    }


    void Polygon::normalize() {
        vec2 g(0.0, 0.0) ;
        double cnt = 0.0 ;
        for(unsigned int i=0; i<size(); i++) {
            g += (*this)[i] ;
            cnt += 1.0 ;
        }
        if(cnt != 0.0) {
            g = (1.0 / cnt) * g ;
        }
        double R = 0.0 ;
        for(unsigned int i=0; i<size(); i++) {
            (*this)[i] -= g ;
            R = gx_max(R,((*this)[i]).length()) ;
        }
        double scal = (R != 0.0) ? (10.0 / R) : 0.0 ;
        for(unsigned int i=0; i<size(); i++) {
            (*this)[i] *= scal ;
            (*this)[i] += vec2(1.0, 1.0) ;
            (*this)[i] *= 0.5 ;                
        }        
    }


    static inline int ogf_sign(double x) {
        return x > 0 ? 1 : (x < 0 ? -1 : 0) ;
    }

    void Polygon::convex_clip(Polygon& to, const Line<real>& L, int E) {
        to.clear() ;
        if(size() == 0) { return ; }
        if(size() == 1) {
            if((L.side((*this)[0])) > 0) {
                to.push_back((*this)[0]) ;
            }
            return ;
        }
        PolygonVertex prev_p = (*this)[size() - 1] ;
        int prev_status = ogf_sign(L.side(prev_p)) ;

        for(unsigned int i=0; i<size(); i++) {
            PolygonVertex p = (*this)[i] ;
            int status = ogf_sign(L.side(p)) ;
            if(
                status != prev_status &&
                status != 0 &&
                prev_status != 0
            ) {
                PolygonVertex intersect ;
                if(intersect_seg_line(prev_p, p, L, intersect)) {
                    sets_intersect(prev_p.bisectors, p.bisectors, intersect.bisectors) ;
                    sets_intersect(prev_p.boundary_edges, p.boundary_edges, intersect.boundary_edges) ;
                    intersect.bisectors.insert(E) ;
                    to.push_back(intersect) ;
                } 
            }

            switch(status) {
            case -1:
                break ;
            case 0:
                to.push_back(p) ;
                break ;
            case 1:
                to.push_back(p) ;
                break ;
            }
            prev_p = p ;
            prev_status = status ;
        }
    }

    //====================================================================================================
        
    void Polygon2::load(const std::string& filename) {
        clear() ;

        std::vector<vec2> vertices ;
        std::ifstream input(filename.c_str()) ;
        if(!input) {
            std::cerr << "Could not open " << filename << std::endl ;
            return ;
        }
        LineInputStream in(input) ;
        while(!in.eof()) {
            in.get_line() ;
            std::string keyword ;
            in >> keyword ;
            if(keyword == "v") {
                vec2 p ;
                in >> p ;
                vertices.push_back(p) ;
            } else if(keyword == "e" || keyword == "s") {
                int v1,v2 ;
                in >> v1 >> v2 ;
                v1-- ; v2-- ;
                if(v1 < 0 || v2 < 0 || v1 >= vertices.size() || v2 >= vertices.size()) {
                    std::cerr << "Invalid edge indices: " << v1 << " " << v2 << std::endl ;
                } else {
                    PolygonEdge E ;
                    E.vertex[0] = vertices[v1] ;
                    E.vertex[1] = vertices[v2] ;

                    if((E.vertex[1] - E.vertex[0]).length() < 1e-20) {
                        std::cerr << "small edge" << std::endl ;
                    }

                    push_back(E) ;
                }
            } 
        }

        // Note: we only insert *this* edge into
        // its extremities's intersection sets, since
        // the neighboring ones cannot participate in
        // intersection computations.
        for(unsigned int i=0; i<size(); i++) {
            (*this)[i].vertex[0].boundary_edges.insert(i) ;
            (*this)[i].vertex[1].boundary_edges.insert(i) ;
            (*this)[i].vertex[0].on_boundary = true ;
            (*this)[i].vertex[1].on_boundary = true ;
        }

        std::cerr << "Polygon2: loaded " << size() << " edges" << std::endl ;
    }
    
    void Polygon2::normalize() {
        vec2 g(0.0, 0.0) ;
        double cnt = 0.0 ;
        for(unsigned int i=0; i<size(); i++) {
            g += (*this)[i].vertex[0] ;
            g += (*this)[i].vertex[1] ;
            cnt += 2.0 ;
        }
        if(cnt != 0.0) {
            g = (1.0 / cnt) * g ;
        }
        double R = 0.0 ;
        for(unsigned int i=0; i<size(); i++) {
            (*this)[i].vertex[0] -= g ;
            (*this)[i].vertex[1] -= g ;
            R = gx_max(R,((*this)[i].vertex[0]).length()) ;
            R = gx_max(R,((*this)[i].vertex[1]).length()) ;
        }
        double scal = (R != 0.0) ? (1.0 / R) : 0.0 ;
        for(unsigned int i=0; i<size(); i++) {
            for(unsigned int j=0; j<2; j++) {
                (*this)[i].vertex[j] *= scal ;
                (*this)[i].vertex[j] += vec2(1.0, 1.0) ;
                (*this)[i].vertex[j] *= 0.5 ;         
            }       
        }        
    }
   
    bool Polygon2::contains(const vec2& p1) {
        int count = 0 ;
        vec2 p2 = p1 ; p2.x = 1e3 ;
        for(unsigned int i=0; i<size(); i++) {
            if(seg_seg_intersect(p1, p2, (*this)[i].vertex[0], (*this)[i].vertex[1])) {
                count++ ;
            }
        }
        return ((count & 1) != 0) ; 
    }
    
    void Polygon2::convex_clip(Polygon2& to, const Line<real>& L, int E, bool close) {
        to.clear() ;

        std::vector<PolygonVertex> from_vertices ;
        std::vector<PolygonVertex> to_vertices ;

        for(unsigned int i=0; i<size(); i++) {
            const PolygonEdge& e = (*this)[i] ;

            double s1 = L.side(e.vertex[0]) ;
            double s2 = L.side(e.vertex[1]) ;

            int config = 
                ((s1 > 0.0) ? 1 : 0) |
                ((s2 > 0.0) ? 2 : 0) ;


            switch(config) {

            case 0: {
            } break ;

            case 1: {  // Case 1: from inside to outside
                PolygonVertex I ;
                if(intersect_seg_line(e.vertex[0], e.vertex[1], L, I)) {
                    sets_intersect(e.vertex[0].bisectors, e.vertex[1].bisectors, I.bisectors) ;
                    sets_intersect(e.vertex[0].boundary_edges, e.vertex[1].boundary_edges, I.boundary_edges) ;
                    I.bisectors.insert(E) ;
//					I.on_boundary = e.vertex[0].on_boundary && e.vertex[1].on_boundary ;
                    to.push_back(PolygonEdge()) ;
                    to.rbegin()->vertex[0] = e.vertex[0] ;
                    to.rbegin()->vertex[1] = I ;
                    if(close) { from_vertices.push_back(I) ; }
                } 
            } break ;

            case 2: { // Case 2: from outside to inside
                PolygonVertex I ;
                if(intersect_seg_line(e.vertex[0], e.vertex[1], L, I)) {
                    sets_intersect(e.vertex[0].bisectors, e.vertex[1].bisectors, I.bisectors) ;
                    sets_intersect(e.vertex[0].boundary_edges, e.vertex[1].boundary_edges, I.boundary_edges) ;
                    I.bisectors.insert(E) ;
//					I.on_boundary = e.vertex[0].on_boundary && e.vertex[1].on_boundary ;
                    to.push_back(PolygonEdge()) ;
                    to.rbegin()->vertex[0] = I ;
                    to.rbegin()->vertex[1] = e.vertex[1] ;
                    if(close) { to_vertices.push_back(I) ; }
                } 
            } break ;
            case 3: {
                to.push_back(e) ;
            } break ;
            }
        }

        if(close) {
            if(from_vertices.size() != to_vertices.size()) {
                std::cerr << from_vertices.size() << " " << to_vertices.size() << std::endl ;
            }
            gx_assert(from_vertices.size() == to_vertices.size()) ;
            unsigned int N = from_vertices.size() ;
            for(unsigned int i=0; i<N; i++) {
                to.push_back(PolygonEdge()) ;
                to.rbegin()->vertex[0] = from_vertices[i] ;
                to.rbegin()->vertex[1] = to_vertices[i] ;
            }
        }
    }

    //====================================================================================================

}
