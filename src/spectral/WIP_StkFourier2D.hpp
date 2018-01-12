#ifndef UTK_STKFOURIER2D_HPP
#define UTK_STKFOURIER2D_HPP

#include <cstdlib>
#include <iostream>
#include <string>
#include <complex>
#include <fstream>
#include <vector>

#include <fftw3.h>

#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"
#include "../pointsets/Histogram.hpp"

#include "../../externals/CImg-1.6.9/CImg.h"
using namespace cimg_library;

namespace utk
{

class StkFourier2D
{
public:
	
	Histogram1dd radial_data;
	Histogram2dd spectrum_data;
	Histogram2dd power_spectrum_data;
	
	StkFourier2D(){
		m_domain = 4;
		m_radial_nbbins=200;
	}
	
	void setDomain(double arg_domain) { m_domain = arg_domain; }
	void setNbBinsRadial(uint arg_nbbins) { m_radial_nbbins = arg_nbbins; }
	
	template < uint D, typename T, typename P>
	bool compute(const Pointset<D, T, P>& arg_pts, bool limit_size=0)
	{		
		uint nPts= arg_pts.size();
		
		uint size = (int)round(4.0 * std::sqrt(nPts)) / 4.0;
		if(size > 256 && limit_size)
			size = 256;
		
		uint m_res = m_domain * 2 * size;
		uint m_hires = m_domain * m_res;
		m_hires = 1 << (int)ceil(log2(m_hires));
		int start = (m_hires - m_res) / 2;

		std::cout << "NPts:" << nPts << std::endl;
		std::cout << "FFT Resolution:" << m_hires << std::endl;
		std::cout << "Final Resolution:" << m_res << std::endl;
		
		fftw_complex* fftPrimal = (fftw_complex*) fftw_malloc(m_hires*m_hires*sizeof(fftw_complex));
		fftw_complex* fftDual = (fftw_complex*) fftw_malloc(m_hires*m_hires*sizeof(fftw_complex));
		fftw_plan fftp = fftw_plan_dft_2d(m_hires, m_hires, fftPrimal, fftDual, -1.0, FFTW_MEASURE);
	
		//CImg<double> primalSpace(m_res, m_res, 1);	
		//CImg< std::complex<double> > dualSpace(m_res, m_res, 1);
		//CImg<double> dualSpaceAmp(m_res, m_res, 1);
		//m_dualSpacePow = CImg<double>(m_res, m_res, 1);
		
		//Pointset<D, double, Point<D, double> > arg_pts_normalized = arg_pts.normalizePositions();
		for(uint p=0; p<arg_pts.size(); p++)
		{
			int x = (int)(m_hires*arg_pts[p].pos()[0])%m_hires;
			int y = (int)(m_hires*arg_pts[p].pos()[1])%m_hires;
			fftPrimal[y*m_hires+x][0] = 1;//arg_pts[p].val();
		}
		
		//Compute FFT
		fftw_execute(fftp);
		
		std::complex<double>* dualSpace = (std::complex<double>*) malloc(sizeof(std::complex<double>)*m_res*m_res);
		double* dualSpaceAmp = (double*) malloc(sizeof(double)*m_res*m_res);
		double* dualSpacePow = (double*) malloc(sizeof(double)*m_res*m_res);
		
		for(uint y = 0; y < m_res; y++)
		for(uint x = 0; x < m_res; x++)
		{
			int sx = (x+m_hires/2 + start)%m_hires;
			int sy = (y+m_hires/2 + start)%m_hires;
			int k = (sy) * m_hires + sx;
			
			std::complex<double> val (fftDual[k][0], fftDual[k][1]);
			dualSpace[x+m_res*y] = val;
			dualSpaceAmp[x+m_res*y] = std::abs(val);
			dualSpacePow[x+m_res*y] = pow(std::abs(val), 2)/nPts;
		}
		
		spectrum_data.resize(m_res*m_res);
		power_spectrum_data.resize(m_res*m_res);
		for(uint y = 0; y < m_res; y++)
		for(uint x = 0; x < m_res; x++)
		{
			HistogramNDValue<2, double, double> val;
			val.first[0] = 2*( (double)x/(double)m_res * m_domain ) - m_domain;
			val.first[1] = 2*( (double)y/(double)m_res * m_domain ) - m_domain;
			
			val.second = dualSpaceAmp[x+m_res*y];
			spectrum_data[x+m_res*y] = val;
			
			val.second = dualSpacePow[x+m_res*y];
			power_spectrum_data[x+m_res*y] = val;
		}

		uint nbbins = m_radial_nbbins;
		double max_r = m_res/2.0; //0.5 the side of the picture
		double bin_width = max_r/(nbbins+1); //otherwise, the last bin is outisde the spectrum
		
		//sum of all the values computed inside each bin
		std::vector<double> histo_values(nbbins, 0);
		std::vector<double> histo_count(nbbins, 0);
	
		for(uint i=0; i<m_res; i++)
		for(uint j=0; j<m_res; j++)
		{
			double r, theta;
			carte2polar(i-m_res/2.0, j-m_res/2.0, r, theta);
			
			for(uint bin_id=0; bin_id<nbbins; bin_id++)
			{
				if (r >= bin_id*bin_width && r < (bin_id+1)*bin_width)
				{
					double c = dualSpacePow[j*m_res+i];
					histo_values[bin_id] += c;
					histo_count[bin_id]++;
					break;
				}
			}
		}
	
		radial_data.resize(nbbins-1);
		//double areabin = 0;
		for(uint i=1; i<nbbins; i++)
		{
			double r = i*bin_width;
			HistogramNDValue<1, double, double> val;
			val.first[0]  = (r/max_r * (double)m_domain);
			val.second = histo_values[i]/histo_count[i];
			radial_data[i-1] = val;
		}
		
		free(dualSpace);
		free(dualSpaceAmp);
		free(dualSpacePow);
		
		fftw_free(fftPrimal);
		fftw_free(fftDual);
		
		return true;
	}
	
private:	
	
