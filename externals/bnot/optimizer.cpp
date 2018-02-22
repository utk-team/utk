#include <map>
#include "scene.h"
#include "timer.h"
#include "ccvt_line_search.h"
#include "matrix/sparse_array.h"
#include "matrix/suite_sparse_qr.h"

typedef CLSWeights<Scene, FT> LSWeights;
typedef CLSPositions<Scene, Point, Vector> LSPositions;

FT Scene::optimize_positions_via_lloyd(bool update)
{
    std::vector<Point> points;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        Point ci = m_rt.compute_centroid(vi);
        points.push_back(ci);
    }
    update_positions(points, !update);
    if (update) update_triangulation();
    
    std::vector<Vector> gradient;
    compute_position_gradient(gradient);
    return compute_norm(gradient);
}

FT Scene::optimize_positions_via_gradient_ascent(FT timestep, bool update)
{
    std::vector<Point> points;
    collect_visible_points(points);
    
    std::vector<Vector> gradient;
    compute_position_gradient(gradient);

    if (timestep <= 0.0)
    {
        double mean_capacity = compute_mean_capacity();
        double max_alpha = 1.0 / mean_capacity;        
        LSPositions line_search(this, 100, max_alpha);
        
        if (m_timer_on) { Timer::start_timer(m_timer, COLOR_RED, "XLineSearch"); std::cout << std::endl; }
        timestep = line_search.run_bt(points, gradient);
        if (m_timer_on) Timer::stop_timer(m_timer, COLOR_RED);
    }
    
    for (unsigned i = 0; i < points.size(); ++i)
    {
        Point  pi = points[i];
        Vector gi = gradient[i];
        points[i] = pi + timestep * gi;
    }

    update_positions(points, !update);
    if (update) update_triangulation();
    
    compute_position_gradient(gradient);
    return compute_norm(gradient);
}

FT Scene::optimize_weights_via_gradient_descent(FT timestep, bool update)
{
    std::vector<FT> gradient;
    compute_weight_gradient(gradient, -1.0);

    std::vector<FT> weights;
    collect_visible_weights(weights);

    if (timestep <= 0.0)
    {
        LSWeights line_search(this, 20, 2.0);
        timestep = line_search.run_bt(weights, gradient);
    }
    
    for (unsigned i = 0; i < weights.size(); ++i)
    {
        FT wi = weights[i];
        FT gi = gradient[i];
        weights[i] = wi + timestep * gi;
    }

    update_weights(weights, !update);
    if (update) update_triangulation();
    
    compute_weight_gradient(gradient, -1.0);
    return compute_norm(gradient);
}

FT Scene::optimize_weights_via_newton(FT timestep, bool update)
{
    std::vector<FT> gradient;
    compute_weight_gradient(gradient, -1.0);

    std::vector<FT> direction;
    bool ok = solve_newton_step(gradient, direction);
    if (!ok) return 0.0;

    std::vector<FT> weights;
    collect_visible_weights(weights);

    if (timestep <= 0.0)
    {
        LSWeights line_search(this, 10, 2.0);
        timestep = line_search.run_bt(weights, gradient);
    }
    if (m_timer_on) std::cout << "WTimestep: " << timestep << std::endl;
    
    for (unsigned i = 0; i < weights.size(); ++i)
    {
        FT wi = weights[i];
        FT gi = direction[i];
        weights[i] = wi + timestep * gi;
    }

    update_weights(weights, !update);
    if (update) update_triangulation();

    compute_weight_gradient(gradient);
    return compute_norm(gradient);
}

bool Scene::solve_newton_step(const std::vector<FT>& b,
                              std::vector<FT>& x)
{
    if (m_timer_on) Timer::start_timer(m_timer, COLOR_BLUE, "LinearSolver");

    unsigned nb = 0;
    std::map<unsigned, unsigned> indices;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        indices[vi->get_index()] = nb++;
    }

    SparseMatrix L(nb, nb);
    build_laplacian(0.5, indices, L);

    bool ok = solve_linear_system(L, x, b);
    if (!ok)
    {
        std::cout << red << "linear solver failed" << white << std::endl;
        return false;
    }

    if (m_timer_on) Timer::stop_timer(m_timer, COLOR_BLUE);
    return true;
}

