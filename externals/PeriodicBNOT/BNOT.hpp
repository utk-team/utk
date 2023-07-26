#pragma once

// STDLIB
#include <iostream>
#include <string.h>
#include <random>
#include <list>

// CGAL
#include <CGAL/basic.h>
#include <CGAL/intersections.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_2.h>

// Suitesparse
#if defined(__APPLE__) || defined(__MACH__)
#include <SuiteSparseQR.hpp>
#include <cholmod.h>
#else
#include <SuiteSparseQR.hpp>
#include <cholmod.h>
#endif


#define BASE_MATHS
#ifdef  BASE_MATHS

#undef min
#undef max

#define MINN -1e100
#define MAXN  1e100
#define EPS   1e-15

namespace PBNOT
{

template <class T>
T compute_mean(const std::vector<T>& data)
{
    T mean = 0.0;
    for (unsigned i = 0; i < data.size(); ++i)
        mean += data[i];
    return (mean / static_cast<double>(data.size()));
}

template <class T>
double compute_norm(std::vector<T>& data)
{
    double norm2 = 0.0;
    for (unsigned i = 0; i < data.size(); ++i)
        norm2 += data[i]*data[i];
    return std::sqrt(norm2);
}

#endif

#define CGAL_CUSTOM_STRUCTURES
#ifdef  CGAL_CUSTOM_STRUCTURES
template <class Kernel, class Vbb = CGAL::Regular_triangulation_vertex_base_2<Kernel> >
class My_vertex_base : public Vbb
{
    
public:
    typedef typename Kernel::FT FT;
    typedef typename Kernel::FT Weight;
    typedef typename Kernel::Point_2 KPoint;
    
    typedef typename Kernel::Weighted_point_2 Point;
    typedef typename Kernel::Weighted_point_2 Weighted_point;
    
    typedef typename Vbb::Triangulation_data_structure TDS;
    typedef typename TDS::Face_handle   Face_handle;
    typedef typename TDS::Vertex_handle Vertex_handle;


    template < typename TDS2 >
    struct Rebind_TDS {
        typedef typename Vbb::template Rebind_TDS<TDS2>::Other    Vb2;
        typedef My_vertex_base<Kernel,Vb2>                           Other;
    };

private:
    unsigned m_index;
    int m_clone;
    FT m_noise;
    
public:
    My_vertex_base() : Vbb() 
    { 
        m_index = 0;
        m_noise = 0.0;
        m_clone = -1;
    }
    
    My_vertex_base(const Weighted_point& p) : Vbb(p)
    {
        m_index = 0;
        m_noise = 0.0;
        m_clone = -1;
    }

    My_vertex_base(const Weighted_point& p, Face_handle f) : Vbb(p, f)
    {
        m_index = 0;
        m_noise = 0.0;
        m_clone = -1;
    }

    operator const Vbb& () const
    {
        return *this;
    }

    int get_clone() const { return m_clone; }
    bool is_clone() const { return (m_clone >= 0); }
    void set_clone(const int clone) { m_clone = clone; }
    
    void set_index(const unsigned x) { m_index = x; }
    unsigned get_index() const { return m_index; }

    void set_noise(const FT noise) { m_noise = noise; }
    FT get_noise() const { return m_noise; }
    
    const KPoint& get_position() const 
    { 
        return this->point().point(); 
    }

    void set_position(const KPoint& p)
    {
        FT w = get_weight();
        Weighted_point wp(p, w);
        this->set_point(wp);
    }
    
    const FT get_weight() const 
    { 
        return this->point().weight(); 
    }
    
    void set_weight(const FT w)
    {
        const KPoint& p = get_position();
        Weighted_point wp(p, w);
        this->set_point(wp);
    }
};

template <class Kernel, class Fbb>
class My_face_base : public Fbb
{
public:
    typedef typename Fbb::Triangulation_data_structure  TDS;
    typedef typename TDS::Vertex_handle Vertex_handle;
    typedef typename TDS::Face_handle   Face_handle;
    
    template < typename TDS2 >
    struct Rebind_TDS {
        typedef typename Fbb::template Rebind_TDS<TDS2>::Other Fb2;
        typedef My_face_base<Kernel, Fb2> Other;
    };
    
public:
    My_face_base() : Fbb() { }
    
    My_face_base(Vertex_handle v1,
                 Vertex_handle v2,
                 Vertex_handle v3)
    : Fbb(v1, v2, v3)
    {
    }
    
    My_face_base(Vertex_handle v1,
                 Vertex_handle v2,
                 Vertex_handle v3,
                 Face_handle f1,
                 Face_handle f2,
                 Face_handle f3)
    : Fbb(v1,v2,v3,f1,f2,f3)
    {
    }    
};

template <class Kernel>
class CDomain
{
public:
    typedef typename Kernel::FT         FT;
    typedef typename Kernel::Point_2    Point;
    typedef typename Kernel::Vector_2   Vector;
    typedef typename Kernel::Ray_2      Ray;
    typedef typename Kernel::Line_2     Line;
    typedef typename Kernel::Segment_2  Segment;
    typedef typename Kernel::Triangle_2 Triangle;
    
public:
    CDomain()
    {
    }

    ~CDomain()
    {
    }
    
    template <class OutputIterator>
    void add_segments(OutputIterator out) const
    {
        Point a(0., 0.);
        Point b(1., 0.);
        Point c(1., 1.);
        Point d(0., 1.);
        out++ = Segment(a, b);
        out++ = Segment(b, c);
        out++ = Segment(c, d);
        out++ = Segment(d, a);
    }
    
    bool is_outside(const Point& p) const
    {
        if (p.x() < 0.) return true;
        if (p.x() > 1.) return true;
        if (p.y() < 0.) return true;
        if (p.y() > 1.) return true;
        return false;
    }
    
    Point clamp(const Point& p) const
    {
        FT x = p.x();
        while (x < 0.0) x += 1.0;
        while (x > 1.0) x -= 1.0;

        FT y = p.y();
        while (y < 0.0) y += 1.0;
        while (y > 1.0) y -= 1.0;
        
        return Point(x, y);
    }
    
    FT compute_area() const
    {
        FT side = 1.0;
        return side*side;
    }
    
    Vector get_offset(int h, int v) const 
    { 
        double x = 0.0;
        if (h == -1) x = -1.0;
        if (h ==  1) x =  1.0;

        double y = 0.0;
        if (v == -1) y = -1.0;
        if (v ==  1) y =  1.0;
        
        return Vector(x, y);
    }
    
    Vector extract_offset(const Point& p) const
    {
        Point q = clamp(p);
        return (p - q);
    }
};

template <class RT>
class CTriangulation : public RT
{
public:
    typedef CTriangulation<RT> Rt;
    
