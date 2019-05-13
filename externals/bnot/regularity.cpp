#include "scene.h"
#include "random.h"

FT Scene::compute_regularity_threshold() const
{
    FT m = compute_mean_capacity();
    return m_tau*0.25*m*m;
}

bool Scene::detect_and_break_regularity(FT max_radius, unsigned max_teleport)
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
    std::cout << blocks.size() << " blocks detected" << std::endl;
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
    std::cout << for_jittering.size() << " vertices jittered" << std::endl;
    
    std::set<Vertex_handle> to_teleport;
    if (blocks.size() < max_teleport) 
        max_teleport = blocks.size();
    if (blocks.size() == max_teleport) 
    {
        to_teleport.insert(blocks.begin(), blocks.end());
    } else {
        while (to_teleport.size() < max_teleport)
        {
            unsigned index = random_int(0, blocks.size()-1);
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
    std::cout << max_teleport << " vertices teleported" << std::endl;
    
    reset_weights(); // it calls update
    
    return true;
}

FT Scene::compute_max_regularity() const
{
    std::vector<FT> variance;
    compute_variance_vector(variance);
    
    std::vector<FT> regularity;
    compute_regularity_vector(variance, regularity);
    
    FT max_value = 0.0;
    for (unsigned i = 0; i < regularity.size(); ++i)
        max_value = std::max(max_value, regularity[i]);
    return max_value;
}

FT Scene::compute_min_regularity() const
{
    std::vector<FT> variance;
    compute_variance_vector(variance);
    
    std::vector<FT> regularity;
    compute_regularity_vector(variance, regularity);
    
    FT min_value = 1.0e100;
    for (unsigned i = 0; i < regularity.size(); ++i)
        min_value = std::min(min_value, regularity[i]);
    return min_value;
}

void Scene::compute_variance_vector(std::vector<FT>& variance) const
{
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        FT V = 0.0;
        Vertex_handle vertex = m_vertices[i];        
        if (!vertex->is_hidden()) V = compute_variance(vertex);
        variance.push_back(V);
    }
}

void Scene::compute_regularity_vector(const std::vector<FT>& variance,
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

FT Scene::compute_regularity(Vertex_handle vi, const std::vector<FT>& variance) const
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

void Scene::jitter_vertices(const std::set<Vertex_handle>& vertices, const FT max_radius)
{
    for (std::set<Vertex_handle>::const_iterator it = vertices.begin(); it != vertices.end(); ++it)
    {
        Vertex_handle vertex = *it;
        Point p = vertex->get_position();
        p = jitter_point(p, max_radius);
        vertex->set_position(p);
    }
}

Point Scene::jitter_point(const Point& p, const FT max_radius) const
{
    FT angle = random_double(0.0, 2.0*M_PI);
    FT radius = random_double(0.0, max_radius);
    Vector d(radius*cos(angle), radius*sin(angle));
    return m_domain.clamp(p + d);
}

FT Scene::compute_alpha() const
{
    unsigned nb = count_visible_sites();    
    FT max_radius = std::sqrt(1.0 / (2.0*std::sqrt(3.0)*nb));
    if (max_radius < EPS) max_radius = 1.0;
    
    std::vector<FT> alpha;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        FT radius = compute_absolute_radius(vi);
        alpha.push_back(radius / max_radius);
    }
    
    FT mean_alpha = 0.0;
    FT min_alpha = 1.0e100;
    for (unsigned i = 0; i < alpha.size(); ++i)
    {
        mean_alpha += alpha[i];
        min_alpha = std::min(min_alpha, alpha[i]);
    }
    mean_alpha /= alpha.size();
    
    FT variance = 0.0;
    for (unsigned i = 0; i < alpha.size(); ++i)
    {
        FT value = alpha[i] - mean_alpha;
        variance += value*value;
    }
    variance /= alpha.size();
    
    // stats
    std::cout << "MinAlpha: " << min_alpha << std::endl;
    std::cout << "MeanAlpha: " << mean_alpha << std::endl;
    std::cout << "VarAlpha: " << variance << std::endl;
    //
    
    return min_alpha;
}

FT Scene::compute_absolute_radius(Vertex_handle vi) const
{
    FT min_dist = 1.0e10;
    const Point& pi = vi->get_position();
    Vertex_circulator vcirc = m_rt.incident_vertices(vi);
    Vertex_circulator vend  = vcirc;
    CGAL_For_all(vcirc, vend)
    {
        Vertex_handle vj = vcirc;
        const Point& pj = vj->get_position();
        FT dist = std::sqrt(CGAL::squared_distance(pi, pj));
        min_dist = std::min(min_dist, dist);
    }
    return 0.5*min_dist;
}