void Scene::build_laplacian(const FT scale,
                            const std::map<unsigned, unsigned>& indices,
                            SparseMatrix& A) const
{
    unsigned nb = A.numRows();
    for (unsigned k = 0; k < m_vertices.size(); ++k)
    {
        Vertex_handle vi = m_vertices[k];
        if (vi->is_hidden()) continue;
        unsigned i = indices.find(vi->get_index())->second;

        double diagi = 0.0;
        SparseArray rowi(nb);
        Edge_circulator ecirc = m_rt.incident_edges(vi);
        Edge_circulator eend  = ecirc;
        CGAL_For_all(ecirc, eend)
        {
            Edge edge = *ecirc;

            Vertex_handle vj = m_rt.get_source(edge);
            if (vj == vi) vj = m_rt.get_target(edge);

            unsigned j = vj->get_index();
            if (vj->is_clone()) j = vj->get_clone();
            j = indices.find(j)->second;

            double coef = scale * m_rt.get_ratio(edge);
            rowi.setValue(j, -coef);
            diagi += coef;
        }

        rowi.setValue(i, diagi);
        A.setRow(i, rowi);
    }
}

bool Scene::solve_linear_system(const SparseMatrix& A,
                                std::vector<double>& x,
                                const std::vector<double>& b) const
{
    SuiteSparseQRFactorizer solver;
    bool ok = solver.factorize(A);
    if (!ok) return false;

    ok = solver.solve(b, x);
    return ok;
}


unsigned Scene::optimize_weights_via_gradient_descent_until_converge(FT timestep,
                                                                     FT threshold,
                                                                     unsigned update,
                                                                     unsigned max_iters)
{
    for (unsigned i = 0; i < max_iters; ++i)
    {
        bool flag = (update == 0 || (i+1) % update == 0);
        FT norm = optimize_weights_via_gradient_descent(timestep, flag);
        if (norm < threshold) return i;
    }
    return max_iters;
}

unsigned Scene::optimize_weights_via_newton_until_converge(FT timestep,
                                                           FT threshold,
                                                           unsigned update,
                                                           unsigned max_iters)
{
    for (unsigned i = 0; i < max_iters; ++i)
    {
        bool flag = (update == 0 || (i+1) % update == 0);
        FT norm = optimize_weights_via_newton(timestep, flag);
        if (norm < threshold) return i;
    }
    return max_iters;
}

unsigned Scene::optimize_all(FT wstep, FT xstep, 
                             unsigned max_newton_iters,
                             FT epsilon, 
                             unsigned max_iters,
                             std::ostream& out,
                             bool breakCon)
{        
    bool global_connectivity = m_fixed_connectivity;
    unsigned nb0 = count_visible_sites();
    
    FT xthreshold = compute_position_threshold(epsilon);
    FT wthreshold = compute_weight_threshold(epsilon);

    out << "NbSites = " << nb0 << std::endl;
    out << "Threshold: " << xthreshold << " ; " << wthreshold << std::endl;
    
    m_fixed_connectivity = true;
    FT coarse_xthreshold = 2.0*xthreshold;
    FT coarse_wthreshold = 2.0*wthreshold;
    
    int maxBreakCon = 20;
    
    do
    {
		unsigned iters = 0;
		while (iters < max_iters)
		{
			iters++;
			reset_weights();        
			optimize_weights_via_newton_until_converge(wstep, coarse_wthreshold, 0, max_newton_iters);        
			FT norm = optimize_positions_via_lloyd(true);
			out << "(Coarse) Norm: " << norm << std::endl;        
			if (norm <= coarse_xthreshold) break;
		}
		
		std::cout << "Partial: " << iters << " iters" << std::endl;
		m_fixed_connectivity = global_connectivity;
		if (iters == max_iters) return iters;

		m_fixed_connectivity = false;
		FT fine_xthreshold = xthreshold;
		FT fine_wthreshold = wthreshold;
		
		while (iters < max_iters)
		{
			iters++;
			unsigned nb1 = count_visible_sites();
			if (nb1 != nb0) reset_weights();
			optimize_weights_via_newton_until_converge(wstep, fine_wthreshold, 0, max_newton_iters);
			FT norm = optimize_positions_via_gradient_ascent(xstep, true);
			out << "(Fine) Norm: " << norm << std::endl;
			if (norm <= fine_xthreshold) break;
		}
		
		maxBreakCon--;
	}
	while(breakCon && detect_and_break_regularity(1.0, 100) && maxBreakCon > 0);

    m_fixed_connectivity = global_connectivity;
    return 0;
}