    typedef typename Rt::Geom_traits    Kernel;
    typedef typename Kernel::FT         FT;
    typedef typename Kernel::FT         Weight;
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

#define CGAL_TYPES
#ifdef  CGAL_TYPES

//typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

// Basic types
typedef Kernel::FT         FT;
typedef Kernel::Point_2    Point;
typedef Kernel::Vector_2   Vector;
typedef Kernel::Ray_2      Ray;
typedef Kernel::Line_2     Line;
typedef Kernel::Segment_2  Segment;
typedef Kernel::Triangle_2 Triangle;

// Traits
typedef Kernel::Weighted_point_2 Weighted_point;
typedef Kernel::FT Weight;

// Vertex
// typedef CGAL::Regular_triangulation_vertex_base_2<Kernel> RVb;
typedef My_vertex_base<Kernel> MVb;

// Face
typedef CGAL::Regular_triangulation_face_base_2<Kernel> RFb;
typedef My_face_base<Kernel, RFb> MFb;

// Triangulation
typedef CGAL::Triangulation_data_structure_2<MVb, MFb> TDS;
typedef CGAL::Regular_triangulation_2<Kernel, TDS> Regular_triangulation;
typedef CTriangulation<Regular_triangulation> RT;

typedef RT::Vertex                   Vertex;
typedef RT::Vertex_handle            Vertex_handle;
typedef RT::Vertex_iterator          Vertex_iterator;
typedef RT::Vertex_circulator        Vertex_circulator;
typedef RT::Finite_vertices_iterator Finite_vertices_iterator;
typedef RT::Hidden_vertices_iterator Hidden_vertices_iterator;

typedef RT::Edge                  Edge;
typedef RT::Edge_iterator         Edge_iterator;
typedef RT::Edge_circulator       Edge_circulator;
typedef RT::Finite_edges_iterator Finite_edges_iterator;

typedef RT::Face                  Face;
typedef RT::Face_handle           Face_handle;
typedef RT::Face_iterator         Face_iterator;
typedef RT::Face_circulator       Face_circulator;
typedef RT::Finite_faces_iterator Finite_faces_iterator;

// Domain
typedef CDomain<Kernel> Domain;
#endif

#define OPTIM
#ifdef  OPTIM
template <class Vector>
class CLBFGS
{
protected:
    unsigned m_limit;
    // FIFO: ( [k-limit-1], ..., [k-1] )
    std::list< std::vector<Vector> > m_s; // s[k] = x[k+1] - x[k]
    std::list< std::vector<Vector> > m_y; // y[k] = grad[k+1] - grad[k]
    std::list<double> m_rho;

    typedef typename std::list< std::vector<Vector> >::const_iterator ConstIterator;
    typedef typename std::list< std::vector<Vector> >::const_reverse_iterator RConstIterator;

public:
    CLBFGS(unsigned size = 0) 
    {
        m_limit = size;
    }

    void clear()
    {
        m_y.clear();
        m_s.clear();
        m_rho.clear();
    }
    
    void set_limit(unsigned nb) 
    { 
        clear();
        m_limit = nb; 
    }
    
    unsigned memory_limit() const { return m_limit; }
    
    unsigned memory_size() const { return m_y.size(); }
    
    void append(const std::vector<Vector>& s, const std::vector<Vector>& y)
    {
        if (memory_limit() == 0) return;
        
        double rho = inner_product(s, y);
        if (rho != 0.0) rho = 1.0/rho;
        
        m_y.push_back(y);
        m_s.push_back(s);
        m_rho.push_back(rho);
        
        if (memory_size() > memory_limit())
        {
            m_y.pop_front();
            m_s.pop_front();
            m_rho.pop_front();
        }
    }

    // for minimization problem
    void solve(std::vector<Vector> q, std::vector<Vector>& z)
    {
        RConstIterator s_rit = m_s.rbegin();
        RConstIterator y_rit = m_y.rbegin();
        std::list<double>::const_reverse_iterator rho_rit = m_rho.rbegin();
        
        unsigned nb = memory_size();
        std::vector<double> alpha(nb);
        for (unsigned i = 0; i < nb; ++i)
        {
            const std::vector<Vector>& s = *s_rit;
            const std::vector<Vector>& y = *y_rit;
            const double rho = *rho_rit;
            
            unsigned index = nb - 1 - i;
            alpha[index] = rho * inner_product(s, q);
            add_to_vector(q, -alpha[index], y);
            
            s_rit++;
            y_rit++;
            rho_rit++;
        }
        
        z = q;
        scale_vector_by_gamma(z);
        
        ConstIterator s_it = m_s.begin();
        ConstIterator y_it = m_y.begin();
        std::list<double>::const_iterator rho_it = m_rho.begin();
        
        for (unsigned i = 0; i < nb; ++i)
        {
            const std::vector<Vector>& s = *s_it;
            const std::vector<Vector>& y = *y_it;
            const double rho = *rho_it;
            
            unsigned index = i;
            double beta = rho * inner_product(y, z);
            add_to_vector(z, alpha[index] - beta, s);
            
            s_it++;
            y_it++;
            rho_it++;            
        }
        
        flip_vector(z);
    }
  
    void scale_vector_by_gamma(std::vector<Vector>& z)
    {
        if (memory_size() == 0) return;
        
        double D = inner_product(m_y.back(), m_y.back());
        if (D == 0.0) return;
        
        double N = inner_product(m_s.back(), m_y.back());        
        double gamma = N / D;
        
        for (unsigned i = 0; i < z.size(); ++i)
            z[i] = gamma*z[i];
    }
    
    double inner_product(const std::vector<Vector>& s, const std::vector<Vector>& y) const
    {
        double sum = 0.0;
        for (unsigned i = 0; i < s.size(); ++i)
            sum += s[i]*y[i];
        return sum;
    }
    
    void add_to_vector(std::vector<Vector>& y, double coef, const std::vector<Vector>& x) const
    {
        for (unsigned i = 0; i < y.size(); ++i)
            y[i] = y[i] + coef*x[i];
    }
    
    void flip_vector(std::vector<Vector>& x) const
    {
        for (unsigned i = 0; i < x.size(); ++i)
            x[i] = -x[i];
    }
};

template <class Position, class Velocity>
class CLineSearch
{
protected:
    /* parameters */
    unsigned m_max_iters;
    double m_max_alpha;
    double m_c1;
    double m_c2;
    double m_c3;
    
    /* initial configuration */
    std::vector<Position> m_x0;
    std::vector<Velocity> m_v0;
    double m_grad_phi0;
    double m_phi0;
    
public:
    CLineSearch(const unsigned max_iters,
                const double max_alpha,
                const double c1 = 1e-4,
                const double c2 = 0.9,
                const double c3 = 1e-6)
    {
        m_max_iters = max_iters;
        m_max_alpha = max_alpha;
        m_c1 = c1;
        m_c2 = c2;
        m_c3 = c3;
    }

    virtual ~CLineSearch() {} 

    double run_bt(const std::vector<Position>& x0, 
                  const std::vector<Velocity>& v0)
    {
        m_x0 = x0;
        m_v0 = v0;
        m_phi0 = evaluate_function();
        m_grad_phi0 = evaluate_gradient();
        double step = back_tracking();
        reset_position();
        return step;
    }
    
    double run_wc(const std::vector<Position>& x0, 
                              const std::vector<Velocity>& v0)
    {
        m_x0 = x0;
        m_v0 = v0;
        m_phi0 = evaluate_function();
        m_grad_phi0 = evaluate_gradient();
        double step = line_search_with_strong_wolfe_conditions();
        reset_position();
        return step;
    }
    
protected:
    double back_tracking()
    {
        double lower_alpha = 0.0;
        double upper_alpha = m_max_alpha;        
        for (unsigned i = 0; i < m_max_iters; ++i) 
        {
            double alpha = pick_alpha(lower_alpha, upper_alpha);
            //std::cout << "alpha = " << alpha << std::endl; // debug
            bool ok = move_position(alpha);
            
            if (ok)
            {
                double phi = evaluate_function();
                if (phi <= (m_phi0 + m_c1 * alpha * m_grad_phi0))
                    return alpha;
            }
            
            //std::cout << "decrease alpha" << std::endl; // debug
            upper_alpha = alpha;
        }
        return lower_alpha;
    }
    
