//
// Source code for the paper
// 
// D. Heck and T. Schloemer and O. Deussen, "Blue Noise Sampling with
// Controlled Aliasing", ACM Trans. Graph., 2013, in press
//
// Copyright (C) 2012,2013 Daniel Heck and Thomas Schloemer
//
#ifndef COMMON_HH_INCLUDED
#define COMMON_HH_INCLUDED

#include <string>
#include <vector>
#include <cmath>
#include <tuple> // std::ignore
#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
double drand48(){
    return rand() / (RAND_MAX + 1.0);
}
#endif

// -------------------- Math --------------------

template <class T, class U>
inline T Clamp(T x, U min, U max) {
    if (x < min)
	return min;
    else if (x > max)
	return max;
    else
	return x;
}


// -------------------- Utility --------------------

bool HasSuffix(const std::string &s, const std::string &suffix);

enum Endianness {
    BigEndian, LittleEndian
};

Endianness SystemEndianness();
void SwapEndian4(uint8_t *buf, int n);
bool WriteFloats(FILE *fp, const float *p, int n, Endianness endian);
bool ReadFloats(FILE *fp, float *p, int n, Endianness endian);

void ReadPFM(int &width, int &height, float *&data, const char *fname);
void WritePFM(int width, int height, const float *data, const char *fname);

// Output floating point matrix as PGM image
void SavePGM(float *p, int rows, int cols, const char *fname);


// -------------------- Point --------------------

struct heck_Point {
    float x, y;
    heck_Point(float x = 0, float y = 0) : x(x), y(y) {}
};

void LoadPoints(const std::string &fname, std::vector<heck_Point> &points);
void WritePoints(const std::string &fname, std::vector<heck_Point> &points);

// -------------------- Curve --------------------

class Curve {
    std::vector<float> y;
public:
    float x0, x1, dx;
    Curve() { x0 = x1 = dx = 0.0f; }
    Curve(const Curve &c);
    Curve(int size, float x0, float x1);

    float &operator[](size_t i) { return y[i]; }
    const float &operator[](size_t i) const { return y[i]; }

    int size() const { return (int)y.size(); }

    int ToIndex(float x) const {
        return static_cast<int>((x-x0)/dx);
    }

    float ToXCenter(int index) const {
        return x0 + (index+0.5f)*dx;
    }
    float ToX(int index) const {
        return x0 + index*dx;
    }

    // Evaluate curve at point 'x'. Use linear interpolation if 'x' falls
    // between two bins.
    float At(float x) const;

    void Write(const std::string &fname);
    static Curve Read(const std::string &fname);

    void FilterGauss(const Curve &source, float sigma);
};


inline Curve FilterGauss(int nbins, const Curve &source, float sigma) {
    Curve c(nbins, source.x0, source.x1);
    c.FilterGauss(source, sigma);
    return c;
}

float Integrate(const Curve &c, float x0, float x1, float *E = NULL);

inline float Integrate(const Curve &c) {
    return Integrate(c, c.x0, c.x1);
}


// -------------------- Toroidal distance --------------------

inline static float Sqr(float x) {
    return x*x;
}
  
// Compute toroidal distance between points a and b. Both a and b must
// lie inside the unit square.
inline static float DistSqrToroidal(float const *a, float const *b) {
    float dx = a[0]-b[0], dy = a[1]-b[1];
    return Sqr(dx + (dx < -.5f) - (dx > .5f)) + 
        Sqr(dy + (dy < -.5f) - (dy > .5f));
}

inline static float DistToroidal(float const *a, float const *b) {
    return sqrtf(DistSqrToroidal(a, b));
}

// -------------------- RDF calculations --------------------

void CalcRDF(Curve &c, size_t npts, const float *pts, 
        float (*distfunc)(const float *, const float *) = DistToroidal);
Curve CalcRDF(int numbins, size_t npts, const float *pts, 
        float sigma = 0, float (*distfunc)(const float *, const float *) = DistToroidal);

Curve RDF2Power(int npts, const Curve &rdf, int nbins, float x0, float x1);
Curve Power2RDF(int npts, const Curve &power, int nbins, float x0, float x1, float smoothing);


//
// Source code for the paper
// 
// D. Heck and T. Schloemer and O. Deussen, "Blue Noise Sampling with
// Controlled Aliasing", ACM Trans. Graph., 2013, in press
//
// Copyright (C) 2012,2013 Daniel Heck and Thomas Schloemer
//
#include "common.hh"
#include <fstream>
#include <iostream>
#include <string.h>
#include <cstdlib>

