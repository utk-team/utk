// 
#include <cmath>

// Qt
#include <QtOpenGL>

// local
#include "ramp.h"
#include "scene.h"

////////////
// BASICS //
////////////

void Scene::draw_point(const Point& point) const
{
    glBegin(GL_POINTS);
    glVertex2d(point.x(), point.y());
    glEnd();    
}

void Scene::draw_segment(const Point& s, const Point& t) const
{
	glBegin(GL_LINES);
    glVertex2d(s.x(), s.y());
    glVertex2d(t.x(), t.y());
	glEnd();
}

void Scene::draw_triangle(const Point& a, const Point& b, const Point& c) const
{
	glBegin(GL_TRIANGLES);
    glVertex2d(a.x(), a.y());
    glVertex2d(b.x(), b.y());
    glVertex2d(c.x(), c.y());
	glEnd();
}

void Scene::draw_polygon(const std::vector<Point>& polygon) const
{
    if (polygon.size() < 1) return;
    for (unsigned i = 1; i < polygon.size(); ++i)
    {
        const Point& a = polygon[i-1];
        const Point& b = polygon[i  ];
        draw_segment(a, b);
    }
    draw_segment(polygon.back(), polygon.front());
}

void Scene::draw_region(const std::vector<Point>& polygon) const
{
    if (polygon.size() < 2) return;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (unsigned i = 2; i < polygon.size(); ++i)
    {
        const Point& a = polygon[0];
        const Point& b = polygon[i-1];
        const Point& c = polygon[i  ];
        draw_triangle(a, b, c);
    }
}

void Scene::generate_circle(unsigned nb, 
                            const FT radius,
                            std::vector<Point>& pts) const
{
    if (nb < 3) nb = 3;
    FT step = 2*M_PI / nb;
    for (unsigned i = 0; i < nb; ++i)
    {
        FT angle = i*step;
        Point q(radius*cos(angle), radius*sin(angle));
        pts.push_back(q);
    }
}

void Scene::draw_circle(const Point& center, 
                        const FT scale,
                        const std::vector<Point>& pts) const
{
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glTranslated(center.x(), center.y(), 0.0);
    glScaled(scale, scale, scale);
    glBegin(GL_POLYGON);
    for (unsigned j = 0; j < pts.size(); ++j)
    {
        const Point& q = pts[j];
        glVertex2d(q.x(), q.y());
    }
    glEnd();
    glPopMatrix();
}

/////////////
// OBJECTS //
/////////////

void Scene::draw_domain(const float line_width,
                        const float red,
                        const float green,
                        const float blue) const
{
    glLineWidth(line_width);
    glColor3d(red, green, blue);
    m_domain.draw();
}

void Scene::draw_sites(const float point_size,
                       const float red,
                       const float green,
                       const float blue) const
{
    glPointSize(point_size);
    glColor3d(red, green, blue);
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;

        const Point& pi = vi->get_position();
        draw_point(pi);
    }
}

void Scene::draw_barycenter(const float point_size,
                            const float red,
                            const float green,
                            const float blue) const
{
    glLineWidth(1.0);
    glPointSize(point_size);
    glColor3d(red, green, blue);
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        const Point& xi = vi->get_position();
        const Point& bi = m_rt.compute_centroid(vi);
        draw_segment(xi, bi);
    }
}

void Scene::draw_vertices(const float point_size) const
{
    glPointSize(point_size);
    for (Finite_vertices_iterator vit = m_rt.finite_vertices_begin(); vit != m_rt.finite_vertices_end(); ++vit)
    {
        Vertex_handle vi = vit;
        if (vi->is_clone()) glColor3d(1., 0., 1.);
        else                glColor3d(1., 0., 0.);
        const Point& pi = vi->get_position();
        draw_point(pi);
    }
    for (Hidden_vertices_iterator vit = m_rt.hidden_vertices_begin(); vit != m_rt.hidden_vertices_end(); ++vit)
    {
        Vertex_handle vi = vit;
        if (vi->is_clone()) glColor3d(0., 1., 0.);
        else                glColor3d(1., 1., 0.);
        const Point& pi = vi->get_position();
        draw_point(pi);
    }    
}

void Scene::draw_primal(const float line_width,
                        const float red,
                        const float green,
                        const float blue) const
{
    glLineWidth(line_width);
    glColor3d(red, green, blue);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        Face_circulator fcirc = m_rt.incident_faces(vi);
        Face_circulator fend = fcirc;
        CGAL_For_all(fcirc, fend)
        {
            Face_handle face = fcirc;
            const Point& p0 = face->vertex(0)->get_position();
            const Point& p1 = face->vertex(1)->get_position();
            const Point& p2 = face->vertex(2)->get_position();
            draw_triangle(p0, p1, p2);
        }
    }
}

