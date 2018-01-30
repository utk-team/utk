#include "../src/samplers/SamplerSobolIndices.hpp"
#include <assert.h>

using namespace utk;

int main(int argc, char** argv)
{
	SamplerSobolIndices s;
	
	//Regular Cases
	{
		Pointset<2, double, Point<2, double> > pts;
		bool reg1 = s.generateSamples<2, double, Point<2, double>>(pts, 1024);
		assert(reg1);
	}
	{
		Pointset<3, double, Point<3, double> > pts;
		bool reg2 = s.generateSamples<3, double, Point<3, double>>(pts, 1024);
		assert(reg2);
	}
	{
		int indices[15];
		for(int i=0; i<15; i++) indices[i] = ((i+5)%15)+1;
		s.setIndices(indices, 15);
		Pointset<15, double, Point<15, double> > pts;
		bool reg3 = s.generateSamples<15, double, Point<15, double> >(pts, 1024);
		assert(reg3);
	}
	{
		Pointset<2, double, Point<2, double> > pts;
		bool reg1 = s.generateSamplesGrayCode<2, double, Point<2, double>>(pts, 1024);
		assert(reg1);
	}
	{
		Pointset<3, double, Point<3, double> > pts;
		bool reg2 = s.generateSamplesGrayCode<3, double, Point<3, double>>(pts, 1024);
		assert(reg2);
	}
	{
		int indices[15];
		for(int i=0; i<15; i++) indices[i] = ((i+5)%15)+1;
		s.setIndices(indices, 15);
		Pointset<15, double, Point<15, double> > pts;
		bool reg3 = s.generateSamplesGrayCode<15, double, Point<15, double> >(pts, 1024);
		assert(reg3);
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
		bool check00=s.generateSamples<2, double, Point<2, double> >(pts, 4096);
		assert(check00);
		
		for(int i=0; i<4096; i++)
		{
			assert(pts[i].pos()[0] < pts.domain.pMax.pos()[0]);
			assert(pts[i].pos()[1] < pts.domain.pMax.pos()[1]);
			
			assert(pts[i].pos()[0] >= pts.domain.pMin.pos()[0]);
			assert(pts[i].pos()[1] >= pts.domain.pMin.pos()[1]);
		}
	}
	//all samples in domain uint
	{
		Pointset<2, uint, Point<2, uint> > pts;
		bool check0=s.generateSamples<2, uint, Point<2, uint> >(pts, 4000);
		assert(check0);
		
		for(int i=0; i<4000; i++)
		{
			assert(pts[i].pos()[0] < pts.domain.pMax.pos()[0]);
			assert(pts[i].pos()[1] < pts.domain.pMax.pos()[1]);
			
			assert(pts[i].pos()[0] >= pts.domain.pMin.pos()[0]);
			assert(pts[i].pos()[1] >= pts.domain.pMin.pos()[1]);
		}
	}
	//no doubles
	{
		Pointset<2, uint, Point<2, uint> > pts;
		bool check1=s.generateSamples<2, uint, Point<2, uint> >(pts, 1024);
		assert(check1);
		
		for(int i=0; i<1024; i++)
		for(int j=0; j<1024; j++)
		{
			if(j==i) continue;
			assert(pts[i].pos()[0] != pts[j].pos()[0]);
			assert(pts[i].pos()[1] != pts[j].pos()[1]);
		}
	}
	//proper indices
	{
		int indices[5];
		for(int i=0; i<5; i++) 
			indices[i] = i+1;
		
		s.setIndices(indices, 5);
		Pointset<5, uint, Point<5, uint> > pts_1;
		bool check2=s.generateSamples<5, uint, Point<5, uint> >(pts_1, 10);
		assert(check2);

		for(int i=0; i<5; i++) 
			indices[i] = ((i+3)%5)+1;
		s.setIndices(indices, 5);
		Pointset<5, uint, Point<5, uint> > pts_2;
		bool check3=s.generateSamples<5, uint, Point<5, uint> >(pts_2, 10);
		assert(check3);
		
		for(int i=0; i<10; i++)
		{
			for(int id=0; id<5; id++)
				assert(pts_1[i].pos()[id] == pts_2[i].pos()[(id+2)%5]);
		}
	}
	//toricity
	{
		Pointset<2, double, Point<2, double> > pts;
		s.generateSamplesGrayCode<2, double, Point<2, double>>(pts, 16);
		assert(pts.toricity);
	}
	//all samples in domain
	{
		Pointset<2, double, Point<2, double> > pts;
		bool check00=s.generateSamplesGrayCode<2, double, Point<2, double> >(pts, 4096);
		assert(check00);
		
		for(int i=0; i<4096; i++)
		{
			assert(pts[i].pos()[0] < pts.domain.pMax.pos()[0]);
			assert(pts[i].pos()[1] < pts.domain.pMax.pos()[1]);
			
			assert(pts[i].pos()[0] >= pts.domain.pMin.pos()[0]);
			assert(pts[i].pos()[1] >= pts.domain.pMin.pos()[1]);
		}
	}
	//all samples in domain uint
	{
		Pointset<2, uint, Point<2, uint> > pts;
		bool check0=s.generateSamplesGrayCode<2, uint, Point<2, uint> >(pts, 4000);
		assert(check0);
		
		for(int i=0; i<4000; i++)
		{
			assert(pts[i].pos()[0] < pts.domain.pMax.pos()[0]);
			assert(pts[i].pos()[1] < pts.domain.pMax.pos()[1]);
			
			assert(pts[i].pos()[0] >= pts.domain.pMin.pos()[0]);
			assert(pts[i].pos()[1] >= pts.domain.pMin.pos()[1]);
		}
	}
	//no doubles
	{
		Pointset<2, uint, Point<2, uint> > pts;
		bool check1=s.generateSamplesGrayCode<2, uint, Point<2, uint> >(pts, 1024);
		assert(check1);
		
		for(int i=0; i<1024; i++)
		for(int j=0; j<1024; j++)
		{
			if(j==i) continue;
			assert(pts[i].pos()[0] != pts[j].pos()[0]);
			assert(pts[i].pos()[1] != pts[j].pos()[1]);
		}
	}
	//proper indices
	{
		int indices[5];
		for(int i=0; i<5; i++) 
			indices[i] = i+1;
		
		s.setIndices(indices, 5);
		Pointset<5, uint, Point<5, uint> > pts_1;
		bool check2=s.generateSamplesGrayCode<5, uint, Point<5, uint> >(pts_1, 10);
		assert(check2);

		for(int i=0; i<5; i++) 
			indices[i] = ((i+3)%5)+1;
		s.setIndices(indices, 5);
		Pointset<5, uint, Point<5, uint> > pts_2;
		bool check3=s.generateSamplesGrayCode<5, uint, Point<5, uint> >(pts_2, 10);
		assert(check3);
		
		for(int i=0; i<10; i++)
		{
			for(int id=0; id<5; id++)
				assert(pts_1[i].pos()[id] == pts_2[i].pos()[(id+2)%5]);
		}
	}

	//Failures
	//wrong indices
	{
		int indices[5];
		for(int i=0; i<5; i++) indices[i] = -1*i -1;
		s.setIndices(indices, 5);
		Pointset<5, double, Point<5, double> > pts_1;
		bool fail1=s.generateSamples<5, double, Point<5, double> >(pts_1, 1024);
		assert(!fail1);
	}
	{
		int indices[5];
		for(int i=0; i<5; i++) indices[i] = 0;
		s.setIndices(indices, 5);
		Pointset<5, double, Point<5, double> > pts_1;
		bool fail2=s.generateSamples<5, double, Point<5, double> >(pts_1, 1024);
		assert(!fail2);
	}
	//not enough indices
	{
		Pointset<150, double, Point<150, double> > pts;
		bool fail3=s.generateSamples<150, double, Point<150, double> >(pts, 1024);
		assert(!fail3);
	}
	//wrong indices
	{
		int indices[5];
		for(int i=0; i<5; i++) indices[i] = -1*i -1;
		s.setIndices(indices, 5);
		Pointset<5, double, Point<5, double> > pts_1;
		bool fail1=s.generateSamplesGrayCode<5, double, Point<5, double> >(pts_1, 1024);
		assert(!fail1);
	}
	{
		int indices[5];
		for(int i=0; i<5; i++) indices[i] = 0;
		s.setIndices(indices, 5);
		Pointset<5, double, Point<5, double> > pts_1;
		bool fail2=s.generateSamplesGrayCode<5, double, Point<5, double> >(pts_1, 1024);
		assert(!fail2);
	}
	//not enough indices
	{
		Pointset<150, double, Point<150, double> > pts;
		bool fail3=s.generateSamplesGrayCode<150, double, Point<150, double> >(pts, 1024);
		assert(!fail3);
	}
	
	return 0;
}