// Precomputed arrays for Bessel function computation
#define PI_4    0.7853981633974483096f
#define DR1     5.7831859629467845211f

static const float JP[5] = {
    -6.068350350393235E-008f,
    6.388945720783375E-006f,
    -3.969646342510940E-004f,
    1.332913422519003E-002f,
    -1.729150680240724E-001f
};
static const float MO[8] = {
    -6.838999669318810E-002f,
    1.864949361379502E-001f,
    -2.145007480346739E-001f,
    1.197549369473540E-001f,
    -3.560281861530129E-003f,
    -4.969382655296620E-002f,
    -3.355424622293709E-006f,
    7.978845717621440E-001f
};
static const float PH[8] = {
    3.242077816988247E+001f,
    -3.630592630518434E+001f,
    1.756221482109099E+001f,
    -4.974978466280903E+000f,
    1.001973420681837E+000f,
    -1.939906941791308E-001f,
    6.490598792654666E-002f,
    -1.249992184872738E-001f
};

// Single precision approximation to j0, about twice as fast as the standard
// implementation. From cephes lib, available at http://www.netlib.org/cephes/
inline float Polynomial(float x, const float *p, int n) {
    float y = p[0];
    for (int i = 1; i < n; ++i)
        y = y * x + p[i];
    return y;
}

inline float j0f(float f) {
    float x = fabsf(f);
    if (x <= 2.f) {
        float z = x*x;
        if (x < 1.0e-3f)
            return (1.f - 0.25f*z);
        return (z - DR1) * Polynomial(z, JP, 5);
	}
    float q = 1.f / x;
    float w = sqrtf(q);
    float p = w * Polynomial(q, MO, 8);
    w = q*q;
    float xn = q * Polynomial(w, PH, 8) - PI_4;
    return p * cosf(xn + x);
}

bool HasSuffix(const std::string &s, const std::string &suffix) {
    int i = s.size() - suffix.size();
    if (i >= 0)
        return s.substr(i) == suffix;
    return false;
}

Endianness SystemEndianness() {
    union { uint32_t i; char c[4]; } e;
    e.i = 1;
    return e.c[0] == 0 ? BigEndian : LittleEndian;
}

void SwapEndian4(uint8_t *buf, int nn) {
    for (int i=0; i<nn; i++) {
        std::swap(buf[4*i], buf[4*i+3]);
        std::swap(buf[4*i+1], buf[4*i+2]);
    }
}

bool WriteFloats(FILE *fp, const float *p, int n, Endianness endian) {
    Endianness sysEndian = SystemEndianness();
    const int nbuf = 256;
    uint8_t buf[nbuf*sizeof(float)];
    while (n > 0) {
        int nn = std::min(n, nbuf);
        memcpy(buf, p, nn * sizeof(float));
        if (endian != sysEndian)
            SwapEndian4(buf, nn);
        if (fwrite(buf, sizeof(float), nn, fp) != (size_t)nn)
            return false;
        p += nn; n -= nn;
    }
    return true;
}

bool ReadFloats(FILE *fp, float *p, int n, Endianness endian) {
    Endianness sysEndian = SystemEndianness();
    const int nbuf = 256;
    uint8_t buf[nbuf*sizeof(float)];
    while (n > 0) {
        int nn = std::min(n, nbuf);
        if (fread(buf, sizeof(float), nn, fp) != (size_t)nn)
            return false;
        if (endian != sysEndian)
            SwapEndian4(buf, nn);
        memcpy(p, buf, nn*sizeof(float));
        p += nn; n -= nn;
    }
    return true;
}

void ReadPFM(int &width, int &height, float *&data, const char *fname) {
    FILE* fp = fopen(fname, "rb");
    char name[100];
    float f;
    std::ignore = fscanf(fp, "%s\n%d %d %f\n", name, &width, &height, &f);
    //printf("f is %f\n", f);
    data = new float[width * height];
    std::ignore = fread((char*) data, sizeof(float), width * height, fp);
    fclose(fp);
}

void WritePFM(int width, int height, const float *data, const char *fname) {
    FILE* fp = fopen(fname, "wb");
    fprintf(fp, "Pf\n");
    fprintf(fp, "%d %d\n-1.00\n", width, height);
    fwrite((const char*) data, sizeof(float), width * height, fp);
    fclose(fp);
}


