#ifndef _SCENE_H_
#define _SCENE_H_

// STL
#include <set>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

//Qt
#include <QtOpenGL>
#include <QString>

// local
#include "matrix/sparse_matrix.h"
#include "console_color.h"
#include "lbfgs.h"
#include "types.h"
#include "util.h"

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

class Scene
{
public:
    typedef boost::mt19937 MTGenerator;
    typedef boost::normal_distribution<double> NormalDist;
    typedef boost::variate_generator<MTGenerator&, NormalDist> GaussianGenerator;

private:
    RT m_rt;
    Domain m_domain;
    std::vector<FT> m_capacities;
    std::vector<Vertex_handle> m_vertices;

    bool m_timer_on;
    std::vector<double> m_timer;

    bool m_fixed_connectivity;

    typedef CLBFGS<Vector> LBFGS;
    LBFGS m_lbfgs_x;
    
    double m_tau;
    
public:
    Scene()
    {
        srand(time(0));
        m_timer_on = false;
        m_fixed_connectivity = true;
        m_lbfgs_x.set_limit(7);
        m_tau = 1.0;
    }
    
    ~Scene()
    {
        clear();
    }    
    
    double get_tau() const { return m_tau; }
    void set_tau(double tau) { m_tau = tau; }
    
    void toggle_timer() { m_timer_on = !m_timer_on; }
    
    void toggle_connectivity() { m_fixed_connectivity = !m_fixed_connectivity; }
    
    bool connectivity_fixed() const { return m_fixed_connectivity; }
    
    void set_lbfgs(unsigned nb) { m_lbfgs_x.set_limit(nb); }
    
    void clear_lbfgs() { m_lbfgs_x.clear(); }
    
    void clear() 
    { 
        clear_triangulation(); 
        clear_lbfgs();
    }
    
    unsigned num_points() { return m_vertices.size(); }
    
    // IO //
    
    unsigned load(const QString& filename);
    
    void load_dat_file(const QString& filename, std::vector<Point>& points) const;
    
    void load_txt_file(const QString& filename, std::vector<Point>& points) const;

    void save(const QString& filename) const;
    
    void save_dat(const QString& filename, const std::vector<Point>& points) const;
    
    void save_txt(const QString& filename, const std::vector<Point>& points) const;
    
    void save_csv(const QString& filename) const;
    
    void save_eps(const QString& filename) const;

    void save_weights(const QString& filename) const;
    
    void save_weighted_points(const QString& filename) const;

    void save_weight_histogram(const QString& filename) const;

    // SITES //
    
    void generate_random_sites(const unsigned nb);
    
    void generate_random_weights(const double epsilon);
    
    void generate_regular_grid(const unsigned nx, const unsigned ny);

    void generate_hextille_grid(const unsigned nb);
    
    void add_white_noise(const double range);

    void add_gaussian_noise(const double range, const double deviation);

    void add_displacement(const double radius);
	
    // RENDER //
    
    void draw_domain(const float line_width,
                     const float red,
                     const float green,
                     const float blue) const;

    void draw_sites(const float point_size,
                    const float red,
                    const float green,
                    const float blue) const;

    void draw_vertices(const float point_size) const;
    
    void draw_primal(const float line_width,
                     const float red,
                     const float green,
                     const float blue) const;
    
    void draw_dual(const float line_width,
                   const float red,
                   const float green,
                   const float blue) const;

    void draw_weights() const;

    void draw_noise() const;

    void draw_capacity() const;
    
    void draw_variance() const;

    void draw_regularity() const;

    void draw_regular_sites() const;

    void draw_cell(Vertex_handle vertex, bool filled) const;
    
    void draw_barycenter(const float point_size,
                         const float red,
                         const float green,
                         const float blue) const;

    void draw_capacity_histogram(const unsigned nbins, 
                                 const double xmin,
                                 const double xmax,
                                 const double ymin,
                                 const double ymax) const;

    void draw_weight_histogram(const double range,
                               const unsigned nbins, 
                               const double xmin,
                               const double xmax,
                               const double ymin,
                               const double ymax) const;

    void draw_noise_histogram(const double range,
                              const unsigned nbins, 
                              const double xmin,
                              const double xmax,
                              const double ymin,
                              const double ymax) const;

    void draw_enclosing_circ_histogram(const double range,
                              const unsigned nbins,
                              const double xmin,
                              const double xmax,
                              const double ymin,
                              const double ymax) const;
    
    void draw_point(const Point& a) const; 
    
    void draw_segment(const Point& a, const Point& b) const;    

    void draw_triangle(const Point& a, const Point& b, const Point& c) const;    

    void draw_polygon(const std::vector<Point>& polygon) const;

    void draw_region(const std::vector<Point>& polygon) const;

    void generate_circle(unsigned nb, 
                         const FT radius,
                         std::vector<Point>& pts) const;
    
    void draw_circle(const Point& center, 
                     const FT scale, 
                     const std::vector<Point>& pts) const;
    
    void draw_histogram(const std::vector<unsigned>& histogram,
                        const double xmin,
                        const double xmax,
                        const double ymin,
                        const double ymax) const;

    // HISTOGRAM //
    
    void compute_capacity_histogram(std::vector<unsigned>& histogram) const;    

    void compute_weight_histogram(const double range, std::vector<unsigned>& histogram) const;    

    void compute_noise_histogram(const double range, std::vector<unsigned>& histogram) const;    

    void compute_enclosing_circ_histogram(const double range, std::vector<unsigned>& histogram) const;

