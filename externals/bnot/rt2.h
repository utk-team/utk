#ifndef _RT2_H_
#define _RT2_H_ 1

// STL
#include <vector>

// CGAL
#include <CGAL/basic.h>
#include <CGAL/intersections.h>
#include "CGAL/squared_distance_2.h"


// local
#include "console_color.h"
#include "random.h"

#undef min
#undef max

#define MINN -1e100
#define MAXN  1e100
#define EPS   1e-15

template <class RT>
class CTriangulation : public RT
{
public:
    typedef CTriangulation<RT> Rt;
    
    typedef typename Rt::Geom_traits    Kernel;
    typedef typename Kernel::FT         FT;
    typedef typename Kernel::Weight     Weight;
    typedef typename Kernel::Point_2    Point;
    typedef typename Kernel::Vector_2   Vector;
    typedef typename Kernel::Ray_2      Ray;
    typedef typename Kernel::Line_2     Line;
    typedef typename Kernel::Segment_2  Segment;
    typedef typename Kernel::Triangle_2 Triangle;
    typedef typename Kernel::Weighted_point_2 Weighted_point;
    
    typedef typename Rt::Vertex                   Vertex;
    typedef typename Rt::Vertex_handle            Vertex_handle;
    typedef typename Rt::Vertex_iterator          Vertex_iterator;
    typedef typename Rt::Vertex_circulator        Vertex_circulator;
    typedef typename Rt::Finite_vertices_iterator Finite_vertices_iterator;
    
    typedef typename Rt::Edge                  Edge;
    typedef typename Rt::Edge_iterator         Edge_iterator;
    typedef typename Rt::Edge_circulator       Edge_circulator;
    typedef typename Rt::Finite_edges_iterator Finite_edges_iterator;
    
    typedef typename Rt::Face                  Face;
    typedef typename Rt::Face_handle           Face_handle;
    typedef typename Rt::Face_iterator         Face_iterator;
    typedef typename Rt::Face_circulator       Face_circulator;
    typedef typename Rt::Finite_faces_iterator Finite_faces_iterator;

public:
    CTriangulation() 
    {
    }
    
    ~CTriangulation()
    {
    }
    
    ////////////
    // BASICS //
    ////////////
    
    Vertex_handle get_source(const Edge& edge) const
    {
        return edge.first->vertex( Rt::ccw(edge.second) );
    }    
    
    Vertex_handle get_target(const Edge& edge) const
    {
        return edge.first->vertex( Rt::cw(edge.second) );        
    }
    
    Vertex_handle get_opposite(const Edge& edge) const
    {
        return edge.first->vertex( edge.second );
    }    
    
    Edge get_twin(const Edge& edge) const
    {
        Face_handle f = edge.first;
        Vertex_handle v = get_source(edge);
        Face_handle nf = f->neighbor(edge.second);
        return Edge(nf, Rt::ccw(nf->index(v)));
    }

    Edge get_next(const Edge& edge) const
    {
        Face_handle f = edge.first;
        int index = Rt::ccw(edge.second);
        return Edge(f, index);
    }

    Edge get_prev(const Edge& edge) const
    {
        Face_handle f = edge.first;
        int index = Rt::cw(edge.second);
        return Edge(f, index);
    }
    
    FT get_length(const Edge& edge) const
    {
        Segment segment = get_segment(edge);
        return std::sqrt(segment.squared_length());
    }
    
    Segment get_segment(const Edge& edge) const
    {
        const Point& ps = get_source(edge)->get_position();
        const Point& pt = get_target(edge)->get_position();
        return Segment(ps, pt);        
    }
    
    FT get_area(Face_handle face) const
    {
        Triangle triangle = get_triangle(face);
        return triangle.area();
    }
    
    Triangle get_triangle(Face_handle face) const
    {
        Vertex_handle v0 = face->vertex(0);
        Vertex_handle v1 = face->vertex(1);
        Vertex_handle v2 = face->vertex(2);
        return Triangle(v0->get_position(), v1->get_position(), v2->get_position());
    }
    
    FT distance(const Point& a, const Point& b) const
    {
        return std::sqrt(CGAL::squared_distance(a, b));
    }
    
    Point project(const Point& x, const Point& a, const Point& b) const
    {
        if (a == b) return a;
        FT dab = distance(a, b);
        Vector n = (b - a) / dab;
        FT dot = n * (x - a);
        return (a + dot*n);
    }
    
    FT get_height(const Point& p, const Point& a, const Point& b) const
	{
        FT len_ab = distance(a, b);
        if (len_ab < EPS) return distance(p, a);
        
		Vector ab = (b - a) / len_ab;
		Vector ab90(-ab.y(), ab.x());
		Vector ap = p - a;
		return (ap*ab90);
	}
    
    FT get_average_length() const
    {
        unsigned nb = 0;
        FT avg_length = 0.0;
        for (Finite_edges_iterator
             eit  = RT::finite_edges_begin(); 
             eit != RT::finite_edges_end();
             ++eit)
        {
            Edge edge = *eit;
            FT length = get_length(edge);
            avg_length += length;
            nb++;
        }
        return (avg_length / nb);
    }

