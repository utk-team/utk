#pragma once

// CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Regular_triangulation_filtered_traits_2.h>
#include <CGAL/Regular_triangulation_2.h>
#include <vector>
#include <vector>
#include <string>
#include <CGAL/intersections.h>
#include <random>


// Suitesparse
#if defined(__APPLE__) || defined(__MACH__)
#include <SuiteSparseQR.hpp>
#include <cholmod.h>
#else
#include <suitesparse/SuiteSparseQR.hpp>
#include <suitesparse/cholmod.h>
#endif

#define UTILS
#ifdef  UTILS

#define MINN -1e100
#define MAXN  1e100
#define EPS   1e-15

#include <cmath>
#include <vector>



namespace BBNOT
{ 
    
template <class T>
T compute_mean(const std::vector<T>& data)
{
    T mean = 0.0;
    for (unsigned i = 0; i < data.size(); ++i)
        mean += data[i];
    return (mean / static_cast<T>(data.size()));
}

template <class T>
void remove_mean(std::vector<T>& data)
{
    T mean = compute_mean(data);
    for (unsigned i = 0; i < data.size(); ++i)
        data[i] -= mean;
}

template <class T>
double compute_norm(std::vector<T>& data)
{
    double norm2 = 0.0;
    for (unsigned i = 0; i < data.size(); ++i)
        norm2 += data[i]*data[i];
    return std::sqrt(norm2);
}

template <class Point, class OutputIterator>
void generate_regular_polygon(unsigned nb, 
                              const double radius,
                              const Point center,
                              OutputIterator out)
{
    if (nb < 3) nb = 3;
    double step = 2*M_PI / nb;
    double start = 0.25*M_PI;
    for (unsigned i = 0; i < nb; ++i)
    {
        double angle = start + i*step;
        Point q(radius*cos(angle) + center.x(), 
                radius*sin(angle) + center.y());
        *out++ = q;
    }
}

template <class T>
double compute_dot_prod(const std::vector<T>& data1, const std::vector<T>& data2)
{
    double dot_prod = 0.0;
    for (unsigned i = 0; i < data1.size(); ++i)
        dot_prod += data1[i]*data2[i];
    return dot_prod;
}

#endif

#define OPTIM
#ifdef  OPTIM
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
        // reset_position(); // FIXME: commented to speed up code
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
    
    virtual ~CLineSearch() {}
protected:
    double back_tracking()
    {
        double lower_alpha = 0.0;
        double upper_alpha = m_max_alpha;        
        for (unsigned i = 0; i < m_max_iters; ++i) 
        {
            double alpha = pick_alpha(lower_alpha, upper_alpha);
            bool ok = move_position(alpha);
            //std::cout << "alpha = " << alpha << std::endl;
            
            if (ok)
            {
                double phi = evaluate_function();
                if (phi <= (m_phi0 + m_c1 * alpha * m_grad_phi0))
                    return alpha;
            }
            
            //std::cout << "decrease alpha" << std::endl;
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

    virtual ~CLSWeights() {} 
    
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
        if (m_scene->connectivity_fixed()) return true;
        m_scene->update_weights(X, false);
        m_scene->update_triangulation();
        return has_same_vertices();
    }
    
    bool has_same_vertices() const
    {
        unsigned nb = m_scene->count_visible_sites();
        // if (nb != m_nb) std::cout << "HiddenVertices: " << m_nb << " -> " << nb << std::endl;
        return (nb == m_nb);
    }
};


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
        if (m_scene->connectivity_fixed()) return true;
        m_scene->update_positions(X, true, false);
        m_scene->update_triangulation();
        return has_same_vertices();
    }
    
    bool has_same_vertices() const
    {
        unsigned nb = m_scene->count_visible_sites();
        // if (nb != m_nb) std::cout << "HiddenVertices: " << m_nb << " -> " << nb << std::endl;
        return (nb == m_nb);
    }
};

#endif

#define SUITESPARSE
#ifdef  SUITESPARSE

#define SPARSE_MATRIX
#ifdef  SPARSE_MATRIX
class SparseArray {
private:
    unsigned _size;
    std::vector<long int> _index;
    std::vector<double>   _value;
    
public:
    // Ctor
    SparseArray(unsigned N = 1) : _size(N)
    { }
    
    SparseArray(const SparseArray& rhs) 
    : _size(rhs._size), _index(rhs._index), _value(rhs._value) 
    { }
    
    // Dtor
    virtual ~SparseArray() 
    { }
    
    // Copy
    unsigned size() const
    { return _size; }
    
    unsigned numNonZeros() const
    { return static_cast<unsigned>(_value.size()); }
    
    unsigned copy(long* index, double* value) const {
        memcpy(index, &(_index[0]), numNonZeros()*sizeof(long int));
        memcpy(value, &(_value[0]), numNonZeros()*sizeof(double));
        return numNonZeros();
    }
    
    // Access
    double getValue(unsigned i) const;
    
    void setValue(unsigned i, const double x);
    
    int findIndex(int index) const;
    
    int readIndex(unsigned i) const;

    double readValue(unsigned i) const;

    void insert(unsigned i, const double x);

    void replace(unsigned i, const double x);

    // Operators
    SparseArray& operator = (const SparseArray& rhs);
    
    SparseArray& operator *= (const double rhs);
    
    SparseArray& operator /= (const double rhs);
    
    SparseArray& operator += (const SparseArray& rhs);
    
    SparseArray& operator -= (const SparseArray& rhs);
    
    // Measures
    double normalize();
    
    double length2() const;
    
    double length() const;
    
    double getMax() const;
    
    double getMin() const;
    
    // Friend
    friend SparseArray operator - (const SparseArray& rhs);
    
    friend SparseArray operator + (const SparseArray& a, const SparseArray& b);
    
    friend SparseArray operator - (const SparseArray& a, const SparseArray& b);
    
    friend SparseArray operator * (const SparseArray& a, const double b);
    
    friend SparseArray operator * (const double a, const SparseArray& b);
    
    friend SparseArray operator / (const SparseArray& a, const double b);
    
    friend double dot(const SparseArray& a, const SparseArray& b);

    friend std::ostream& operator << (std::ostream& out, const SparseArray& a);
    
    friend std::istream& operator >> (std::istream& in, SparseArray& a);
    
    friend class SparseMatrix;
};
    
inline 
SparseArray operator - (const SparseArray& rhs) { 
    SparseArray neg(rhs);
    for (unsigned i=0; i<neg.numNonZeros(); ++i) 
        neg._value[i] = - neg._value[i];
    return neg;
}

inline 
SparseArray operator + (const SparseArray& a, const SparseArray& b) { 
    SparseArray rt(a); 
    rt += b; 
    return rt; 
}

inline 
SparseArray operator - (const SparseArray& a, const SparseArray& b) { 
    SparseArray rt(a); 
    rt -= b; 
    return rt;
}

inline 
SparseArray operator * (const SparseArray& a, const double b) { 
    SparseArray rt(a); 
    rt *= b; 
    return rt; 
}

inline 
SparseArray operator * (const double a, const SparseArray& b) { 
    SparseArray rt(b); 
    rt *= a; 
    return rt; 
}

inline 
SparseArray operator / (const SparseArray& a, double b) { 
    SparseArray rt(a); 
    rt /= b; 
    return rt; 
}

inline 
double dot(const SparseArray& a, const SparseArray& b) { 
    double sum = 0.; 
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        int pos = b.findIndex( a.readIndex(i) );
        if (pos != -1) sum += a.readValue(i) * b.readValue(pos);
    }
    return sum; 
}

inline 
std::ostream& operator << (std::ostream& out, const SparseArray& a) { 
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        out << a._index[i] << " "; 
        out << a._value[i] << " "; 
    }
    return out; 
}

inline 
std::istream& operator >> (std::istream& in, SparseArray& a) {
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        in >> a._index[i];
        in >> a._value[i];
    }
    return in;
}


int SparseArray::findIndex(int index) const {
    for (unsigned i=0; i<numNonZeros(); ++i)
        if (_index[i] == index) return i;
    return -1;
}

int SparseArray::readIndex(unsigned i) const {
    return static_cast<int>(_index[i]);
}

double SparseArray::readValue(unsigned i) const {
    return _value[i];
}

void SparseArray::insert(unsigned i, const double x) {
    _index.push_back(i);
    _value.push_back(x);
}

void SparseArray::replace(unsigned i, const double x) {
    _value[i] = x;
}

double SparseArray::getValue(unsigned i) const {
    int pos = findIndex(i);
    if (pos == -1) return 0.;
    return readValue(pos);
}

void SparseArray::setValue(unsigned i, const double x) {
    int pos = findIndex(i);
    if (pos == -1) insert(i, x);
    else _value[pos] = x;
}

SparseArray& SparseArray::operator = (const SparseArray& rhs) {
    _size  = rhs._size;
    _index = rhs._index;
    _value = rhs._value;
    return *this;
}

