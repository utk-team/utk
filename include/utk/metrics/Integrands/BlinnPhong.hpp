/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * and Bastien Doignies bastien.doignies@liris.cnrs.fr 
 * and David Coeurjolly David.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2023 CNRS Université de Lyon
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
#pragma once

#include <cmath>
#include <utk/utils/FastPRNG.hpp>
#include "../IntegrationTest.hpp"

namespace utk
{
    namespace blinnphong
    {
        struct vec3
        {
            //! constructeur par defaut.
            vec3( ) : x(0), y(0), z(0) {}
            
            vec3( const double _x, const double _y, const double _z ) : x(_x), y(_y), z(_z) {}
            vec3( const vec3& a, const vec3& b ) : x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) {}
            
            //! renvoie la ieme composante.
            double operator() ( const unsigned int i ) const { return (&x)[i]; }
            double& operator() ( const unsigned int i ) { return (&x)[i]; }
            
            double x, y, z;
        };

        typedef vec3 Vector;
        typedef vec3 Point;


        vec3 operator- ( const vec3& a, const vec3& b ) 
        { 
            return vec3(a.x - b.x, a.y - b.y, a.z - b.z); 
        }

        vec3 operator+ ( const vec3& a, const vec3& b )
        { 
            return vec3(a.x + b.x, a.y + b.y, a.z + b.z); 
        }

        vec3 operator* ( const double k, const vec3& a )
        { 
            return vec3(k*a.x, k*a.y, k*a.z); 
        }

        vec3 operator* ( const vec3& a, const double k )
        { 
            return vec3(k*a.x, k*a.y, k*a.z); 
        }

        vec3 operator/ ( const vec3& a, const double k )
        { 
            return vec3(a.x/k, a.y/k, a.z/k); 
        }

        vec3 operator- ( const vec3& a ) 
        { 
            return vec3(-a.x, -a.y, -a.z); 
        }

        //! renvoie le produit scalaire de 2 vecteurs.
        double dot( const vec3& a, const vec3& b )
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        //! renvoie un vecteur unitaire / longueur == 1.
        vec3 normalize( const vec3& v )
        {
            double k= 1 / std::sqrt(dot(v, v));
            return k * v;
        }

        //! renvoie la longueur du vecteur v.
        double length( const vec3& v )
        {
            return std::sqrt(dot(v, v));
        }

        //! renvoie le produit vectoriel de 2 vecteurs.
        vec3 cross( const vec3& u, const vec3& v )
        {
            return vec3(
                (u.y * v.z) - (u.z * v.y),
                (u.z * v.x) - (u.x * v.z),
                (u.x * v.y) - (u.y * v.x));
        }

        double clamp( const double x, const double xmin, const double xmax )
        {
            if(x < xmin) return xmin;
            if(x > xmax) return xmax;
            return x;
        }

        struct QuadSource
        {
            const double eps= 1e-5;
            
            Point a, b, c, d;
            Vector n;
            double emission;
            
            Point p;
            Vector x;
            Vector y;
            Vector z;

            Vector n0;
            Vector n2;
        
            double x0;
            double y0;
            double x1;
            double y1;
            double z0;
            double k;
            
            double S;
            
