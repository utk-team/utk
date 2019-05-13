#ifndef _LBFGS_H_
#define _LBFGS_H_ 1

#include <list>
#include <vector>

template <class Vector>
class CLBFGS
{
protected:
    unsigned m_limit;
    // FIFO: ( [k-limit-1], ..., [k-1] )
    std::list< std::vector<Vector> > m_s; // s[k] = x[k+1] - x[k]
    std::list< std::vector<Vector> > m_y; // y[k] = grad[k+1] - grad[k]
    std::list<double> m_rho;

    typedef typename std::list< std::vector<Vector> >::const_iterator ConstIterator;
    typedef typename std::list< std::vector<Vector> >::const_reverse_iterator RConstIterator;

public:
    CLBFGS(unsigned size = 0) 
    {
        m_limit = size;
    }

    void clear()
    {
        m_y.clear();
        m_s.clear();
        m_rho.clear();
    }
    
    void set_limit(unsigned nb) 
    { 
        clear();
        m_limit = nb; 
    }
    
    unsigned memory_limit() const { return m_limit; }
    
    unsigned memory_size() const { return m_y.size(); }
    
    void append(const std::vector<Vector>& s, const std::vector<Vector>& y)
    {
        if (memory_limit() == 0) return;
        
        double rho = inner_product(s, y);
        if (rho != 0.0) rho = 1.0/rho;
        
        m_y.push_back(y);
        m_s.push_back(s);
        m_rho.push_back(rho);
        
        if (memory_size() > memory_limit())
        {
            m_y.pop_front();
            m_s.pop_front();
            m_rho.pop_front();
        }
    }

    // for minimization problem
    void solve(std::vector<Vector> q, std::vector<Vector>& z)
    {
        RConstIterator s_rit = m_s.rbegin();
        RConstIterator y_rit = m_y.rbegin();
        std::list<double>::const_reverse_iterator rho_rit = m_rho.rbegin();
        
        unsigned nb = memory_size();
        std::vector<double> alpha(nb);
        for (unsigned i = 0; i < nb; ++i)
        {
            const std::vector<Vector>& s = *s_rit;
            const std::vector<Vector>& y = *y_rit;
            const double rho = *rho_rit;
            
            unsigned index = nb - 1 - i;
            alpha[index] = rho * inner_product(s, q);
            add_to_vector(q, -alpha[index], y);
            
            s_rit++;
            y_rit++;
            rho_rit++;
        }
        
        z = q;
        scale_vector_by_gamma(z);
        
        ConstIterator s_it = m_s.begin();
        ConstIterator y_it = m_y.begin();
        std::list<double>::const_iterator rho_it = m_rho.begin();
        
        for (unsigned i = 0; i < nb; ++i)
        {
            const std::vector<Vector>& s = *s_it;
            const std::vector<Vector>& y = *y_it;
            const double rho = *rho_it;
            
            unsigned index = i;
            double beta = rho * inner_product(y, z);
            add_to_vector(z, alpha[index] - beta, s);
            
            s_it++;
            y_it++;
            rho_it++;            
        }
        
        flip_vector(z);
    }
  
    void scale_vector_by_gamma(std::vector<Vector>& z)
    {
        if (memory_size() == 0) return;
        
        double D = inner_product(m_y.back(), m_y.back());
        if (D == 0.0) return;
        
        double N = inner_product(m_s.back(), m_y.back());        
        double gamma = N / D;
        
        for (unsigned i = 0; i < z.size(); ++i)
            z[i] = gamma*z[i];
    }
    
    double inner_product(const std::vector<Vector>& s, const std::vector<Vector>& y) const
    {
        double sum = 0.0;
        for (unsigned i = 0; i < s.size(); ++i)
            sum += s[i]*y[i];
        return sum;
    }
    
    void add_to_vector(std::vector<Vector>& y, double coef, const std::vector<Vector>& x) const
    {
        for (unsigned i = 0; i < y.size(); ++i)
            y[i] = y[i] + coef*x[i];
    }
    
    void flip_vector(std::vector<Vector>& x) const
    {
        for (unsigned i = 0; i < x.size(); ++i)
            x[i] = -x[i];
    }
};

#endif
