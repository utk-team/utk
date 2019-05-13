#include <QColor>
#include "scene.h"

unsigned Scene::load(const QString& filename)
{
    std::vector<Point> points;
    std::vector<FT> weights;
    std::vector<FT> noise;
    
    if (filename.contains(".dat", Qt::CaseInsensitive))
        load_dat_file(filename, points);
    else if (filename.contains(".txt", Qt::CaseInsensitive))
        load_txt_file(filename, points);
    
    if(points.empty()) {
        std::cout << red << "try (.dat) or (.txt) file format" << white << std::endl;
        return 0;
    }
    
    weights.resize(points.size(), 0.0);
    noise.resize(points.size(), 0.0);
    
    clear();
    construct_triangulation(points, weights, noise);
    return m_vertices.size();
}

void Scene::load_dat_file(const QString& filename, std::vector<Point>& points) const
{
    std::ifstream ifs(qPrintable(filename));
    Point point;
    while (ifs >> point) points.push_back(point);
    ifs.close();
}

void Scene::load_txt_file(const QString& filename, std::vector<Point>& points) const
{
    std::ifstream ifs(qPrintable(filename));
    
    // kill the first line (contains the number of points)
    int numPoints;
    ifs >> numPoints;
    
    Point point;
    while (ifs >> point) points.push_back(point);
    ifs.close();
}

void Scene::save(const QString& filename) const
{
    std::vector<Point> points;
    collect_visible_points(points);
    
    if (filename.contains(".dat", Qt::CaseInsensitive))
    {
        save_dat(filename, points);
        return;
    }
    
    if (filename.contains(".txt", Qt::CaseInsensitive))
    {
        save_txt(filename, points);
        return;
    }
    
    if (filename.contains(".csv", Qt::CaseInsensitive))
    {
        save_csv(filename);
        return;
    }
    
    if (filename.contains(".eps", Qt::CaseInsensitive))
    {
        save_eps(filename);
        return;
    }
    
    if (filename.contains(".w", Qt::CaseInsensitive))
    {
        save_weighted_points(filename);
        return;
    }
    
    if (filename.contains(".histw", Qt::CaseInsensitive))
    {
        save_weight_histogram(filename);
        return;
    }
    
    if (filename.contains(".r", Qt::CaseInsensitive))
    {
        save_weights(filename);
        return;
    }
    
    std::cout << red << "try (.dat, .txt, .csv, .eps. .w, .histw, .r) file format" << white << std::endl;
}

void Scene::save_dat(const QString& filename, const std::vector<Point>& points) const
{
    std::ofstream ofs(qPrintable(filename));

    for (unsigned i = 0; i < points.size(); ++i)
    {
        ofs << points[i] << std::endl;
    }
    ofs.close();
}

void Scene::save_txt(const QString& filename, const std::vector<Point>& points) const
{
    std::ofstream ofs(qPrintable(filename));
    ofs << points.size() << std::endl;
    for (unsigned i = 0; i < points.size(); ++i)
    {
        ofs << points[i] << std::endl;
    }
    ofs.close();
}

void Scene::save_csv(const QString& filename) const
{
    std::ofstream ofs(qPrintable(filename));
    ofs << "Weight, DualArea, Capacity" << std::endl;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT wi = vi->get_weight();
        FT Vi = m_rt.compute_area(vi);
        FT Ci = m_capacities[i];
        ofs << wi << ", " << Vi << ", " << Ci << std::endl;
    }
    ofs.close();
}

