#include "scene.h"
#include "timer.h"

unsigned Scene::count_visible_sites() const
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

void Scene::collect_visible_points(std::vector<Point>& points) const
{
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        points.push_back(vi->get_position());
    }
}

void Scene::collect_visible_weights(std::vector<FT>& weights) const
{
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        weights.push_back(vi->get_weight());
    }
}

void Scene::collect_sites(std::vector<Point>& points,
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

void Scene::clear_triangulation()
{
    m_capacities.clear();
    m_vertices.clear();
    m_rt.clear();
}

void Scene::update_triangulation()
{
    std::vector<Point> points;
    std::vector<FT> weights;
    std::vector<FT> noise;
    collect_sites(points, weights, noise);
    construct_triangulation(points, weights, noise);
}

void Scene::construct_triangulation(const std::vector<Point>& points,
                                    const std::vector<FT>& weights,
                                    const std::vector<FT>& noise)
{
    if (m_timer_on)
        Timer::start_timer(m_timer, COLOR_BLUE, "Triangulation");
    
    clear_triangulation();
    populate_vertices(points, weights, noise);
    make_torus();
    compute_capacities(m_capacities);
    
    if (m_timer_on)
        Timer::stop_timer(m_timer, COLOR_BLUE);
}

void Scene::populate_vertices(const std::vector<Point>& points,
                              const std::vector<FT>& weights,
                              const std::vector<FT>& noise)
{
    unsigned nsites = points.size();
    for (unsigned i = 0; i < nsites; ++i)
    {
		//std::cout << "Populating (1/3) " << i << "/" << points.size() << "\r";
        Vertex_handle vertex = insert_vertex(points[i], weights[i], noise[i], i, -1);
        m_vertices.push_back(vertex);
    }
}

void Scene::make_torus()
{
    unsigned index = m_vertices.size();
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

Vertex_handle Scene::insert_vertex(const Point& point,
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

FT Scene::compute_mean_capacity() const
{
    FT domain_area = m_domain.compute_area();
    unsigned nb = count_visible_sites();
    return (domain_area / nb);
}

void Scene::compute_capacities(std::vector<FT>& capacities) const
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

void Scene::reset_weights()
{
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vertex = m_vertices[i];
        vertex->set_weight(0.0);
    }
    update_triangulation();
}

void Scene::update_positions(const std::vector<Point>& points, 
                             bool update_clone,
                             bool hidden)
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

void Scene::update_weights(const std::vector<FT>& weights, 
                           bool update_clone,
                           bool hidden)
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