    //////////////
    // CAPACITY //
    //////////////

    FT compute_area(Vertex_handle vertex) const
    {
        std::vector<Point> polygon;
        build_polygon(vertex, std::back_inserter(polygon));        
        return compute_polygon_area(polygon);
    }

    FT compute_polygon_area(const std::vector<Point>& polygon) const
	{
        if (polygon.size() < 3) return 0.0;
        FT sum_areas = 0.0;
        Point p0 = polygon[0];
        for (unsigned i = 2; i < polygon.size(); i++)
        {
            Point p1 = polygon[i-1];
            Point p2 = polygon[i  ];
            Triangle triangle(p0, p1, p2);
            sum_areas += triangle.area();
        }
        return sum_areas;
}
    
    Point compute_centroid(Vertex_handle vertex) const
    {
        std::vector<Point> polygon;
        build_polygon(vertex, std::back_inserter(polygon));        
        return compute_polygon_centroid(polygon);
    }

    Point compute_polygon_centroid(const std::vector<Point>& polygon) const
    {
        if (polygon.size() < 3) return Point();
		FT sum_area = 0.0;
        Point p0 = polygon[0];
        Vector sum_vec = CGAL::NULL_VECTOR;
		for (unsigned i = 2; i < polygon.size(); i++)
		{
			Point p1 = polygon[i-1];
			Point p2 = polygon[i  ];
			Triangle triangle(p0, p1, p2);
            FT area = triangle.area();
            Point barycenter = CGAL::centroid(triangle);
            sum_vec = sum_vec + area*(barycenter - CGAL::ORIGIN);
			sum_area += area;
		}
        return CGAL::ORIGIN + (sum_vec / sum_area);
    }
    
    template <class OutputIterator>
    void build_polygon(Vertex_handle vertex, OutputIterator out) const
    {
        Face_circulator fcirc = RT::incident_faces(vertex);
        Face_circulator fend  = fcirc;
        CGAL_For_all(fcirc, fend)
        {
            Face_handle face = fcirc;
            Point cw = get_cw(face);
            out++ = cw;
        }
    }
    
    ////////////////
    // Hk and Dij //
    ////////////////
    
    Point get_cw(Face_handle face) const
    {
        return RT::dual(face);
    }
    
    Point get_cw(const Edge& edge) const
    {
        Vertex_handle va = get_source(edge);
        Vertex_handle vb = get_target(edge);

        const Point& a = va->get_position();
        const Point& b = vb->get_position(); 
        if (a == b) return a;
        
        Vector ab = (b - a) / distance(a, b);
        FT dab = get_dij(va, vb);
        return a + dab*ab;
    }
    
    FT get_dij(Vertex_handle vi, Vertex_handle vj) const
    {
        const Point& pi = vi->get_position();
        const Point& pj = vj->get_position();
        const FT wi = vi->get_weight();
        const FT wj = vj->get_weight();
        
        FT lij = distance(pi, pj);
        FT dij = 0.5*(lij + (wi-wj)/lij);
        return dij;
    }
    
    FT get_h(Face_handle face, Vertex_handle vi) const
    {
        int i = face->index(vi);
        int j = RT::ccw(i);
        int k = RT::cw(i);
        
        Vertex_handle vj = face->vertex(j);
        Vertex_handle vk = face->vertex(k);

        const Point& pj = vj->get_position();
        const Point& pk = vk->get_position();
        
        const Point& cw = get_cw(face);
        FT h = get_height(cw, pj ,pk);
        return h;
    }
    
    Point get_dual_edge_midpoint(Edge edge) const
    {
        // Assumption: edge is inside mesh
        Face_handle left_face = edge.first;
        Face_handle right_face = get_twin(edge).first;
        Point left_cw = get_cw(left_face);
        Point right_cw = get_cw(right_face);
        return CGAL::midpoint(left_cw, right_cw);
    }
    
    /////////
    // DEC //
    /////////
    
    FT get_primal_length(const Edge& edge) const
    {
        return get_length(edge);
    }
    
    FT get_dual_length(const Edge& edge) const
    {
        Face_handle face = edge.first;
        Vertex_handle vertex = get_opposite(edge);
        FT ha = get_h(face, vertex);
        
        Edge twin = get_twin(edge);
        face = twin.first;
        vertex = get_opposite(twin);
        FT hb = get_h(face, vertex);
        
        return (ha + hb);
    }
    
    FT get_primal_area(Face_handle face) const
    {
        return std::abs(get_area(face));
    }
    
    FT get_dual_area(Vertex_handle vertex) const
    {
        return compute_area(vertex);
    }
    
    FT get_ratio(Vertex_handle vertex) const
    {
        return get_dual_area(vertex);
    }
    
    FT get_ratio(const Edge& edge) const
    {
        FT primal_len = get_primal_length(edge);
        return get_dual_length(edge) / primal_len;
    }
    
    FT get_ratio(Face_handle face) const
    {
        return 1.0 / get_primal_area(face);
    }
};

#endif