    // INIT //

    FT compute_mean_capacity() const;

    unsigned count_visible_sites() const;
    
    void collect_visible_points(std::vector<Point>& points) const;

    void collect_visible_weights(std::vector<FT>& weights) const;

    void collect_sites(std::vector<Point>& points,
                       std::vector<FT>& weights,
                       std::vector<FT>& noise) const;
    
    void clear_triangulation();
    
    void update_triangulation();
    
    void construct_triangulation(const std::vector<Point>& points,
                                 const std::vector<FT>& weights,
                                 const std::vector<FT>& noise);
    
    void populate_vertices(const std::vector<Point>& points,
                           const std::vector<FT>& weights,
                           const std::vector<FT>& noise);
    
    void make_torus();
    
    Vertex_handle insert_vertex(const Point& point,
                                const FT weight,
                                const FT noise, 
                                const unsigned index,
                                const int clone);
    
    void compute_capacities(std::vector<FT>& capacities) const;

    void reset_weights();
    
    void update_positions(const std::vector<Point>& points, 
                          bool update_clone,
                          bool hidden = true);

    void update_weights(const std::vector<FT>& weights, 
                        bool update_clone,
                        bool hidden = true);

    // ENERGY //
    
    FT compute_total_area() const;
    
    FT compute_cvt_energy() const;
    
    FT compute_wcvt_energy();

    FT compute_cvt_energy_per_vertex(Vertex_handle vertex) const;
        
    FT compute_cvt_energy_per_edge(Edge edge, Vertex_handle vi) const;
    
    FT compute_variance(Vertex_handle vertex) const;
    
    FT compute_variance_per_segment(const Point& x, const Segment& ab) const;
    
    FT compute_variance_per_right_triangle(const Point& x, const Point& a, const Point& b) const;
    
    void compute_position_gradient(std::vector<Vector>& gradient, FT coef = 1.0);
    
    void compute_weight_gradient(std::vector<FT>& gradient, FT coef = 1.0);
    
    FT compute_weight_gradient_numeric(Vertex_handle vertex);
    
    FT compute_weight_threshold(FT epsilon) const;
    
    FT compute_position_threshold(FT epsilon) const;

    // OPTIMIZER //
    
    FT optimize_positions_via_lloyd(bool update);

    FT optimize_positions_via_gradient_ascent(FT timestep, 
                                              bool update);

    FT optimize_weights_via_gradient_descent(FT timestep, 
                                             bool update);    
    
    FT optimize_weights_via_newton(FT timestep, 
                                   bool update);
    
    unsigned optimize_weights_via_gradient_descent_until_converge(FT timestep, 
                                                                  FT threshold,
                                                                  unsigned update,
                                                                  unsigned max_iters);
    
    unsigned optimize_weights_via_newton_until_converge(FT timestep, 
                                                        FT threshold,
                                                        unsigned update,
                                                        unsigned max_iters);

    unsigned optimize_all(FT wstep, FT xstep, unsigned max_newton_iters,
                          FT epsilon, unsigned max_iters,
                          std::ostream& out,
                          bool breakCon = true);

    bool solve_newton_step(const std::vector<FT>& b, 
                           std::vector<FT>& x);
    
    void build_laplacian(const FT scale,
                         const std::map<unsigned, unsigned>& indices,
                         SparseMatrix& A) const;
    
    bool solve_linear_system(const SparseMatrix& A,
                             std::vector<double>& x,
                             const std::vector<double>& b) const;

    // REGULARITY //
    
    bool detect_and_break_regularity(FT max_radius, unsigned max_teleport);
    
    FT compute_regularity_threshold() const;
    
    FT compute_max_regularity() const;

    FT compute_min_regularity() const;
    
    void compute_variance_vector(std::vector<FT>& variance) const;
    
    void compute_regularity_vector(const std::vector<FT>& variance,
                                   std::vector<FT>& regularity) const;
    
    FT compute_regularity(Vertex_handle vi, const std::vector<FT>& variance) const;
    
    void jitter_vertices(const std::set<Vertex_handle>& vertices, const FT max_radius);

    Point jitter_point(const Point& p, const FT max_radius) const;
    
    FT compute_alpha() const;
    
    FT compute_absolute_radius(Vertex_handle vertex) const;
    
    // ODT //
    
    bool optimize_positions_via_ODT(bool update);
    
    // CCDT //
    
    void optimize_positions_via_CCDT(unsigned max_iters);
    
    void run_ccdt_topological_phase();
    
    void run_ccdt_geometric_phase();
    
    Point compute_ccdt_position_per_vertex(Vertex_handle vi) const;
    
    // Penalty method //
    
    FT compute_penalty_threshold(FT epsilon, bool use_grad) const;
    
    unsigned optimize_positions_via_penalty_until_converge(FT stiffness, 
                                                           FT timestep,
                                                           FT epsilon, 
                                                           unsigned max_iters,
                                                           bool use_grad);
    
    FT optimize_positions_via_penalty(FT stiffness, FT timestep);
    
    FT compute_penalty_energy(FT stiffness) const;
    
    void compute_penalty_gradient(std::vector<Vector>& gradient, FT stiffness, 
                                  FT coef = 1.0) const;
    
    Vector compute_penalty_gradient_per_vertex(Vertex_handle vi, FT stiffness) const;
    
    void update_lbfgs(const std::vector<Point>& p0, const std::vector<Point>& p1,
                      const std::vector<Vector>& v0, const std::vector<Vector>& v1);
};

#endif