SparseArray& SparseArray::operator *= (const double rhs) {
    for (unsigned i=0; i<numNonZeros(); ++i)
        _value[i] *= rhs;
    return *this;
}

SparseArray& SparseArray::operator /= (const double rhs) {
    for (unsigned i=0; i<numNonZeros(); ++i)
        _value[i] /= rhs;
    return *this;
}

SparseArray& SparseArray::operator += (const SparseArray& rhs) {
    for (unsigned i=0; i<rhs.numNonZeros(); ++i) {
        int pos = findIndex( rhs.readIndex(i) );
        if (pos == -1) insert( rhs.readIndex(i), rhs.readValue(i) );
        else _value[pos] += rhs.readValue(i);
    }
    return *this;
}

SparseArray& SparseArray::operator -= (const SparseArray& rhs) {
    for (unsigned i=0; i<rhs.numNonZeros(); ++i) {
        int pos = findIndex( rhs.readIndex(i) );
        if (pos == -1) insert( rhs.readIndex(i), -rhs.readValue(i) );
        else _value[pos] -= rhs.readValue(i);
    }
    return *this;
}

double SparseArray::length2() const { 
    double sum = 0; 
    for (unsigned i=0; i<numNonZeros(); ++i) 
        sum += _value[i]*_value[i]; 
    return sum; 
}

double SparseArray::length() const  {
    return std::sqrt( length2() ); 
}

double SparseArray::normalize() { 
    double len = length(); 
    if ( len > 1e-10 )
        *this /= len;
    return len;
}

double SparseArray::getMax() const {
    if (numNonZeros() == 0) return 0.;
    double value = _value[0];
    for (unsigned i=1; i<numNonZeros(); ++i)
        value = std::max(value, _value[i]);
    return value;
}

double SparseArray::getMin() const {
    if (numNonZeros() == 0) return 0.;
    double value = _value[0];
    for (unsigned i=1; i<numNonZeros(); ++i)
        value = std::min(value, _value[i]);
    return value;
}

class SparseMatrix {
private:
    std::vector<SparseArray> _row;
    
public:
    // Ctor
    SparseMatrix(unsigned nrows, unsigned ncols);

    SparseMatrix(const SparseMatrix& rhs);
    
    // Dtor
    ~SparseMatrix();
    
    // Access
    
    unsigned numRows() const;

    unsigned numColumns() const;
    
    unsigned numNonZeros() const;
    
    unsigned copyCRS(long*& rowptr, long*& index, double*& value) const;
    
    unsigned copyCCS(long*& colptr, long*& index, double*& value) const;

    double getValue(unsigned i, unsigned j) const;
    
    void setValue(unsigned i, unsigned j, const double value); 
    
    SparseArray getRow(unsigned i) const;
    
    SparseArray getColumn(unsigned i) const;
    
    SparseMatrix getTranspose() const;

    void setRow(unsigned i, const SparseArray& row);

    void setColumn(unsigned i, const SparseArray& col);

    // Operators
    
    SparseMatrix& operator = (const SparseMatrix& rhs);

    SparseMatrix& operator += (const SparseMatrix& rhs);

    SparseMatrix& operator -= (const SparseMatrix& rhs);
    
    SparseMatrix& operator *= (const SparseMatrix& rhs);

    SparseMatrix& operator *= (const double rhs);

    SparseMatrix& operator /= (const double rhs);
    
    SparseMatrix multiply(const double b) const;
    
    SparseMatrix multiply(const SparseMatrix& b) const;
};

// Friend Operators

inline
SparseMatrix operator - (const SparseMatrix& rhs) {
    SparseMatrix A(rhs.numRows(), rhs.numColumns());
    for (unsigned i=0; i<rhs.numRows(); ++i) {
        A.setRow(i, -rhs.getRow(i));
    }
    return A;
}

inline
SparseMatrix operator + (const SparseMatrix& a, const SparseMatrix& b) {
    SparseMatrix c = a;
    c += b;
    return c;
}

inline
SparseMatrix operator - (const SparseMatrix& a, const SparseMatrix& b) {
    SparseMatrix c = a;
    c -= b;
    return c;        
}

inline
SparseMatrix operator * (const SparseMatrix& a, const SparseMatrix& b) {
    return a.multiply(b);
}

inline
SparseMatrix operator * (const SparseMatrix& a, const double b) {
    return a.multiply(b);
}

inline
SparseMatrix operator * (const double a, const SparseMatrix& b) {
    return b.multiply(a);
}

inline
SparseMatrix operator / (const SparseMatrix& a, const double b) {
    SparseMatrix c(a.numRows(), a.numColumns());
    for (unsigned i=0; i<a.numRows(); ++i)
        c.setRow(i, a.getRow(i) / b);
    return c;        
}

// Concatenation

inline
SparseMatrix Concatenate(const SparseMatrix& A, const SparseMatrix& B) {
    SparseMatrix C(A.numRows() + B.numRows(), A.numColumns());
    
    // copy A
    for (unsigned i=0; i<A.numRows(); ++i) {
        C.setRow(i, A.getRow(i));
    }
    
    // copy B
    for (unsigned i=0; i<B.numRows(); ++i) {
        C.setRow(A.numRows() + i, B.getRow(i));
    }
    
    return C;        
}

SparseMatrix::SparseMatrix(unsigned nrows, unsigned ncols) :
_row(nrows, SparseArray(ncols)) {
}

SparseMatrix::SparseMatrix(const SparseMatrix& rhs)
: _row(rhs._row) { 
}

// Dtor
SparseMatrix::~SparseMatrix() { 
}

// Access

unsigned SparseMatrix::numRows() const {
    return static_cast<unsigned>(_row.size());
}

unsigned SparseMatrix::numColumns() const {
    return _row[0].size();
}

unsigned SparseMatrix::numNonZeros() const {
    unsigned N = 0;
    for (unsigned i=0; i<numRows(); ++i) {
        N += _row[i].numNonZeros();
    }
    return N;
}

unsigned SparseMatrix::copyCRS(long*& rowptr, long*& index, double*& value) const {
    unsigned N = numNonZeros();
	rowptr = (long int*) malloc((numRows() + 1) * sizeof(long int));
	index  = (long int*) malloc(N * sizeof(long int));
	value  = (double*) malloc(N * sizeof(double));
	
	rowptr[0] = 0;
	for (unsigned i = 0; i < numRows(); ++i) {
		int offset = _row[i].numNonZeros();
        _row[i].copy( &(index[rowptr[i]]), &(value[rowptr[i]]) );
		rowptr[i+1] = rowptr[i] + offset;
	}
	return N;
}    

unsigned SparseMatrix::copyCCS(long*& colptr, long*& index, double*& value) const {
	unsigned N = numNonZeros();
	colptr = (long int*) malloc((numColumns() + 1) * sizeof(long int));
	index  = (long int*) malloc(N * sizeof(long int));
	value  = (double*) malloc(N * sizeof(double));
	
	colptr[0] = 0;
	for (unsigned i = 0; i < numColumns(); ++i) {
        SparseArray col = getColumn(i);
		int offset = col.numNonZeros();
        col.copy( &(index[colptr[i]]), &(value[colptr[i]]) );
		colptr[i+1] = colptr[i] + offset;
	}
	return N;
}

double SparseMatrix::getValue(unsigned i, unsigned j) const { 
    return _row[i].getValue(j);
}

void SparseMatrix::setValue(unsigned i, unsigned j, const double value) { 
    _row[i].setValue(j, value);
}

SparseArray SparseMatrix::getRow(unsigned i) const {
    return _row[i];
}

SparseArray SparseMatrix::getColumn(unsigned i) const { 
    SparseArray column;
    for (unsigned j=0; j<numRows(); ++j) {
        int pos = _row[j].findIndex(i);
        if (pos != -1) column.insert(j, _row[j].readValue(pos));
    }
    return column;
}

SparseMatrix SparseMatrix::getTranspose() const {
    SparseMatrix transpose(numColumns(), numRows());
    for (unsigned i=0; i<numRows(); ++i) {
        unsigned nnz = _row[i].numNonZeros();
        for (unsigned j=0; j<nnz; ++j) {
            int  index = _row[i].readIndex(j);
            double value = _row[i].readValue(j);
            transpose._row[index].insert(i, value);
        }
    }
    return transpose;
}

void SparseMatrix::setRow(unsigned i, const SparseArray& row) {
    _row[i] = row;
}

void SparseMatrix::setColumn(unsigned i, const SparseArray& col) {
    unsigned nnz = col.numNonZeros();
    for (unsigned j=0; j<nnz; ++j) {
        int  index = col.readIndex(j);
        double value = col.readValue(j);
        _row[index].setValue(i, value);
    }
}

// Operators

