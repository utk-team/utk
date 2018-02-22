#include "scene.h"
#include "timer.h"
#include "penalty_line_search.h"

typedef CPenaltyLS<Scene, Point, Vector> PenaltyLS;

FT Scene::compute_penalty_threshold(FT epsilon, bool use_grad) const
{
    if (use_grad)
    {
        // reference: 1e-4 for 1000 sites
        FT A = compute_total_area();
        unsigned N = count_visible_sites();
        return (0.1*epsilon) * (std::sqrt(A*A*A)) / FT(N);
    }
    
    // reference: 1e-6 for 1000 sites
    FT A = compute_total_area();
    return (1.0e-6*epsilon) * (A*A);
}

unsigned Scene::optimize_positions_via_penalty_until_converge(FT stiffness, 
                                                              FT timestep,
                                                              FT epsilon, 
                                                              unsigned max_iters,
                                                              bool use_grad)
{
    FT E0 = compute_penalty_energy(stiffness);
    FT threshold = compute_penalty_threshold(epsilon, use_grad);
    std::cout << "Threshold: " << threshold << std::endl;
    for (unsigned iters = 0; iters < max_iters; ++iters)
    {
        FT norm = optimize_positions_via_penalty(stiffness, timestep);
        std::cout << "Norm: " << norm << std::endl;        
        
        FT E1 = compute_penalty_energy(stiffness);
        FT rate = (E0-E1) / E0;
        std::cout << "Rate: " << rate << std::endl;        
        E0 = E1;
        
        if (use_grad) {
            if (norm < threshold) return iters;
        } else {
            if (rate > 0.0 && rate < threshold) return iters;
        }
    }
    return max_iters;
}

FT Scene::optimize_positions_via_penalty(FT stiffness, FT timestep)
{
    std::vector<Point> points;
    collect_visible_points(points);
    
    std::vector<Vector> gradient;
    compute_penalty_gradient(gradient, stiffness);
    
    std::vector<Vector> direction;
    m_lbfgs_x.solve(gradient, direction);
    
    if (timestep <= 0.0)
    {        
        PenaltyLS line_search(this, stiffness, 100, 2.0);
        timestep = line_search.run_bt(points, direction);
    }
    //std::cout << "Step: " << timestep << std::endl;
    
    std::vector<Point> new_points;
    for (unsigned i = 0; i < points.size(); ++i)
    {
        Point  pi = points[i];
        Vector gi = direction[i];
        new_points.push_back( pi + timestep * gi );
    }
    update_positions(new_points, false);
    update_triangulation();
    
    std::vector<Vector> new_gradient;
    compute_penalty_gradient(new_gradient, stiffness);

    if (m_lbfgs_x.memory_limit() > 0)
        update_lbfgs(points, new_points, gradient, new_gradient);

    return compute_norm(new_gradient);
}

void Scene::update_lbfgs(const std::vector<Point>& p0, const std::vector<Point>& p1,
                         const std::vector<Vector>& v0, const std::vector<Vector>& v1)
{
    std::vector<Vector> s;
    for (unsigned i = 0; i < p0.size(); ++i)
        s.push_back( p1[i] - p0[i] );
    
    std::vector<Vector> y;
    for (unsigned i = 0; i < v0.size(); ++i)
        y.push_back( v1[i] - v0[i] );
    
    m_lbfgs_x.append(s, y);
}

FT Scene::compute_penalty_energy(FT stiffness) const
{
    FT cvt = 0.0;
    FT constraints = 0.0;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {    
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        cvt += compute_cvt_energy_per_vertex(vi);
        FT Vi = m_rt.compute_area(vi);
        constraints += Vi*Vi;        
    }
    return cvt + stiffness*constraints;
}

void Scene::compute_penalty_gradient(std::vector<Vector>& gradient, 
                                     FT stiffness,
                                     FT coef) const
{
    gradient.clear();
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];        
        if (vi->is_hidden()) continue;
        
        Vector gi = compute_penalty_gradient_per_vertex(vi, stiffness);
        gradient.push_back(coef * gi);
    }
}

Vector Scene::compute_penalty_gradient_per_vertex(Vertex_handle vi, FT stiffness) const
{
    // Assumption: vi is visible
    FT Vi = m_rt.compute_area(vi);
    Point xi = vi->get_position();
    Point ci = m_rt.compute_centroid(vi);
    Vector grad_cvt = 2.0*Vi*(xi - ci);

    Vector grad_penalty = CGAL::NULL_VECTOR;
    Edge_circulator ecirc = m_rt.incident_edges(vi);
    Edge_circulator eend = ecirc;
    CGAL_For_all(ecirc, eend)
    {
        Edge edge = *ecirc;
        Vertex_handle vj = m_rt.get_source(edge);
        if (vj == vi) vj = m_rt.get_target(edge);
        
        FT Vj = m_rt.compute_area(vj);
        FT ratio = m_rt.get_ratio(edge);
        Point cij = m_rt.get_dual_edge_midpoint(edge);

        grad_penalty = grad_penalty + (Vi - Vj)*ratio*(cij - xi);
    }
    
    return grad_cvt + 2.0*stiffness*grad_penalty;
}