    double line_search_with_strong_wolfe_conditions()
    {
        double lower_alpha = 0.0;
        double upper_alpha = m_max_alpha;
        
        double last_phi = m_phi0;
        double last_alpha = lower_alpha;
        for (unsigned i = 0; i < m_max_iters; ++i)
        {
            double alpha = pick_alpha(last_alpha, upper_alpha);
            if (std::abs(alpha - last_alpha) < m_c3) break;
            bool ok = move_position(alpha);
                  
            if (!ok)
            {
                upper_alpha = alpha;
                continue;
            }
            
            double phi = evaluate_function();
            if (phi >= last_phi)
            {
                //std::cout << "function not decreasing: call zoom" << std::endl;
                return zoom(last_alpha, alpha, last_phi);
            }

            if (phi > (m_phi0 + m_c1 * alpha * m_grad_phi0))
            {
                //std::cout << "Armijo failed: call zoom" << std::endl;
                return zoom(last_alpha, alpha, last_phi);
            }
            
            double grad_phi = evaluate_gradient();
            if (std::abs(grad_phi) <= m_c2 * std::abs(m_grad_phi0))
            {
                //std::cout << "Done: " << alpha << std::endl;
                return alpha;                
            }
            
            if (grad_phi >= 0.0)
            {
                //std::cout << "alpha is too far: call zoom in reverse order" << std::endl;
                return zoom(alpha, last_alpha, phi);                
            }

            //std::cout << std::abs(grad_phi) << " > " << m_c2 * std::abs(m_grad_phi0) << std::endl;
            //std::cout << "increase alpha and try again" << std::endl;
            last_phi = phi;
            last_alpha = alpha;
        }
        
        //std::cout << "search failed" << std::endl;
        return lower_alpha;
    }
    
    double zoom(double lower_alpha, double upper_alpha, double lower_phi)
    {
        if (std::abs(upper_alpha - lower_alpha) < m_c3)
            return lower_alpha;
        
        double alpha = pick_alpha(lower_alpha, upper_alpha);
        bool ok = move_position(alpha);
        
        if (!ok) 
        {
            return zoom(lower_alpha, alpha, lower_phi);
        }
        
        double phi = evaluate_function();
        if (phi >= lower_phi)
        {
            //std::cout << "Zoom: function not decreasing: call zoom again" << std::endl;
            return zoom(lower_alpha, alpha, lower_phi);
        }
        
        if (phi > m_phi0 + m_c1 * alpha * m_grad_phi0)
        {   
            //std::cout << "Zoom: Armijo failed: call zoom again" << std::endl;
            return zoom(lower_alpha, alpha, lower_phi);
        }
        
        double grad_phi = evaluate_gradient();
        if (std::abs(grad_phi) <= m_c2 * std::abs(m_grad_phi0))
        {
            //std::cout << "Zoom: done" << std::endl;
            return alpha;            
        }
        
        if (grad_phi * (upper_alpha - lower_alpha) >= 0.0)
        {
            //std::cout << "Zoom: revert zoom" << std::endl;
            upper_alpha = lower_alpha;
        }
        
        //std::cout << "Zoom: increase lower" << std::endl;
        lower_phi = phi;
        lower_alpha = alpha;
        return zoom(lower_alpha, upper_alpha, lower_phi);
    }
    
    double pick_alpha(const double lower, const double upper) const
    {
        return 0.5*(lower + upper);
    }
    
    void reset_position()
    {
        move_position(0.0);
    }
    
    bool move_position(const double alpha)
    {
        std::vector<Position> X;
        for (unsigned i = 0; i < m_x0.size(); ++i)
        {
            Position pi = m_x0[i] + alpha*m_v0[i];
            X.push_back(pi);
        }
        return update_scene(X);
    }

    double evaluate_function() const
    {
        return compute_function();
    }

    double evaluate_gradient() const
    {
        std::vector<Velocity> V;
        compute_gradient(V);
        
        /* V * m_v0 */
        double sum = 0.0;
        for (unsigned i = 0; i < m_v0.size(); ++i)
            sum += V[i] * m_v0[i];
        return sum;
    }

    virtual double compute_function() const = 0;

    virtual void compute_gradient(std::vector<Velocity>& V) const = 0;

    virtual bool update_scene(const std::vector<Position>& X) = 0;
};

template <class Scene, class T>
class CLSWeights : public CLineSearch<T, T>
{
protected:
    Scene* m_scene;
    unsigned m_nb;
    
public:
    CLSWeights(Scene* scene,
              const unsigned max_iters,
              const double max_alpha)
    : CLineSearch<T, T>(max_iters, max_alpha)
    {
        m_scene = scene;
        m_nb = m_scene->count_visible_sites();
    }
    
    double compute_function() const
    {
        return m_scene->compute_wcvt_energy();
    }
    
    void compute_gradient(std::vector<T>& V) const
    {
        m_scene->compute_weight_gradient(V);
    }
    
    bool update_scene(const std::vector<T>& X)
    {
        if (m_scene->connectivity_fixed())
        {        
            m_scene->update_weights(X, true, false);
            return true;
        }
        
        m_scene->update_weights(X, false, false);
        m_scene->update_triangulation();
        return has_same_vertices();
    }
    
    bool has_same_vertices() const
    {
        unsigned nb = m_scene->count_visible_sites();
        // if (nb != m_nb) std::cout << "HiddenVertices: " << m_nb << " -> " << nb << std::endl;
        return (nb == m_nb);
    }
};

template <class Scene, class Position, class Velocity>
class CLSPositions : public CLineSearch<Position, Velocity>
{
protected:
    Scene* m_scene;
    unsigned m_nb;

public:
    CLSPositions(Scene* scene,
                const unsigned max_iters,
                const double max_alpha)
    : CLineSearch<Position, Velocity>(max_iters, max_alpha)
    {
        m_scene = scene;
        m_nb = m_scene->count_visible_sites();
    }
    
    double compute_function() const
    {
        return ( - m_scene->compute_wcvt_energy() );
    }
    
    void compute_gradient(std::vector<Velocity>& V) const
    {
        m_scene->compute_position_gradient(V, -1.0);
    }
    
    bool update_scene(const std::vector<Position>& X)
    {
        if (m_scene->connectivity_fixed())
        {
            m_scene->update_positions(X, true, false);
            return true;
        }
         
        m_scene->update_positions(X, false, false);
        m_scene->update_triangulation();
        return has_same_vertices();
    }
    
    bool has_same_vertices() const
    {
        unsigned nb = m_scene->count_visible_sites();
        // if (nb != m_nb) std::cout << "HiddenVertices: " << m_nb << " -> " << nb << std::endl;
        return (nb == m_nb);
    }
};

#endif

#define SPARSE_MATRIX
#ifdef  SPARSE_MATRIX
class SparseArray {
private:
    unsigned _size;
    std::vector<long int> _index;
    std::vector<double>   _value;
    
public:
    // Ctor
    SparseArray(unsigned N = 1) : _size(N)
    { }
    
    SparseArray(const SparseArray& rhs) 
    : _size(rhs._size), _index(rhs._index), _value(rhs._value) 
    { }
    
    // Dtor
    virtual ~SparseArray() 
    { }
    
    // Copy
    unsigned size() const
    { return _size; }
    
    unsigned numNonZeros() const
    { return static_cast<unsigned>(_value.size()); }
    
    unsigned copy(long* index, double* value) const {
        memcpy(index, &(_index[0]), numNonZeros()*sizeof(long int));
        memcpy(value, &(_value[0]), numNonZeros()*sizeof(double));
        return numNonZeros();
    }
    
    // Access
    double getValue(unsigned i) const;
    
    void setValue(unsigned i, const double x);
    
    int findIndex(int index) const;
    
    int readIndex(unsigned i) const;

    double readValue(unsigned i) const;

    void insert(unsigned i, const double x);

    void replace(unsigned i, const double x);

    // Operators
    SparseArray& operator = (const SparseArray& rhs);
    
    SparseArray& operator *= (const double rhs);
    
    SparseArray& operator /= (const double rhs);
    
    SparseArray& operator += (const SparseArray& rhs);
    
    SparseArray& operator -= (const SparseArray& rhs);
    
    // Measures
    double normalize();
    
    double length2() const;
    
    double length() const;
    
    double getMax() const;
    
    double getMin() const;
    
    // Friend
    friend SparseArray operator - (const SparseArray& rhs);
    
    friend SparseArray operator + (const SparseArray& a, const SparseArray& b);
    
    friend SparseArray operator - (const SparseArray& a, const SparseArray& b);
    
    friend SparseArray operator * (const SparseArray& a, const double b);
    
    friend SparseArray operator * (const double a, const SparseArray& b);
    
    friend SparseArray operator / (const SparseArray& a, const double b);
    