SparseMatrix& SparseMatrix::operator = (const SparseMatrix& rhs) {
    _row = rhs._row;
    return *this;
}

SparseMatrix& SparseMatrix::operator += (const SparseMatrix& rhs) {
    for (unsigned i=0; i<numRows(); ++i) _row[i] += rhs._row[i];
    return *this;
}

SparseMatrix& SparseMatrix::operator -= (const SparseMatrix& rhs) {
    for (unsigned i=0; i<numRows(); ++i) _row[i] -= rhs._row[i];
    return *this;
}

SparseMatrix& SparseMatrix::operator *= (const SparseMatrix& rhs) {
    *this = multiply(rhs);
    return *this;
}

SparseMatrix& SparseMatrix::operator *= (const double rhs) {
    *this = multiply(rhs);
    return *this;
}    

SparseMatrix& SparseMatrix::operator /= (const double rhs) {
    for (unsigned i=0; i<numRows(); ++i) {
        _row[i] /= rhs;
    }
    return *this;
}    

// return (this * b)
SparseMatrix SparseMatrix::multiply(const SparseMatrix& b) const 
{
    SparseMatrix c(numRows(), b.numColumns());
    SparseMatrix bt = b.getTranspose();
    for (unsigned i = 0; i < c.numRows(); ++i) 
    {
        SparseArray c_i(c.numColumns());        
        SparseArray a_i = getRow(i);
        for (unsigned j = 0; j < c.numColumns(); ++j) 
        {
            double value = dot(a_i, bt.getRow(j));
            if (value > 1e-10) c_i.insert(j, value);
        }
        c.setRow(i, c_i);
    }
    return c;
}

SparseMatrix SparseMatrix::multiply(const double b) const 
{
    SparseMatrix c(numRows(), numColumns());
    for (unsigned i=0; i<numRows(); ++i)
        c.setRow(i, getRow(i) * b);
    return c;    
}

class SuiteSparseQRFactorizer {
private:
    cholmod_common _common;
    SuiteSparseQR_factorization<double>* _QR;
    
public:
    SuiteSparseQRFactorizer()  {
        cholmod_l_start(&_common);
        _QR = NULL;
    }
    
    ~SuiteSparseQRFactorizer() {
        if (_QR) SuiteSparseQR_free<double>(&_QR, &_common);
        cholmod_l_finish(&_common);
    }
    
    bool factorize(const SparseMatrix& A) {
        if (_QR) return false;
        cholmod_sparse* At = convert_transpose_to_cholmod_sparse(A);
        _QR = SuiteSparseQR_factorize<double>(SPQR_ORDERING_DEFAULT, SPQR_DEFAULT_TOL, At, &_common);
        cholmod_l_free_sparse(&At, &_common);
        return true;
    }
    
    bool solve(const std::vector<double>& rhs, std::vector<double>& x) {
        if (!_QR) return false;
        
        cholmod_dense* b = convert_to_cholmod_dense(rhs);
        cholmod_dense* y = SuiteSparseQR_solve(SPQR_RTX_EQUALS_ETB, _QR, b, &_common);
        cholmod_l_free_dense(&b, &_common);
        
        cholmod_dense* z = SuiteSparseQR_qmult(SPQR_QX, _QR, y, &_common);
        cholmod_l_free_dense(&y, &_common);
        
        convert_to_array(z, x);
        cholmod_l_free_dense(&z, &_common);
        return true;
    }
    
private:
    cholmod_sparse* convert_transpose_to_cholmod_sparse(const SparseMatrix& A) {
        // B = A'
        long* colptr;
        long* index;
        double* value;
        int nnz = (int) A.copyCRS(colptr, index, value);
        cholmod_sparse* B = cholmod_l_allocate_sparse(A.numColumns(), A.numRows(), nnz, 
                                                      1, 1, 0, CHOLMOD_REAL, &_common);
        B->p = (void*) colptr;
        B->i = (void*) index;
        B->x = (void*) value;
        return B;
    }
    
    cholmod_dense* convert_to_cholmod_dense(const std::vector<double>& a) {
        cholmod_dense* b = cholmod_l_allocate_dense(a.size(), 1, a.size(), CHOLMOD_REAL, &_common);
        double* tmp = (double*) b->x;
        for (unsigned i=0; i<a.size(); ++i) tmp[i] = a[i];
        return b;
    }

    void convert_to_array(cholmod_dense* a, std::vector<double>& b) {
        b.resize(a->nrow);
        double* tmp = (double*) a->x;
        for (unsigned i=0; i<a->nrow; ++i) b[i] = tmp[i];
    }
};


#endif


#endif


#define CONVEX_POLYGONS
#ifdef  CONVEX_POLYGONS

template <class Kernel>
class CConvexPolygon
{
public:
    typedef typename Kernel::FT         FT;
    typedef typename Kernel::Point_2    Point;
    typedef typename Kernel::Vector_2   Vector;
    typedef typename Kernel::Ray_2      Ray;
    typedef typename Kernel::Line_2     Line;
    typedef typename Kernel::Segment_2  Segment;
    typedef typename Kernel::Triangle_2 Triangle;
    
private:
    std::vector<Point> m_points;
    
public:
    CConvexPolygon() { }

    CConvexPolygon(const CConvexPolygon& rhs)
    {
        m_points = rhs.m_points;
    }

    CConvexPolygon& operator = (const CConvexPolygon& rhs)
    {
        m_points = rhs.m_points;
        return *this;
    }
    
    void clear()
    {
        m_points.clear();
    }
    
    // GET //

    std::vector<Point>& points() { return m_points; }

    unsigned nb_points() const { return m_points.size();  }
    
    const Point& get_point(unsigned i) const { return m_points[i]; }
    
    const std::vector<Point>& get_points() const { return m_points; }
    
    Segment get_segment(unsigned i) const
    {
        unsigned j = static_cast<unsigned>((i + 1) % m_points.size());
        const Point& a = m_points[i];
        const Point& b = m_points[j];
        return Segment(a, b);
    }    
    
    template <class OutputIterator>
    void collect_segments(OutputIterator out) const
    {
        for (unsigned i = 0; i < m_points.size(); ++i)
        {        
            Segment segment = get_segment(i);
            *out++ = segment;
        }
    }
    
    // APPEND //
    
    void append_point(const Point& p)
    {
        m_points.push_back(p);
    }
    
    template <class InputIterator>
    void append(InputIterator first, InputIterator last)
    {
        m_points.insert(m_points.end(), first, last);
    }
    
    // INIT //
    
    void init_regular_polygon(unsigned nb,
                              FT radius, 
                              Point center = Point(0.0, 0.0))
    {
        clear();
        generate_regular_polygon(nb, radius, center, 
                                 std::back_inserter(m_points));
    }

    void init_rectangle(FT width, 
                        FT height,
                        Point center = Point(0.0, 0.0))
    {
        clear();
        m_points.push_back(Point(center.x() - width, center.y() - height));
        m_points.push_back(Point(center.x() + width, center.y() - height));
        m_points.push_back(Point(center.x() + width, center.y() + height));
        m_points.push_back(Point(center.x() - width, center.y() + height));
    }

    // AREA //

    FT compute_area() const
    {
        if (m_points.size() < 3) return 0.0;
        
        FT area = 0.0;
        const Point& p0 = get_point(0);
        for (unsigned i = 2; i < m_points.size(); ++i)
        {
            Segment segment = get_segment(i-1);
            Triangle triangle(p0, segment.source(), segment.target());
            area += triangle.area();
        }
        return area;
    }

    // CENTROID //

    Point compute_centroid() const
    {
        if (m_points.empty()) return Point();
        if (m_points.size() == 1) return get_point(0);
        if (m_points.size() == 2) return CGAL::midpoint(get_point(0), 
                                                        get_point(1));
        
        FT sum_area = 0.0;
        Vector sum_vector = CGAL::NULL_VECTOR;
        const Point& p0 = get_point(0);
        for (unsigned i = 2; i < m_points.size(); ++i)
        {
            Segment segment = get_segment(i-1);
            Triangle triangle(p0, segment.source(), segment.target());
            
            FT area = triangle.area();
            Point barycenter = CGAL::centroid(triangle);
            
            sum_area += area;
            sum_vector = sum_vector + area*(barycenter - CGAL::ORIGIN);
        }
        if (sum_area == 0.0) return get_point(0);
        return CGAL::ORIGIN + (sum_vector / sum_area);
    }

    // VARIANCE //
    
