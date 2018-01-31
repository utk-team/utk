#include "../../externals/CImg-1.6.9/CImg.h"
#include "../parameters/ParamParser_getopt.hpp"

using namespace utk;
using namespace cimg_library;

int main(int argc, char** argv)
{
	srand48(time(NULL));
	
	//PARSE PARAM
	ParamParser_getopt parser;
	std::string param_image;
	parser.addShortOption('i', &param_image, 1, assignString, displayString, (char*)"<string> the image to halftone", (char*)"Image");
	std::string param_output;
	parser.addShortOption('o', &param_output, 1, assignString, displayString, (char*)"<string> the output halftoned image", (char*)"Output");
	uint param_sizematrix = 8;
	parser.addShortOption('s', &param_sizematrix, 1, assignUInt, displayUInt, (char*)"<uint=8> the size of the threshold matrix", (char*)"Matrix size");
	parser.parse(argc, argv);
	
	if(param_image.empty())
	{
		ERROR("Parameter -i mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	if(param_output.empty())
	{
		ERROR("Parameter -o mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	
	double** random_matrix;
	random_matrix = new double*[param_sizematrix];
	for(uint i=0; i<param_sizematrix; i++)
		random_matrix[i] = new double[param_sizematrix];

	for(int i=0; i<param_sizematrix; i++)
	for(int j=0; j<param_sizematrix; j++)
		random_matrix[i][j] = drand48()*255.0;
	
	CImg<unsigned char> matrix(param_sizematrix, param_sizematrix, 1, 1);
	for(uint i=0; i<param_sizematrix; i++)
	for(uint j=0; j<param_sizematrix; j++)
		matrix.atXYZC(i, j, 0, 1) = random_matrix[i][j];
	matrix.save("randommatrix_128.png");

	CImg<unsigned char> m_image = CImg<unsigned char>(param_image.c_str());
	uint m_imgheight = m_image.height();
	uint m_imgwidth = m_image.width();
	uint m_imgchannels = m_image.spectrum();

	/*Convert to BW*/
	for(uint i=0; i<m_imgwidth; i++)
	for(uint j=0; j<m_imgheight; j++)
	{
		int bw_value=0;
		for(uint c=0; c<m_imgchannels; c++)
			bw_value += m_image.atXYZC(i, j, 0, c);
		
		bw_value /= m_imgchannels;

		double threshold = random_matrix[i%param_sizematrix][j%param_sizematrix];
		if(bw_value > threshold)
			for(uint c=0; c<m_imgchannels; c++)
				m_image.atXYZC(i, j, 0, c) = 255;
		else
			for(uint c=0; c<m_imgchannels; c++)
				m_image.atXYZC(i, j, 0, c) = 0;
	}

	m_image.save(param_output.c_str());
	
	return 0;
}