void LoadPoints(const std::string &fname, std::vector<heck_Point> &points) {
    int npts = 0;
    if (HasSuffix(fname, ".rps")) {
        FILE *fp= fopen(fname.c_str(), "rb");
        if (!fp) {
            std::cerr << "Cannot load '" << fname << "'.\n";
            exit(1);
        }
        fseek(fp, 0, SEEK_END);
        npts = ftell(fp) / 8;
        points.resize(npts);
        fseek(fp, 0, SEEK_SET);
        ReadFloats(fp, &points[0].x, npts*2, LittleEndian);
        fclose(fp);
    } else {
        std::ifstream fp(fname.c_str());
        if (!fp) {
            std::cerr << "Cannot load '" << fname << "'.\n";
            exit(1);
        }
        fp >> npts;
        points.reserve(npts);
        heck_Point pt;
        while ((int)points.size() < npts && (fp >> pt.x >> pt.y))
            points.push_back(pt);
    }
}

void WritePoints(const std::string &fname, std::vector<heck_Point> &points) {
    std::ofstream fp(fname.c_str(), std::ios::app);
    if (!fp) {
        std::cerr << "Cannot create '" << fname << "'.\n";
        exit(1);
    }
    //fp << points.size() << "\n";
    for (unsigned i = 0; i < points.size(); i++)
        fp << points[i].x << " " << points[i].y << "\n"; 
}