    FT compute_variance(const Point& q) const
    {
        FT variance = 0.0;
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            Segment segment = get_segment(i);
            variance += compute_variance_per_segment(q, segment);
        }
        return variance;
    }
    
    FT compute_variance_per_segment(const Point& x, const Segment& ab) const
    {
        // \int_(x, a, b) |y - x|^2 dy
        // (x, a, b) can be either CCW or CW
        // return signed variance
        
        Point a = ab.source();
        Point b = ab.target();
        Point q = compute_orthogonal_projection(x, ab);
                
        Triangle xqa(x, q, a);
        FT E0 = compute_variance_per_right_triangle(x, q, a); // >=0
        //if (xqa.orientation() == CGAL::COUNTERCLOCKWISE)
        if (xqa.area() >= 0.0)
            E0 = - E0;
        
        Triangle xqb(x, q, b);
        FT E1 = compute_variance_per_right_triangle(x, q, b); // >=0
        //if (xqb.orientation() == CGAL::CLOCKWISE)
        if (xqb.area() < 0.0)
            E1 = - E1;
        
        return (E0 + E1);
    }
    
    FT compute_variance_per_right_triangle(const Point& x, const Point& a, const Point& b) const
    {
        // E = \int_(x, a, b) |y - x|^2 dy
        // where (x, a, b) is 90o at a
        // E >= 0
        FT base   = std::sqrt(CGAL::squared_distance(a, b));
        FT height = std::sqrt(CGAL::squared_distance(a, x));
        return (height*height*height*base)/4 + (height*base*base*base)/12;
    }

    // INSIDE //
    
    bool is_outside(const Point& p) const
    {
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            Line line = get_segment(i).supporting_line();
            if (line.has_on_negative_side(p))
                return true;
        }
        return false;
    }
    
    // CLAMP //
    
    Point clamp(const Point& p) const
    {
        FT min_dist2 = 1e100;
        Point clamped_point = p;
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            Segment segment = get_segment(i);
            Line line = segment.supporting_line();
            if (!line.has_on_negative_side(p)) continue;
                
            Point q = project_to(p, segment);
            FT dist2 = CGAL::squared_distance(p, q);
            if (dist2 < min_dist2)
            {
                clamped_point = q;
                min_dist2 = dist2;
            }
        }
        return clamped_point;
    }
    
    Segment clamp(const Segment& segment) const
    {
        Point ps = segment.source();
        Point pt = segment.target();
        
        bool source_inside = !is_outside(ps);
        bool target_inside = !is_outside(pt);

        if (source_inside && target_inside)
            return segment;

        if (!source_inside && !target_inside)
            return clamp_if_possible(segment);
        
        Point q = intersect(segment);
        if (source_inside) pt = q;
        if (target_inside) ps = q;
        return Segment(ps, pt);
    }
    
    Segment clamp_if_possible(const Segment& segment) const
    {
        // both ps and pt are outside
        Point ps = segment.source();
        Point pt = segment.target();
        
        unsigned ns = find_nearest_line(ps);
        CGAL::Object rs  = CGAL::intersection(segment, get_segment(ns));
        const Point* iqs = CGAL::object_cast<Point>(&rs);
        if (!iqs) return Segment(ps, ps);

        unsigned nt = find_nearest_line(pt);
        CGAL::Object rt  = CGAL::intersection(segment, get_segment(nt));
        const Point* iqt = CGAL::object_cast<Point>(&rt);
        if (!iqt) return Segment(pt, pt);
        
        return Segment(*iqs, *iqt);
    }
    
    // PROJECT //
    
    Point project(const Point& p) const
    {
        FT min_dist2 = 1e100;
        Point projected_point = p;
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            Segment segment = get_segment(i);
            Point q = project_to(p, segment);
            FT dist2 = CGAL::squared_distance(p, q);
            if (dist2 < min_dist2)
            {
                projected_point = q;
                min_dist2 = dist2;
            }
        }
        return projected_point;
    }
    
    Point project_to(const Point& query, const Segment& segment) const
    {
        Point proj = compute_orthogonal_projection(query, segment);
        if (segment.has_on(proj)) return proj;
        
        const Point& ps = segment.source();
        const Point& pt = segment.target();
        FT ds2 = CGAL::squared_distance(proj, ps);
        FT dt2 = CGAL::squared_distance(proj, pt);
        if (ds2 < dt2) return ps;
        return pt;
    }
    
    Point compute_orthogonal_projection(const Point& query, const Segment& segment) const
    {
        const Point& a = segment.source();
        const Point& b = segment.target();
        if (a == b) return a;
        
        FT dab = std::sqrt(CGAL::squared_distance(a, b));
        Vector n = (b - a) / dab;
        FT daq = n * (query - a);
        Point q = a + daq*n;
        
        // debug
        /*
        Line line = segment.supporting_line();
        Point proj = line.projection(query);
        FT diff = std::sqrt(CGAL::squared_distance(proj, q));
        if (diff > 1e-10) 
            std::cout << "Projection: mine " << q << " ; CGAL " << proj << std::endl;
        */
        //
        
        return q;
    }
    
    unsigned find_nearest_line(const Point& p) const
    {
        unsigned nearest = 0;
        FT min_dist2 = 1e100;
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            Segment segment = get_segment(i);
            Point q = compute_orthogonal_projection(p, segment);
            FT dist2 = CGAL::squared_distance(p, q);
            if (dist2 < min_dist2)
            {
                nearest = i;
                min_dist2 = dist2;
            }
        }
        return nearest;
    }
    
    // INTERSECT //
    
    Point intersect(const Segment& segment) const
    {
        // assume that segment has one endpoint inside and the other outside
        // since the polygon is convex, the intersection is unique
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            Segment si = get_segment(i);
            CGAL::Object result = CGAL::intersection(segment, si);
            const Point* iq = CGAL::object_cast<Point>(&result);
            if (iq) return *iq;
        }
        // never come here
        return Point(0.0, 0.0);
    }
};

#endif

#define DOMAIN
#ifdef  DOMAIN

template <class Kernel>
class CDomain
{
public:
    typedef typename Kernel::FT         FT;
    typedef typename Kernel::Point_2    Point;
    typedef typename Kernel::Vector_2   Vector;
    typedef typename Kernel::Ray_2      Ray;
    typedef typename Kernel::Line_2     Line;
    typedef typename Kernel::Segment_2  Segment;
    typedef typename Kernel::Triangle_2 Triangle;

private:
    std::vector<Point>  m_points; // boundary
    FT m_width, m_height; // bbox
    FT m_area;
    
public:
    CDomain() 
    { 
        clear();
    }
    
    void clear()
    {
        m_points.clear();
        m_height = 0.0;
        m_width = 0.0;
        m_area = 0.0;
    }
    
    // GET / SET //

    Point origin() const { return Point(0.0, 0.0); }
    
    const Point& get_point(unsigned i) const { return m_points[i]; }    
        
    const FT width() const { return m_width; }
    
    const FT height() const { return m_height; }
    
    // BOUNDARY //
    
    Segment get_segment(unsigned i) const
    {
        unsigned j = static_cast<unsigned>((i + 1) % m_points.size());
        const Point& a = m_points[i];
        const Point& b = m_points[j];
        return Segment(a, b);
    }    
    
    template <class OutputIterator>
    void add_segments(OutputIterator out)
    {
        for (unsigned i = 0; i < m_points.size(); ++i)
        {        
            Segment segment = get_segment(i);
            *out++ = segment;
        }
    }

    // INIT //
    
    void init_polygon(const FT radius, 
                      const unsigned nb)
    {
        clear();
        m_width  = radius;
        m_height = radius;
        generate_regular_polygon(nb, radius, Point(0.0, 0.0), 
                                 std::back_inserter(m_points));
    }

    void init_rectangle(const FT width, 
                        const FT height)
    {
        clear();
        m_width  = width;
        m_height = height;
        m_points.push_back(Point(-width, -height));
        m_points.push_back(Point( width, -height));
        m_points.push_back(Point( width,  height));
        m_points.push_back(Point(-width,  height));
    }

    void init_area()
    {
        m_area = compute_polygon_area();
    }
    
    // AREA //

    FT get_area() const { return m_area; }

    FT compute_polygon_area() const
    {
        // rect
        return 4.0*width()*height();
    }
    
    // INSIDE / OUTSIDE //
    
    bool is_inside(const Point& p) const
    {
        if (std::abs(p.x()) > width ()) return false;
        if (std::abs(p.y()) > height()) return false;
        return true;
    }
    
    bool is_outside(const Point& p) const
    {
        return !is_inside(p);
    }
    
