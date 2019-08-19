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
#include "../parameters/ParamParser.hpp"
#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"


std::string param_input;
bool param_display_help = false;
std::string param_output="output_pts.dat";
bool param_verbose = true;


int initParserScrambler(utk::ParamParser* parser)
{
	parser->addShortOption('i', &param_input, 1, utk::assignString, utk::displayString, "[string]\tInput file", "Input");
	parser->addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, "[string=output_pts.dat]\tOutput file", "Output");

	parser->addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, "\t\t\tSilent mode", "Verbose" );
	parser->addShortOption('h', &param_display_help, 0, utk::assignBoolTrue, NULL, "\t\t\t\tDisplays this help message");
    
    return 0;
}

int dealParamParserScrambler(utk::ParamParser* parser)
{
	if(param_input.empty())
	{
		ERROR("Argument -i required");
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	if(param_output.empty())
	{
		ERROR("Argument -o required");
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	if(param_display_help)
	{
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	if(param_verbose)
	{
		parser->displayState();
	}

	return 1;
}
