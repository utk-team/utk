#ifndef _UTK_FOURIER3D_
#define _UTK_FOURIER3D_

#include <iostream>
#include <random>
#include <complex>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/math/RealFFT.h>
#include <DGtal/images/ImageContainerBySTLVector.h>
#include <DGtal/io/writers/RawWriter.h>
#include "../pointsets/Pointset.hpp"

using namespace DGtal;
using namespace Z3i;

namespace utk {

class Fourier3D
{
public:
	Fourier3D() {};
	~Fourier3D() {};
	
	template <unsigned int D, typename T, typename P>
	bool compute(Pointset<P>& pts, std::string arg_name="fourier3d.raw") {
		
		//trace.beginBlock("Running DGtal Fourier");
		std::cout << "Generated a file " << arg_name << std::endl;
		
		const unsigned int N=pts.size();
		const unsigned int Size=static_cast<unsigned int>(4.0*std::floor(cbrt(N)));
		
		std::cout << "From " << N << " points" << std::endl;
		
		//trace.info() << "Size= "<<Size<<std::endl;
		
		DGtal::Z3i::Domain domain(DGtal::Z3i::Point(0,0,0), DGtal::Z3i::Point(Size-1,Size-1,Size-1));
		RealFFT<DGtal::Z3i::Domain, double> fft( domain );
		typedef RealFFT<DGtal::Z3i::Domain,double>::RealPoint RealPoint;
		
		auto spatial = fft.getSpatialImage();
		
		//Scale factors, we sample [0,1]^2
		fft.setScaledSpatialExtent(RealPoint::diagonal(cbrt(N)));
		fft.setScaledSpatialLowerBound(RealPoint::diagonal(0.0));
		
		//adding some points
		/*std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0,Size-2);
		std::fill( spatial.begin(), spatial.end(), 0. );
		for (auto i=0; i< N; ++i)
		{
			Point p(dis(gen), dis(gen));
			spatial.setValue( p, 1.0);
		}
		trace.info() << spatial<<std::endl;*/
		
		for(int i=0; i<pts.size(); i++)
		{
			DGtal::Z3i::Point p(pts[i].pos()[0]*(Size-1), pts[i].pos()[1]*(Size-1), pts[i].pos()[2]*(Size-1));
			//std::cout << p << std::endl;
			spatial.setValue(p, 1.0);
		}
		
		//compute the FFT
		//@todo Flags on the FFT PLAN
		fft.forwardFFT();
		const auto freqImage= fft.getFreqImage();
		
		//Values at DC
		
		bool apply_conj;
		const auto aPoint = fft.calcNativeFreqCoords( RealPoint(0.0,0.0,0.0) , apply_conj );
		//trace.info()<<" Value at DC (raw)=" << freqImage( aPoint  )<< std::endl;
		
		
		RealFFT<DGtal::Z3i::Domain,  double>::Complex DC= fft.getScaledFreqValue(RealPoint(0.0,0.0,0.0) );
		trace.info() << "Value at DC= "<<DC<<std::endl;
		trace.info() << "Value from freq = "<< freqImage( DGtal::Z3i::Point::diagonal(0))<<std::endl;
		
		// Complex -> long double (puissance)
		// writers rawWriter double
		ImageContainerBySTLVector<DGtal::Z3i::Domain, double> image(domain);
		//trace.info() << freqImage<<std::endl;

		for(auto it: image.domain())
		{
			RealPoint p = (it);
			p -= RealPoint::diagonal(Size/2.0);
			p /= (double)(cbrt(N));
			image.setValue(it, std::norm( fft.getScaledFreqValue( p ) ) );
			//std::cout << std::norm( fft.getScaledFreqValue( p ) ) << std::endl;
		}
		
		std::cout << "The File Pattern is : %s" << std::endl;
		std::cout << "The Data Scalar Type is : double" << std::endl;
		std::cout << "The Data Byte Order is : BigEndian" << std::endl;
		std::cout << "The File Dimensionality is : 3" << std::endl;
		std::cout << "The Data Origin is : 0 0 0" << std::endl;
		std::cout << "The Data Specing is : 1 1 1" << std::endl;
		std::cout << "The Data Extent is : " << 0 << " " << Size-1 << std::endl;
		
		//trace.endBlock();
		
		RawWriter<ImageContainerBySTLVector<DGtal::Z3i::Domain, double> >::exportRaw<double>(arg_name,  image);
		
		return true;
	}
};

}//end namespace utk

#endif