#include <math.h>   // smallpt, a Path Tracer by Kevin Beason, 2009
#include <stdlib.h> // Make : g++ -O3 -fopenmp explicit.cpp -o explicit
#include <stdio.h>  //        Remove "-fopenmp" for g++ version < 4.2
#include "../../samplers/SamplerSobolIndices.hpp"
#include "../../samplers/SamplerWhitenoise.hpp"
#include "../../samplers/SamplerRegularGrid.hpp"
#include "../../samplers/SamplerStratified.hpp"
#include "../../samplers/SamplerVictor2D2D_06022017.hpp"

#define DEPTH 3

utk::SamplerSobolIndices sampler;
std::vector< std::vector< utk::Pointset<2, double, utk::Point<2, double> > > > pts;

struct Vec 
{        // Usage: time ./explicit 16 && xv image.ppm
  double x, y, z;                  // position, also color (r,g,b)
  Vec(double x_=0, double y_=0, double z_=0){ x=x_; y=y_; z=z_; }
  Vec operator+(const Vec &b) const { return Vec(x+b.x,y+b.y,z+b.z); }
  Vec operator-(const Vec &b) const { return Vec(x-b.x,y-b.y,z-b.z); }
  Vec operator*(double b) const { return Vec(x*b,y*b,z*b); }
  Vec mult(const Vec &b) const { return Vec(x*b.x,y*b.y,z*b.z); }
  Vec& norm(){ return *this = *this * (1/sqrt(x*x+y*y+z*z)); }
  double dot(const Vec &b) const { return x*b.x+y*b.y+z*b.z; } // cross:
  Vec operator%(Vec&b){return Vec(y*b.z-z*b.y,
	  z*b.x-x*b.z,
	  x*b.y-y*b.x);}
};

struct Ray 
{ 
  Vec o, d; Ray(Vec o_, Vec d_) : o(o_), d(d_) {} 
};

enum Refl_t { DIFF, SPEC, REFR };  // material types, used in radiance()

struct Sphere 
{
  double rad;       // radius
  Vec p, e, c;      // position, emission, color
  Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive)

  Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_):
    rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}
  
  double intersect(const Ray &r) const { // returns distance, 0 if nohit
    Vec op = p-r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    double t, eps=1e-4, b=op.dot(r.d), det=b*b-op.dot(op)+rad*rad;
    if (det<0) return 0; else det=sqrt(det);
    return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
  }
};

Sphere spheres[] = {//Scene: radius, position, emission, color, material
  Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left
  Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght
  Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back
  Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//Frnt
  Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm
  Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top
  Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, DIFF),
  Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, DIFF),
  //Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr
  //Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas
  Sphere(1.5, Vec(50,81.6-16.5,81.6),Vec(4,4,4)*100,  Vec(), DIFF),//Lite
};

int numSpheres = sizeof(spheres)/sizeof(Sphere);

inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; }

inline int toInt(double x){ return int(pow(clamp(x),1/2.2)*255+.5); }

inline bool intersect(const Ray &r, double &t, int &id)
{
  double n=sizeof(spheres)/sizeof(Sphere), d, inf=t=1e20;
  for(int i=int(n);i--;) if((d=spheres[i].intersect(r))&&d<t){t=d;id=i;}
  return t<inf;
}

Vec radiance(const Ray &r, int depth, unsigned short *Xi, int idsample, int idpixel, int E=1)
{
  
  double t;                               // distance to intersection
  int id=0;                               // id of intersected object
  
  if (!intersect(r, t, id)) return Vec(); // if miss, return black
  
  const Sphere &obj = spheres[id];        // the hit object
  Vec x=r.o+r.d*t, n=(x-obj.p).norm(), nl=n.dot(r.d)<0?n:n*-1, f=obj.c;
  double p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z; // max refl
  
  if (++depth>=DEPTH||!p)
  {
    /*if (erand48(Xi)<p) 
      f=f*(1/p); 
    else*/
      return obj.e*E;
  }
  
  if (obj.refl == DIFF){                  // Ideal DIFFUSE reflection
    
    double s1 = pts[idpixel][depth][idsample].pos()[0]; //erand48(Xi);
    double s2 = pts[idpixel][depth][idsample].pos()[1]; //erand48(Xi);
    
    double r1=2*M_PI*s1, r2=s2, r2s=sqrt(r2);

	Vec w=nl, u=((fabs(w.x)>.1?Vec(0,1):Vec(1))%w).norm(), v=w%u;
    Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).norm();

    // Loop over any lights
    Vec e;
    for (int i=0; i<numSpheres; i++){
      const Sphere &s = spheres[i];
      if (s.e.x<=0 && s.e.y<=0 && s.e.z<=0) continue; // skip non-lights
      
      Vec sw=s.p-x, su=((fabs(sw.x)>.1?Vec(0,1):Vec(1))%sw).norm(), sv=sw%su;
      double cos_a_max = sqrt(1-s.rad*s.rad/(x-s.p).dot(x-s.p));
      double eps1 = erand48(Xi), eps2 = erand48(Xi);
      double cos_a = 1-eps1+eps1*cos_a_max;
      double sin_a = sqrt(1-cos_a*cos_a);
      double phi = 2*M_PI*eps2;
      Vec l = su*cos(phi)*sin_a + sv*sin(phi)*sin_a + sw*cos_a;
      l.norm();
      if (intersect(Ray(x,l), t, id) && id==i){  // shadow ray
        double omega = 2*M_PI*(1-cos_a_max);
        e = e + f.mult(s.e*l.dot(nl)*omega)*M_1_PI;  // 1/pi for brdf
      }
    }
    
    return obj.e*E+e+f.mult(radiance(Ray(x,d),depth,Xi,idsample,idpixel, 0));
  } 
  else if (obj.refl == SPEC)              // Ideal SPECULAR reflection
    return obj.e + f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,Xi,idsample,idpixel));
  else if (obj.refl == REFR)
  {

    Ray reflRay(x, r.d-n*2*n.dot(r.d));     // Ideal dielectric REFRACTION
    bool into = n.dot(nl)>0;                // Ray from outside going in?
    double nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=r.d.dot(nl), cos2t;
    if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)    // Total internal reflection
      return obj.e + f.mult(radiance(reflRay,depth,Xi,idsample,idpixel));
    Vec tdir = (r.d*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).norm();
    double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
    double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
    
    return obj.e + f.mult(depth>2 ? (erand48(Xi)<P ?   // Russian roulette
      radiance(reflRay,depth,Xi,idsample,idpixel)*RP:radiance(Ray(x,tdir),depth,Xi,idsample,idpixel)*TP) :
      radiance(reflRay,depth,Xi,idsample,idpixel)*Re+radiance(Ray(x,tdir),depth,Xi,idsample,idpixel)*Tr);
  }
  std::cout << "ERROR Unknown material" << std::endl;
  exit(1);
  return obj.e;
}

