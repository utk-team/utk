#include "../../parameters/ParamParser_getopt.hpp"
#include "../../../externals/CImg-1.6.9/CImg.h"

#define D 2
using namespace cimg_library;
using namespace utk;

void swap_subset_image_x(CImg<double>& img_rgb, uint x, uint size_chunck)
{
	for(int p=x; p<x+size_chunck; p++)
	{
		for(uint y=0; y<img_rgb.height(); y++)
		for(uint c=0; c<img_rgb.spectrum(); c++)
		{
			uint tmp = img_rgb.atXYZC(p, y, 0, c);
			img_rgb.atXYZC(p, y, 0, c) = img_rgb.atXYZC(p+size_chunck, y, 0, c);
			img_rgb.atXYZC(p+size_chunck, y, 0, c) = tmp;
		}
	}
}

void swap_subset_image_y(CImg<double>& img_rgb, uint x, uint size_chunck)
{
	for(int p=x; p<x+size_chunck; p++)
	{
		for(uint y=0; y<img_rgb.height(); y++)
		for(uint c=0; c<img_rgb.spectrum(); c++)
		{
			uint tmp = img_rgb.atXYZC(y, p, 0, c);
			img_rgb.atXYZC(y, p, 0, c) = img_rgb.atXYZC(y, p+size_chunck, 0, c);
			img_rgb.atXYZC(y, p+size_chunck, 0, c) = tmp;
		}
	}
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	ParamParser_getopt parser;

	std::string param_image = "";
	std::string param_output = "";
	parser.addLongOption((char*)"image", &param_image, 1, assignString, displayString, (char*)"<string>\tThe image to scramble", (char*)"Image");
	parser.addShortOption('o', &param_output, 1, assignString, displayString, (char*)"<string>\tThe image to output", (char*)"Output");
	parser.parse(argc, argv);
	
	if(param_image=="")
	{
		std::cout << "Parameter --image is mandatory" << std::endl;
		return 0;
	}
	if(param_output=="")
	{
		std::cout << "Parameter --o is mandatory" << std::endl;
		return 0;
	}

	std::string m_imagepath;
	CImg<double> m_image_rgb;
	CImg<double> m_image_bw;
	uint m_imgwidth = 0;
	uint m_imgheight = 0;
	uint m_imgchannels = 0;
	
	m_imagepath=param_image;
	m_image_rgb = CImg<double>(m_imagepath.c_str());
	m_imgheight = m_image_rgb.height();
	m_imgwidth = m_image_rgb.width();
	
	CImg<double> m_image_xshuffled = m_image_rgb;
	CImg<double> m_image_yshuffled = m_image_rgb;
	
	int lvl_max = 4;
	
	//X only
	for(int l=0; l<lvl_max; l++)
	{
		int size_chunck = m_imgwidth/(1 << (l+1));
		for(int i=0; i<(1 << l); i++)
		{
			bool swap = rand()%2;
			if(swap) swap_subset_image_x(m_image_xshuffled, 2*i*size_chunck, size_chunck); 
			if(swap) swap_subset_image_x(m_image_rgb, 2*i*size_chunck, size_chunck); 
		}
	}
	
	//Y only
	for(int l=0; l<lvl_max; l++)
	{
		int size_chunck = m_imgheight/(1 << (l+1));
		for(int i=0; i<(1 << l); i++)
		{
			bool swap = rand()%2;
			if(swap) swap_subset_image_y(m_image_yshuffled, 2*i*size_chunck, size_chunck); 
			if(swap) swap_subset_image_y(m_image_rgb, 2*i*size_chunck, size_chunck); 
		}
	}
	
	m_image_xshuffled.save("totoX.png");
	m_image_yshuffled.save("totoY.png");
	m_image_rgb.save("toto.png");
	
	return 0;
}