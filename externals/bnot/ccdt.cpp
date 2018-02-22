#include "scene.h"
#include "timer.h"

void Scene::optimize_positions_via_CCDT(unsigned max_iters)
{
    for (unsigned iters = 0; iters < max_iters; ++iters)
    {
        run_ccdt_geometric_phase();
        run_ccdt_topological_phase();
    }
}

void Scene::run_ccdt_topological_phase()
{
    if (m_timer_on) Timer::start_timer(m_timer, COLOR_GREEN, "CCDT-T");
    update_triangulation();
    if (m_timer_on) Timer::stop_timer(m_timer, COLOR_GREEN);
}

void Scene::run_ccdt_geometric_phase()
{
    if (m_timer_on) Timer::start_timer(m_timer, COLOR_GREEN, "CCDT-G");
    std::vector<Point> points;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        Point p = compute_ccdt_position_per_vertex(vi);
        vi->set_position(p);
        points.push_back(p);
    }
    update_positions(points, true);
    if (m_timer_on) Timer::stop_timer(m_timer, COLOR_GREEN);
}

Point Scene::compute_ccdt_position_per_vertex(Vertex_handle vi) const
{
    // vi is not hidden
    FT sum_djaj = 0.0;
    FT sum_djbj = 0.0;
    FT sum_djcj = 0.0;
    std::vector<FT> S;
    std::vector<FT> R_col_1, R_col_2;
    
    Face_circulator fcirc = m_rt.incident_faces(vi);
    Face_circulator fend = fcirc;
    CGAL_For_all(fcirc, fend)
    {
        Face_handle face = fcirc;
        unsigned i = face->index(vi);
        unsigned j = m_rt.ccw(i);
        unsigned k = m_rt.cw(i);
        
        Point pi = vi->get_position();
        Point pj = face->vertex(j)->get_position();
        Point pk = face->vertex(k)->get_position();
        
        FT a_i = pj.y()        - pk.y();
        FT b_i = pk.x()        - pj.x();
        FT c_i = pj.x()*pk.y() - pk.x()*pj.y();

        // density function
        FT d_i = 1.0;
        //
        
        R_col_1.push_back(d_i * a_i);
        R_col_2.push_back(d_i * b_i);
        S.push_back(d_i * c_i);
        
        sum_djaj += d_i * a_i;
        sum_djbj += d_i * b_i;
        sum_djcj += d_i * c_i;
    }
    
    unsigned degree = R_col_1.size();
    for (unsigned a = 0; a < degree; ++a)
    {
        R_col_1[a]  -= (1./ (FT) degree) * sum_djaj;
        R_col_2[a]  -= (1./ (FT) degree) * sum_djbj;
        S[a]        -= (1./ (FT) degree) * sum_djcj;
    }
    
    FT a = compute_dot_prod(R_col_1, R_col_1);
    FT b = compute_dot_prod(R_col_1, R_col_2);
    FT c = b;
    FT d = compute_dot_prod(R_col_2, R_col_2);
    FT e = - compute_dot_prod(R_col_1, S);
    FT f = - compute_dot_prod(R_col_2, S);
    
    FT x = (d*e - b*f) / (a*d - b*c);
    FT y = (a*f - c*e) / (a*d - b*c);
    return Point(x, y);
}