void Scene::save_eps(const QString& filename) const
{
    double scale  = 512.;
    double radius = 2.75;

    double min = 0;
    double max = 2 * radius + scale;
    radius /= scale;

    std::ofstream ofs(qPrintable(filename), std::ofstream::out | std::ofstream::trunc );
    ofs << "%!PS-Adobe-3.1 EPSF-3.0\n";
    ofs << "%%HiResBoundingBox: " << min << " " << min << " " << max << " " << max << std::endl;
    ofs << "%%BoundingBox: " << min << " " << min << " " << max << " " << max << std::endl;
    ofs << "%%CropBox: " << min << " " << min << " " << max << " " << max << "\n";
    ofs << "/radius { " << radius << " } def\n";
    ofs << "/p { radius 0 360 arc closepath fill stroke } def\n";
    ofs << "gsave " << scale << " " << scale << " scale\n";
    ofs << "0 0 0 setrgbcolor" << std::endl;

    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        const Point& pi = vi->get_position();
        ofs << pi.x() << " " << pi.y() << " p" << std::endl;
    }

    ofs << "grestore" << std::endl ;
    ofs.close();
}

void Scene::save_weight_histogram(const QString& filename) const
{
    std::vector<unsigned> histogram(256);
    compute_weight_histogram(1.0, histogram);
    
    std::ofstream ofs(qPrintable(filename));
    ofs << histogram.size() << std::endl;
    for (unsigned i = 0; i < histogram.size(); ++i)
    {
        ofs << histogram[i] << std::endl;
    }
    ofs.close();
}

void Scene::save_weights(const QString& filename) const
{
    std::ofstream ofs(qPrintable(filename));
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        double w = vi->get_weight();
        ofs << w << std::endl;
    }
    ofs.close();    
}

void Scene::save_weighted_points(const QString& filename) const
{
    double scale  = 512.;
    double radius = 2.75;
    double wx = scale + 2.0*radius;
    double wy = scale + 2.0*radius;
    
    double min_x = 0;
    double max_x = wx;
    double min_y = 0;
    double max_y = wy;
    radius = 0.005;;

    double min_w = 0.0;
    double max_w = 0.0;
    double max_R2 = 0.0;
    std::vector<double> x, y;
    std::vector<double> weights;
    for (unsigned i = 0; i < m_vertices.size(); ++i) 
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        double w = vi->get_weight();
        Point p = vi->get_position();
        max_R2 = std::max(max_R2, std::abs(w));
        max_w = std::max(max_w, w);
        min_w = std::min(min_w, w);
        
        weights.push_back(w);
        x.push_back(p.x());
        y.push_back(p.y());
    }
    double range_w = max_w - min_w;
    if (range_w == 0.0) range_w = 1.0;
    
    std::vector<double> radii;
    double avg_len = m_rt.get_average_length();
    for (unsigned i = 0; i < weights.size(); ++i) 
    {
        //double w = weights[i];
        //double r = 0.5 * avg_len * std::sqrt(std::abs(w) / max_R2);
        double r = radius;
        radii.push_back(r);
    }
    
    std::ofstream ofs(qPrintable(filename));
    ofs << "%!PS-Adobe-3.1 EPSF-3.0\n";
    ofs << "%%HiResBoundingBox: " 
    << min_x << " " << min_y << " " << max_x << " " << max_y << std::endl;
    ofs << "%%BoundingBox: " 
    << min_x << " " << min_y << " " << max_x << " " << max_y << std::endl;
    ofs << "%%CropBox: " 
    << min_x << " " << min_y << " " << max_x << " " << max_y << "\n";
    ofs << "gsave " << scale << " " << scale << " scale\n";
    
    for (unsigned i = 0; i < weights.size(); ++i) 
    {
        ofs << x[i] << " " << y[i] << "  " << radii[i];
        ofs << " 0 360 arc closepath ";
        
        /*
         if (weights[i] < 0.0)
         ofs << "0 0 1 ";
         else
         ofs << "1 0 0 ";
         */
        
        QColor c;
        double t = (weights[i] - min_w)/range_w;
        c.setHsv(360*(1.0-t) + 120*t, 255, 225);
        
        ofs << c.redF() << " " << c.greenF() << " " << c.blueF() << " ";
        ofs << "setrgbcolor fill" << std::endl;
    }
    
    ofs << "grestore" << std::endl;
    ofs.close();
}