    friend double dot(const SparseArray& a, const SparseArray& b);

    friend std::ostream& operator << (std::ostream& out, const SparseArray& a);
    
    friend std::istream& operator >> (std::istream& in, SparseArray& a);
    
    friend class SparseMatrix;
};
    
inline 
SparseArray operator - (const SparseArray& rhs) { 
    SparseArray neg(rhs);
    for (unsigned i=0; i<neg.numNonZeros(); ++i) 
        neg._value[i] = - neg._value[i];
    return neg;
}

inline 
SparseArray operator + (const SparseArray& a, const SparseArray& b) { 
    SparseArray rt(a); 
    rt += b; 
    return rt; 
}

inline 
SparseArray operator - (const SparseArray& a, const SparseArray& b) { 
    SparseArray rt(a); 
    rt -= b; 
    return rt;
}

inline 
SparseArray operator * (const SparseArray& a, const double b) { 
    SparseArray rt(a); 
    rt *= b; 
    return rt; 
}

inline 
SparseArray operator * (const double a, const SparseArray& b) { 
    SparseArray rt(b); 
    rt *= a; 
    return rt; 
}

inline 
SparseArray operator / (const SparseArray& a, double b) { 
    SparseArray rt(a); 
    rt /= b; 
    return rt; 
}

inline 
double dot(const SparseArray& a, const SparseArray& b) { 
    double sum = 0.; 
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        int pos = b.findIndex( a.readIndex(i) );
        if (pos != -1) sum += a.readValue(i) * b.readValue(pos);
    }
    return sum; 
}

inline 
std::ostream& operator << (std::ostream& out, const SparseArray& a) { 
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        out << a._index[i] << " "; 
        out << a._value[i] << " "; 
    }
    return out; 
}

inline 
std::istream& operator >> (std::istream& in, SparseArray& a) {
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        in >> a._index[i];
        in >> a._value[i];
    }
    return in;
}


int SparseArray::findIndex(int index) const {
    for (unsigned i=0; i<numNonZeros(); ++i)
        if (_index[i] == index) return i;
    return -1;
}

int SparseArray::readIndex(unsigned i) const {
    return static_cast<int>(_index[i]);
}

double SparseArray::readValue(unsigned i) const {
    return _value[i];
}

void SparseArray::insert(unsigned i, const double x) {
    _index.push_back(i);
    _value.push_back(x);
}

void SparseArray::replace(unsigned i, const double x) {
    _value[i] = x;
}

double SparseArray::getValue(unsigned i) const {
    int pos = findIndex(i);
    if (pos == -1) return 0.;
    return readValue(pos);
}

void SparseArray::setValue(unsigned i, const double x) {
    int pos = findIndex(i);
    if (pos == -1) insert(i, x);
    else _value[pos] = x;
}

SparseArray& SparseArray::operator = (const SparseArray& rhs) {
    _size  = rhs._size;
    _index = rhs._index;
    _value = rhs._value;
    return *this;
}

SparseArray& SparseArray::operator *= (const double rhs) {
    for (unsigned i=0; i<numNonZeros(); ++i)
        _value[i] *= rhs;
    return *this;
}

SparseArray& SparseArray::operator /= (const double rhs) {
    for (unsigned i=0; i<numNonZeros(); ++i)
        _value[i] /= rhs;
    return *this;
}

SparseArray& SparseArray::operator += (const SparseArray& rhs) {
    for (unsigned i=0; i<rhs.numNonZeros(); ++i) {
        int pos = findIndex( rhs.readIndex(i) );
        if (pos == -1) insert( rhs.readIndex(i), rhs.readValue(i) );
        else _value[pos] += rhs.readValue(i);
    }
    return *this;
}

SparseArray& SparseArray::operator -= (const SparseArray& rhs) {
    for (unsigned i=0; i<rhs.numNonZeros(); ++i) {
        int pos = findIndex( rhs.readIndex(i) );
        if (pos == -1) insert( rhs.readIndex(i), -rhs.readValue(i) );
        else _value[pos] -= rhs.readValue(i);
    }
    return *this;
}

double SparseArray::length2() const { 
    double sum = 0; 
    for (unsigned i=0; i<numNonZeros(); ++i) 
        sum += _value[i]*_value[i]; 
    return sum; 
}

double SparseArray::length() const  {
    return std::sqrt( length2() ); 
}

double SparseArray::normalize() { 
    double len = length(); 
    if ( len > 1e-10 )
        *this /= len;
    return len;
}

double SparseArray::getMax() const {
    if (numNonZeros() == 0) return 0.;
    double value = _value[0];
    for (unsigned i=1; i<numNonZeros(); ++i)
        value = std::max(value, _value[i]);
    return value;
}

double SparseArray::getMin() const {
    if (numNonZeros() == 0) return 0.;
    double value = _value[0];
    for (unsigned i=1; i<numNonZeros(); ++i)
        value = std::min(value, _value[i]);
    return value;
}

class SparseMatrix {
private:
    std::vector<SparseArray> _row;
    
public:
    // Ctor
    SparseMatrix(unsigned nrows, unsigned ncols);

    SparseMatrix(const SparseMatrix& rhs);
    
    // Dtor
    ~SparseMatrix();
    
    // Access
    
    unsigned numRows() const;

    unsigned numColumns() const;
    
    unsigned numNonZeros() const;
    
    unsigned copyCRS(long*& rowptr, long*& index, double*& value) const;
    
    unsigned copyCCS(long*& colptr, long*& index, double*& value) const;

    double getValue(unsigned i, unsigned j) const;
    
    void setValue(unsigned i, unsigned j, const double value); 
    
    SparseArray getRow(unsigned i) const;
    
    SparseArray getColumn(unsigned i) const;
    
    SparseMatrix getTranspose() const;

    void setRow(unsigned i, const SparseArray& row);

    void setColumn(unsigned i, const SparseArray& col);

    // Operators
    
    SparseMatrix& operator = (const SparseMatrix& rhs);

    SparseMatrix& operator += (const SparseMatrix& rhs);

    SparseMatrix& operator -= (const SparseMatrix& rhs);
    
    SparseMatrix& operator *= (const SparseMatrix& rhs);

    SparseMatrix& operator *= (const double rhs);

    SparseMatrix& operator /= (const double rhs);
    
    SparseMatrix multiply(const double b) const;
    
    SparseMatrix multiply(const SparseMatrix& b) const;
};

// Friend Operators

inline
SparseMatrix operator - (const SparseMatrix& rhs) {
    SparseMatrix A(rhs.numRows(), rhs.numColumns());
    for (unsigned i=0; i<rhs.numRows(); ++i) {
        A.setRow(i, -rhs.getRow(i));
    }
    return A;
}

inline
SparseMatrix operator + (const SparseMatrix& a, const SparseMatrix& b) {
    SparseMatrix c = a;
    c += b;
    return c;
}

inline
SparseMatrix operator - (const SparseMatrix& a, const SparseMatrix& b) {
    SparseMatrix c = a;
    c -= b;
    return c;        
}

inline
SparseMatrix operator * (const SparseMatrix& a, const SparseMatrix& b) {
    return a.multiply(b);
}

inline
SparseMatrix operator * (const SparseMatrix& a, const double b) {
    return a.multiply(b);
}

inline
SparseMatrix operator * (const double a, const SparseMatrix& b) {
    return b.multiply(a);
}

inline
SparseMatrix operator / (const SparseMatrix& a, const double b) {
    SparseMatrix c(a.numRows(), a.numColumns());
    for (unsigned i=0; i<a.numRows(); ++i)
        c.setRow(i, a.getRow(i) / b);
    return c;        
}

// Concatenation

inline
SparseMatrix Concatenate(const SparseMatrix& A, const SparseMatrix& B) {
    SparseMatrix C(A.numRows() + B.numRows(), A.numColumns());
    
    // copy A
    for (unsigned i=0; i<A.numRows(); ++i) {
        C.setRow(i, A.getRow(i));
    }
    
    // copy B
    for (unsigned i=0; i<B.numRows(); ++i) {
        C.setRow(A.numRows() + i, B.getRow(i));
    }
    
    return C;        
}

