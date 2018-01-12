#include "../pointsets/Pointset.hpp"
#include "../samplers/SamplerWhitenoise.hpp"
#include "../samplers/SamplerHalton.hpp"
#include "../io/fileIO.hpp"

#include <cstdlib>

using namespace utk;

int main(int argc, char** argv)
{
	/********************
	 * REGULAR CASES
	 *******************/

	SamplerWhitenoise whitenoise;
	SamplerHalton halton;
	
	std::cout << "Regular Cases" << std::endl;

	{
	Pointset<2, unsigned int, Point<2, unsigned int> > pts;
	halton.generateSamples<2, unsigned int, Point<2, unsigned int> >(pts, 16);

	PointsetWriter<2, unsigned int, Point<2, unsigned int> > writer;
	writer.open("testIO0.dat");
	writer.writePointset(pts);
	writer.close();

	Pointset<2, unsigned int, Point<2, unsigned int> > pts2;

	PointsetReader<2, unsigned int, Point<2, unsigned int> > reader;
	reader.open("testIO0.dat");
	reader.readPointset(pts2);
	reader.close();

	assert(pts.size() == pts2.size());
	for(unsigned int i=0; i<pts2.size(); i++)
		assert(pts[i] == pts2[i]);
	}

	{
	Pointset<2, char, Point<2, char> > pts;

	for(unsigned int i=0; i<16; i++)
		pts.push_back(Point<2, char>((i%26)+'a', (i%26)+'a'));

	PointsetWriter<2, char, Point<2, char> > writer;
	writer.open("testIO1.dat");
	writer.writePointset(pts);
	writer.close();

	Pointset<2, char, Point<2, char> > pts2;

	PointsetReader<2, char, Point<2, char> > reader;
	reader.open("testIO1.dat");
	reader.readPointset(pts2);
	reader.close();

	std::cout << pts.size() << " " << pts2.size() << std::endl;
	assert(pts.size() == pts2.size());
	for(unsigned int i=0; i<pts2.size(); i++)
		assert(pts[i] == pts2[i]);
	}

	{
	Pointset<2, double,  Point<2, double> > pts;

	whitenoise.generateSamples<2, double, Point<2, double> >(pts, 16);

	PointsetWriter<2, double, Point<2, double> > writer;
	writer.open("testIO2.dat");
	writer.writePointset(pts);
	writer.close();

	Pointset<2, double, Point<2, double> > pts2;

	PointsetReader<2, double, Point<2, double> > reader;
	reader.open("testIO2.dat");
	reader.readPointset(pts2);
	reader.close();

	assert(pts.size() == pts2.size());
	for(unsigned int i=0; i<pts2.size(); i++)
		assert( fabs( (pts[i].pos() - pts2[i].pos()).length() ) < 1e-14);
	}



	/**************************************
	 * FAIL FILE OPEN
	 *************************************/

	std::cout << "Opening incorrect files" << std::endl;

	{
	PointsetReader<2, unsigned int, Point<2, unsigned int> > reader;
	assert(!reader.open("iloveunicorns.dat"));
	}

	{
	PointsetReader<2, unsigned int, Point<2, unsigned int> > reader;
	assert(!reader.open("iloveunicorns.tutu"));
	}

	{
	PointsetWriter<2, unsigned int, Point<2, unsigned int> > writer;
	assert(!writer.open("iloveunicorns.tutu"));
	}


	/************************************
	 * EMPTY POINTSET
	 ***********************************/

	std::cout << "Empty pointset" << std::endl;

	{
		Pointset<3, float, Point<3, float> > pts;

		PointsetWriter<3, float, Point<3, float> > writer;
		writer.open("testIO3.dat");
		writer.writePointset(pts);
		writer.close();

		Pointset<3, float, Point<3, float> > pts2;
		PointsetReader<3, float, Point<3, float> > reader;
		reader.open("testIO3.dat");
		reader.readPointset(pts2);
		reader.close();

		assert(pts.size() == pts2.size() && pts.size() == 0);
	}


	/************************************
	 * SEVERAL POINTSETS
	 ***********************************/

	std::cout << "Several pointsets" << std::endl;

	{
	Pointset<2, double, Point<2, double> > pts[10];

	PointsetWriter<2, double, Point<2, double> > writer;
	writer.open("testIO5.dat");
	for(int p=0; p<10; p++)
	{
		std::cout << "wrote " << p << std::endl;
		whitenoise.generateSamples<2, double, Point<2, double> >(pts[p], 1000);

		writer.writePointset(pts[p]);
	}
	writer.close();

	Pointset<2, double, Point<2, double> > pts2[10];

	PointsetReader<2, double, Point<2, double> > reader;
	reader.open("testIO5.dat");
	int p=0;
	while(reader.readPointset(pts2[p]))
	{
		std::cout << "read " << p << " w " << pts2[p].size() << " samples" << std::endl;
		assert(pts[p].size() == pts2[p].size());
		for(unsigned int i=0; i<pts2[p].size(); i++) {
			assert( fabs( (pts[p][i].pos() - pts2[p][i].pos()).length() ) < 1e-14);
		}
		p++;
	}
	reader.close();
	assert(p==10);
	}


	/************************************
	 * SEVERAL MASSIVE POINTSETS
	 ***********************************/

	std::cout << "Several Massive Pointsets" << std::endl;

	{
	Pointset<2, double,  Point<2, double> > pts[10];

	PointsetWriter<2, double,  Point<2, double> > writer;
	writer.open("testIO5.dat");
	for(int p=0; p<10; p++)
	{
		std::cout << "wrote " << p << std::endl;
		whitenoise.generateSamples<2, double, Point<2, double> >(pts[p], 1000000);

		writer.writePointset(pts[p]);
	}
	writer.close();

	Pointset<2, double,  Point<2, double> > pts2[10];

	PointsetReader<2, double,  Point<2, double> > reader;
	reader.open("testIO5.dat");
	int p=0;
	while(reader.readPointset(pts2[p]))
	{
		std::cout << "read " << p << std::endl;
		assert(pts[p].size() == pts2[p].size());
		for(unsigned int i=0; i<pts2[p].size(); i++) {
			assert( fabs( (pts[p][i].pos() - pts2[p][i].pos()).length() ) < 1e-14);
		}
		p++;
	}
	reader.close();
	assert(p==10);

	}


	/************************************
	 * REUSE READER & WRITER
	 ***********************************/

	std::cout << "Reuse Read & Write" << std::endl;

	{
		Pointset<2, unsigned int, Point<2, unsigned int> > pts;

		halton.generateSamples<2, unsigned int, Point<2, unsigned int> >(pts, 16);

		PointsetWriter<2, unsigned int, Point<2, unsigned int> > writer;
		writer.open("testIO6.dat");
		writer.writePointset(pts);
		writer.close();

		Pointset<2, unsigned int, Point<2, unsigned int> > pts2;

		PointsetReader<2, unsigned int, Point<2, unsigned int> > reader;
		reader.open("testIO6.dat");
		reader.readPointset(pts2);
		reader.close();

		assert(pts.size() == pts2.size());
		for(unsigned int i=0; i<pts2.size(); i++)
			assert(pts[i] == pts2[i]);


		Pointset<2, unsigned int, Point<2, unsigned int> > pts3;
		halton.generateSamples<2, unsigned int, Point<2, unsigned int> >(pts3, 16);
		
		writer.open("testIO6_2.dat");
		writer.writePointset(pts3);
		writer.close();

		Pointset<2, unsigned int, Point<2, unsigned int> > pts4;
		reader.open("testIO6_2.dat");
		reader.readPointset(pts4);
		reader.close();

		assert(pts.size() == pts4.size());
		for(unsigned int i=0; i<pts4.size(); i++)
			assert(pts3[i] == pts4[i]);
	}

	std::cout << "PASSED" << std::endl;
	return 0;
}
