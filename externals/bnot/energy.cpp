#include "scene.h"
#include "timer.h"
#include "console_color.h"

FT Scene::compute_weight_threshold(FT epsilon) const
{
    // reference: 3e-5 for 1000 sites
    FT A = compute_total_area();
    unsigned N = count_visible_sites();
    return  (0.03*epsilon) * (A) / FT(N);
}

FT Scene::compute_position_threshold(FT epsilon) const
{
    // reference: 3e-5 for 1000 sites
    FT A = compute_total_area();
    unsigned N = count_visible_sites();
    return (0.03*epsilon) * (std::sqrt(A*A*A)) / FT(N);
}

void Scene::compute_position_gradient(std::vector<Vector>& gradient, FT coef)
{
    if (m_timer_on)
        Timer::start_timer(m_timer, COLOR_BLUE, "XGrad");

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
    
    if (m_timer_on)
        Timer::stop_timer(m_timer, COLOR_BLUE);
}

void Scene::compute_weight_gradient(std::vector<FT>& gradient, FT coef)
{
    if (m_timer_on)
        Timer::start_timer(m_timer, COLOR_BLUE, "WGrad");

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
    
    if (m_timer_on)
        Timer::stop_timer(m_timer, COLOR_BLUE);
}

FT Scene::compute_weight_gradient_numeric(Vertex_handle vertex)
{
    FT eps = 1e-8;
    FT original_w = vertex->get_weight();
    
    vertex->set_weight(original_w + eps);
    update_triangulation();
    FT pos_energy = compute_wcvt_energy();
    
    vertex->set_weight(original_w - eps);
    update_triangulation();
    FT neg_energy = compute_wcvt_energy();
    
    vertex->set_weight(original_w);
    update_triangulation();
    
    return (pos_energy - neg_energy) / (2*eps);
}

FT Scene::compute_total_area() const
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

FT Scene::compute_wcvt_energy()
{
    if (m_timer_on)
        Timer::start_timer(m_timer, COLOR_BLUE, "Energy");

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
    
    if (m_timer_on)
        Timer::stop_timer(m_timer, COLOR_BLUE);

    return (w_dot_V - E_cvt);
}

FT Scene::compute_cvt_energy() const
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

FT Scene::compute_cvt_energy_per_vertex(Vertex_handle vertex) const
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

FT Scene::compute_cvt_energy_per_edge(Edge edge, Vertex_handle vi) const
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

FT Scene::compute_variance(Vertex_handle vertex) const
{
    std::vector<Point> points;
    Point q = vertex->get_position();
    m_rt.build_polygon(vertex, std::back_inserter(points));
    
    FT variance = 0.0;
    for (unsigned i = 0; i < points.size(); ++i)
    {
        unsigned j = (i + 1) % points.size();
        Segment segment(points[i], points[j]);
        variance += compute_variance_per_segment(q, segment);
    }
    return variance;
}

FT Scene::compute_variance_per_segment(const Point& x, const Segment& ab) const
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

FT Scene::compute_variance_per_right_triangle(const Point& x, const Point& a, const Point& b) const
{
    // E = \int_(x, a, b) |y - x|^2 dy
    // where (x, a, b) is 90o at a
    // E >= 0
    FT base   = std::sqrt(CGAL::squared_distance(a, b));
    FT height = std::sqrt(CGAL::squared_distance(a, x));
    return (height*height*height*base)/4 + (height*base*base*base)/12;
}