            // quad abcd dans le sens trigo 
            QuadSource( const Point& p, const Point& _a, const Point& _b, const Point& _c, const Point& _d ) : a(_a), b(_b), c(_c), d(_d), 
                n( normalize( cross(Vector(a, b), Vector(a, d)) )), emission(1)
            {
                // precalcule la parametrisation du quad abcd, observe depuis p...
            
                // "An Area-Preserving Parametrization for Spherical Rectangles"
                // cf https://www.arnoldrenderer.com/research/egsr2013_spherical_rectangle.pdf
                Vector ex= Vector(a, b);
                Vector ey= Vector(a, d);
                double exl = length(ex);
                double eyl = length(ey);
                
                // compute local reference system 'R'
                x = ex / exl;
                y = ey / eyl;
                z = cross(x, y);
                
                // compute rectangle coords in local reference system
                Vector v = Vector(p, a);
                z0 = dot(v, z);
                
                // flip 'z' to make it point against 'Q'
                if(z0 > 0)
                {
                    z = -z;
                    z0 = -z0;
                }
                
                x0 = dot(v, x);
                y0 = dot(v, y);
                x1 = x0 + exl;
                y1 = y0 + eyl;
                
                // create vectors to four vertices
                Vector v00 = Vector(x0, y0, z0);
                Vector v01 = Vector(x0, y1, z0);
                Vector v10 = Vector(x1, y0, z0);
                Vector v11 = Vector(x1, y1, z0);
                
                // compute normals to edges
                n0 = normalize(cross(v00, v10));
                Vector n1 = normalize(cross(v10, v11));
                n2 = normalize(cross(v11, v01));
                Vector n3 = normalize(cross(v01, v00));
                
                // compute internal angles (gamma_i)
                double g0 = std::acos(-dot(n0, n1));
                double g1 = std::acos(-dot(n1, n2));
                double g2 = std::acos(-dot(n2, n3));
                double g3 = std::acos(-dot(n3, n0));
                
                // compute solid angle from internal angles
                k = double(2*M_PI) - g2 - g3;
                S = g0 + g1 - k;
            }

            QuadSource& operator=(const QuadSource& other)
            {
                // eps = other.eps;
                a = other.a;
                b = other.b;
                c = other.c;
                d = other.d;
                n = other.n;

                emission = other.emission;
                p = other.p;
                
                x = other.x;
                y = other.y;
                z = other.z;

                n0 = other.n0;
                n2 = other.n2;

                x0 = other.x0;
                x1 = other.x1;
                y0 = other.y0;
                y1 = other.y1;
                z0 = other.z0;
                
                k = other.k;
                S = other.S;

                return *this;
            }

            Vector sample( const double u1, const double u2 ) const
            {
                // sample
                // 1. compute 'cu'
                double au = u1 * S + k;
                double fu = (std::cos(au) * n0.z - n2.z) / std::sin(au);
                double cu = 1 / std::sqrt(fu*fu + n0.z*n0.z) * (fu>0 ? +1 : -1);
                cu = clamp(cu, -1, 1); // avoid NaNs
                
                // 2. compute 'xu'
                double xu = -(cu * z0) / std::sqrt(1 - cu*cu);
                xu = clamp(xu, x0, x1); // avoid Infs
                
                // 3. compute 'yv'
                double d = std::sqrt(xu*xu + z0*z0);
                double h0 = y0 / std::sqrt(d*d + y0*y0);
                double h1 = y1 / std::sqrt(d*d + y1*y1);
                double hv = h0 + u2 * (h1 - h0);
                double yv = (hv*hv < 1-eps) ? (hv*d) / std::sqrt(1 - hv*hv) : y1;
                
                // 4. transform (xu, yv, z0) to world coords
                return xu*x + yv*y + z0*z;
            }

            // p+s doit etre un point sur la source !!
            double pdf( const Vector& s ) const
            {
                if(dot(s, n) >= 0)
                    return 0;
            
                // pdf 
                if(S <= 0) return 0;
                return 1 / S;
            }
        };
    
        struct Brdf
        {
            double diffuse;
            double kd;
            double ns;
            
            Brdf( const double& color, const double plastic, const double alpha ) : diffuse(color), kd(1 - plastic), ns(alpha) {}
            
