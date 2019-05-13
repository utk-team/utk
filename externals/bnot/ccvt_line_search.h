    #ifndef _CCVT_LINE_SEARCH_H_
#define _CCVT_LINE_SEARCH_H_

#include "line_search.h"

//------------//
// CWCWeights //
//------------//

template <class Scene, class T>
class CLSWeights : public CLineSearch<T, T>
{
protected:
    Scene* m_scene;
    unsigned m_nb;
    
public:
    CLSWeights(Scene* scene,
              const unsigned max_iters,
              const double max_alpha)
    : CLineSearch<T, T>(max_iters, max_alpha)
    {
        m_scene = scene;
        m_nb = m_scene->count_visible_sites();
    }
    
    double compute_function() const
    {
        return m_scene->compute_wcvt_energy();
    }
    
    void compute_gradient(std::vector<T>& V) const
    {
        m_scene->compute_weight_gradient(V);
    }
    
    bool update_scene(const std::vector<T>& X)
    {
        if (m_scene->connectivity_fixed())
        {        
            m_scene->update_weights(X, true, false);
            return true;
        }
        
        m_scene->update_weights(X, false, false);
        m_scene->update_triangulation();
        return has_same_vertices();
    }
    
    bool has_same_vertices() const
    {
        unsigned nb = m_scene->count_visible_sites();
        if (nb != m_nb) std::cout << "HiddenVertices: " << m_nb << " -> " << nb << std::endl;
        return (nb == m_nb);
    }
};

//-------------//
// CWCPosition //
//-------------//

template <class Scene, class Position, class Velocity>
class CLSPositions : public CLineSearch<Position, Velocity>
{
protected:
    Scene* m_scene;
    unsigned m_nb;

public:
    CLSPositions(Scene* scene,
                const unsigned max_iters,
                const double max_alpha)
    : CLineSearch<Position, Velocity>(max_iters, max_alpha)
    {
        m_scene = scene;
        m_nb = m_scene->count_visible_sites();
    }
    
    double compute_function() const
    {
        return ( - m_scene->compute_wcvt_energy() );
    }
    
    void compute_gradient(std::vector<Velocity>& V) const
    {
        m_scene->compute_position_gradient(V, -1.0);
    }
    
    bool update_scene(const std::vector<Position>& X)
    {
        if (m_scene->connectivity_fixed())
        {
            m_scene->update_positions(X, true, false);
            return true;
        }
         
        m_scene->update_positions(X, false, false);
        m_scene->update_triangulation();
        return has_same_vertices();
    }
    
    bool has_same_vertices() const
    {
        unsigned nb = m_scene->count_visible_sites();
        if (nb != m_nb) std::cout << "HiddenVertices: " << m_nb << " -> " << nb << std::endl;
        return (nb == m_nb);
    }
};

#endif