SparseMatrix::SparseMatrix(unsigned nrows, unsigned ncols) :
_row(nrows, SparseArray(ncols)) {
}

SparseMatrix::SparseMatrix(const SparseMatrix& rhs)
: _row(rhs._row) { 
}

// Dtor
SparseMatrix::~SparseMatrix() { 
}

// Access

unsigned SparseMatrix::numRows() const {
    return static_cast<unsigned>(_row.size());
}

unsigned SparseMatrix::numColumns() const {
    return _row[0].size();
}

unsigned SparseMatrix::numNonZeros() const {
    unsigned N = 0;
    for (unsigned i=0; i<numRows(); ++i) {
        N += _row[i].numNonZeros();
    }
    return N;
}

unsigned SparseMatrix::copyCRS(long*& rowptr, long*& index, double*& value) const {
    unsigned N = numNonZeros();
	rowptr = (long int*) malloc((numRows() + 1) * sizeof(long int));
	index  = (long int*) malloc(N * sizeof(long int));
	value  = (double*) malloc(N * sizeof(double));
	
	rowptr[0] = 0;
	for (unsigned i = 0; i < numRows(); ++i) {
		int offset = _row[i].numNonZeros();
        _row[i].copy( &(index[rowptr[i]]), &(value[rowptr[i]]) );
		rowptr[i+1] = rowptr[i] + offset;
	}
	return N;
}    

unsigned SparseMatrix::copyCCS(long*& colptr, long*& index, double*& value) const {
	unsigned N = numNonZeros();
	colptr = (long int*) malloc((numColumns() + 1) * sizeof(long int));
	index  = (long int*) malloc(N * sizeof(long int));
	value  = (double*) malloc(N * sizeof(double));
	
	colptr[0] = 0;
	for (unsigned i = 0; i < numColumns(); ++i) {
        SparseArray col = getColumn(i);
		int offset = col.numNonZeros();
        col.copy( &(index[colptr[i]]), &(value[colptr[i]]) );
		colptr[i+1] = colptr[i] + offset;
	}
	return N;
}

double SparseMatrix::getValue(unsigned i, unsigned j) const { 
    return _row[i].getValue(j);
}

void SparseMatrix::setValue(unsigned i, unsigned j, const double value) { 
    _row[i].setValue(j, value);
}

SparseArray SparseMatrix::getRow(unsigned i) const {
    return _row[i];
}

SparseArray SparseMatrix::getColumn(unsigned i) const { 
    SparseArray column;
    for (unsigned j=0; j<numRows(); ++j) {
        int pos = _row[j].findIndex(i);
        if (pos != -1) column.insert(j, _row[j].readValue(pos));
    }
    return column;
}

SparseMatrix SparseMatrix::getTranspose() const {
    SparseMatrix transpose(numColumns(), numRows());
    for (unsigned i=0; i<numRows(); ++i) {
        unsigned nnz = _row[i].numNonZeros();
        for (unsigned j=0; j<nnz; ++j) {
            int  index = _row[i].readIndex(j);
            double value = _row[i].readValue(j);
            transpose._row[index].insert(i, value);
        }
    }
    return transpose;
}

void SparseMatrix::setRow(unsigned i, const SparseArray& row) {
    _row[i] = row;
}

void SparseMatrix::setColumn(unsigned i, const SparseArray& col) {
    unsigned nnz = col.numNonZeros();
    for (unsigned j=0; j<nnz; ++j) {
        int  index = col.readIndex(j);
        double value = col.readValue(j);
        _row[index].setValue(i, value);
    }
}

// Operators

SparseMatrix& SparseMatrix::operator = (const SparseMatrix& rhs) {
    _row = rhs._row;
    return *this;
}

SparseMatrix& SparseMatrix::operator += (const SparseMatrix& rhs) {
    for (unsigned i=0; i<numRows(); ++i) _row[i] += rhs._row[i];
    return *this;
}

SparseMatrix& SparseMatrix::operator -= (const SparseMatrix& rhs) {
    for (unsigned i=0; i<numRows(); ++i) _row[i] -= rhs._row[i];
    return *this;
}

SparseMatrix& SparseMatrix::operator *= (const SparseMatrix& rhs) {
    *this = multiply(rhs);
    return *this;
}

SparseMatrix& SparseMatrix::operator *= (const double rhs) {
    *this = multiply(rhs);
    return *this;
}    

SparseMatrix& SparseMatrix::operator /= (const double rhs) {
    for (unsigned i=0; i<numRows(); ++i) {
        _row[i] /= rhs;
    }
    return *this;
}    

// return (this * b)
SparseMatrix SparseMatrix::multiply(const SparseMatrix& b) const 
{
    SparseMatrix c(numRows(), b.numColumns());
    SparseMatrix bt = b.getTranspose();
    for (unsigned i = 0; i < c.numRows(); ++i) 
    {
        SparseArray c_i(c.numColumns());        
        SparseArray a_i = getRow(i);
        for (unsigned j = 0; j < c.numColumns(); ++j) 
        {
            double value = dot(a_i, bt.getRow(j));
            if (value > 1e-10) c_i.insert(j, value);
        }
        c.setRow(i, c_i);
    }
    return c;
}

SparseMatrix SparseMatrix::multiply(const double b) const 
{
    SparseMatrix c(numRows(), numColumns());
    for (unsigned i=0; i<numRows(); ++i)
        c.setRow(i, getRow(i) * b);
    return c;    
}

class SuiteSparseQRFactorizer {
private:
    cholmod_common _common;
    SuiteSparseQR_factorization<double>* _QR;
    
public:
    SuiteSparseQRFactorizer()  {
        cholmod_l_start(&_common);
        _QR = NULL;
    }
    
    ~SuiteSparseQRFactorizer() {
        if (_QR) SuiteSparseQR_free<double>(&_QR, &_common);
        cholmod_l_finish(&_common);
    }
    
    bool factorize(const SparseMatrix& A) {
        if (_QR) return false;
        cholmod_sparse* At = convert_transpose_to_cholmod_sparse(A);
        _QR = SuiteSparseQR_factorize<double>(SPQR_ORDERING_DEFAULT, SPQR_DEFAULT_TOL, At, &_common);
        cholmod_l_free_sparse(&At, &_common);
        return true;
    }
    
    bool solve(const std::vector<double>& rhs, std::vector<double>& x) {
        if (!_QR) return false;
        
        cholmod_dense* b = convert_to_cholmod_dense(rhs);
        cholmod_dense* y = SuiteSparseQR_solve(SPQR_RTX_EQUALS_ETB, _QR, b, &_common);
        cholmod_l_free_dense(&b, &_common);
        
        cholmod_dense* z = SuiteSparseQR_qmult(SPQR_QX, _QR, y, &_common);
        cholmod_l_free_dense(&y, &_common);
        
        convert_to_array(z, x);
        cholmod_l_free_dense(&z, &_common);
        return true;
    }
    
private:
    cholmod_sparse* convert_transpose_to_cholmod_sparse(const SparseMatrix& A) {
        // B = A'
        long* colptr;
        long* index;
        double* value;
        int nnz = (int) A.copyCRS(colptr, index, value);
        cholmod_sparse* B = cholmod_l_allocate_sparse(A.numColumns(), A.numRows(), nnz, 
                                                      1, 1, 0, CHOLMOD_REAL, &_common);
        B->p = (void*) colptr;
        B->i = (void*) index;
        B->x = (void*) value;
        return B;
    }
    
    cholmod_dense* convert_to_cholmod_dense(const std::vector<double>& a) {
        cholmod_dense* b = cholmod_l_allocate_dense(a.size(), 1, a.size(), CHOLMOD_REAL, &_common);
        double* tmp = (double*) b->x;
        for (unsigned i=0; i<a.size(); ++i) tmp[i] = a[i];
        return b;
    }