void Scene::draw_dual(const float line_width,
                      const float red,
                      const float green,
                      const float blue) const
{
    glLineWidth(line_width);
    glColor3d(red, green, blue);
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        draw_cell(vi, false);
    }
}

void Scene::draw_weights() const
{
    std::vector<Point> points;
    generate_circle(100, 1.0, points);
    
    /*
     FT max_R2 = 0.0;
     for (unsigned i = 0; i < m_vertices.size(); ++i) 
     {
     Vertex_handle vi = m_vertices[i];
     if (vi->is_hidden()) continue;
     FT R2 = std::abs(vi->get_weight());
     max_R2 = std::max(R2, max_R2);
     }
     
     FT avg_len = 0.25*m_rt.get_average_length();
     for (unsigned i = 0; i < m_vertices.size(); ++i) 
     {
     Vertex_handle vi = m_vertices[i];
     if (vi->is_hidden()) continue;
     
     const Point& p = vi->get_position();
     FT w = vi->get_weight();
     
     FT R = std::sqrt(std::abs(w) / max_R2);
     if (R < EPS) continue;
     
     R *= avg_len;
     if (w < 0.0) glColor3d(0., 0., 1.);
     else         glColor3d(1., 0., 0.);
     draw_circle(p, R, points);
     }
    */
    
    FT min_w = 0.0;
    FT max_w = 0.0;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        FT w = vi->get_weight();
        min_w = std::min(min_w, w);
        max_w = std::max(max_w, w);
    }
    FT range_w = max_w - min_w;
    if (range_w == 0.0) range_w = 1.0;
    
    std::vector<FT> value;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) 
        {
            value.push_back(0.0);
            continue;
        }
        FT w = vi->get_weight();
        FT t = (w - min_w) / range_w;
        value.push_back(t); //std::sqrt(t));
    }
    
    FT avg_len = 0.5*m_rt.get_average_length();
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        FT t = value[i];
        
        QColor c;
        c.setHsv(360*(1.0-t) + 120*t, 255, 225);
        glColor3d(c.redF(), c.greenF(), c.blueF());
        
        const Point& p = vi->get_position();
        draw_circle(p, avg_len, points);
    }
}

void Scene::draw_noise() const
{
    std::vector<Point> points;
    generate_circle(100, 1.0, points);
    
    FT max_R = 0.0;
    unsigned nb = 0;
    FT mean_noise = 0.0;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT ni = vi->get_noise();
        mean_noise += ni;
        nb++;
        
        FT R = std::abs(ni);
        max_R = std::max(R, max_R);
    }
    if (nb == 0) return;
    mean_noise /= nb;
    max_R -= mean_noise;
    
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        const Point& p = vi->get_position();
        FT ni = vi->get_noise();
        ni -= mean_noise;
        
        FT R = std::abs(ni);
        if (R < 1e-8) continue;
        
        R = 0.01 * (R / max_R);
        if (ni < 0.0) glColor3d(0.0, 0.5, 1.0);
        else          glColor3d(1.0, 0.5, 0.0);
        draw_circle(p, R, points);
    }
}

void Scene::draw_capacity() const
{
    FT max_value = 0.0;
    std::vector<FT> values;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;

        FT A = m_rt.compute_area(vi);
        FT C = m_capacities[vi->get_index()];
        FT ratio = A / C;
        values.push_back(ratio);
        max_value = std::max(ratio, max_value);
    }
    std::cout << "CapacityRange: " << max_value << std::endl;
    
    Ramp ramp;
    unsigned j = 0;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;

        FT ratio = values[j++];
        ramp.gl_color(ratio, max_value);
        draw_cell(vi, true);
    }
}

void Scene::draw_regular_sites() const
{
    std::vector<FT> variance;
    compute_variance_vector(variance);
    
    std::vector<FT> regularity;
    compute_regularity_vector(variance, regularity);
    
    FT threshold = compute_regularity_threshold();
    std::cout << "Regularity Threshold: " << threshold << std::endl;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT value = regularity[vi->get_index()];
        if (value > threshold) continue;
        glColor3d(0.7, 0.2, 0.2);
        draw_cell(vi, true);
    }
}

