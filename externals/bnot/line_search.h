#ifndef _LINE_SEARCH_H_
#define _LINE_SEARCH_H_

/*
 * Based on "Numerical Optimizaiton"
 * by J. Nocedal and S. Wright
 * pages 60-61
 */

#include <cmath>
#include <vector>

/*
 * USAGE:
 * Implement virtual methods:
 *   double compute_function() const;
 *   void compute_gradient(std::vector<Velocity>& V) const;
 *   void update_scene(const std::vector<Position>& X);
 */
 
template <class Position, class Velocity>
class CLineSearch
{
protected:
    /* parameters */
    unsigned m_max_iters;
    double m_max_alpha;
    double m_c1;
    double m_c2;
    double m_c3;
    
    /* initial configuration */
    std::vector<Position> m_x0;
    std::vector<Velocity> m_v0;
    double m_grad_phi0;
    double m_phi0;
    
public:
    CLineSearch(const unsigned max_iters,
                const double max_alpha,
                const double c1 = 1e-4,
                const double c2 = 0.9,
                const double c3 = 1e-6)
    {
        m_max_iters = max_iters;
        m_max_alpha = max_alpha;
        m_c1 = c1;
        m_c2 = c2;
        m_c3 = c3;
    }

    double run_bt(const std::vector<Position>& x0, 
                  const std::vector<Velocity>& v0)
    {
        m_x0 = x0;
        m_v0 = v0;
        m_phi0 = evaluate_function();
        m_grad_phi0 = evaluate_gradient();
        double step = back_tracking();
        reset_position();
        return step;
    }
    
    double run_wc(const std::vector<Position>& x0, 
                              const std::vector<Velocity>& v0)
    {
        m_x0 = x0;
        m_v0 = v0;
        m_phi0 = evaluate_function();
        m_grad_phi0 = evaluate_gradient();
        double step = line_search_with_strong_wolfe_conditions();
        reset_position();
        return step;
    }
    
protected:
    double back_tracking()
    {
        double lower_alpha = 0.0;
        double upper_alpha = m_max_alpha;        
        for (unsigned i = 0; i < m_max_iters; ++i) 
        {
            double alpha = pick_alpha(lower_alpha, upper_alpha);
            //std::cout << "alpha = " << alpha << std::endl; // debug
            bool ok = move_position(alpha);
            
            if (ok)
            {
                double phi = evaluate_function();
                if (phi <= (m_phi0 + m_c1 * alpha * m_grad_phi0))
                    return alpha;
            }
            
            //std::cout << "decrease alpha" << std::endl; // debug
            upper_alpha = alpha;
        }
        return lower_alpha;
    }
    
    double line_search_with_strong_wolfe_conditions()
    {
        double lower_alpha = 0.0;
        double upper_alpha = m_max_alpha;
        
        double last_phi = m_phi0;
        double last_alpha = lower_alpha;
        for (unsigned i = 0; i < m_max_iters; ++i)
        {
            double alpha = pick_alpha(last_alpha, upper_alpha);
            if (std::abs(alpha - last_alpha) < m_c3) break;
            bool ok = move_position(alpha);
                  
            if (!ok)
            {
                upper_alpha = alpha;
                continue;
            }
            
            double phi = evaluate_function();
            if (phi >= last_phi)
            {
                //std::cout << "function not decreasing: call zoom" << std::endl;
                return zoom(last_alpha, alpha, last_phi);
            }

            if (phi > (m_phi0 + m_c1 * alpha * m_grad_phi0))
            {
                //std::cout << "Armijo failed: call zoom" << std::endl;
                return zoom(last_alpha, alpha, last_phi);
            }
            
            double grad_phi = evaluate_gradient();
            if (std::abs(grad_phi) <= m_c2 * std::abs(m_grad_phi0))
            {
                //std::cout << "Done: " << alpha << std::endl;
                return alpha;                
            }
            
            if (grad_phi >= 0.0)
            {
                //std::cout << "alpha is too far: call zoom in reverse order" << std::endl;
                return zoom(alpha, last_alpha, phi);                
            }

            //std::cout << std::abs(grad_phi) << " > " << m_c2 * std::abs(m_grad_phi0) << std::endl;
            //std::cout << "increase alpha and try again" << std::endl;
            last_phi = phi;
            last_alpha = alpha;
        }
        
        //std::cout << "search failed" << std::endl;
        return lower_alpha;
    }
    
    double zoom(double lower_alpha, double upper_alpha, double lower_phi)
    {
        if (std::abs(upper_alpha - lower_alpha) < m_c3)
            return lower_alpha;
        
        double alpha = pick_alpha(lower_alpha, upper_alpha);
        bool ok = move_position(alpha);
        
        if (!ok) 
        {
            return zoom(lower_alpha, alpha, lower_phi);
        }
        
        double phi = evaluate_function();
        if (phi >= lower_phi)
        {
            //std::cout << "Zoom: function not decreasing: call zoom again" << std::endl;
            return zoom(lower_alpha, alpha, lower_phi);
        }
        
        if (phi > m_phi0 + m_c1 * alpha * m_grad_phi0)
        {   
            //std::cout << "Zoom: Armijo failed: call zoom again" << std::endl;
            return zoom(lower_alpha, alpha, lower_phi);
        }
        
        double grad_phi = evaluate_gradient();
        if (std::abs(grad_phi) <= m_c2 * std::abs(m_grad_phi0))
        {
            //std::cout << "Zoom: done" << std::endl;
            return alpha;            
        }
        
        if (grad_phi * (upper_alpha - lower_alpha) >= 0.0)
        {
            //std::cout << "Zoom: revert zoom" << std::endl;
            upper_alpha = lower_alpha;
        }
        
        //std::cout << "Zoom: increase lower" << std::endl;
        lower_phi = phi;
        lower_alpha = alpha;
        return zoom(lower_alpha, upper_alpha, lower_phi);
    }
    
    double pick_alpha(const double lower, const double upper) const
    {
        return 0.5*(lower + upper);
    }
    
    void reset_position()
    {
        move_position(0.0);
    }
    
    bool move_position(const double alpha)
    {
        std::vector<Position> X;
        for (unsigned i = 0; i < m_x0.size(); ++i)
        {
            Position pi = m_x0[i] + alpha*m_v0[i];
            X.push_back(pi);
        }
        return update_scene(X);
    }

    double evaluate_function() const
    {
        return compute_function();
    }

    double evaluate_gradient() const
    {
        std::vector<Velocity> V;
        compute_gradient(V);
        
        /* V * m_v0 */
        double sum = 0.0;
        for (unsigned i = 0; i < m_v0.size(); ++i)
            sum += V[i] * m_v0[i];
        return sum;
    }

    virtual double compute_function() const = 0;

    virtual void compute_gradient(std::vector<Velocity>& V) const = 0;

    virtual bool update_scene(const std::vector<Position>& X) = 0;
};

#endif
