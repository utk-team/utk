#include "scene.h"

bool Scene::optimize_positions_via_ODT(bool update) 
{    
    std::vector<Point> points;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT one_ring_area = 0.0;
        Vector sum_vec = CGAL::NULL_VECTOR;
        Face_circulator fcirc = m_rt.incident_faces(vi);
        Face_circulator fend = fcirc;
        CGAL_For_all(fcirc, fend)
        {
            Face_handle face = fcirc;
            Triangle triangle = m_rt.get_triangle(face);
            
            Point centroid = CGAL::centroid(triangle);
            FT area = triangle.area();
            
            one_ring_area += area;
            sum_vec = sum_vec + area*(centroid - CGAL::ORIGIN);
        }
        
        Point pi = CGAL::ORIGIN + (sum_vec / one_ring_area);
        points.push_back(pi);
    }
    update_positions(points, !update);
    if (update) update_triangulation();
}

