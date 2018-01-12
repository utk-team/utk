
/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#include "samplers/utksampler_pixel.h"
//#include "utk_config.h"
#include "paramset.h"
#include "sampling.h"
#include "stats.h"
#include <vector>
#include "../../../../Recherche/UnicornTK2/src/io/fileIO.hpp"

/*****
 * Only runs under the following conditions
 * - Integrator "path"
 * and
 * --nthreads 1
 *****/

/********
 * Total number of 2D projections
 * = image + lens + nbBounce*(light pt + light scatter + bsdf)
 * For 10 bounces, 1+1+10*(1+1+1) = 34
 * For 3 bounces, 1+1+3*(1+1+1) = 11
 * For 2 bounces, 1+1+2*(1+1+1) = 8
 * For 1 bounce, 1+1+1+1+1 = 5

 * Total number of 1D projections
 * = mb + nbBounce*(light choice) + (nbBounces-3)*(russian roulette) -> Russian Roulette kicks in only at third bounce
*********/

/********************************************
 * SAMPLING
 *******************************************/
#define NBBOUNCES 1
#define IMGRES 512
#define NBDIM 2+NBBOUNCES*3
#define FILENAME "pts2.dat"

#define INDEPENDENT_DIMENSIONS
	
namespace pbrt {
	
UtkPixelSampler::UtkPixelSampler(int arg_nPixelSamples,
                      int arg_nSampledDimensions)  : PixelSampler(arg_nPixelSamples, arg_nSampledDimensions),
          nPixelSamples(arg_nPixelSamples)
{
	utk::PointsetReader<NBDIM*2, double, utk::Point<NBDIM*2,double> > reader;
	reader.open(FILENAME);
	reader.readPointset(full_pointset);
	reader.close();
}
	
// 1D Regular Sampling
void UtkPixelSampler::UtkSample1D(Float *samp, int nSamples, RNG &rng) {
    Float invNSamples = (Float)1 / nSamples;
    for (int i = 0; i < nSamples; ++i) {
        Float delta = 0.5f;
        samp[i] = std::min((i + delta) * invNSamples, OneMinusEpsilon);
	}
}

void UtkPixelSampler::readPtsFromFileGlobal(utk::Pointset<2, double, utk::Point<2,double> >& pts, int nSamples, int proj2d, double res_x, double res_y, double px_x, double px_y)
{
	pts.resize(nSamples);
	
	int nbpts = full_pointset.size();
	//std::cout << proj2d << "  Total pts " << nbpts << std::endl;
	
	double coord01_pixel_x = (double)px_x/(double)res_x;
	double coord01_pixel_y = (double)px_y/(double)res_y;
	double pixel_size = 1.0/(double)res_x;
	
	//std::cout << "Pixel " << coord01_pixel_x << "," << coord01_pixel_y << std::endl;
	//std::cout << "Res " << res_x << "," << res_y << std::endl;
	
	double min_coord01_x = coord01_pixel_x;
	double min_coord01_y = coord01_pixel_y;
	double max_coord01_x = (double)(px_x+1)/(double)res_x;
	double max_coord01_y = (double)(px_y+1)/(double)res_y;

	double nbTilesTotal = (double)(nSamples*res_x*res_y) / (double)nbpts;
	assert(res_pts == nbpts/nSamples);
	double tilesize01_x = 1.0/sqrt(nbTilesTotal);
	double tilesize01_y = 1.0/sqrt(nbTilesTotal);
	
	int tilecoord_x = coord01_pixel_x / tilesize01_x;
	int tilecoord_y = coord01_pixel_y / tilesize01_y;
	
	if(px_x == 0 && px_y == 0)
	{
		std::cout << "\nTiled pixel set " << 1.0/tilesize01_x << "," << 1.0/tilesize01_y << " times" << std::endl;
		std::cout << "Each tile has a size of " << tilesize01_x << "," << tilesize01_y << std::endl;
		std::cout << "And contains " << nbpts << " samples" << std::endl;
		std::cout << "Totalling " << nbpts*(1.0/tilesize01_x)*(1.0/tilesize01_x) << " samples in " << nbTilesTotal << "tiles" << std::endl;
		std::cout << "For " << res_x*res_y << " pixels of " << nSamples << " samples each =" << res_x*res_y*nSamples << std::endl;
	}
	
	
#ifdef INDEPENDENT_DIMENSIONS
	int n = 0;
	for(int i=0; i<nbpts; i++)
	{
		double tiled_coord_pts_x = (full_pointset[i].pos()[2*proj2d] + tilecoord_x)*tilesize01_x;
		double tiled_coord_pts_y = (full_pointset[i].pos()[2*proj2d+1] + tilecoord_y)*tilesize01_y;
		
		if(tiled_coord_pts_x >= min_coord01_x && tiled_coord_pts_x < max_coord01_x &&
		tiled_coord_pts_y >= min_coord01_y && tiled_coord_pts_y < max_coord01_y)
		{
			pts[n].pos()[0] = (tiled_coord_pts_x - min_coord01_x)/pixel_size;
			pts[n].pos()[1] = (tiled_coord_pts_y - min_coord01_y)/pixel_size;
			n++;
		}
		if(n==nSamples)
			break;
	}
	
	if(n != nSamples)
	{
		std::cout << "Not enough samples in pixel " << min_coord01_x << "," << min_coord01_y << " for the dimensions " << 2*proj2d << ":" << 2*proj2d+1 << " (" << n << " samples only)" << std::endl;
		exit(1);
	}
	
	/*std::stringstream filename; filename << "utksamples_" << 2*proj2d << "_" << px_x << "_" << px_y << ".dat";
		std::ofstream file(filename.str(), std::ios::app);
	for(int n=0; n<nSamples; n++)
		file << pts[n].pos()[0] << " " << pts[n].pos()[1] << std::endl;
	file.close();*/
	
#elif defined(DEPENDENT_DIMENSIONS)
	if(proj2d == 0)
	{
		int n = 0;
		for(int i=0; i<nbpts; i++)
		{
			double tiled_coord_pts_x = (full_pointset[i].pos()[0] + tilecoord_x)*tilesize01_x;
			double tiled_coord_pts_y = (full_pointset[i].pos()[1] + tilecoord_y)*tilesize01_y;
			
			if(tiled_coord_pts_x >= min_coord01_x && tiled_coord_pts_x < max_coord01_x &&
			tiled_coord_pts_y >= min_coord01_y && tiled_coord_pts_y < max_coord01_y)
			{
				pts[n].pos()[0] = (tiled_coord_pts_x - min_coord01_x)/pixel_size;
				pts[n].pos()[1] = (tiled_coord_pts_y - min_coord01_y)/pixel_size;
				if(n > 256)
				{
					std::cout << "indexPerPixel contains only 256 cells" << std::endl;
					exit(1);
				}
				indexPerPixel[(int)px_x][(int)px_y][n] = i;
				n++;
			}
			if(n==nSamples)
				break;
		}
		
		if(n != nSamples)
		{
			std::cout << "Not enough samples in pixel " << min_coord01_x << "," << min_coord01_y << " (" << n << " samples only)" << std::endl;
			exit(1);
		}
	}
	else
	{
		//if(indexPerPixel[(int)px_x][(int)px_y].size() != nSamples)
		//{
		//	std::cout << "Not enough samples in pixel " << min_coord01_x << "," << min_coord01_y << " (" << indexPerPixel[(int)px_x][(int)px_y].size() << " samples only)" << std::endl;
		//	exit(1);
		//}
			
		for(int n=0; n<nSamples; n++)
		{
			if(indexPerPixel[(int)px_x][(int)px_y][n] >= full_pointset.size())
			{
				std::cout << "Requiring access to unexisting sample no " << indexPerPixel[(int)px_x][(int)px_y][n] << std::endl;
				exit(1);
			}
			if(proj2d >= NBDIM)
			{
				std::cout << "Requiring access to unexisting dimension no " << proj2d << std::endl;
				exit(1);
			}
			int id = indexPerPixel[(int)px_x][(int)px_y][n];
			//std::cout << "proj " << proj2d << " requires acces to pixel " << id << std::endl;
			pts[n].pos()[0] = full_pointset[id].pos()[2*proj2d];
			pts[n].pos()[1] = full_pointset[id].pos()[2*proj2d+1];
		}
	}
	
	/*std::stringstream filename; filename << "utksamples_" << 2*proj2d << "_" << px_x << "_" << px_y << ".dat";
		std::ofstream file(filename.str(), std::ios::app);
	for(int n=0; n<nSamples; n++)
		file << indexPerPixel[(int)px_x][(int)px_y][n] << " " << pts[n].pos()[0] << " " << pts[n].pos()[1] << std::endl;
	file.close();*/
#else
	std::cout << "I don't know how to handle the dimensions !" << std::endl;
	exit (1);
#endif
	
}


void UtkPixelSampler::readPtsFromFileLocal(utk::Pointset<2, double, utk::Point<2,double> >& pts, int nSamples, int proj2d, double res_x, double res_y, double px_x, double px_y)
{
	pts.resize(nSamples);

	for(int n=0; n<nSamples; n++)
	{
		pts[n].pos()[0] = full_pointset[n].pos()[2*proj2d];
		pts[n].pos()[1] = full_pointset[n].pos()[2*proj2d+1];
	}
}

void UtkPixelSampler::readPtsFromFileLocalCP(utk::Pointset<2, double, utk::Point<2,double> >& pts, int nSamples, int proj2d, double res_x, double res_y, double px_x, double px_y)
{
	pts.resize(nSamples);

	double randx = drand48();
	double randy = drand48();
	for(int n=0; n<nSamples; n++)
	{
		pts[n].pos()[0] = full_pointset[n].pos()[2*proj2d]+randx;
		pts[n].pos()[1] = full_pointset[n].pos()[2*proj2d+1]+randy;
		if(pts[n].pos()[0] > 1) pts[n].pos()[0]-=1;
		if(pts[n].pos()[1] > 1) pts[n].pos()[1]-=1;
	}
}



void UtkPixelSampler::UtkSample2D(Point2f *samp, int nSamples, int proj2D, int dim, RNG &rng, const Point2i& p) {
	
	utk::Pointset<2, double, utk::Point<2, double> > pts;
	readPtsFromFileLocal(pts, nSamples, proj2D, IMGRES, IMGRES, p.x, p.y);

	/*if(mapping[proj2D] == STRATIFIED)
		sampler_stratified.generateSamples<2, double, utk::Point<2, double> >(pts, nSamples);
	else if(mapping[proj2D] == REGULAR)
		sampler_reggrid.generateSamples<2, double, utk::Point<2, double> >(pts, nSamples);
	else if(mapping[proj2D] == WHITENOISE)
		sampler_whitenoise.generateSamples<2, double, utk::Point<2, double> >(pts, nSamples);
	else if(mapping[proj2D] == GLOBAL_DATSAMPLER)*/
	
	/*else
		sampler_whitenoise.generateSamples<2, double, utk::Point<2, double> >(pts, nSamples);*/
	/*else if(mapping[proj2D] == LDBN)
		sampler_ldbn.generateSamples<2, double, utk::Point<2, double> >(pts, nSamples);
	else if(mapping[proj2D] == SOBOL)
	{
		int indices[2]; indices[0] = 1; indices[1] = 2;
		sampler_sobol.setIndices(indices, 2);
		sampler_sobol.generateSamples<2, double, utk::Point<2, double> >(pts, nSamples);
	}
	else if(mapping[proj2D] == HALTON)
	{
		uint baseList[2]; baseList[0] = 2; baseList[1] = 3;
		sampler_halton.setBaseList(baseList, 2);
		sampler_halton.generateSamples<2, double, utk::Point<2, double> >(pts, nSamples);
	}
	else if(mapping[proj2D] == FORCEDRANDOM)
	{
		
		if(sampler_precomputed[proj2D].size() == 0)
		{
			sampler_precomputed[proj2D].resize(nSamples);
			std::stringstream filename;
			filename << "/home/logarithme/Desktop/Installed/pbrt-v3-orig/pointsets/FORCEDRANDOM_" << nSamples << ".dat";
			std::ifstream file;
			file.open(filename.str());
			if(!file.is_open())
				std::cout << "[ERROR] File " << filename.str() << " not found" << std::endl;
			
			for(int i=0; i<nSamples; i++)
				file >> sampler_precomputed[proj2D][i].pos()[0] >> sampler_precomputed[proj2D][i].pos()[1];
			file.close();
		}
		
		pts.resize(nSamples);
		for(int i=0; i<nSamples; i++)
			pts[i] = sampler_precomputed[proj2D][i];
	}
	else
		sampler_whitenoise.generateSamples<2, double, utk::Point<2, double> >(pts, nSamples);
	*/
	
	/*if(randomizing[proj2D])
	{
		double cp_x = drand48();//pts[0].pos()[0];//rng.UniformFloat();
		double cp_y = drand48();//pts[0].pos()[1];//rng.UniformFloat();
		
		for(int i=0; i<pts.size(); i++)
		{
			pts[i].pos()[0] += cp_x;
			pts[i].pos()[1] += cp_y;
			pts[i].pos()[0] = fmod( pts[i].pos()[0] , 1.0);
			pts[i].pos()[1] = fmod( pts[i].pos()[1] , 1.0);

			assert(pts[i].pos()[0] >=0);
			assert(pts[i].pos()[0] <1);
			assert(pts[i].pos()[1] >=0);
			assert(pts[i].pos()[1] <1);
		}
		
		if(pts.size() != nSamples) 
		{
			std::cout << "[ERROR] pts.size() != nSamples, the number of samples generated is not the wanted number of samples" << std::endl;
			exit(12345);
		}
	}*/
	
	for (int i = 0; i < nSamples; ++i) {
		samp[i][0] = pts[i].pos()[0];
		samp[i][1] = pts[i].pos()[1];
	}
}

Float UtkPixelSampler::Get1D() {
    ProfilePhase _(Prof::GetSample);
    CHECK_LT(currentPixelSampleIndex, samplesPerPixel);
    if (current1DDimension < samples1D.size())
        return samples1D[current1DDimension++][currentPixelSampleIndex];
    else
	{
		std::cout << "[WARNING] Sampler 1D dimensions set to " << samples1D.size() << ", filling the " << current1DDimension+1 << "th dimension with whitenoise" << std::endl;
        return rng.UniformFloat();
	}
}

Point2f UtkPixelSampler::Get2D() {
    ProfilePhase _(Prof::GetSample);
    CHECK_LT(currentPixelSampleIndex, samplesPerPixel);
	
	if (current2DDimension < samples2D.size())
	{
		/*if(currentPixel.x == 0 && currentPixel.y == 0)
		{
			std::ofstream debugfile;
			debugfile.open("debug.dat", std::ios::app);
			if(current2DDimension == 0)
				debugfile << std::endl;
			debugfile << samples2D[current2DDimension][currentPixelSampleIndex][0] << " " << samples2D[current2DDimension][currentPixelSampleIndex][1] << " ";
			debugfile.close();
		}*/
        return samples2D[current2DDimension++][currentPixelSampleIndex];
	}
    else
	{
		std::cout << "[WARNING] Sampler 2D dimensions set to " << samples1D.size() << ", filling the " << current2DDimension+1 << "th dimension with whitenoise" << std::endl;
        return Point2f(rng.UniformFloat(), rng.UniformFloat());
	}
}

// StratifiedSampler Method Definitions
void UtkPixelSampler::StartPixel(const Point2i &p) {
    
	ProfilePhase _(Prof::StartPixel);
    
	// Generate single stratified samples for the pixel
    for (size_t i = 0; i < samples1D.size(); ++i) {
        UtkSample1D(&samples1D[i][0], nPixelSamples, rng);
		//if(shuffle) Shuffle(&samples1D[i][0], nPixelSamples, 1, rng);
    }
    
    for (size_t i = 0; i < samples2D.size(); ++i) {
		int proj2d = i;
		int dim = i*2;
		/*if(proj2d > 5) 
		{
			proj2d-=2;
			proj2d%=3;
			proj2d+=2;
		}*/
        UtkSample2D(&samples2D[i][0], nPixelSamples, proj2d, dim, rng, p);
        //if(shuffle[proj2d]) Shuffle(&samples2D[i][0], nPixelSamples, 1, rng);
    }

    assert( samples1DArraySizes.size() == 0);
	assert( samples2DArraySizes.size() == 0);
    // Generate arrays of stratified samples for the pixel
    /*for (size_t i = 0; i < samples1DArraySizes.size(); ++i)
        for (int64_t j = 0; j < samplesPerPixel; ++j) {
            int count = samples1DArraySizes[i];
            UtkSample1D(&sampleArray1D[i][j * count], count, rng);
            Shuffle(&sampleArray1D[i][j * count], count, 1, rng);
        }
    for (size_t i = 0; i < samples2DArraySizes.size(); ++i)
        for (int64_t j = 0; j < samplesPerPixel; ++j) {
            int count = samples2DArraySizes[i];
            //LatinHypercube(&sampleArray2D[i][j * count].x, count, 2, rng);
			UtkSample2D(&sampleArray2D[i][j * count], count, rng);
        }*/
    PixelSampler::StartPixel(p);
}

std::unique_ptr<Sampler> UtkPixelSampler::Clone(int seed) {
    UtkPixelSampler *ss = new UtkPixelSampler(*this);
    ss->rng.SetSequence(seed);
    return std::unique_ptr<Sampler>(ss);
}

UtkPixelSampler *CreateUtkPixelSampler(const ParamSet &params) {
	srand48(clock());
    int nsamp = params.FindOneInt("pixelsamples", 4);
    int sd = params.FindOneInt("dimensions", NBDIM);
    if (PbrtOptions.quickRender) nsamp = 1;
    return new UtkPixelSampler(nsamp, sd);
}

}  // namespace pbrt
