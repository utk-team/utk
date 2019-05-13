#include "scene.h"

void Scene::compute_capacity_histogram(std::vector<unsigned>& histogram) const
{
    unsigned nsites = count_visible_sites();
    if (nsites == 0) return;
    
    unsigned nbins = histogram.size();
    FT mean = m_domain.compute_area() / nsites;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT value = 0.5 * (m_rt.compute_area(vi) / mean);
        if (value > 1.0) value = 1.0;
        if (value < 0.0) value = 0.0;
        
        unsigned bin = unsigned(ceil(value*(nbins-1)));
        histogram[bin]++;
    }
}

void Scene::compute_weight_histogram(const double range, std::vector<unsigned>& histogram) const
{
    unsigned nsites = count_visible_sites();
    if (nsites == 0) return;
    
    std::vector<FT> weights;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        FT value = vi->get_weight();
        weights.push_back(value);
    }
    
    FT sum = 0.0;
    FT min_w = 0.0;
    FT max_w = 0.0;
    for (unsigned i = 0; i < weights.size(); ++i)
    {
        min_w = std::min(min_w, weights[i]);
        max_w = std::max(max_w, weights[i]);
        sum += weights[i];
    }
    FT range_w = max_w - min_w;
    if (range_w == 0.0) range_w = 1.0;    
    FT avg_len2 = std::pow(m_rt.get_average_length(), 2);
    
    std::cout << "AvgLen2: " << avg_len2 << std::endl;
    std::cout << "WeightsRange: " << min_w << " ; " << max_w << std::endl;
    std::cout << "Weights\%: " << min_w/avg_len2 << " ; " << max_w/avg_len2 << std::endl;
    
    unsigned nbins = histogram.size();
    for (unsigned i = 0; i < weights.size(); ++i)
    {
        FT value = (weights[i] - min_w) / range_w;
        unsigned bin = unsigned(ceil(value*(nbins-1)));
        histogram[bin]++;
    }
}

void Scene::compute_noise_histogram(const double range, std::vector<unsigned>& histogram) const
{
    unsigned nsites = count_visible_sites();
    if (nsites == 0) return;
    
    unsigned nbins = histogram.size();
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT value = vi->get_noise();
        if (value >  range) value =  range;
        if (value < -range) value = -range;
        
        value = (value + range) / (2*range);
        unsigned bin = unsigned(ceil(value*(nbins-1)));
        histogram[bin]++;
    }
}

void Scene::compute_enclosing_circ_histogram(const double range, std::vector<unsigned>& histogram) const
{
    unsigned nsites = count_visible_sites();
    if (nsites == 0) return;

    unsigned nbins = histogram.size();
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;

        Face_circulator fcirc = m_rt.incident_faces(vi);
        Face_circulator fend  = fcirc;
        FT max_dist = 0.;

        CGAL_For_all(fcirc, fend)
        {
            Face_handle face = fcirc;
            Point cw = m_rt.get_cw(face);
            FT dist_to_centroid = CGAL::squared_distance(cw, vi->get_position());
            max_dist = std::max(max_dist, dist_to_centroid);
        }

        if (max_dist > range) max_dist = range;
        if (max_dist < -range) max_dist = -range;

        max_dist = (max_dist + range) / (2*range);
        unsigned bin = unsigned(ceil(max_dist*(nbins-1)));
        histogram[bin]++;
    }
}