	double m_domain;
	uint m_radial_nbbins;
	
	void carte2polar(double x, double y, double &r, double &theta)
	{
		r = sqrt(x*x+y*y);
		if (x != 0)
			theta = atan(y/x);
		else
			theta = 0;
	}
	
};

}//end namespace utk

#endif


/*#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include <stk/stk.hpp>
#include <fftw3.h>

namespace boostPO = boost::program_options;

template<typename POS, typename VAL>
void basisRadialMean(
	const stk::BaseDomain<POS>* i_domain,
	const stk::Histogram<2, POS, VAL>& i_spec,
	stk::Histogram<1, POS, VAL>& o_rspec
	)
{
	o_rspec.fill(0);
	
	POS pos;
	int arrayPos;
	
	stk::Histogram<1, POS, int> count(o_rspec.getSize());
	count.fill(0);
	count.setBoundaries(o_rspec.getMinPosition(), o_rspec.getMaxPosition());
	
	for(int i=0; i<i_spec.getArraySize(); i++)
	{
		try
		{
			stk::Vector<2, POS> p = i_spec.getPosFromIndice(i);
			pos = p[0];
			p[0] = p[1];
			p[1] = pos;
			stk::Vector<2, POS> p2 = i_domain->inverseTransform(p)*std::sqrt(i_domain->volume());
			pos = p2.norm();
			count.get(pos)++;
			o_rspec.get(pos) += i_spec.getFromIndice(i);
		}
		catch(stk::exception::OutOfRange& e) { }
	}
	
	for(int i=0; i<o_rspec.getArraySize(); i++)
	{
		o_rspec[i] /= count[i];
	}
}

void make2dChart(
	const std::string& filename,
	const stk::Histogram2dd& data,
	double normMin,
	double normMax,
	bool enableLabel,
	const std::string& title,
	int nPatch,
	int nPts)
{
	#ifdef CAIRO_ENABLED
		stk::io::Chart2d chart;
		chart.add(data);
		chart.setZMin(normMin);
		chart.setZMax(normMax);

		if(enableLabel)
		{
			chart.addLabel(title);

			{
				std::stringstream out;
				out << "Number of patches : " << nPatch;
				chart.addLabel(out.str());
			}
			{
				std::stringstream out;
				out << "Number of points : " << nPts;
				chart.addLabel(out.str());
			}
			{
				std::stringstream out;
				out << "Normalization : " << normMin << " -> " << normMax;
				chart.addLabel(out.str());
			}
			{
				std::stringstream out;
				out << "Maximum value : " << data.getMax(2).at(1);
				chart.addLabel(out.str());
			}
		}
		
		chart.draw(filename);
#else
		stk::io::writePng(filename, data, normMin, normMax);
#endif
}

const int distributionRes = 1000;
const double distributionMax = 5.0;
typedef stk::Vector<distributionRes, double> DiscreteDistribution;

int main(int argc, char** argv)
{
	// ARG PARSER *****************************************************
	std::vector<std::string> fn_input;
	
	std::string fn_fft_amp_png;
	std::string fn_fft_amp_dat;
	std::string fn_fft_pow_png;
	std::string fn_fft_pow_dat;
	std::string fn_rspec_dat;
	std::string fn_rspec_png;
	std::string fn_pspec_dat;
	std::string fn_pspec_png;
	std::string fn_aspec_dat;
	std::string fn_aspec_png;
	
	int hires, res, nPatches, graph_norm;
	double gaussianSigma;
	double domain;
	double subPatch;
	
	//Option list :
	// a..d.f.hi..lmnopqrs..vw...
	// A...........M.O..R...V....
	
	boostPO::variables_map vm;
	boostPO::options_description desc("Allowed options");
	desc.add_options()
		("help,h",
			"produce help message")
		("input,i",
			boostPO::value< std::vector<std::string> >(&fn_input)->composing(),
			".pts filenames of initial distribution")
		("nbFile,n",
			boostPO::value<int>(&nPatches)->default_value(1000000),
			"number of initial distribution spectrum to mean (default: all files specified in --input argument)")
		("fft-amp,o",
			boostPO::value<std::string>(&fn_fft_amp_png),
			"2D fourier spectrum filename (png) : use [0; 2] normalization")
		("odat",
			boostPO::value<std::string>(&fn_fft_amp_dat),
			"2D fourier spectrum filename (dat)")
			
		("fft-pow,O",
			boostPO::value<std::string>(&fn_fft_pow_png),
			"2D fourier power spectrum filename (png) : use [0; 2] normalization")
		("Odat",
			boostPO::value<std::string>(&fn_fft_pow_dat),
			"2D fourier power spectrum filename (dat)")
			
		("rspec,r",
			boostPO::value<std::string>(&fn_rspec_png),
			"radial mean spectrum filename (png)")
		("rdat",
			boostPO::value<std::string>(&fn_rspec_dat),
			"radial mean spectrum filename (dat)")
			
		("pspec,R",
			boostPO::value<std::string>(&fn_pspec_png),
			"radial power spectrum filename (png)")
		("Rdat",
			boostPO::value<std::string>(&fn_pspec_dat),
			"radial power spectrum filename (dat)")

		("aspec,A",
			boostPO::value<std::string>(&fn_aspec_png),
			"radial anisotropy spectrum filename (png)")
		("Adat",
			boostPO::value<std::string>(&fn_aspec_dat),
			"radial anisotropy spectrum filename (dat)")
			
		("domain",
			boostPO::value<double>(&domain)->default_value(4.0),
			"define the domain size in fourier. Multiple of Sqrt(n)")
		("res",
			boostPO::value<int>(&res),
			"spectrum resolution (size), override --domain parameter")
		("fast,f",
			"compute a low quality fourier (spatial resolution roughly the same as spectrum resolution / exactly the same if --exact is specified)")
		("exact",
			"do not round spatial resolution to the next power of two")
		("absolute-coord",
			"Use absolute coordinates for 1D graphs")
		("normalize",
			"Move points that are outside the domain inside the domaine.")
		;
		
	boostPO::positional_options_description p;
	p.add("input", -1);
	
	try
	{	
		boostPO::store(
			boostPO::command_line_parser(argc, argv).
				options(desc).positional(p).run(), vm);
		boostPO::notify(vm);
	}
	catch(boost::program_options::error& e)
	{
		std::cerr << e.what() << "\n";
		std::cout << desc << "\n";
		exit(EXIT_FAILURE);
	}
	
	if(vm.count("help") || fn_input.size() == 0)
	{
		std::cout << desc << "\n";
		exit(EXIT_SUCCESS);
	}
	
	// INIT ***********************************************************
	
	bool is_base_domain = false;

	bool enable_fft_amp_png = (vm.count("fft-amp") || vm.count("all-output"));
	bool enable_fft_amp_dat = (vm.count("odat"));
	bool enable_fft_pow_png = (vm.count("fft-pow") || vm.count("all-output"));
	bool enable_fft_pow_dat = (vm.count("Odat"));
	bool enable_rspec_dat = (vm.count("rdat") || vm.count("all-output"));
	bool enable_rspec_png = (vm.count("rspec") || vm.count("all-output"));
	bool enable_pspec_dat = (vm.count("Rdat") || vm.count("all-output"));
	bool enable_pspec_png = (vm.count("pspec") || vm.count("all-output"));
	bool enable_aspec_dat = (vm.count("Adat") || vm.count("all-output"));
	bool enable_aspec_png = (vm.count("aspec") || vm.count("all-output"));
	
	bool enable_aspec = enable_aspec_png || enable_aspec_dat;
	bool enable_pspec = enable_aspec || enable_pspec_dat || enable_pspec_png;
	bool enable_rspec = enable_rspec_dat || enable_rspec_png;
	
	int nPts;
	if(!vm.count("res"))
	{
		stk::PointSet2dd pts;
		stk::io::read(fn_input[0], pts);
		nPts= pts.size();
		res = (domain * stk::conversion::optimalDFTResolution(pts)) / 4;
	}
	
	double ampNormalization = std::sqrt(nPts*M_PI)/2.0;
	
	hires = res;
	if (!vm.count("fast"))
	{
		hires *= 4;
	}
	if (!vm.count("exact"))
	{
		// round hires to the next power of two
		hires = 1 << (int)ceil(log2(hires));
	}
	int start = (hires - res) / 2;

	std::cout << "FFT Resolution:" << hires << std::endl;
	std::cout << "Final Resolution:" << res << std::endl;

	
	fftw_complex* fftPrimal = (fftw_complex*) fftw_malloc(hires*hires*sizeof(fftw_complex));
	fftw_complex* fftDual = (fftw_complex*) fftw_malloc(hires*hires*sizeof(fftw_complex));
	fftw_plan fftp = fftw_plan_dft_2d(hires, hires, fftPrimal, fftDual, -1.0, FFTW_MEASURE);
	
	for(int i=0; i<hires*hires; i++) fftPrimal[i][1] = 0;

    stk::Histogram2dd primalSpace(stk::Vector2i(res, res));	
	stk::Histogram2dc dualSpace(stk::Vector2i(res, res));	
	stk::Histogram2dd dualSpaceAmp(stk::Vector2i(res, res));
	stk::Histogram2dd dualSpacePow(stk::Vector2i(res, res));
	stk::Histogram2dd dualSpacePowAvg(stk::Vector2i(res, res));
	dualSpacePowAvg.fill(0.0);
	
	stk::BaseDomain<double>* globalBaseDomain = NULL;
	
	nPts = 0;
	int nCurPatches = 0;
	for(int fnIter=0; fnIter<fn_input.size(); fnIter++)
	{
		if(nPatches <= nCurPatches) break;

		stk::io::PointSetInputStream<2, double, double> stream(fn_input[fnIter]);
		do
		{
			// LOADING ************************************************
			
			if(nPatches <= nCurPatches) break;
			nCurPatches++;
			
			//Read pointset
			stk::PointSet2dd pts;
			stream.read(pts);
			nPts += pts.size();
			
			std::cout
				<< "\r" 
				<< (nCurPatches-1) << " point sets analysed"
				<< " (file " << (fnIter+1) << " on " << fn_input.size() << ")"
				<< "           ";
			std::cout.flush();

			//Options on pointset normalization
			if(vm.count("normalize")) pts.normalize();
			if(!vm.count("weight")) for(unsigned int i=0; i<pts.size(); i++) pts.at(i).val() = 1;
						
			// FREQUENCY DOMAIN **************************************
			
			stk::BaseDomain<double>* baseDomain = dynamic_cast<stk::BaseDomain<double>*>(pts.domain());
			if(baseDomain != NULL)
			{
				if(globalBaseDomain == NULL)
				{
					globalBaseDomain = dynamic_cast<stk::BaseDomain<double>*>(baseDomain->clone());
					is_base_domain = true;
				}
				
				for(int j=0; j<pts.size(); j++)
				{
					pts[j].pos() = baseDomain->posInBasis(pts[j].pos());
				}
				pts.unitToroidalDomain();
			}
			else
			{
				stk::RectangularDomain<2, double>* rectangularDomain = dynamic_cast<stk::RectangularDomain<2, double>*>(pts.domain());
				
				if(rectangularDomain != NULL)
				{
					for(int j=0; j<pts.size(); j++)
					{
						pts[j].pos() = (pts[j].pos() - rectangularDomain->boundingBoxMin())/rectangularDomain->boundingBoxSize();
					}
				}
				pts.unitToroidalDomain();
			}
			
			//Discretize
			for(int p=0; p<hires*hires; p++) fftPrimal[p][0] = 0;
			for(int p=0; p<pts.size(); p++)
			{
				int x = (int)(hires*pts[p].pos()[0])%hires;
				int y = (int)(hires*pts[p].pos()[1])%hires;
				fftPrimal[y*hires+x][0] = pts[p].val();
			}
			
			//Compute FFT
			fftw_execute(fftp);
			
			for(int y = 0; y < res; y++)
			{
				for(int x = 0; x < res; x++)
				{
					int sx = (x+hires/2 + start)%hires;
					int sy = (y+hires/2 + start)%hires;
					int k = (sy) * hires + sx;
					dualSpace[y * res + x] = stk::Complexd(fftDual[k][0], fftDual[k][1]);
					//primalSpace[y * res + x] = fftPrimal[k][0]*1000;
				}
			}
			
			//Compute Power with mean and variance
			stk::conversion::abs(dualSpace, dualSpaceAmp);
			stk::fourier::powerSpectrum(pts, dualSpaceAmp, dualSpacePow);
			for(int p=0; p<dualSpacePow.getArraySize(); p++)
			{
				double& histo = dualSpacePow[p];
				double& histoMean = dualSpacePowAvg[p];
				double delta = histo - histoMean;
				histoMean += delta/(double)nCurPatches;
			}
			
			//make2dChart(
			//"primal.png", primalSpace,
			//0.0, 2.0,
			//vm.count("labels"), "Pointset", nPatches, nPts);
		}
		while(stream.next());
	}
	
	fftw_destroy_plan(fftp);
	fftw_free(fftPrimal);
	fftw_free(fftDual);
	
	
	nPatches = nCurPatches;
	
	std::cout
		<< "\r" 
		<< nCurPatches << " point sets analysed"
		<< "                        " << std::endl;

	// OUTPUT *********************************************************

	if(is_base_domain)
	{
		stk::Histogram2dd dualSpacePowAvg_tmp = dualSpacePowAvg;

		stk::Vector2d matInit0 = globalBaseDomain->vector(0);
		stk::Vector2d matInit1 = globalBaseDomain->vector(1);

		double ang = -atan(matInit1[0]/matInit1[1]);
		stk::Vector2d matRot0(cos(ang), -sin(ang)), matRotInv0;
		stk::Vector2d matRot1(sin(ang), cos(ang)), matRotInv1;
		double det = matRot0[0]*matRot1[1] - matRot0[1]*matRot1[0];
		if(std::abs(det) > 1e-30)
		{
			matRotInv0[0] = matRot1[1]/det;
			matRotInv1[0] = -matRot0[1]/det;
			matRotInv0[1] = -matRot1[0]/det;
			matRotInv1[1] = matRot0[0]/det;
		}
		stk::Vector2d mat0, mat1;
		mat0[0] =(matInit0*matRotInv0).total() / matInit0[0];
		mat0[1] =(matInit0*matRotInv1).total() / matInit0[0];
		mat1[0] =(matInit1*matRotInv0).total() / matInit0[0];
		mat1[1] =(matInit1*matRotInv1).total() / matInit0[0];

		if(vm.count("print-matrix"))
		{
			std::cout << "Matrix:" << std::endl;
			std::cout << mat0[0] << "\t" << mat0[1] << std::endl;
			std::cout << mat1[0] << "\t" << mat1[1] << std::endl;
		}

		stk::Vector2d coord, newCoord;
		for(int i=0; i<dualSpacePowAvg.getArraySize(); i++)
		{
			double halfsz = dualSpacePowAvg.getSize()[0]/2.;
			coord = dualSpacePowAvg.getPosFromIndice(i) - stk::Vector2d(halfsz);
			newCoord[0] = (mat0*coord).total() + halfsz;
			newCoord[1] = (mat1*coord).total() + halfsz;
			dualSpacePowAvg[i] = dualSpacePowAvg_tmp.getFromPosInterpolate(newCoord);
		}
	}
	
	//Define space boundaries
	nPts /= nPatches;
	if(vm.count("absolute-coord"))
	{
		dualSpacePowAvg.setBoundaries(-res / 2, res / 2 - 1);
	}
	else
	{
		double fact = std::sqrt((double)nPts);
		dualSpacePowAvg.setBoundaries(-res / (2 * fact), (res / 2 - 1) / fact);
		
	}
	
	ampNormalization = std::sqrt(nPts*M_PI)/2.0;
	double ampVarNormalization = sqrt(nPts - (nPts*M_PI)/4.0f);
	double base_fact = 1.0;
	if(is_base_domain) base_fact = 0.9;
	
	// OUTPUT POW  ****************************************************
	if(enable_fft_pow_png)
	{
		make2dChart(
			fn_fft_pow_png, dualSpacePowAvg,
			0.0, 2.0,
			vm.count("labels"), "Power spectrum", nPatches, nPts);
	}
	if(enable_fft_pow_dat)
	{
		stk::io::writeDat(fn_fft_pow_dat, dualSpacePowAvg);
	}
	
	// RSPEC / PSPEC / ASPEC ******************************************

	if(enable_pspec)
	{
		stk::Histogram1dd pspec(floor(res*base_fact/2));
		pspec.setBoundaries(0, dualSpacePowAvg.getMaxPosition()*base_fact);
		stk::conversion::radialMean(dualSpacePowAvg, pspec);
		
#ifdef CAIRO_ENABLED
		if(enable_pspec_png)
		{
			//~ stk::Histogram1dd pspecVar(floor(res*base_fact/2));
			//~ pspecVar.setBoundaries(0, dualSpacePowVar.getMaxPosition()*base_fact);
			//~ stk::conversion::radialMean(dualSpacePowVar, pspecVar);

			stk::io::Chart pspecChart;
			
			pspecChart.add(pspec, stk::io::Chart::LineStyle()
				.strokeColor(stk::Vector4d(70.0/255.0, 68.0/255.0, 157.0/255.0, 1.0))
				.filling(true)
			);
			
			//~ pspecChart.add(pspecVar, stk::io::Chart::LineStyle()
				//~ .strokeColor(stk::Vector4d(70.0/255.0, 68.0/255.0, 157.0/255.0, 0.7))
				//~ .dash(true)
			//~ );
			
			pspecChart.setYMin(0.0);
			pspecChart.setYMax(3.0);
			pspecChart.setXLabel("frequency");
			pspecChart.setYLabel("power");

			if(vm.count("absolute-coord")) pspecChart.addVLine(std::sqrt(nPts));
			else pspecChart.addVLine(1.0);
			pspecChart.addVLine(1.074569932, stk::io::Chart::LineStyle()
				.dash(true)
			);
			
			pspecChart.addHLine(1.0);
			
			pspecChart.draw(fn_pspec_png, stk::Vector2i(1024, 256+128));
		}
#endif

		if(enable_pspec_dat)
		{
			stk::io::writeDat(fn_pspec_dat, pspec);
		}
	
		if(enable_aspec && nPatches > 1)
		{
			stk::Histogram1dd aspec(floor(res*base_fact/2));
			aspec.setBoundaries(0, dualSpacePowAvg.getMaxPosition()*base_fact);
			stk::conversion::anisotropy(dualSpacePowAvg, pspec, aspec);
			
			if(enable_aspec_png)
			{
#ifdef CAIRO_ENABLED
				stk::io::Chart aspecChart;
				aspecChart.add(aspec, stk::io::Chart::LineStyle()
					.strokeColor(stk::Vector4d(70.0/255.0, 68.0/255.0, 157.0/255.0, 1.0))
				);

				
				double maxVal = 10*std::log10(nPatches);

				double fact=1.5;//10;
				aspecChart.setYMin(-fact*maxVal);
				aspecChart.setYMax(+fact*maxVal);

				aspecChart.setXLabel("frequency");
				aspecChart.setYLabel("anisotropy");
			
				if(vm.count("absolute-coord")) aspecChart.addVLine(sqrt(nPts));
				else aspecChart.addVLine(1.0);
				aspecChart.addVLine(1.074569932, stk::io::Chart::LineStyle()
					.dash(true)
				);

				aspecChart.addHLine(0);
				aspecChart.addHLine(-maxVal, stk::io::Chart::LineStyle()
					.dash(true)
				);

				aspecChart.draw(fn_aspec_png, stk::Vector2i(1024, 256+128));
#endif
			}
			if(enable_aspec_dat)
			{
				stk::io::writeDat(fn_aspec_dat, aspec);
			}
		}
	}
	
	if(globalBaseDomain != NULL) delete globalBaseDomain;
	

	exit(EXIT_SUCCESS);
}
*/