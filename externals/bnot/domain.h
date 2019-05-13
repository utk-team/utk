#ifndef _DOMAIN_H_
#define _DOMAIN_H_

// FIXME: make domain general

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
    
public:
    CDomain()
    {
    }

    ~CDomain()
    {
    }
    
    template <class OutputIterator>
    void add_segments(OutputIterator out) const
    {
        Point a(0., 0.);
        Point b(1., 0.);
        Point c(1., 1.);
        Point d(0., 1.);
        out++ = Segment(a, b);
        out++ = Segment(b, c);
        out++ = Segment(c, d);
        out++ = Segment(d, a);
    }
    
    bool is_outside(const Point& p) const
    {
        if (p.x() < 0.) return true;
        if (p.x() > 1.) return true;
        if (p.y() < 0.) return true;
        if (p.y() > 1.) return true;
        return false;
    }
    
    Point clamp(const Point& p) const
    {
        FT x = p.x();
        while (x < 0.0) x += 1.0;
        while (x > 1.0) x -= 1.0;

        FT y = p.y();
        while (y < 0.0) y += 1.0;
        while (y > 1.0) y -= 1.0;
        
        return Point(x, y);
    }
    
    void draw() const
    {
        glBegin(GL_LINE_LOOP);
        glVertex2d(0., 1.);
        glVertex2d(1., 1.);
        glVertex2d(1., 0.);
        glVertex2d(0., 0.);
        glEnd();
    }    
    
    FT compute_area() const
    {
        FT side = 1.0;
        return side*side;
    }
    
    Vector get_offset(int h, int v) const 
    { 
        double x = 0.0;
        if (h == -1) x = -1.0;
        if (h ==  1) x =  1.0;

        double y = 0.0;
        if (v == -1) y = -1.0;
        if (v ==  1) y =  1.0;
        
        return Vector(x, y);
    }
    
    Vector extract_offset(const Point& p) const
    {
        Point q = clamp(p);
        return (p - q);
    }
};

#endif