int main(int argc, char *argv[])
{
	/*sampler.setLookupTableInFile(argv[2]);
	sampler.setTileSize(16);
	sampler.generatesNewPatterns(true);
	sampler.useOptimizedPermuts(false);*/
	
	int w=1024, h=768, samps = argc>=2 ? atoi(argv[1]) : 1; // # samples
	Ray cam(Vec(50,52,295.6), Vec(0,-0.042612,-1).norm()); // cam pos, dir
	Vec cx=Vec(w*.5135/h), cy=(cx%cam.d).norm()*.5135, r, *c=new Vec[w*h];
	//#pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP

	uint smax = h>w ? h : w;
	std::cout << smax << std::endl;
	
	utk::Pointset<2, double, utk::Point<2, double> > pts_full;
	uint nbtiles=2;
	uint nbsample = (samps*smax*smax) / (nbtiles*nbtiles);
	sampler.generateSamples< 2, double, utk::Point<2, double> >(pts_full, nbsample);
	
	pts.resize(w*h);
	std::vector< uint > spp;
	spp.resize(w*h);
	
	for (int y=0; y<h*w; y++)
	{
		spp[y] = 0;
		pts[y].resize(DEPTH);
		for(int d=0; d<DEPTH; d++)
			pts[y][d].resize(samps);
	}

	for(uint tile0=0; tile0<nbtiles; tile0++)
	for(uint tile1=0; tile1<nbtiles; tile1++)
	{
		for(uint i=0; i<pts_full.size(); i++)
		{
			double x = pts_full[i].pos()[0] * (smax/nbtiles) + tile0*(smax/nbtiles);
			double y = pts_full[i].pos()[1] * (smax/nbtiles) + tile1*(smax/nbtiles);
			
			int xi = x;
			int yi = y;
			
			if(y >= h || x >= w)
				continue;
			
			for(int d=0; d<DEPTH; d++)
			{
				pts[xi*h+yi][d][spp[xi*h+yi]].pos()[0] = x -xi;
				pts[xi*h+yi][d][spp[xi*h+yi]].pos()[1] = y -yi;
			}
			spp[xi*h+yi]+= 1;
		}
	}
	
	for (int y=0; y<h; y++)
	{                       // Loop over image rows
		fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps,100.*y/(h-1));
		unsigned short Xi[3]={0,0,(unsigned short)(y*y*y)};

		for (unsigned short x=0; x<w; x++) 
		{   // Loop cols
			int i = (h-y-1)*w+x;
			r = Vec();

			for (int s=0; s<samps; s++)
			{
				//double r1=2*erand48(Xi), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
				//double r2=2*erand48(Xi), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
			
				double r1 = pts[x*h+y][0][s].pos()[0];//erand48(Xi);
				double r2 = pts[x*h+y][0][s].pos()[1];//erand48(Xi);
				
				Vec d = cx*( (r1 + x)/w - .5) +
							cy*( (r2 + y)/h - .5) + cam.d;
					r = r + radiance(Ray(cam.o+d*140,d.norm()),0,Xi,s, x*h+y)*(1./samps);
			} // Camera rays are pushed ^^^^^ forward to start in interior
				
			c[i] = c[i] + Vec(clamp(r.x),clamp(r.y),clamp(r.z));
			
		}
	}

	FILE *f = fopen("image.ppm", "w");         // Write image to PPM file.
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
	for (int i=0; i<w*h; i++)
	fprintf(f,"%d %d %d ", toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));
}