            // evalue la brdf : mixture d'un terme diffus et d'un reflet,
            double f( const Vector& l, const Vector& o ) const
            {
                double cos_theta= l.z;
                if(cos_theta <= 0)      // l et o doivent etre normalises !!
                    return 0;
                
                Vector h= normalize(o + l);
                double cos_theta_h= h.z;
                if(cos_theta_h < 0)
                    return 0;
                
                double d= 1 / double(M_PI) * diffuse;
                double s= (ns + 8) / double(8*M_PI) * std::pow(cos_theta_h, ns);
                return kd * d + (1 - kd) * s;
            }
        };
    }; // Blinn Phong

    class BlinnPhong : public Integrand
    {
    public:
        BlinnPhong(uint32_t dim) : 
            Integrand(2), 
            quad(
                blinnphong::Point( 0.0, -1.0, 0.0),
                blinnphong::Point(-1.0,  0.0, 0.0),
                blinnphong::Point( 1.0,  0.0, 0.0),
                blinnphong::Point( 1.0,  0.0, 1.0),
                blinnphong::Point(-1.0,  0.0, 1.0)
            ), 
            o(0.0, std::sqrt(3 / 4), 0.5), 
            alpha(50.0)
        {
            ((void) dim); 
        }

        double GetScalar(const GenerationParameter& params, const std::string& name, utk::PCG32& gen, double dmin = -1.0, double dmax = 1.0)
        {
            const std::string namemin = name + std::string("min");
            const std::string namemax = name + std::string("max");

            double mmin = dmin;
            double mmax = dmax;

            if (params.find(namemin) != params.end())
                mmin = std::get<double>(params.at(namemin)); 

            if (params.find(namemax) != params.end())
                mmax = std::get<double>(params.at(namemax)); 

            return std::uniform_real_distribution<double>(mmin, mmax)(gen);
        }

        blinnphong::Point GetPosition(const GenerationParameter& params, utk::PCG32& gen)
        {
            double range = GetScalar(params, "p", gen);
            
            double xnorm = std::normal_distribution<double>(0.0, 1.0)(gen);
            double ynorm = std::normal_distribution<double>(0.0, 1.0)(gen);
            double znorm = std::normal_distribution<double>(0.0, 1.0)(gen);
            
            ynorm = -std::abs(ynorm);
            blinnphong::Point p = blinnphong::Point(xnorm, ynorm, znorm);
            p = blinnphong::normalize(p);
            p = p * std::pow(range, 1 / 3);
            return p;
        }

        void GenerateRandom(const GenerationParameter& params, uint64_t seed) override
        {
            utk::PCG32 gen(seed);
            p = GetPosition(params, gen);

            alpha = GetScalar(params, "alpha", gen, 10, 100);

            const double cos_phi   = std::uniform_real_distribution<double>(0, 1.0)(gen);
            const double sin_phi   = std::sqrt(1.0 - cos_phi * cos_phi);
            const double cos_theta = std::uniform_real_distribution<double>(0, 1.0)(gen);
            const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

            o     = blinnphong::Vector(cos_phi * sin_theta, -sin_phi * sin_theta, cos_theta);
            quad  = blinnphong::QuadSource(
                p,
                blinnphong::Point(-1.0,  0.0, 0.0),
                blinnphong::Point( 1.0,  0.0, 0.0),
                blinnphong::Point( 1.0,  0.0, 1.0),
                blinnphong::Point(-1.0,  0.0, 1.0)
            );
        }

        virtual void WriteInStream (std::ostream& stream) const override
        {
            stream << p.x << " " << p.y << " " << p.z;
            stream << " " << o.x << " " << o.y << " " << o.z;
            stream << " " << alpha;
        }

        void ReadFromStream(std::istream& stream) override 
        {
            stream >> p.x >> p.y >> p.z;
            stream >> o.x >> o.y >> o.z;
            stream >> alpha;

            quad = blinnphong::QuadSource(
                p,
                blinnphong::Point(-1.0,  0.0, 0.0),
                blinnphong::Point( 1.0,  0.0, 0.0),
                blinnphong::Point( 1.0,  0.0, 1.0),
                blinnphong::Point(-1.0,  0.0, 1.0)
            );
        }

        double eval(const double* pts) const override
        {
            constexpr static double PI = 3.141592653589793238462;
            
            const double u1 = pts[0];
            const double u2 = pts[1];
            
            double sample = 0.;
            
            blinnphong::Vector l = quad.sample(u1, u2);
            double pdf = quad.pdf(l);

            if(pdf > 0)
            {
                // ou encore plus simple, on evalue sur place le reflet
                l = normalize(l);
                double cos_theta= std::max(0.0, l.z);
             
                if(cos_theta > 0) 
                {
                    blinnphong::Vector h = normalize(o + l);
                    double cos_theta_h   = std::max(double(0), h.z);
                    double fr = (alpha + 8) / double(8 * PI) * std::pow(cos_theta_h, alpha);
                    
                    sample = fr * cos_theta / pdf;
                }
            }
            
            return sample;
        }

    private:
        blinnphong::QuadSource quad;
    
        blinnphong::Point p;
        blinnphong::Vector o;
        double alpha;
    };
};