#include "../../../externals/CImg-1.6.9/CImg.h"
#include "../../parameters/ParamParser_getopt.hpp"

using namespace utk;
using namespace cimg_library;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	
	//PARSE PARAM
	
	std::string param_image;
	parser.addShortOption('i', &param_image, 1, assignString, displayString, (char*)"<string> the image to halftone", (char*)"Image");
	std::string param_output;
	parser.addShortOption('o', &param_output, 1, assignString, displayString, (char*)"<string> the output halftoned image", (char*)"Output");
	uint param_threshold=127;
	parser.addShortOption('t', &param_threshold, 1, assignUInt, displayUInt, (char*)"<uint=127, in [0,255]> the threshold value", (char*)"Threshold");
	//PARSING
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
	
	CImg<unsigned char> matrix(128, 128, 1, 1);
	for(uint i=0; i<128; i++)
	for(uint j=0; j<128; j++)
		matrix.atXYZC(i, j, 0, 1) = param_threshold;
	matrix.save("constantmatrix_128.png");
	
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

		if(bw_value > param_threshold)
			for(uint c=0; c<m_imgchannels; c++)
				m_image.atXYZC(i, j, 0, c) = 255;
		else
			for(uint c=0; c<m_imgchannels; c++)
				m_image.atXYZC(i, j, 0, c) = 0;
	}

	m_image.save(param_output.c_str());
	
	return 0;
}