    bool is_corner(const Point& p) const
    {
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            const Point& pi = get_point(i);
            if (p == pi) return true;
        }
        return false;
    }
    
    // CLAMP //
    
    Point clamp(const Point& p) const
    {
        // rect
        FT x = p.x();
        if (x >  width()) x =  width();
        if (x < -width()) x = -width();
        
        FT y = p.y();
        if (y >  height()) y =  height();
        if (y < -height()) y = -height();

        return Point(x, y);
    }
    
    Segment clamp(const Segment& segment) const
    {
        Point ps = segment.source();
        Point pt = segment.target();
        
        bool source_inside = is_inside(ps);
        bool target_inside = is_inside(pt);

        if (source_inside && target_inside)
            return segment;

        if (!source_inside && !target_inside)
            return clamp_if_possible(segment);
        
        Point q = intersect(segment);
        if (source_inside) pt = q;
        if (target_inside) ps = q;
        return Segment(ps, pt);
    }
    
    Segment clamp_if_possible(const Segment& segment) const
    {
        // both ps and pt are outside
        Point ps = segment.source();
        Point pt = segment.target();
        
        unsigned ns = find_nearest_line(ps);
        CGAL::Object rs  = CGAL::intersection(segment, get_segment(ns));
        const Point* iqs = CGAL::object_cast<Point>(&rs);
        if (!iqs) return Segment(ps, ps);
        
        unsigned nt = find_nearest_line(pt);
        CGAL::Object rt  = CGAL::intersection(segment, get_segment(nt));
        const Point* iqt = CGAL::object_cast<Point>(&rt);
        if (!iqt) return Segment(pt, pt);
        
        return Segment(*iqs, *iqt);
    }    
    
    // PROJECT //
    
    Point project(const Point& p) const
    {
        FT min_dist2 = 1e100;
        Point projected_point = p;
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            Segment segment = get_segment(i);
            Point q = project_to(p, segment);
            FT dist2 = CGAL::squared_distance(p, q);
            if (dist2 < min_dist2)
            {
                projected_point = q;
                min_dist2 = dist2;
            }
        }
        return projected_point;
    }
    
    Point project_to(const Point& query, const Segment& segment) const
    {
        Point proj = compute_orthogonal_projection(query, segment);
        if (segment.has_on(proj)) return proj;
        
        const Point& ps = segment.source();
        const Point& pt = segment.target();
        FT ds2 = CGAL::squared_distance(proj, ps);
        FT dt2 = CGAL::squared_distance(proj, pt);
        if (ds2 < dt2) return ps;
        return pt;
    }
    
    Point compute_orthogonal_projection(const Point& query, const Segment& segment) const
    {
        const Point& a = segment.source();
        const Point& b = segment.target();
        FT dab = std::sqrt(CGAL::squared_distance(a, b));
        Vector n = (b - a) / dab;
        FT daq = n * (query - a);
        Point q = a + daq*n;
        return q;
    }
    
    unsigned find_nearest_line(const Point& p) const
    {
        // rect
        FT x = p.x();
        FT xpos = std::abs(x - width());
        FT xneg = std::abs(x + width());
        FT xbest = std::min(xpos, xneg);
        
        FT y = p.y();
        FT ypos = std::abs(y - height());
        FT yneg = std::abs(y + height());
        FT ybest = std::min(ypos, yneg);
        
        if (xbest < ybest)
        {
            if (xpos < xneg) return 1;
            return 3;
        }
        
        if (ypos < yneg) return 2;
        return 0;
    }
    
    int find_side(const Point& a) const
    {
        int side = -1;
        if (std::abs(a.x() - width ()) < EPS) side = 1;
        if (std::abs(a.x() + width ()) < EPS) side = 3;
        if (std::abs(a.y() - height()) < EPS) side = 2;
        if (std::abs(a.y() + height()) < EPS) side = 0;
        return side;
    }
    
    // assume that an intersection exists
    // since the polygon is convex, the intersection is unique
    Point intersect(const Segment& segment) const
    {
        for (unsigned i = 0; i < m_points.size(); ++i)
        {
            Segment si = get_segment(i);
            CGAL::Object result = CGAL::intersection(segment, si);
            const Point* iq = CGAL::object_cast<Point>(&result);
            if (iq) return *iq;
        }
        // never come here
        return Point(0.0, 0.0);
    }
};

#endif


#define CGAL_CUSTOM_STRUCTURES
#ifdef  CGAL_CUSTOM_STRUCTURES

#undef min
#undef max


template <class Kernel, class Vbb>
class My_vertex_base : public Vbb
{
public:
    typedef CConvexPolygon<Kernel> ConvexPolygon;
    
    typedef typename Kernel::FT FT;
    typedef typename Kernel::FT   Weight;
    typedef typename Kernel::Point_2  KPoint;
    typedef typename Kernel::Vector_2 Vector;
    
    typedef typename Kernel::Weighted_point_2 Point;
    typedef typename Kernel::Weighted_point_2 Weighted_point;
    
    typedef typename Vbb::Triangulation_data_structure TDS;
    typedef typename TDS::Face_handle   Face_handle;
    typedef typename TDS::Vertex_handle Vertex_handle;

    template < typename TDS2 >
    struct Rebind_TDS {
        typedef typename Vbb::template Rebind_TDS<TDS2>::Other Vb2;
        typedef My_vertex_base<Kernel, Vb2> Other;
    };
        
private:
    int m_index;
    ConvexPolygon m_cell;
    
public:
    My_vertex_base() : Vbb() 
    { 
        m_index = -1;
    }
   
    My_vertex_base(const Weighted_point& p) : Vbb(p)
    {
        m_index = -1;
    }

    My_vertex_base(const Weighted_point& p, Face_handle f) : Vbb(p, f)
    {
        m_index = -1;
    }

    ~My_vertex_base()
    {
    }
    
    void set_index(const int x) { m_index = x; }
    int get_index() const { return m_index; }

    // POSITION / WEIGHT //
    
    const KPoint& get_position() const 
    { 
        return this->point().point(); 
    }

    void set_position(const KPoint& p)
    {
        FT w = get_weight();
        Weighted_point wp(p, w);
        this->set_point(wp);
    }
    
    const FT get_weight() const 
    { 
        return this->point().weight(); 
    }
    
    void set_weight(const FT w)
    {
        const KPoint& p = get_position();
        Weighted_point wp(p, w);
        this->set_point(wp);
    }
    
    // CELL //

    void reset_cell()
    {
        m_cell.clear();
    }
    
    void set_cell(const ConvexPolygon& shape)
    {
        m_cell = shape;
    }
        
    // AREA ; VARIANCE ; CENTROID //
    
    FT compute_area() const
    {
        return m_cell.compute_area();
    }
    
    KPoint compute_centroid() const
    {
        return m_cell.compute_centroid();
    }
    
    FT compute_variance() const
    {
        return m_cell.compute_variance(get_position());
    }
};

template <class Kernel, class Fbb>
class My_face_base : public Fbb
{
public:
    typedef typename Fbb::Triangulation_data_structure  TDS;
    typedef typename TDS::Vertex_handle Vertex_handle;
    typedef typename TDS::Face_handle   Face_handle;
    
    template < typename TDS2 >
    struct Rebind_TDS {
        typedef typename Fbb::template Rebind_TDS<TDS2>::Other Fb2;
        typedef My_face_base<Kernel, Fb2> Other;
    };
    
public:
    My_face_base() 
    : Fbb() 
    { 
    }
    
    My_face_base(Vertex_handle v1,
                 Vertex_handle v2,
                 Vertex_handle v3)
    : Fbb(v1, v2, v3)
    {
    }
    
    My_face_base(Vertex_handle v1,
                 Vertex_handle v2,
                 Vertex_handle v3,
                 Face_handle f1,
                 Face_handle f2,
                 Face_handle f3)
    : Fbb(v1,v2,v3,f1,f2,f3)
    {
    }
};

template <class RT>
class CTriangulation : public RT 
{
public:
    typedef CTriangulation<RT> Rt;
    
    typedef typename Rt::Geom_traits    Kernel;
    typedef typename Kernel::FT         FT;
    typedef typename Kernel::FT     Weight;
    typedef typename Kernel::Point_2    Point;
    typedef typename Kernel::Vector_2   Vector;
    typedef typename Kernel::Ray_2      Ray;
    typedef typename Kernel::Line_2     Line;
    typedef typename Kernel::Segment_2  Segment;
    typedef typename Kernel::Triangle_2 Triangle;
    typedef typename Kernel::Weighted_point_2 Weighted_point;
    
    typedef typename Rt::Vertex                   Vertex;
    typedef typename Rt::Vertex_handle            Vertex_handle;
    typedef typename Rt::Vertex_iterator          Vertex_iterator;
    typedef typename Rt::Vertex_circulator        Vertex_circulator;
    typedef typename Rt::Finite_vertices_iterator Finite_vertices_iterator;
    
    typedef typename Rt::Edge                  Edge;
    typedef typename Rt::Edge_iterator         Edge_iterator;
    typedef typename Rt::Edge_circulator       Edge_circulator;
    typedef typename Rt::Finite_edges_iterator Finite_edges_iterator;
    
    typedef typename Rt::Face                  Face;
    typedef typename Rt::Face_handle           Face_handle;
    typedef typename Rt::Face_iterator         Face_iterator;
    typedef typename Rt::Face_circulator       Face_circulator;
    typedef typename Rt::Finite_faces_iterator Finite_faces_iterator;
    
