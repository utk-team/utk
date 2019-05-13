#include "scene.h"
#include "random.h"
#include "boost/random.hpp"

void Scene::generate_random_sites(const unsigned nb)
{
    std::vector<Point> points;
    for (unsigned i = 0; i < nb; ++i)
    {
        double x = random_double(0.0, 1.0);
        double y = random_double(0.0, 1.0);
        points.push_back(Point(x, y));
    }

    std::vector<FT> noise(points.size(), 0.0);
    std::vector<FT> weights(points.size(), 0.0);
    construct_triangulation(points, weights, noise);
    std::cout << "Insert " << points.size() << std::endl;
}

void Scene::generate_random_weights(const double epsilon)
{
    std::vector<FT> weights;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT wi = random_double(-epsilon, epsilon);
        weights.push_back(wi);
    }
    if (weights.empty()) return;
    remove_mean(weights);

    unsigned j = 0;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        FT wi = 0.0;
        Vertex_handle vi = m_vertices[i];
        if (!vi->is_hidden()) wi = weights[j++];
        vi->set_weight(wi);
    }
    update_triangulation();
}

void Scene::generate_regular_grid(const unsigned nx, const unsigned ny)
{
    FT stepx = 1.0 / nx;
    FT stepy = 1.0 / ny;
    std::vector<Point> points;
    for (unsigned i = 0; i < nx; ++i)
    {
        FT x = (i + 0.5)*stepx;
        for (unsigned j = 0; j < ny; ++j)
        {
            FT y = (j + 0.5)*stepy;
            points.push_back(Point(x, y));
        }
    }
    
    std::vector<FT> noise(points.size(), 0.0);
    std::vector<FT> weights(points.size(), 0.0);
    construct_triangulation(points, weights, noise);
    std::cout << "Insert " << points.size() << std::endl;
}

void Scene::generate_hextille_grid(const unsigned nb)
{
    FT step = 1.0 / nb;
    std::vector<Point> points;
    for (unsigned i = 0; i < nb; ++i)
    {
        FT y = (i + 0.5)*step;
        for (unsigned j = 0; j < nb; ++j)
        {
            FT x = (j + 0.5)*step;
            if (i % 2 == 1) x += 0.5*step;
            points.push_back(Point(x, y));
        }
    }
    
    std::vector<FT> noise(points.size(), 0.0);
    std::vector<FT> weights(points.size(), 0.0);
    construct_triangulation(points, weights, noise);
    std::cout << "Insert " << points.size() << std::endl;
}

void Scene::add_white_noise(const double range)
{
    if (m_vertices.empty())
    {
        std::cout << red << "empty scene" << white << std::endl;
        return;
    }
    
    std::vector<FT> noise;
    std::vector<FT> weights;
    std::vector<Point> points;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        weights.push_back(vi->get_weight());
        points.push_back(vi->get_position());

        FT ni = random_double(-range, range);
        noise.push_back(ni);
    }
    construct_triangulation(points, weights, noise);
}


void Scene::add_gaussian_noise(const double range, const double deviation)
{
    if (m_vertices.empty())
    {
        std::cout << red << "empty scene" << white << std::endl;
        return;
    }

    MTGenerator mtg(static_cast<unsigned> (time(0)));
    NormalDist gaussian(0.0, deviation);
    GaussianGenerator gaussian_generator(mtg, gaussian);

    std::vector<FT> noise;
    std::vector<FT> weights;
    std::vector<Point> points;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        weights.push_back(vi->get_weight());
        points.push_back(vi->get_position());

        // truncated gaussian via accept-reject
        FT ni = gaussian_generator();
        while (std::abs(ni) > range) 
            ni = gaussian_generator();
        noise.push_back(ni);
    }
    construct_triangulation(points, weights, noise);
}

void Scene::add_displacement(const double radius)
{
    if (m_vertices.empty())
    {
        std::cout << red << "empty scene" << white << std::endl;
        return;
    }
    
    Vector mean(0.0, 0.0);
    std::vector<Vector> displ;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {    
        FT angle = random_double(0.0, 2.0*M_PI);
        Vector di(cos(angle), sin(angle));
        displ.push_back(di);
        mean = mean + di;
    }
    mean = mean / displ.size();
    
    std::vector<FT> noise;
    std::vector<FT> weights;
    std::vector<Point> points;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        noise.push_back(vi->get_noise());
        weights.push_back(vi->get_weight());
        
        Point pi = vi->get_position() + radius*displ[i];
        pi = m_domain.clamp(pi);
        points.push_back(pi);
    }
    construct_triangulation(points, weights, noise);
}
