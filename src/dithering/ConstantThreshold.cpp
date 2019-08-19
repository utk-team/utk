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
#include "../../externals/CImg-1.6.9/CImg.h"
#include "../parameters/ParamParser_getopt.hpp"

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