    typedef CDomain<Kernel> Domain;
    typedef CConvexPolygon<Kernel> ConvexPolygon;
    
private:
    Domain* m_domain;
    
public:
    CTriangulation() 
    {
        m_domain = NULL;
    }
    
    ~CTriangulation()
    {
    }
    
    void set_domain(Domain* domain)
    {
        m_domain = domain;
    }
    
    ////////////
    // ACCESS //
    ////////////
    
    Vertex_handle get_source(const Edge& edge) const
    {
        return edge.first->vertex( Rt::ccw(edge.second) );
    }    
    
    Vertex_handle get_target(const Edge& edge) const
    {
        return edge.first->vertex( Rt::cw(edge.second) );        
    }
    
    Vertex_handle get_opposite(const Edge& edge) const
    {
        return edge.first->vertex( edge.second );
    }    
    
    Edge get_twin(const Edge& edge) const
    {
        Face_handle f = edge.first;
        Vertex_handle v = get_source(edge);
        Face_handle nf = f->neighbor(edge.second);
        return Edge(nf, Rt::ccw(nf->index(v)));
    }
    
    Edge get_next(const Edge& edge) const
    {
        Face_handle f = edge.first;
        int index = Rt::ccw(edge.second);
        return Edge(f, index);
    }
    
    Edge get_prev(const Edge& edge) const
    {
        Face_handle f = edge.first;
        int index = Rt::cw(edge.second);
        return Edge(f, index);
    }
    
    FT get_length(const Edge& edge) const
    {
        Segment segment = get_segment(edge);
        return std::sqrt(segment.squared_length());
    }
    
    Segment get_segment(const Edge& edge) const
    {
        const Point& ps = get_source(edge)->get_position();
        const Point& pt = get_target(edge)->get_position();
        return Segment(ps, pt);        
    }
    
    FT get_area(Face_handle face) const
    {
        Triangle triangle = get_triangle(face);
        return triangle.area();
    }
    
    Triangle get_triangle(Face_handle face) const
    {
        Vertex_handle v0 = face->vertex(0);
        Vertex_handle v1 = face->vertex(1);
        Vertex_handle v2 = face->vertex(2);
        return Triangle(v0->get_position(), v1->get_position(), v2->get_position());
    }
    
    Vector get_orthogonal_vector(const Edge& edge) const
    {
        const Point& ps = get_source(edge)->get_position();
        const Point& pt = get_target(edge)->get_position();
        Vector vst = pt - ps;
        return Vector(-vst.y(), vst.x());
    }
    
    FT get_average_length() const
    {
        unsigned nb = 0;
        FT avg_length = 0.0;
        for (Finite_edges_iterator
             eit  = RT::finite_edges_begin(); 
             eit != RT::finite_edges_end();
             ++eit)
        {
            Edge edge = *eit;
            FT length = get_length(edge);
            avg_length += length;
            nb++;
        }
        return (avg_length / nb);
    }
    
    ////////////
    // STATIC //
    ////////////
    
    static FT distance(const Point& a, const Point& b)
    {
        return std::sqrt(CGAL::squared_distance(a, b));
    }
    
    static Vector get_orthogonal(const Vector& vec) 
    {
        return Vector(-vec.y(), vec.x());        
    }

    static Point project(const Point& x, const Point& a, const Point& b)
    {
        FT dab = distance(a, b);
        Vector n = (b - a) / dab;
        FT dot = n * (x - a);
        return (a + dot*n);
    }
    
    static FT get_height(const Point& p, const Point& a, const Point& b)
    {
        FT len_ab = distance(a, b);
        if (len_ab < EPS) return distance(p, a);
        
        Vector ap = p - a;
        Vector ab = (b - a) / len_ab;
        Vector ab90 = get_orthogonal(ab);
        return (ap*ab90);
    }
    
    static FT cross_product(const Vector& a, const Vector& b) 
    {
        return a.x()*b.y() - a.y()*b.x();
    }
    
    ///////////////////////
    // INSIDE / BOUNDARY //
    ///////////////////////
    
    bool is_inside(Face_handle face) const
    {
        if (RT::is_infinite(face)) return false;
        return true;
    }
    
    bool is_inside(const Edge& edge) const
    {
        Edge twin = get_twin(edge);
        bool left = is_inside(edge.first);
        bool right = is_inside(twin.first);
        return (left || right);
    }
    
    bool is_boundary(const Edge& edge) const
    {
        Edge twin = get_twin(edge);
        bool left = is_inside(edge.first);
        bool right = is_inside(twin.first);
        return (left != right);
    }
    
    bool is_boundary(Vertex_handle vertex) const
    {
        if (vertex->is_hidden()) return false;
        Face_circulator fcirc = RT::incident_faces(vertex);
        Face_circulator fend  = fcirc;
        CGAL_For_all(fcirc, fend)
        {
            Face_handle face = fcirc;
            if (!is_inside(face)) 
                return true;
        }
        return false;
    }
    
    ////////////////
    // DUAL POINT //
    ////////////////
    
    Point get_dual(Face_handle face) const
    {
        return RT::dual(face);
    }

    Point get_edge_cw(const Edge& edge) const
    {
        Vertex_handle vi = get_source(edge);
        Vertex_handle vj = get_target(edge);
        const Point& pi = vi->get_position();
        const Point& pj = vj->get_position();
        const FT wi = vi->get_weight();
        const FT wj = vj->get_weight();
        
        const FT lij = get_length(edge);
        const FT dij = 0.5*(lij + (wi - wj)/lij);
        Vector vecij = (pj - pi) / lij;
        return pi + dij*vecij;
    }

    ///////////////
    // DUAL EDGE //
    ///////////////

    Segment get_dual_segment(const Edge& edge) const
    {
        Segment segment = get_dual(edge);
        return m_domain->clamp(segment);
    }
    
    Segment get_dual(const Edge& edge) const
    {
        /*
         * primal edge = (s,t)
         * dual edge = (right, left)
         * primal x dual > 0
         */
        Edge twin = get_twin(edge);
        Face_handle left_face  = edge.first;
        Face_handle right_face = twin.first;
        bool left_inside  = is_inside( left_face);
        bool right_inside = is_inside(right_face);

        if (left_inside && right_inside)
        {
            Point left_cw  = get_dual( left_face);
            Point right_cw = get_dual(right_face);
            return Segment(right_cw, left_cw);
        }
        
        Vector vec90 = get_orthogonal_vector(edge);
        if (!left_inside && !right_inside)
        {
            Point cw = get_edge_cw(edge);
            Line line(cw, vec90);
            return Segment(line.point(-100), line.point(100));
        }
        
        if (left_inside)
        {
            Point cw = get_dual(left_face);
            Ray ray(cw, -vec90);
            return Segment(ray.point(100), cw);
        }
        
        Point cw = get_dual(right_face);
        Ray ray(cw, vec90);
        return Segment(cw, ray.point(100));
    }

    ////////////////
    // ATTRIBUTES //
    ////////////////
    
    FT compute_area() const
    {
        FT area = 0.0;
        for (Finite_vertices_iterator
             vit = RT::finite_vertices_begin();
             vit != RT::finite_vertices_end();
             vit++)
        {
            Vertex_handle vi = vit;
            area += compute_area(vi);
        }
        return area;
    }
    
    FT compute_area(Vertex_handle vertex) const
    {
        return vertex->compute_area();
    }

    Point compute_centroid(Vertex_handle vertex) const
    {
        return vertex->compute_centroid();
    }
    
    FT compute_variance(Vertex_handle vertex) const
    {    
        return vertex->compute_variance();
    }
    
    ///////////////////////////
    // PRE-COMPUTE DUAL CELL //
    ///////////////////////////
    
    void pre_compute_cells()
    {
        for (Finite_vertices_iterator
            vi = Rt::finite_vertices_begin();
            vi != Rt::finite_vertices_end();
            ++vi)
        {
            Vertex_handle vertex = vi;
            pre_compute_cell(vertex);
        }
    }
    
    void pre_compute_cell(Vertex_handle vertex)
    {
        ConvexPolygon shape;
        build_polygon(vertex, shape.points());
        vertex->set_cell(shape);
    }
    
    bool build_polygon(Vertex_handle vi, std::vector<Point>& points) const
    {
        std::vector<Segment> segments;
        Edge_circulator ecirc = RT::incident_edges(vi);
        Edge_circulator eend  = ecirc;
        CGAL_For_all(ecirc, eend)
        {
            Edge edge = *ecirc;
            if (!is_inside(edge)) continue;
            
            Edge twin = get_twin(edge);
            Segment segment = get_dual_segment(twin);
            if (segment.is_degenerate()) continue;
            segments.push_back(segment);
        }
        if (segments.empty()) return false;
        
        Point first_pt = segments.front().source();        
        Point last_pt = first_pt;
        for (unsigned i = 0; i < segments.size(); ++i)
        {
            Segment segment = segments[i];
            Point ps = segment.source();
            Point pt = segment.target();
            
            if (ps != last_pt)
            {
                fill_gap(last_pt, ps, points);
                points.push_back(ps);
            }
            
            points.push_back(pt);
            last_pt = pt;
        }
        
        if (first_pt != last_pt)
        {
            fill_gap(last_pt, first_pt, points);
            points.push_back(first_pt);
        }
        return true;
    }
    