void Scene::draw_regularity() const
{
    std::vector<FT> variance;
    compute_variance_vector(variance);

    std::vector<FT> regularity;
    compute_regularity_vector(variance, regularity);

    FT max_value = 0.0;
    FT min_value = 1.0e100;
    for (unsigned i = 0; i < regularity.size(); ++i)
    {
        max_value = std::max(max_value, regularity[i]);
        min_value = std::min(min_value, regularity[i]);        
    }
    std::cout << "RegularityRange: " << max_value - min_value << std::endl;
    
    Ramp ramp;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT value = regularity[vi->get_index()];
        ramp.gl_color(value-min_value, max_value-min_value);
        draw_cell(vi, true);
    }
}

void Scene::draw_variance() const
{
    FT max_variance = 0.0;
    FT min_variance = 1.0e100;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT variance = compute_cvt_energy_per_vertex(vi);
        min_variance = std::min(min_variance, variance);
        max_variance = std::max(max_variance, variance);
    }
    std::cout << "VarianceRange: " << min_variance << " ; " <<  max_variance << std::endl;
    
    Ramp ramp;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT variance = compute_cvt_energy_per_vertex(vi);
        ramp.gl_color(variance-min_variance, max_variance-min_variance);
        draw_cell(vi, true);
    }
}

void Scene::draw_cell(Vertex_handle vertex, bool filled) const
{
    std::vector<Point> polygon;
    m_rt.build_polygon(vertex, std::back_inserter(polygon));
    if (filled) draw_region(polygon);
    else        draw_polygon(polygon);
}

void Scene::draw_capacity_histogram(const unsigned nbins, 
                                    const double xmin,
                                    const double xmax,
                                    const double ymin,
                                    const double ymax) const
{
    std::vector<unsigned> histogram(nbins, 0);
    compute_capacity_histogram(histogram);
    draw_histogram(histogram, xmin, xmax, ymin, ymax);
}
    
void Scene::draw_weight_histogram(const double range,
                                  const unsigned nbins, 
                                  const double xmin,
                                  const double xmax,
                                  const double ymin,
                                  const double ymax) const
{
    std::vector<unsigned> histogram(nbins, 0);
    compute_weight_histogram(range, histogram);
    draw_histogram(histogram, xmin, xmax, ymin, ymax);    
}

void Scene::draw_noise_histogram(const double range,
                                 const unsigned nbins, 
                                 const double xmin,
                                 const double xmax,
                                 const double ymin,
                                 const double ymax) const
{
    std::vector<unsigned> histogram(nbins, 0);
    compute_noise_histogram(range, histogram);
    draw_histogram(histogram, xmin, xmax, ymin, ymax);    
}

void Scene::draw_enclosing_circ_histogram(const double range,
                                 const unsigned nbins,
                                 const double xmin,
                                 const double xmax,
                                 const double ymin,
                                 const double ymax) const
{
    std::vector<unsigned> histogram(nbins, 0);
    compute_enclosing_circ_histogram(range, histogram);
    draw_histogram(histogram, xmin, xmax, ymin, ymax);
}

void Scene::draw_histogram(const std::vector<unsigned>& histogram, 
                           const double xmin,
                           const double xmax,
                           const double ymin,
                           const double ymax) const
{
    unsigned nbins = histogram.size();
    if (nbins < 2) return;

    // horizontal line
    glColor3d(0., 0., 0.);
    glBegin(GL_LINES);
    glVertex2d(xmin, ymin);
    glVertex2d(xmax, ymin);
    glEnd();
    
    // mean line
    double xstep = (xmax - xmin) / (nbins-1);
    unsigned offset = unsigned(ceil(0.5*nbins));
    glColor3d(1., 0., 1.);
    glBegin(GL_LINES);
    glVertex2d(xmin + offset*xstep, ymin);
    glVertex2d(xmin + offset*xstep, ymax);
    glEnd();

    unsigned max_value = 0;
    for (unsigned i = 0; i < histogram.size(); ++i)
        max_value = std::max(max_value, histogram[i]);
    if (max_value == 0) return;

    // debug
    //std::cout << "HistMax: " << max_value << std::endl;
    //
    
    glColor3d(0.3, 0.3, 0.3);
    for (unsigned i = 0; i < nbins; ++i)
    {
        double height = double(histogram[i]) / double(max_value); // FIXME: scale height
        double x0 = xmin + i*xstep;
        double y0 = ymin;
        double y1 = ymin + (ymax - ymin)*height;
     
        glBegin(GL_LINES);
        glVertex2d(x0, y0);
        glVertex2d(x0, y1);
        glEnd();
    }
}
