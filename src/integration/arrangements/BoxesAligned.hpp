#ifndef _BOXES_ALIGNED_H_
#define _BOXES_ALIGNED_H_

#include "TriangleArrangement.hpp"

namespace utk
{
	
class BoxesAligned: public TriangleArrangement
{
public:
	virtual void generateTriangles(const unsigned int seed, std::string histogram_lumi)
	{
		srand48(1234);
		triangles.clear();
		load_histogram(histogram_lumi);
		//load_orientations(0, 3.14159, 0, 0.1);
		
		generateVeryLowFreq(drand48()*200, seed);
		generateLowFreq(drand48()*1000, seed);
		generateMidFreq(drand48()*2000, seed);
		generateHighFreq(drand48()*4000, seed);
		//generateVeryHighFreq(drand48()*8000, seed);
	}
private:
	void generateVeryLowFreq(const uint N, const unsigned int seed)
	{
		double s = 1/sqrt(N)*0.75;
		generateBoxesRandom(N, seed, 0.1*TRIANGLE_MAXSIZE, 0.2*TRIANGLE_MAXSIZE, s*TRIANGLE_MAXSIZE);
	}
	void generateLowFreq(const uint N, const unsigned int seed)
	{
		double s = 1/sqrt(N)*0.75;
		generateBoxesRandom(N, seed, 0.2*TRIANGLE_MAXSIZE, 0.3*TRIANGLE_MAXSIZE, s*TRIANGLE_MAXSIZE);
	}
	void generateMidFreq(const uint N, const unsigned int seed)
	{
		double s = 1/sqrt(N)*0.75;
		generateBoxesRandom(N, seed, 0.3*TRIANGLE_MAXSIZE, 0.4*TRIANGLE_MAXSIZE, s*TRIANGLE_MAXSIZE);
	}
	void generateHighFreq(const uint N, const unsigned int seed)
	{
		double s = 1/sqrt(N)*0.75;
		generateBoxesRandom(N, seed, 0.4*TRIANGLE_MAXSIZE, 0.5*TRIANGLE_MAXSIZE, s*TRIANGLE_MAXSIZE);
	}
	void generateVeryHighFreq(const uint N, const unsigned int seed)
	{
		double s = 1/sqrt(N)*0.75;
		generateBoxesRandom(N, seed, 0.5*TRIANGLE_MAXSIZE, 0.6*TRIANGLE_MAXSIZE, s*TRIANGLE_MAXSIZE);
	}
	
	void generateBoxesRandom(const uint N, const unsigned int seed, const float depth_min, const float depth_max, const float size);
};

/*******************************************/
/******** Triangle Soup generation *********/
/*******************************************/

void BoxesAligned::generateBoxesRandom(const uint N, const unsigned int seed, const float depth_min, const float depth_max, const float size)
{
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distribution(1, (TRIANGLE_MAXSIZE-1));
	
	double N_side = sqrt(N);

	double tgl_size = size;
	std::uniform_int_distribution<int> distribution_Z(depth_min, depth_max);
	
	//for(double i=0; i<N_side; i++)
	//for(double j=0; j<N_side; j++)
	for(double i=0; i<N; i++)	
	{
		bool to_generate = distribution(generator)%5;
		if (to_generate == 0)
			continue;
		
		float tgl_tag = generateTag();
		
		//The upper left corner of the box
		/*jittered value from the grid corner*/
		/*Point_3 init = Point_3( (double)distribution(generator)/(double)(TRIANGLE_MAXSIZE-1)*(TRIANGLE_MAXSIZE/(3*N_side)) + (i/N_side * TRIANGLE_MAXSIZE)+1, 
								(double)distribution(generator)/(double)(TRIANGLE_MAXSIZE-1)*(TRIANGLE_MAXSIZE/(3*N_side)) + (j/N_side * TRIANGLE_MAXSIZE)+1, 
								distribution_Z(generator) );*/
		Point_3 init = Point_3( (double)distribution(generator), 
								(double)distribution(generator), 
								distribution_Z(generator) );
		//The size of the box, deopending on the resolution of the underlying grid and a jittering factor
		double current_size = (double)distribution(generator)/(double)(TRIANGLE_MAXSIZE-1)*tgl_size*0.25 + tgl_size;
	
		//A random jitering of the box to avoid too obvious structure
		float theta = drand48()*0.01;//generateAngle();
		Point_3 vect = Point_3 ( cos(theta), sin(theta), 0); //unitary vector poiting to the lower right corner
		
		//compute bottom left corner
		double x,y,z;
		x = (init.x() + current_size*vect.x()).to_double();
		y = (init.y() + current_size*vect.y()).to_double();
		z = init.z().to_double();
		backToBounds(x, y);
		Point_3 diff_0 = Point_3 (x, y, z);
		
		//jiterin the box again
		theta =  (3.14159/2.0) + drand48()*0.01;//generateAngle();
		
		//compute bottom left corner
		x = init.x().to_double() + current_size*(cos(theta));
		y = init.y().to_double() + current_size*(sin(theta));
		z = init.z().to_double();
		backToBounds(x, y);
		Point_3 diff_1 = Point_3 (x, y, z);
		
		//compute upper right corner
		x = (diff_1.x() + current_size*vect.x()).to_double();
		y = (diff_1.y() + current_size*vect.y()).to_double();
		z = diff_1.z().to_double();
		backToBounds(x, y);
		Point_3 diff_2 = Point_3 (x, y, z);

		triangles.push_back (Data_triangle_3 (Triangle_3 (init, diff_0, diff_1),
											tgl_tag));
		triangles.push_back (Data_triangle_3 (Triangle_3 (diff_2, diff_0, diff_1),
											tgl_tag));
	}
}

}

#endif