    void fill_gap(const Point& a, const Point& b, std::vector<Point>& points) const
    {
        if (a == b) return;
        
        int aside = m_domain->find_side(a);
        if (aside == -1) return;

        int bside = m_domain->find_side(b);
        if (bside == -1) return;
        
        while (aside != bside)
        {
            aside = (aside + 1) % 4;
            Point q = m_domain->get_point(aside);
            points.push_back(q);
        }
    }
    
    /////////
    // DEC //
    /////////
    
    FT get_primal_length(const Edge& edge) const
    {
        return get_length(edge);
    }
    
    FT get_dual_length(const Edge& edge) const
    {
        Segment dual_segment = get_dual_segment(edge);
        FT dual_length = std::sqrt(dual_segment.squared_length());
        if (dual_length < EPS) return 0.0;
        
        Segment primal_segment = get_segment(edge);
        Vector primal_vector = primal_segment.to_vector();
        Vector dual_vector = dual_segment.to_vector();

        FT cross = cross_product(primal_vector, dual_vector);
        if (cross < 0.0) dual_length = - dual_length;
        return dual_length;
    }
    
    FT get_ratio(const Edge& edge) const
    {
        FT mass = 1.0;
        FT dual_len = get_dual_length(edge);
        FT primal_len = get_primal_length(edge);
        return mass * (dual_len / primal_len);
    }
    
    ////////////
    // LOCATE //
    ////////////
    
    Vertex_handle find_nearest_vertex(const Point& query,
                                      Vertex_handle candidate = Vertex_handle()) const
    {
        typename Kernel::Compare_power_distance_2 cmp_power_distance =
        Rt::geom_traits().compare_power_distance_2_object();
        
        Vertex_handle vertex = candidate;
        if (vertex == Vertex_handle()) vertex = Rt::finite_vertex();
        
        Vertex_handle vclosest;
        do {
            vclosest = vertex;
            Weighted_point wp = vertex->point();
            
            Vertex_circulator vcirc = Rt::incident_vertices(vertex);
            Vertex_circulator vend  = vcirc;
            CGAL_For_all(vcirc, vend)
            {
                Vertex_handle v = vcirc;
                if (is_infinite(v)) continue;
                
                if (cmp_power_distance(query, v->point(), wp) == CGAL::SMALLER ) 
                {
                    vertex = v;
                    break;
                }
            }
        } while (vclosest != vertex);
        return vclosest;  
    }    
};

#endif

#define CGAL_TYPES
#ifdef  CGAL_TYPES

// local
// #include "primitives.h"
// #include "rt2.h"

#undef min
#undef max

//typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

// Basic types
typedef Kernel::FT         FT;
typedef Kernel::Point_2    Point;
typedef Kernel::Vector_2   Vector;
typedef Kernel::Ray_2      Ray;
typedef Kernel::Line_2     Line;
typedef Kernel::Segment_2  Segment;
typedef Kernel::Triangle_2 Triangle;

typedef Kernel::Weighted_point_2 Weighted_point;
typedef Kernel::FT Weight;

// Vertex
typedef CGAL::Regular_triangulation_vertex_base_2<Kernel> RVb;
typedef My_vertex_base<Kernel, RVb> MVb;

// Face
typedef CGAL::Regular_triangulation_face_base_2<Kernel> RFb;
typedef My_face_base<Kernel, RFb> MFb;

// Triangulation
typedef CGAL::Triangulation_data_structure_2<MVb, MFb> TDS;
typedef CGAL::Regular_triangulation_2<Kernel, TDS> Regular_triangulation;
typedef CTriangulation<Regular_triangulation> RT;

typedef RT::Vertex                   Vertex;
typedef RT::Vertex_handle            Vertex_handle;
typedef RT::Vertex_iterator          Vertex_iterator;
typedef RT::Vertex_circulator        Vertex_circulator;
typedef RT::Finite_vertices_iterator Finite_vertices_iterator;
typedef RT::Hidden_vertices_iterator Hidden_vertices_iterator;

typedef RT::Edge                  Edge;
typedef RT::Edge_iterator         Edge_iterator;
typedef RT::Edge_circulator       Edge_circulator;
typedef RT::Finite_edges_iterator Finite_edges_iterator;

typedef RT::Face                  Face;
typedef RT::Face_handle           Face_handle;
typedef RT::Face_iterator         Face_iterator;
typedef RT::Face_circulator       Face_circulator;
typedef RT::Finite_faces_iterator Finite_faces_iterator;

// Domain
typedef RT::Domain     Domain;

#endif

class Scene
{    
public:    
    typedef CLSWeights<Scene, FT> LSWeights;
    typedef CLSPositions<Scene, Point, Vector> LSPositions;
private:
    RT m_rt;
    Domain m_domain;
    std::vector<FT> m_capacities;
    std::vector<Vertex_handle> m_vertices;
    bool m_fixed_connectivity;
    double m_tau;
    
    bool m_timer_on;
    std::vector<double> m_timer;

// Custom code, not part of BNOT source code
private:
    mutable std::mt19937 m_generator;
public:
    double random_double(const double min, const double max) const
    {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(m_generator);
    }

    int random_int(const int min, const int max) const
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(m_generator);
    }

    void seed(std::size_t seed)
    {
        m_generator.seed(seed);
    }
