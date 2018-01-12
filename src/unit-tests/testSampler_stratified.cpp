#include "../samplers/SamplerStratified.hpp"
#include <cassert>

using namespace utk;

int main(int argc, char** argv)
{
	SamplerStratified s;
	
	//Regular Cases
	{
		Pointset<2, double, Point<2, double> > pts;
		bool reg1=s.generateSamples<2, double, Point<2, double> >(pts, 1024);
		assert(reg1);
	}
	{
		Pointset<3, double, Point<3, double> > pts;
		bool reg2=s.generateSamples<3, double, Point<3, double> >(pts, 1024);
		assert(reg2);
	}
	{
		Pointset<15, double, Point<15, double> > pts;
		bool reg3=s.generateSamples<15, double, Point<15, double> >(pts, 1024);
		assert(reg3);
	}
	{
		Pointset<150, double, Point<150, double> > pts;
		bool reg4=s.generateSamples<150, double, Point<150, double> >(pts, 100);
		assert(reg4);
	}
	
	//Checks
	//toricity
	{
		Pointset<2, double, Point<2, double> > pts;
		s.generateSamples<2, double, Point<2, double>>(pts, 16);
		assert(pts.toricity);
	}
	//all samples in domain
	{
		Pointset<2, double, Point<2, double> > pts;
		s.generateSamples<2, double, Point<2, double> >(pts, 4096);
		
		for(int i=0; i<4096; i++)
		{
			assert(pts[i].pos()[0] < pts.domain.pMax.pos()[0]);
			assert(pts[i].pos()[1] < pts.domain.pMax.pos()[1]);
			
			assert(pts[i].pos()[0] >= pts.domain.pMin.pos()[0]);
			assert(pts[i].pos()[1] >= pts.domain.pMin.pos()[1]);
		}
	}
	//no doubles
	{
		Pointset<2, double, Point<2, double> > pts;
		s.generateSamples<2, double, Point<2, double> >(pts, 1024);
		
		for(int i=0; i<1024; i++)
		for(int j=0; j<1024; j++)
		{
			if(j==i) continue;
			assert(fabs(pts[i].pos()[0] - pts[j].pos()[0]) > 1e-7 || fabs(pts[i].pos()[1] - pts[j].pos()[1]) > 1e-7);
		}
	}
	
	//Failures
	{
		Pointset<2, uint, Point<2, uint> > pts;
		bool fail1=s.generateSamples<2, uint, Point<2, uint> >(pts, 100);
		assert(!fail1);
	}
	
	return 0;
}