void SavePGM(float *p, int rows, int cols, const char *fname) {
    FILE *fp = fopen(fname, "wb");
    if (!fp) {
        fprintf(stderr, "Cannot open '%s' for writing.\n", fname);
        exit(1);
    }

    int maxval = 255;
    fprintf(fp, "P2\n%d %d\n%d\n", cols, rows, maxval);

    for (int y=0; y<rows; y++) {
        for (int x=0; x<cols; x++) {
            int pp = (int)(p[y*cols + x] * maxval);
            if (pp < 0) pp = 0;
            if (pp > maxval) pp = maxval;
            fprintf(fp, "% 3d ", pp);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}


// -------------------- Curve --------------------

Curve::Curve(const Curve &c) {
    x0 = c.x0; 
    x1 = c.x1;
    dx = c.dx;
    y = c.y;
}

Curve::Curve(int size, float x0, float x1) {
    y.resize(size);
    
    this->x0 = x0;
    this->x1 = x1;
    dx = 1.f/size * (x1 - x0);
}

float Curve::At(float x) const {
    float xx = (x-x0)/dx;
    if (xx < 0) xx = 0;
    int i = (int)floor(xx);
    int ii = i+1;
    float a = xx-i;
    if (i >= (int)y.size()) i = y.size()-1;
    if (ii >= (int)y.size()) ii = y.size()-1;
    return (1-a)*y[i] + a * y[ii];
}

void Curve::Write(const std::string &fname) {
    std::ofstream os(fname.c_str());
    for (int j=0; j<size(); j++)
        os << ToX(j) << " " << y[j] << "\n";
}

Curve Curve::Read(const std::string &fname) {
    std::ifstream is(fname.c_str());
    std::vector<float> x, y;
    float xx, yy;
    while (is >> xx >> yy) {
        x.push_back(xx);
        y.push_back(yy);
    }
    if (x.empty()) {
        std::cerr << "Could not load RDF from '" << fname << "'.\n";
        exit(1);
    }

    int n = x.size();
    float dx = n > 1 ? x[1] - x[0] : 1;
    Curve c(n, x[0]-dx/2, x[n-1] + dx/2);
    for (int i=0; i<n; i++)
        c.y[i] = y[i];
    return c;
}



void Curve::FilterGauss(const Curve &source, float sigma) {
    if (sigma == 0){
        y = source.y;
        return;
    }
  
    float dd = (float)source.size() / size();
  
    for (int i=0; i<size(); i++) {
        float a = 0.0f, sumw = 0.0f;

        int jmin = std::max(0, (int)floor(dd * (i - 5 * sigma)));
        int jmax = std::min((int)source.size()-1, (int)ceil(dd * (i + 5*sigma)));
        for (int j=jmin; j<=jmax; j++) {
            float d = j/dd-i;
            float w = exp(-d*d/(2*sigma*sigma));
            a += source[j] * w;
            sumw += w;
        }
        y[i] = a / sumw;
    }
}

float Integrate(const Curve &c, float x0, float x1, float *E) {
    bool negate = false;
    if (x0 > x1) {
        std::swap(x0, x1);
        negate = true;
    }

    float T = 0.0f, M = 0.0f;
    int i0 = c.ToIndex(x0);
    int i1 = c.ToIndex(x1+c.dx);
    if (i0 < 0) i0 = 0;
    if (i1 < 0) i1 = 0;
    if (i0 >= c.size()) i0 = c.size()-1;
    if (i1 >= c.size()) i1 = c.size()-1;

    T = 0.5 * (c[i0] + c[i1]);
    for (int i=i0+2; i<i1; i+=2) {
        T += c[i];
        M += c[i-1];
    }
    if (E != NULL)
        *E = c.dx * 2.0f * fabs(T-M);
    float a = c.dx*(T+M);
    return negate ? -a : a;
}


// -------------------- RDF Calculation --------------------

void CalcRDF(Curve &c, size_t npts, const float *pts, float (*distfunc)(const float *, const float *)) {
    std::vector<unsigned long> bins(c.size());
    for (unsigned j=0; j<npts; j++) {
        for (unsigned k=j+1; k<npts; k++) {
            float d = distfunc(&pts[2*j], &pts[2*k]);
            int idx = c.ToIndex(d);
            if (0 <= idx && idx < c.size())
                bins[idx]++;
//            if (d < c.x1)
//                bins[c.ToIndex(d)]++;
        }
    }
    const float scale = npts * (npts-1)/2 * M_PI * c.dx * c.dx;
    for (int j=0; j<c.size(); j++)
        c[j] = bins[j] / (scale * (2*j+1));
}

Curve CalcRDF(int numbins, size_t npts, const float *pts, 
        float sigma, float (*distfunc)(const float *, const float *))
{
    Curve c(numbins, 0, 0.5f);
    CalcRDF(c, npts, pts, distfunc);
    if (sigma == 0)
        return c;
    else
        return FilterGauss(numbins, c, sigma);
}

float HannWindow(float x, float xlim) {
    return x > xlim ? 0.0f : Sqr(cosf(M_PI * x / (2*xlim)));
}

float BlackmanWindow(float x, float xlim) {
    return x > xlim ? 0.0f : 0.43 + 0.5*cosf(M_PI*x/xlim) + 0.08*cosf(2*M_PI*x/xlim);
}

float HammingWindow(float x, float xlim) {
    return x > xlim ? 0.0f : 0.54f + 0.46f * cosf(M_PI * x/xlim);
}

float WelchWindow(float x, float xlim) {
    return x > xlim ? 0.0f : 1 - x*x/(xlim*xlim);
}

Curve RDF2Power(int npts, const Curve &rdf, int nbins, float x0, float x1) {
    Curve power(nbins, x0, x1);
    Curve tmp(rdf);
    for (int i=0; i<nbins; i++) {
        const float u0 = power.ToX(i);
        const float u = 2*M_PI*u0;
        const float wndsize = rdf.x1 * std::min(0.5f, std::max(0.2f, 4*u0/sqrtf(npts)));
        for (int j=0; j<tmp.size(); j++) {
            float x = rdf.ToX(j);
            float wnd = BlackmanWindow(x, wndsize);
//            float wnd = HannWindow(x, wndsize);
            tmp[j] = (rdf[j]-1) * j0f(u*x) * x * wnd;
        }
        power[i] = fabs(1 + 2 * M_PI * Integrate(tmp) * npts);
    }
    return power;
}

Curve Power2RDF(int npts, const Curve &power, int nbins, float x0, float x1, float smoothing) {
    Curve rdf(nbins, x0, x1);
    Curve tmp(power);
    for (int i=0; i<rdf.size(); i++) {
        float u = rdf.ToX(i);
        for (int j=0; j<tmp.size(); j++) {
            float x = tmp.ToX(j);
            float wnd = BlackmanWindow(x, power.x1);
            tmp[j] = (power[j]-1) * jn(0, 2*M_PI * u * x) * x * wnd;
        }
        rdf[i] = 1 + 2 * M_PI * Integrate(tmp) / npts;
    }
    for (int i=0; i<rdf.size(); i++) {
        if (rdf[i] < 0) {
            printf("Warning: RDF has negative components\n");
            break;
        }
    }
    return FilterGauss(nbins, rdf, smoothing);
}



#endif