// End custom code
public:
    Scene()
    {
        srand(0);
        m_tau = 1.0;
        init_domain(4);
        m_rt.set_domain(&m_domain);
        m_fixed_connectivity = false;
        m_timer_on = false;
    }

    bool connectivity_fixed() const { return m_fixed_connectivity; }


    Vertex_handle insert_vertex(const Point& point,
                                const FT weight,
                                const unsigned index)
    {
        Weighted_point wp(point, weight);
        Vertex_handle vertex = m_rt.insert(wp);

        if (vertex->get_index() != -1)
            return Vertex_handle();
        
        vertex->set_index(index);
        return vertex;
    }

    void populate_vertices(const std::vector<Point>& points,
                              const std::vector<FT>& weights)
    {
        // if (m_timer_on)
        //     Timer::start_timer(m_timer, COLOR_YELLOW, "Populate");

        unsigned nb = 0;
        unsigned nsites = static_cast<unsigned>(points.size());
        for (unsigned i = 0; i < nsites; ++i)
        {
            Vertex_handle vertex = insert_vertex(points[i], weights[i], nb);
            if (vertex == Vertex_handle()) continue;
            m_vertices.push_back(vertex);
            nb++;
        }
        
        // if (m_timer_on)
        //     Timer::stop_timer(m_timer, COLOR_YELLOW);
    }

    unsigned count_visible_sites() const
    {
        unsigned nb = 0;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            nb++;
        }
        return nb;
    }

    FT compute_mean_capacity() const
    {
        FT domain_area = m_domain.get_area();
        unsigned nb = count_visible_sites();
        return (domain_area / nb);
    }

    void compute_capacities(std::vector<FT>& capacities) const
    {
        FT C = compute_mean_capacity();
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            FT Ci = 0.0;
            Vertex_handle vi = m_vertices[i];
            if (!vi->is_hidden()) Ci = C;
            capacities.push_back(Ci);
        }
    }

    void construct_triangulation(const std::vector<Point>& points,
                                 const std::vector<FT>& weights)
    {
        // if (m_timer_on)
        // {
        //     Timer::start_timer(m_timer, COLOR_BLUE, "Triangulation");
        //     std::cout << std::endl;
        // }
        
        clear_triangulation();
        populate_vertices(points, weights);
        m_rt.pre_compute_cells();
        compute_capacities(m_capacities);

        // if (m_timer_on)
        //     Timer::stop_timer(m_timer, COLOR_BLUE);
    }

    void init_domain(unsigned nb)
    {
        clear();
        if (nb != 4) 
        {
            // std::cout << "only rectangular domains supported" << std::endl;
            return;
        }
        m_domain.init_rectangle(0.5, 0.5);
        m_domain.init_area();
    }
    
    
    void collect_sites(std::vector<Point>& points,
                              std::vector<FT>& weights) const
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            Point pi = vi->get_position();
            points.push_back(pi);
            
            FT wi = 0.0;
            wi = vi->get_weight();
            weights.push_back(wi);
        }
    }


    void update_triangulation()
    {
        std::vector<FT> weights;
        std::vector<Point> points;
        collect_sites(points, weights);
        construct_triangulation(points, weights);
    }

    void update_positions(const std::vector<Point>& points, bool clamp = true, bool hidden = true)
    {
        unsigned j = 0;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (hidden && vi->is_hidden()) continue;
            
            Point pi = points[j++];
            if (clamp) pi = m_domain.clamp(pi);
            vi->set_position(pi);
        }
    }

    void update_weights(const std::vector<FT>& weights, bool hidden = true)
    {    
        unsigned j = 0;
        FT mean = compute_mean(weights);
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (hidden && vi->is_hidden()) continue;
            vi->set_weight(weights[j++] - mean);
        }
    }

    void compute_position_gradient(std::vector<Vector>& gradient, FT coef = 1.0)
    {
        // if (m_timer_on)
        //     Timer::start_timer(m_timer, COLOR_BLUE, "XGrad");
        
        gradient.clear();
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];        
            if (vi->is_hidden()) continue;
            
            FT Vi = m_rt.compute_area(vi);
            Point xi = vi->get_position();
            Point ci = m_rt.compute_centroid(vi);
            Vector gi = -2.0*Vi*(xi - ci);
            gradient.push_back(coef * gi);
        }

        // if (m_timer_on)
        //     Timer::stop_timer(m_timer, COLOR_BLUE);
    }

    FT optimize_positions_via_lloyd(bool update)
    {
        std::vector<Point> points;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;        
            Point ci = m_rt.compute_centroid(vi);
            points.push_back(ci);
        }
        update_positions(points);
        if (update) update_triangulation();

        std::vector<Vector> gradient;
        compute_position_gradient(gradient);
        return compute_norm(gradient);
    }

    FT compute_total_area() const
    {
        return m_rt.compute_area();
    }

    FT compute_weight_threshold(FT epsilon) const
    {
        // reference: 0.3e-5 for 1000 sites
        FT A = compute_total_area(); // A = 1
        unsigned N = count_visible_sites();
        return  (0.03*epsilon) * (A) / FT(N);
    }

    FT compute_position_threshold(FT epsilon) const
    {
        // reference: 3e-5 for 1000 sites
        FT A = compute_total_area(); // A = 1
        unsigned N = count_visible_sites();
        return (0.03*epsilon) * std::sqrt(A*A*A) / FT(N);
    }

    void reset_weights()
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vertex = m_vertices[i];
            vertex->set_weight(0.0);
        }
        update_triangulation();
    }

    FT compute_wcvt_energy()
    {
        // if (m_timer_on)
        //     Timer::start_timer(m_timer, COLOR_BLUE, "Energy");

        FT cvt = 0.0;
        FT w_dot_V = 0.0;
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {    
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            
            FT Vi = m_rt.compute_area(vi);
            FT wi = vi->get_weight();
            FT Ci = m_capacities[i];
            w_dot_V += wi * (Vi - Ci);
            
            cvt += m_rt.compute_variance(vi);
        }
        
        // if (m_timer_on)
        //     Timer::stop_timer(m_timer, COLOR_BLUE);

        return (w_dot_V - cvt);
    }

    void compute_weight_gradient(std::vector<FT>& gradient, FT coef = 1.0)
    {
        // if (m_timer_on)
        //     Timer::start_timer(m_timer, COLOR_BLUE, "WGrad");

        gradient.clear();
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;        
            
            FT Ci = m_capacities[i];
            FT Vi = m_rt.compute_area(vi);
            FT Gi = Vi - Ci;
            gradient.push_back(coef * Gi);
        }

        // if (m_timer_on)
        //     Timer::stop_timer(m_timer, COLOR_BLUE);
    }

    bool solve_linear_system(const SparseMatrix& A,
                             std::vector<double>& x,
                             const std::vector<double>& b) const
    {
        SuiteSparseQRFactorizer solver;
        bool ok = solver.factorize(A);
        if (!ok) return false;
        
        ok = solver.solve(b, x);
        return ok;
    }

    void build_laplacian(const FT scale,
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
                if (!m_rt.is_inside(edge)) continue;
                
                Vertex_handle vj = m_rt.get_source(edge);
                if (vj == vi) vj = m_rt.get_target(edge);
                
                unsigned j = vj->get_index();
                j = indices.find(j)->second;
                
                double coef = scale * m_rt.get_ratio(edge);
                if (std::abs(coef) < EPS) continue;
                
                rowi.setValue(j, -coef);
                diagi += coef;
            }
            
            rowi.setValue(i, diagi);
            A.setRow(i, rowi);
        }
    }

    bool solve_newton_step(const std::vector<FT>& b, std::vector<FT>& x)
    {
        // if (m_timer_on) Timer::start_timer(m_timer, COLOR_BLUE, "LinearSolver");

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
            // std::cout << red << "linear solver failed" << white << std::endl;
            return false;
        }

        // if (m_timer_on) Timer::stop_timer(m_timer, COLOR_BLUE);
        return true;
    }


    void collect_visible_weights(std::vector<FT>& weights) const
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            weights.push_back(vi->get_weight());
        }
    }

    void collect_visible_points(std::vector<Point>& points) const
    {
        for (unsigned i = 0; i < m_vertices.size(); ++i)
        {
            Vertex_handle vi = m_vertices[i];
            if (vi->is_hidden()) continue;
            points.push_back(vi->get_position());
        }
    }

    FT optimize_weights_via_newton(FT timestep, bool update)
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
            LSWeights line_search(this, 20, 2.0);
            /* FT step = */ (void) line_search.run_bt(weights, direction);
        } else {
            for (unsigned i = 0; i < weights.size(); ++i)
            {
                FT wi = weights[i];
                FT gi = direction[i];
                weights[i] = wi + timestep*gi;
            }    
            update_weights(weights);
            if (update) update_triangulation();
        }
        
        compute_weight_gradient(gradient);
        return compute_norm(gradient);
    }

    unsigned optimize_weights_via_newton_until_converge(FT timestep, 
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

    FT optimize_positions_via_gradient_ascent(FT timestep, bool update)
    {
        std::vector<Point> points;
        collect_visible_points(points);
        
        std::vector<Vector> gradient;
        compute_position_gradient(gradient);
        
        if (timestep <= 0.0)
        {
            double mean_capacity = compute_mean(m_capacities);
            double max_alpha = 1.0 / mean_capacity;        
            LSPositions line_search(this, 100, max_alpha);
            
            // if (m_timer_on) { Timer::start_timer(m_timer, COLOR_RED, "XLineSearch"); std::cout << std::endl; }
            /* FT step = */ (void) line_search.run_bt(points, gradient);
            // if (m_timer_on) Timer::stop_timer(m_timer, COLOR_RED);
            
            gradient.clear();
            compute_position_gradient(gradient);
            return compute_norm(gradient);
        }

        for (unsigned i = 0; i < points.size(); ++i)
        {
            Point  pi = points[i];
            Vector gi = gradient[i];
            points[i] = pi + timestep*gi;
        }

        update_positions(points);
        if (update) update_triangulation();
        
        gradient.clear();
        compute_position_gradient(gradient);
        return compute_norm(gradient);
    }

    unsigned optimize_all(FT wstep, FT xstep, 
                          unsigned max_newton_iters,
                          FT epsilon, 
                          unsigned max_iters,
                          std::ostream& out)
    {
        bool global_connectivity = m_fixed_connectivity;
        FT norm = optimize_positions_via_lloyd(true);    
        unsigned nb0 = count_visible_sites();
         
        FT xthreshold = compute_position_threshold(epsilon);
        FT wthreshold = compute_weight_threshold(epsilon);
        
        // out << "NbSites: " << nb0 << std::endl;
        // out << "Threshold: " << xthreshold << " ; " << wthreshold << std::endl;
         
        m_fixed_connectivity = false;
        FT coarse_xthreshold = 2.0*xthreshold;
        FT coarse_wthreshold = 2.0*wthreshold;
        
        unsigned iters = 0;
        while (iters < max_iters)
        {
            iters++;
            reset_weights();        
            optimize_weights_via_newton_until_converge(wstep, coarse_wthreshold, 0, max_newton_iters);        
            norm = optimize_positions_via_lloyd(true);
            // std::cout << "Norm: " << norm << std::endl;
            if (norm <= coarse_xthreshold) break;
        }
        
        // std::cout << "Partial: " << iters << " iters" << std::endl;
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
            norm = optimize_positions_via_gradient_ascent(xstep, true);
            // std::cout << "Norm: " << norm << std::endl;
            if (norm <= fine_xthreshold) break;
        }
        optimize_weights_via_newton_until_converge(wstep, 0.1*fine_wthreshold, 0, max_newton_iters);
         
        m_fixed_connectivity = global_connectivity;
        return iters;
    }

    ~Scene()
    {
        clear();
    }    

    void clear_triangulation()
    {
        m_capacities.clear();
        m_vertices.clear();
        m_rt.clear();
    }

    
    void clear()
    {
        clear_triangulation();
    }
};

}