/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
#ifndef _TRIANGLE_RANDOM_H_
#define _TRIANGLE_RANDOM_H_

#include "TriangleArrangement.hpp"

namespace utk
{

class TriangleRandom : public TriangleArrangement
{
public:
	virtual void generateTriangles(const unsigned int seed, std::string histogram_lumi)
	{
		srand48(1234);
		//generateTriangles_Random(N, seed, triangles, histogram_lumi, 0.2, 1, 0.01, 0.5*TRIANGLE_MAXSIZE, anisotropy);
		//std::cerr << "N is always set to 50+100+200+1000+2000=3350" << std::endl;
        generateLowFrequencies(drand48()*100, seed, triangles, histogram_lumi);
        generateLowMidFrequencies(drand48()*250, seed, triangles, histogram_lumi);
        generateHighMidFrequencies(drand48()*500, seed, triangles, histogram_lumi);
        //generateHighFrequencies(drand48()*1000, seed, triangles, histogram_lumi);
        //generateVeryHighFrequencies(drand48()*2000, seed, triangles, histogram_lumi);

	}
private:
	
    void generateTriangles_Random(const unsigned int N, const unsigned int seed, std::list<Data_triangle_3> &triangles, std::string histogram_lumi,
								double size, double subpart, double min_depth, double max_depth);
	
	void generateVeryHighFrequencies(const unsigned int N, const unsigned int seed,
							std::list<Data_triangle_3> &triangles, std::string histogram_lumi) {
		double size = 0.01;
		double min_depth = 0.95*TRIANGLE_MAXSIZE;
		double max_depth = TRIANGLE_MAXSIZE;
		//generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 0.5, min_depth, max_depth, anisotropy);
		generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 1, min_depth, max_depth);
	}
	void generateHighFrequencies(const unsigned int N, const unsigned int seed,
								std::list<Data_triangle_3> &triangles, std::string histogram_lumi) {
		double size = 0.02;
		double min_depth = 0.8*TRIANGLE_MAXSIZE;
		double max_depth = 0.9*TRIANGLE_MAXSIZE;
		//generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 0.7, min_depth, max_depth, anisotropy);
		generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 1, min_depth, max_depth);
	}
	void generateHighMidFrequencies(const unsigned int N, const unsigned int seed,
								std::list<Data_triangle_3> &triangles, std::string histogram_lumi) {
		double size = 0.04;
		double min_depth = 0.6*TRIANGLE_MAXSIZE;
		double max_depth = 0.7*TRIANGLE_MAXSIZE;
		//generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 0.8, min_depth, max_depth, anisotropy);
		generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 1, min_depth, max_depth);
	}
	void generateLowMidFrequencies(const unsigned int N, const unsigned int seed,
								std::list<Data_triangle_3> &triangles, std::string histogram_lumi) {
		double size = 0.08;
		double min_depth = 0.3*TRIANGLE_MAXSIZE;
		double max_depth = 0.5*TRIANGLE_MAXSIZE;
		//generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 0.85, min_depth, max_depth, anisotropy);
		generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 1, min_depth, max_depth);
	}
	void generateLowFrequencies(const unsigned int N, const unsigned int seed,
								std::list<Data_triangle_3> &triangles, std::string histogram_lumi) {
		double size = 0.12;
		double min_depth = 0;
		double max_depth = 0.2*TRIANGLE_MAXSIZE;
		//generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 1, min_depth, max_depth, anisotropy);
		generateTriangles_Random(N, seed, triangles, histogram_lumi, size, 1, min_depth, max_depth);
	}
};


/*******************************************/
/******** Triangle Soup generation *********/
/*******************************************/

void TriangleRandom::generateTriangles_Random(const unsigned int N, const unsigned int seed, std::list<Data_triangle_3> &triangles, std::string histogram_lumi,
								double size, double subpart, double min_depth, double max_depth)
{	
	load_histogram(histogram_lumi);
	//load_orientations(-3.14159, 3.14159, 0, anisotropy);

	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distribution(1, TRIANGLE_MAXSIZE-1);
	std::uniform_int_distribution<int> distribution_Z(min_depth, max_depth);

	for(unsigned int i=0; i<N; i+=1)
	{
		float tgl_tag = generateTag();
		
		//first point of the triangle
		double x = distribution(generator)*subpart;
		double y = distribution(generator)*subpart;
		double z = distribution_Z(generator);
		
		//direction to first point
		double theta = drand48()*2*3.14159;//generateAngle();
		//std::cout << theta << std::endl;
		//the second point is 90° ccw or cw from the first
		double ccw = distribution(generator)%2;
		
		Point_3 init = Point_3(x, y, z);
		
		double x0 = x+(cos(theta)*(TRIANGLE_MAXSIZE*size));
		double y0 = y+(sin(theta)*(TRIANGLE_MAXSIZE*size));
		bool mv = backToBounds(x0, y0);
		Point_3 p0 = Point_3(x0, y0, z);
		
		if (!ccw)
			theta += drand48()*3.14159/2.0;
		else
			theta -= drand48()*3.14159/2.0;
		double x1 = x+(cos(theta)*(TRIANGLE_MAXSIZE*size));
		double y1 = y+(sin(theta)*(TRIANGLE_MAXSIZE*size));
		mv = (mv || backToBounds(x1, y1) );
		Point_3 p1 = Point_3(x1, y1, z);
		
		/*std::cout << "init " << x << " " << y << " " << z << std::endl;
		std::cout << "p0 " << x0 << " " << y0 << " " << z << std::endl;
		std::cout << "p1 " << x1 << " " << y1 << " " << z << std::endl;*/	
		
		if(!mv)		
			triangles.push_back (Data_triangle_3 (Triangle_3 (init, p0, p1),
											tgl_tag));
		else
			i--;
	}
}

} //end namespace utk

#endif