    void convert_to_array(cholmod_dense* a, std::vector<double>& b) {
        b.resize(a->nrow);
        double* tmp = (double*) a->x;
        for (unsigned i=0; i<a->nrow; ++i) b[i] = tmp[i];
    }
};


#endif

// Main sampler class
class Scene
{
public:
    typedef CLSWeights<Scene, FT> LSWeights;
    typedef CLSPositions<Scene, Point, Vector> LSPositions;

// Custom code, not part of BNOT source code
private:
    mutable std::mt19937 m_generator;
public:
    double random_double(const double min, const double max) const
    {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(m_generator);
    }

    int random_int(const int min, const int max) const
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(m_generator);
    }

    void seed(std::size_t seed)
    {
        m_generator.seed(seed);
    }
// End custom code
private:
    RT m_rt;
    Domain m_domain;
    std::vector<FT> m_capacities;
    std::vector<Vertex_handle> m_vertices;
    
    bool m_fixed_connectivity;

    CLBFGS<Vector> m_lbfgs_x;

    double m_tau;
public:


    Scene()
    {
        m_fixed_connectivity = true;
        m_lbfgs_x.set_limit(7);
        m_tau = 1.0;
    }

    bool connectivity_fixed() const { return m_fixed_connectivity; }


    Vertex_handle insert_vertex(const Point& point,
                                const FT weight,
                                const FT noise, 
                                const unsigned index,
                                const int clone)
    {
        Weighted_point wp(point, weight);
        Vertex_handle vertex = m_rt.insert(wp);
        vertex->set_noise(noise);
        vertex->set_index(index);
        vertex->set_clone(clone);
        return vertex;
    }

    void populate_vertices(const std::vector<Point>& points,
                           const std::vector<FT>& weights,
                           const std::vector<FT>& noise)
    {
        unsigned nsites = static_cast<unsigned int>(points.size());
        for (unsigned i = 0; i < nsites; ++i)
        {
            //std::cout << "Populating (1/3) " << i << "/" << points.size() << "\r";
            Vertex_handle vertex = insert_vertex(points[i], weights[i], noise[i], i, -1);
            m_vertices.push_back(vertex);
        }
    }

    void make_torus()
    {
        unsigned index = static_cast<unsigned int>(m_vertices.size());
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            //std::cout << "Make Torus (2/3) " <<  i << "/" << m_vertices.size() << "\r";
            Vertex_handle vertex = m_vertices[i];
            const Point& p = vertex->get_position();
            const FT w = vertex->get_weight();
            const FT n = vertex->get_noise();
            
            Vector offset;
            offset = m_domain.get_offset(-1, -1);
            insert_vertex(p + offset, w, n, index++, i);
            
            offset = m_domain.get_offset(-1, 0);
            insert_vertex(p + offset, w, n, index++, i);
            
            offset = m_domain.get_offset(-1, 1);
            insert_vertex(p + offset, w, n, index++, i);
            
            offset = m_domain.get_offset(0, -1);
            insert_vertex(p + offset, w, n, index++, i);
            
            offset = m_domain.get_offset(0, 1);
            insert_vertex(p + offset, w, n, index++, i);
            
            offset = m_domain.get_offset(1, -1);
            insert_vertex(p + offset, w, n, index++, i);
            
            offset = m_domain.get_offset(1, 0);
            insert_vertex(p + offset, w, n, index++, i);
            
            offset = m_domain.get_offset(1, 1);
            insert_vertex(p + offset, w, n, index++, i);
        }
    }

    FT compute_mean_capacity() const
    {
        FT domain_area = m_domain.compute_area();
        unsigned nb = count_visible_sites();
        return (domain_area / nb);
    }

    unsigned count_visible_sites() const
    {
        unsigned nb = 0;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            nb++;
        }
        return nb;   
    }

    void compute_capacities(std::vector<FT>& capacities) const
    {
        FT total_area = 0.0;
        FT mean_capacity = compute_mean_capacity();
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            //std::cout << "Compute Capacity (3/3) " <<  i << "/" << m_vertices.size() << "\r";
            FT Ci = 0.0;
            Vertex_handle vi = m_vertices[i];
            if (!vi->is_hidden()) 
            {
                FT noise = vi->get_noise();
                Ci = (1.0 + noise) * mean_capacity;
            }
            capacities.push_back(Ci);
            total_area += Ci;
        }
        
        FT ratio = m_domain.compute_area() / total_area;
        for (unsigned i = 0; i < capacities.size(); ++i)
            capacities[i] *= ratio;
    }

    void construct_triangulation(const std::vector<Point>& points,
                                 const std::vector<FT>& weights,
                                 const std::vector<FT>& noise)
    {
        clear_triangulation();
        populate_vertices(points, weights, noise);
        make_torus();
        compute_capacities(m_capacities);
    }

    void clear_triangulation()
    {
        m_capacities.clear();
        m_vertices.clear();
        m_rt.clear();
    }

    void clear_lbfgs() { m_lbfgs_x.clear(); }

    void clear()
    {
        clear_triangulation();
        clear_lbfgs();
    }

    FT compute_total_area() const
    {
        FT total = 0.0;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {    
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            total += m_rt.compute_area(vi);
        }
        return total;
    }

    FT compute_position_threshold(FT epsilon) const
    {
        FT A = compute_total_area();
        unsigned N = count_visible_sites();
        return (0.03*epsilon) * (std::sqrt(A*A*A)) / FT(N);
    }

    FT compute_weight_threshold(FT epsilon) const
    {
        // reference: 3e-5 for 1000 sites
        FT A = compute_total_area();
        unsigned N = count_visible_sites();
        return  (0.03*epsilon) * (A) / FT(N);
    }

    FT compute_regularity_threshold() const
    {
        FT m = compute_mean_capacity();
        return m_tau*0.25*m*m;
    }

    FT compute_variance_per_right_triangle(const Point& x, const Point& a, const Point& b) const
    {
        // E = \int_(x, a, b) |y - x|^2 dy
        // where (x, a, b) is 90o at a
        // E >= 0
        FT base   = std::sqrt(CGAL::squared_distance(a, b));
        FT height = std::sqrt(CGAL::squared_distance(a, x));
        return (height*height*height*base)/4 + (height*base*base*base)/12;
    }

    FT compute_variance_per_segment(const Point& x, const Segment& ab) const
    {
        // \int_(x, a, b) |y - x|^2 dy
        // (x, a, b) can be either CCW or CW
        // return signed variance
        
        Point a = ab.source();
        Point b = ab.target();
        Point q = m_rt.project(x, a, b);
        
        Triangle xqa(x, q, a);
        FT E0 = compute_variance_per_right_triangle(x, q, a); // >=0
        if (xqa.orientation() == CGAL::COUNTERCLOCKWISE)
            E0 = - E0;
        
        Triangle xqb(x, q, b);
        FT E1 = compute_variance_per_right_triangle(x, q, b); // >=0
        if (xqb.orientation() == CGAL::CLOCKWISE)
            E1 = - E1;

        return (E0 + E1);
    }

    FT compute_variance(Vertex_handle vertex) const
    {
        std::vector<Point> points;
        Point q = vertex->get_position();
        m_rt.build_polygon(vertex, std::back_inserter(points));
        
        FT variance = 0.0;
        for (unsigned i = 0; i < points.size(); ++i)
        {
            unsigned j = (i + 1) % static_cast<unsigned>(points.size());
            Segment segment(points[i], points[j]);
            variance += compute_variance_per_segment(q, segment);
        }
        return variance;
    }

    FT compute_cvt_energy_per_edge(Edge edge, Vertex_handle vi) const
    {
        if (m_rt.get_target(edge) != vi) edge = m_rt.get_twin(edge);
        Edge twin = m_rt.get_twin(edge);
        
        Vertex_handle vj = m_rt.get_source(edge);
        Vertex_handle va = m_rt.get_opposite(edge);
        Vertex_handle vb = m_rt.get_opposite(twin);
        
        FT dij = m_rt.get_dij(vi, vj);
        FT ha = m_rt.get_h(edge.first, va);
        FT hb = m_rt.get_h(twin.first, vb);
        
        FT E0 = ha*ha*ha*dij/12 + ha*dij*dij*dij/4;
        FT E1 = hb*hb*hb*dij/12 + hb*dij*dij*dij/4;
        return (E0 + E1);
    }

    FT compute_cvt_energy_per_vertex(Vertex_handle vertex) const
    {
        FT E = 0.0;
        Edge_circulator ecirc = m_rt.incident_edges(vertex);
        Edge_circulator eend  = ecirc;
        CGAL_For_all(ecirc, eend)
        {
            Edge edge = *ecirc;
            E += compute_cvt_energy_per_edge(edge, vertex);
        }
        return E;
    }

    FT compute_cvt_energy() const
    {
        FT E = 0.0;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {    
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;        
            E += compute_cvt_energy_per_vertex(vi);
        }
        return E;
    }

    FT compute_wcvt_energy()
    {
        FT w_dot_V = 0.0;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {    
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;        
            
            FT Vi = m_rt.compute_area(vi);
            FT wi = vi->get_weight();
            FT Ci = m_capacities[i];
            w_dot_V += wi * (Vi - Ci);
        }
        FT E_cvt = compute_cvt_energy();
     
        return (w_dot_V - E_cvt);
    }

    void compute_variance_vector(std::vector<FT>& variance) const
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            FT V = 0.0;
            Vertex_handle vertex = m_vertices[i];        
            if (!vertex->is_hidden()) V = compute_variance(vertex);
            variance.push_back(V);
        }
    }

    FT compute_regularity(Vertex_handle vi, const std::vector<FT>& variance) const
    {
        // regularity = absolute deviation
        FT deviation = 0.0;
        unsigned degree = 0;
        FT Vi = variance[vi->get_index()];
        Vertex_circulator vcirc = m_rt.incident_vertices(vi);
        Vertex_circulator vend  = vcirc;
        CGAL_For_all(vcirc, vend)
        {
            Vertex_handle vj = vcirc;
            unsigned j = vj->get_index();
            if (vj->is_clone()) j = vj->get_clone();
            FT Vj = variance[j];        
            deviation += std::abs(Vi - Vj);
            degree++;
        }
        return (deviation / degree);
    }

    void compute_regularity_vector(const std::vector<FT>& variance,
                                         std::vector<FT>& regularity) const
    {
        // regularity = normalized absolute deviation
        FT C = compute_mean_capacity();
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            FT R = 0.0;
            Vertex_handle vertex = m_vertices[i];
            if (!vertex->is_hidden()) R = compute_regularity(vertex, variance);
            regularity.push_back(R/C);
        }
    }

    Point jitter_point(const Point& p, const FT max_radius) const
    {
        // TODO : ramdom_double
        FT angle = random_double(0.0, 2.0*M_PI);
        FT radius = random_double(0.0, max_radius);
        Vector d(radius * cos(angle), radius*sin(angle));
        return m_domain.clamp(p + d);
    }

    void jitter_vertices(const std::set<Vertex_handle>& vertices, const FT max_radius)
    {
        for (std::set<Vertex_handle>::const_iterator it = vertices.begin(); it != vertices.end(); ++it)
        {
            Vertex_handle vertex = *it;
            Point p = vertex->get_position();
            p = jitter_point(p, max_radius);
            vertex->set_position(p);
        }
    }

    void collect_sites(std::vector<Point>& points,
                          std::vector<FT>& weights,
                          std::vector<FT>& noise) const
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            
            Point pi = vi->get_position();
            points.push_back(pi);
            
            FT ni = vi->get_noise();
            noise.push_back(ni);
            
            FT wi = 0.0;
            //if (!vi->is_hidden())
            wi = vi->get_weight();        
            weights.push_back(wi);
        }
    }

    void update_triangulation()
    {
        std::vector<Point> points;
        std::vector<FT> weights;
        std::vector<FT> noise;
        collect_sites(points, weights, noise);
        construct_triangulation(points, weights, noise);
    }

    void reset_weights()
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vertex = m_vertices[i];
            vertex->set_weight(0.0);
        }
        update_triangulation();
    }

    bool detect_and_break_regularity(FT max_radius, unsigned max_teleport)
    {
        FT threshold = compute_regularity_threshold();
        FT m = compute_mean_capacity();
        max_radius *= m;
            
        std::vector<FT> variance;
        compute_variance_vector(variance);
        
        std::vector<FT> regularity;
        compute_regularity_vector(variance, regularity);
         
        std::vector<Vertex_handle> blocks;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            if (regularity[i] < threshold) 
                blocks.push_back(vi);
        }

        // std::cout << blocks.size() << " blocks detected" << std::endl;
        if (blocks.empty()) return false;
        
        std::set<Vertex_handle> for_jittering;
        if (max_radius > 0.0)
        {
            for (unsigned i = 0; i < blocks.size(); ++i)
            {
                Vertex_handle vi = blocks[i];
                for_jittering.insert(vi);
                
                Vertex_circulator vcirc = m_rt.incident_vertices(vi);
                Vertex_circulator vend  = vcirc;
                CGAL_For_all(vcirc, vend)
                {
                    Vertex_handle vj = vcirc;
                    if (vj->is_clone()) 
                        vj = m_vertices[vj->get_clone()];
                    for_jittering.insert(vj);
                }
            }
            jitter_vertices(for_jittering, max_radius);
        }

        // std::cout << for_jittering.size() << " vertices jittered" << std::endl;
    
        std::set<Vertex_handle> to_teleport;
        if (blocks.size() < max_teleport) 
            max_teleport = static_cast<unsigned int>(blocks.size());
        if (blocks.size() == max_teleport) 
        {
            to_teleport.insert(blocks.begin(), blocks.end());
        } else {
            while (to_teleport.size() < max_teleport)
            {
                unsigned index = random_int(0, static_cast<int>(blocks.size()-1));
                to_teleport.insert(blocks[index]);
            }
        }

        for (std::set<Vertex_handle>::iterator it = to_teleport.begin(); it != to_teleport.end(); ++it)
        {
            Vertex_handle vertex = *it;
            double x = random_double(0.0, 1.0);
            double y = random_double(0.0, 1.0);
            vertex->set_position(Point(x, y));
        }
        // std::cout << max_teleport << " vertices teleported" << std::endl;

        reset_weights();

        return true;
    }
    
    void compute_weight_gradient(std::vector<FT>& gradient, FT coef = 1.0)
    {
        gradient.clear();
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            
            FT Ci = m_capacities[i];
            FT Vi = m_rt.compute_area(vi);
            FT Gi = Vi - Ci;
            gradient.push_back(coef * Gi);
        }
    }

    void collect_visible_weights(std::vector<FT>& weights) const
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            weights.push_back(vi->get_weight());
        }
    }

    void build_laplacian(const FT scale,
                            const std::map<unsigned, unsigned>& indices,
                            SparseMatrix& A) const
    {
        unsigned nb = A.numRows();
        for (unsigned k = 0; k < m_vertices.size(); ++k)
        {
            Vertex_handle vi = m_vertices[k];
            if (vi->is_hidden()) continue;
            unsigned i = indices.find(vi->get_index())->second;

            double diagi = 0.0;
            SparseArray rowi(nb);
            Edge_circulator ecirc = m_rt.incident_edges(vi);
            Edge_circulator eend  = ecirc;
            CGAL_For_all(ecirc, eend)
            {
                Edge edge = *ecirc;

                Vertex_handle vj = m_rt.get_source(edge);
                if (vj == vi) vj = m_rt.get_target(edge);

                unsigned j = vj->get_index();
                if (vj->is_clone()) j = vj->get_clone();
                j = indices.find(j)->second;

                double coef = scale * m_rt.get_ratio(edge);
                rowi.setValue(j, -coef);
                diagi += coef;
            }

            rowi.setValue(i, diagi);
            A.setRow(i, rowi);
        }
    }

    bool solve_linear_system(const SparseMatrix& A,
                                    std::vector<double>& x,
                                    const std::vector<double>& b) const
    {
        SuiteSparseQRFactorizer solver;
        bool ok = solver.factorize(A);
        if (!ok) return false;

        ok = solver.solve(b, x);
        return ok;
    }


    bool solve_newton_step(const std::vector<FT>& b,
                           std::vector<FT>& x)
    {
        unsigned nb = 0;
        std::map<unsigned, unsigned> indices;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            indices[vi->get_index()] = nb++;
        }

        ((void) b);
        ((void) x);
        SparseMatrix L(nb, nb);
        build_laplacian(0.5, indices, L);
        //
        bool ok = solve_linear_system(L, x, b);
        if (!ok)
        {
            // std::cout << "linear solver failed" << std::endl;
            return false;
        }

        return true;
    }

    void update_weights(const std::vector<FT>& weights, 
                        bool update_clone,
                        bool hidden = true)
    {    
        unsigned j = 0;
        FT mean = compute_mean(weights);
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (hidden && vi->is_hidden()) continue;
            vi->set_weight(weights[j++] - mean);
        }
        
        if (!update_clone) return;
        
        for (Finite_vertices_iterator 
            vit = m_rt.finite_vertices_begin(); 
            vit != m_rt.finite_vertices_end(); 
            ++vit)
        {
            Vertex_handle vi = vit;
            if (!vi->is_clone()) continue;
            
            unsigned index = vi->get_clone();
            FT w = m_vertices[index]->get_weight();
            vi->set_weight(w);
        }
    }

    FT optimize_weights_via_newton(FT timestep, bool update)
    {
        std::vector<FT> gradient;
        compute_weight_gradient(gradient, -1.0);

        std::vector<FT> direction;
        bool ok = solve_newton_step(gradient, direction);
        if (!ok) return 0.0;

        std::vector<FT> weights;
        collect_visible_weights(weights);

        if (timestep <= 0.0)
        {
            LSWeights line_search(this, 10, 2.0);
            timestep = line_search.run_bt(weights, gradient);
        }
        
        for (unsigned i = 0; i < weights.size(); ++i)
        {
            FT wi = weights[i];
            FT gi = direction[i];
            weights[i] = wi + timestep * gi;
        }

        update_weights(weights, !update);
        if (update) update_triangulation();

        compute_weight_gradient(gradient);
        return compute_norm(gradient);
    }

    unsigned optimize_weights_via_newton_until_converge(FT timestep,
                                                        FT threshold,
                                                        unsigned update,
                                                        unsigned max_iters)
    {
        for (unsigned i = 0; i < max_iters; ++i)
        {
            bool flag = (update == 0 || (i+1) % update == 0);
            FT norm = optimize_weights_via_newton(timestep, flag);
            if (norm < threshold) return i;
        }
        return max_iters;
    }

    void update_positions(const std::vector<Point>& points, 
                             bool update_clone,
                             bool hidden = true)
    {
        unsigned j = 0;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (hidden && vi->is_hidden()) continue;
            
            Point pi = m_domain.clamp(points[j++]);
            vi->set_position(pi);
        }
        
        if (!update_clone) return;
        
        for (Finite_vertices_iterator 
            vit = m_rt.finite_vertices_begin(); 
            vit != m_rt.finite_vertices_end(); 
            ++vit)
        {
            Vertex_handle vi = vit;
            if (!vi->is_clone()) continue;
            
            unsigned index = vi->get_clone();
            Point p = m_vertices[index]->get_position();
            Vector o = m_domain.extract_offset(vi->get_position());
            vi->set_position(p + o);
        }
    }
    
    void compute_position_gradient(std::vector<Vector>& gradient, FT coef = 1.0)
    {
        gradient.clear();
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];        
            if (vi->is_hidden()) continue;
            
            FT Vi = m_rt.compute_area(vi);
            Point xi = vi->get_position();
            Point ci = m_rt.compute_centroid(vi);
            Vector gi = -2.0*Vi*(xi - ci);
            gradient.push_back(coef * gi);
        }
    }

    FT optimize_positions_via_lloyd(bool update)
    {
        std::vector<Point> points;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            
            Point ci = m_rt.compute_centroid(vi);
            points.push_back(ci);
        }
        update_positions(points, !update);
        if (update) update_triangulation();
        
        std::vector<Vector> gradient;
        compute_position_gradient(gradient);
        return compute_norm(gradient);
    }

    void collect_visible_points(std::vector<Point>& points) const
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            points.push_back(vi->get_position());
        }
    }

    FT optimize_positions_via_gradient_ascent(FT timestep, bool update)
    {
        std::vector<Point> points;
        collect_visible_points(points);
        
        std::vector<Vector> gradient;
        compute_position_gradient(gradient);

        if (timestep <= 0.0)
        {
            double mean_capacity = compute_mean_capacity();
            double max_alpha = 1.0 / mean_capacity;       

            LSPositions line_search(this, 100, max_alpha);
            timestep = line_search.run_bt(points, gradient);
        }
        
        for (unsigned i = 0; i < points.size(); ++i)
        {
            Point  pi = points[i];
            Vector gi = gradient[i];
            points[i] = pi + timestep * gi;
        }

        update_positions(points, !update);
        if (update) update_triangulation();
        
        compute_position_gradient(gradient);
        return compute_norm(gradient);
    }


    unsigned optimize_all(FT wstep, FT xstep, unsigned max_newton_iters,
                          FT epsilon, unsigned max_iters,
                          std::ostream& out,
                          bool breakCon = true)
    {
        bool global_connectivity = m_fixed_connectivity;
        unsigned nb0 = count_visible_sites();

        FT xthreshold = compute_position_threshold(epsilon);
        FT wthreshold = compute_weight_threshold(epsilon);
            
        // out << "NbSites = " << nb0 << std::endl;
        // out << "Threshold: " << xthreshold << " ; " << wthreshold << std::endl;

        m_fixed_connectivity = true;
        FT coarse_xthreshold = 2.0*xthreshold;
        FT coarse_wthreshold = 2.0*wthreshold;
        
        int maxBreakCon = 20;
        do
        {
            unsigned iters = 0;
            while (iters < max_iters)
            {
                iters++;
                reset_weights();        
                optimize_weights_via_newton_until_converge(wstep, coarse_wthreshold, 0, max_newton_iters);        
                FT norm = optimize_positions_via_lloyd(true);
                // out << "(Coarse) Norm: " << norm << std::endl;        
                if (norm <= coarse_xthreshold) break;
            }

            // std::cout << "Partial: " << iters << " iters" << std::endl;
            m_fixed_connectivity = global_connectivity;
            if (iters == max_iters) return iters;

            m_fixed_connectivity = false;
            FT fine_xthreshold = xthreshold;
            FT fine_wthreshold = wthreshold;

            while (iters < max_iters)
            {
                iters++;
                unsigned nb1 = count_visible_sites();
                if (nb1 != nb0) reset_weights();
                optimize_weights_via_newton_until_converge(wstep, fine_wthreshold, 0, max_newton_iters);
                FT norm = optimize_positions_via_gradient_ascent(xstep, true);
                // out << "(Fine) Norm: " << norm << std::endl;
                if (norm <= fine_xthreshold) break;
            }
            
            maxBreakCon--;

        } while(breakCon && detect_and_break_regularity(1.0, 100) && maxBreakCon > 0);
        return static_cast<unsigned>(global_connectivity + nb0 + xthreshold + wthreshold + coarse_wthreshold + coarse_xthreshold + maxBreakCon);
    }

    ~Scene()
    {
        clear();
    }
};

}