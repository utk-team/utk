
#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_SAMPLERS_UTKPIXEL_H
#define PBRT_SAMPLERS_UTKPIXEL_H

// samplers/stratified.h*
#include "sampler.h"
#include "rng.h"
#include "../../../../Recherche/UnicornTK2/src/pointsets/Pointset.hpp"

namespace pbrt {

/*******
 * Those values need to be manually set
 * into the .pbrt file to match the ones entered here
 * and those in the file utk_config.h
 *
 * The file pts.dat must contains points
 * in NBDIM*2 dimensions = 2*(2+#define NBBOUNCES 1
 */
#define NBBOUNCES 1
#define IMGRES 512
#define NBDIM 2+NBBOUNCES*3
#define FILENAME "pts2.dat"

// UtkPixelSampler Declarations
class UtkPixelSampler : public PixelSampler {
  public:
    // UtkPixelSampler Public Methods
    UtkPixelSampler(int arg_nPixelSamples,
                      int arg_nSampledDimensions);
    void StartPixel(const Point2i &);
    std::unique_ptr<Sampler> Clone(int seed);
	Point2f Get2D();
	Float Get1D();

  private:
    const int nPixelSamples;
	int indexPerPixel[IMGRES][IMGRES][256];
	utk::Pointset<NBDIM*2, double, utk::Point<NBDIM*2,double> > full_pointset;
	
	void UtkSample1D(Float *samp, int nSamples, RNG &rng);
	void readPtsFromFileGlobal(utk::Pointset<2, double, utk::Point<2,double> >& pts, int nSamples, int proj2d, double res_x, double res_y, double px_x, double px_y);
	void readPtsFromFileLocal(utk::Pointset<2, double, utk::Point<2,double> >& pts, int nSamples, int proj2d, double res_x, double res_y, double px_x, double px_y);
	void readPtsFromFileLocalCP(utk::Pointset<2, double, utk::Point<2,double> >& pts, int nSamples, int proj2d, double res_x, double res_y, double px_x, double px_y);
	void UtkSample2D(Point2f *samp, int nSamples, int proj2D, int dim, RNG &rng, const Point2i& p);

};

UtkPixelSampler *CreateUtkPixelSampler(const ParamSet &params);

}  // namespace pbrt

#endif  // PBRT_SAMPLERS_UTKPIXEL_H

